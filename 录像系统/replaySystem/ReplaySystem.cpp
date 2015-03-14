#include "ReplaySystem.h"
#include "ReplayFile.h"
#include "CCLuaEngine.h"
#include "../scriptEngine/ScriptSystem.h"

USING_NS_CC;

#define REPLAY_FILE                 "replay.gtr"

// 取得绝对时间(以距离客户端启动为起始点)
uint32 CReplaySystem::getCurrentTime()
{
    if (m_initTime)
        return OS::GetTimeMS() - m_initTime;
    
    return 0;
}

// 触摸回调拦截接口, 录制模式下需要记录下触摸信息
void CReplaySystem::touchCallback(CCSet *pTouches, CCEvent *pEvent, unsigned int uIndex)
{
    CCTouch *pTouch;
    CCSetIterator setIter;
    int nCount = 0;
    
    if (IReplaySystem.isOnRecordMode())
    {
        CTouchOpcode* pNewTouch = new CTouchOpcode(IReplaySystem.getCurrentTime(), uIndex);
        for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
        {
            pTouch = (CCTouch *)(*setIter);

/*
            DLOG("touches set: %d",          ++nCount);
            DLOG("time: %d\n",               IReplaySystem.getCurrentTime());
            DLOG("id: %d\n",                 pTouch->getID());
            DLOG("startPoint: %f, %f\n",     pTouch->getStartLocationInView().x, pTouch->getStartLocationInView().y);
            DLOG("m_point: %f, %f\n",        pTouch->getLocationInView().x, pTouch->getLocationInView().y);
            DLOG("prevPoint: %f, %f\n",      pTouch->getPreviousLocation().x, pTouch->getPreviousLocation().y);
            DLOG("uIndex: %d\n",             uIndex);*/

            pNewTouch->add(pTouch->getID(), pTouch->getLocationInView().x, pTouch->getLocationInView().y);
        }

        IReplay.addOpcode(pNewTouch);
    }
    else
    {
        /*
        // 打印LOG处理
        DLOG("*********************");
        for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
        {
            pTouch = (CCTouch *)(*setIter);

            DLOG("touches set: %d",          ++nCount);
            DLOG("time: %d\n",               IReplaySystem.getCurrentTime());
            DLOG("id: %d\n",                 pTouch->getID());
            DLOG("startPoint: %f, %f\n",     pTouch->getStartLocationInView().x, pTouch->getStartLocationInView().y);
            DLOG("m_point: %f, %f\n",        pTouch->getLocationInView().x, pTouch->getLocationInView().y);
            DLOG("prevPoint: %f, %f\n",      pTouch->getPreviousLocation().x, pTouch->getPreviousLocation().y);
            DLOG("uIndex: %d\n",             uIndex);
        }
        */
    }
}

// 发包拦截
bool CReplaySystem::sendPacketCallback(INetPacket* pNetPacket)
{
    // 回放模式下, 首先将客户端的发包统统压入一个队列(用于校验处理), 并且在上层直接返回, 屏蔽客户端的发包
    if (isOnReplayMode()) 
    {
        CSendPacketOpcode::pushCheckOpcode(pNetPacket->GetOpcode());
        return false;
    }

    // 录制模式下, 记录当前的发包数据
    if (isOnRecordMode())
        IReplay.addOpcode(new CSendPacketOpcode(getCurrentTime(), pNetPacket));

    return true;
}

// 收包拦截
bool CReplaySystem::recvPacketCallback(INetPacket* pNetPacket)
{
    // 录制模式下, 记录下当前的收包数据 
    if (isOnRecordMode())
        IReplay.addOpcode(new CRecvPacketOpcode(getCurrentTime(), pNetPacket));

    return true;
}

// 系统退出
bool CReplaySystem::exit()
{
    // 记录模式需要保存入文件
    if (IReplaySystem.isOnRecordMode())
        IReplay.saveToFile(REPLAY_FILE);

    return true;
}

// 是否是录制模式
bool CReplaySystem::isOnRecordMode()
{
    return m_nState == E_STATE_RECORD;
}

// 是否是回放模式
bool CReplaySystem::isOnReplayMode()
{
    return m_nState == E_STATE_REPLAY;
}

// 从LUA中读取配置 初始化系统模式
bool CReplaySystem::initFromLua()
{
    int nMode = IScriptEngine->GetGlobalVal("REPLAY_MODE");
    // 回放模式
    if (nMode == 1)
        init(E_STATE_RECORD);
    else if (nMode == 2)
        init(E_STATE_REPLAY);

    return true;
}

// 初始化
bool CReplaySystem::init(int nState, const char* szPath /* = NULL */)
{
    // 加载脚本逻辑文件
    CCLuaEngine* pEngine    = CCLuaEngine::defaultEngine();
    std::string  replayPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("lua/replay/replay.lua");
    pEngine->executeScriptFile(replayPath.c_str());


    m_pTouchDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    m_nState           = nState; 

    // 初始化系统时间
    m_initTime         = OS::GetTimeMS();
    
    // 记录模式下设置触摸拦截回调
    if (m_nState == E_STATE_RECORD)
    {
        m_pTouchDispatcher->setRecordTouchCallback(touchCallback);
        IReplay.init(REPLAY_FILE, false);
    }
    else if (m_nState == E_STATE_REPLAY)
    {
        // 回放模式
        m_pTouchDispatcher->setRecordTouchCallback(touchCallback);

        // 屏蔽触摸事件
        m_pTouchDispatcher->setDispatchEvents(false);

        // 设置触摸拦截回调
        CTouchOpcode::setTouchDispatcher(m_pTouchDispatcher);

        // 加载录像文件
        IReplay.init(REPLAY_FILE, true);

        //CCScheduler* m_scheduler = new CCScheduler();

        // 注册一个全局定时器(供录像系统的总循环处理)
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
    }

    return true;
}

// 依次读取模拟节点, 开始播放录像文件
void CReplaySystem::update(float dt)
{
    IReplay.excute();
}

// 切换前后台拦截
bool CReplaySystem::activeCallback(bool bActive)
{
    // 回放模式下, 需要所有的切换前后台操作(并且在上层拦截CCDirector::sharedDirector()->stopAnimation() 这里会让游戏循环终止, 导致录像的定时器卡主, 无法播放)
    if (IReplaySystem.isOnReplayMode())
        return false;

    // 录制模式下, 记录下切换前后台的操作
    if (IReplaySystem.isOnRecordMode())
    {
        if (bActive)
            IReplay.addOpcode(new CActiveOpcode(getCurrentTime(), E_ACTIVE_TRUE));
        else
            IReplay.addOpcode(new CActiveOpcode(getCurrentTime(), E_ACTIVE_FALSE));
    }

    return true;
}

bool CReplaySystem::connectCallback(int nType, int nErrorCode)
{
    // 回放模式下 需要拦截上层的连接/断开逻辑流程 (
    if (IReplaySystem.isOnReplayMode())
        return false;

    if (IReplaySystem.isOnRecordMode())
        IReplay.addOpcode(new CReconnectOpcode(getCurrentTime(), nType, nErrorCode));

    return true;
}