#include "ReplayFile.h"
#include "Opcode.h"
#include "../scriptEngine/ScriptSystem.h"

USING_NS_CC;

CReplayFile::CReplayFile(): m_pHead(0), m_pTail(0), m_initTime(0), m_bReplay(false)
{
    m_nNumOpcode = 1;
}

// 析构, 释放内存
CReplayFile::~CReplayFile()
{
    stNode* pNode;
    if (m_bReplay)
    {
        while (m_pHead)
        {
            pNode   = m_pHead;
            m_pHead = m_pHead->m_pNext;

            delete pNode->m_pData;
            delete pNode;
        }
    }
}

bool CReplayFile::init(const char* szFile, bool bReplay)
{
    // 取得录像文件路径
    std::string replayPath = CCFileUtils::sharedFileUtils()->getWritablePath();
    replayPath += szFile;
    m_bReplay   = bReplay;

    ELOG("replay file: %s", replayPath.c_str());
    
    // 回放模式, 调用lua加载录像文件
    if (m_bReplay)
    {
        IScriptEngine->CallLua("loadReplayFile", "s", replayPath.c_str());
        m_initTime  = OS::GetTimeMS();
    }
    else
    {
        // 录制模式下, 新建录像文件
        IScriptEngine->CallLua("createReplayfile", "s", replayPath.c_str());
        IScriptEngine->CallLua("writeToReplayFile", "s", "Replay = {}\n");
    }

    return true;
}

// 添加一个操作码
void CReplayFile::addOpcode(CReplayOpcode* pNew)
{
    // 回放模式中, 新建操作码放入队列中
    if (m_bReplay)
    {
        stNode* pNewNode = new stNode(pNew);
        if (m_pTail)
        {
            m_pTail->m_pNext    = pNewNode;
            m_pTail             = pNewNode;
        }
        else
            m_pHead = m_pTail   = pNewNode;
    }
    else
    {
        // 录制模式中 直接写入文件
        pNew->save(m_nNumOpcode);
        delete pNew;
    }
    
    m_nNumOpcode++;
}

// 录像文件的执行流程
bool CReplayFile::excute()
{
    stNode* pNode;
    static int nCount = 1;

    // 取得当前的系统时间
    int currentTime = OS::GetTimeMS() - m_initTime;

    // printf("current time: %d\n", currentTime);

    if (m_pHead)
    {
        // 丢给当前节点执行
        if (m_pHead->m_pData->execute(currentTime))
        {
            // 执行完成以后删除当前节点
            pNode   = m_pHead;
            m_pHead = m_pHead->m_pNext;

            // 对于回放模式下, 播放完一个节点即可释放当前节点了
            delete pNode->m_pData;
            delete pNode;

            printf("current frame: %d\n", nCount++);

            return true;
        }
    }
    else
        ELOG("replay finish!");
    
    return false;
}

// 关闭录像文件句柄
bool CReplayFile::saveToFile(const char* szFile)
{
    IScriptEngine->CallLua("closeReplayFile");
    return true;
}