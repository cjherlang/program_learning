#include "ReplayFile.h"
#include "Opcode.h"
#include "../scriptEngine/ScriptSystem.h"

USING_NS_CC;

CReplayFile::CReplayFile(): m_pHead(0), m_pTail(0), m_initTime(0), m_bReplay(false)
{
    m_nNumOpcode = 1;
}

// ����, �ͷ��ڴ�
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
    // ȡ��¼���ļ�·��
    std::string replayPath = CCFileUtils::sharedFileUtils()->getWritablePath();
    replayPath += szFile;
    m_bReplay   = bReplay;

    ELOG("replay file: %s", replayPath.c_str());
    
    // �ط�ģʽ, ����lua����¼���ļ�
    if (m_bReplay)
    {
        IScriptEngine->CallLua("loadReplayFile", "s", replayPath.c_str());
        m_initTime  = OS::GetTimeMS();
    }
    else
    {
        // ¼��ģʽ��, �½�¼���ļ�
        IScriptEngine->CallLua("createReplayfile", "s", replayPath.c_str());
        IScriptEngine->CallLua("writeToReplayFile", "s", "Replay = {}\n");
    }

    return true;
}

// ���һ��������
void CReplayFile::addOpcode(CReplayOpcode* pNew)
{
    // �ط�ģʽ��, �½���������������
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
        // ¼��ģʽ�� ֱ��д���ļ�
        pNew->save(m_nNumOpcode);
        delete pNew;
    }
    
    m_nNumOpcode++;
}

// ¼���ļ���ִ������
bool CReplayFile::excute()
{
    stNode* pNode;
    static int nCount = 1;

    // ȡ�õ�ǰ��ϵͳʱ��
    int currentTime = OS::GetTimeMS() - m_initTime;

    // printf("current time: %d\n", currentTime);

    if (m_pHead)
    {
        // ������ǰ�ڵ�ִ��
        if (m_pHead->m_pData->execute(currentTime))
        {
            // ִ������Ժ�ɾ����ǰ�ڵ�
            pNode   = m_pHead;
            m_pHead = m_pHead->m_pNext;

            // ���ڻط�ģʽ��, ������һ���ڵ㼴���ͷŵ�ǰ�ڵ���
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

// �ر�¼���ļ����
bool CReplayFile::saveToFile(const char* szFile)
{
    IScriptEngine->CallLua("closeReplayFile");
    return true;
}