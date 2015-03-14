#include "ReplaySystem.h"
#include "ReplayFile.h"
#include "CCLuaEngine.h"
#include "../scriptEngine/ScriptSystem.h"

USING_NS_CC;

#define REPLAY_FILE                 "replay.gtr"

// ȡ�þ���ʱ��(�Ծ���ͻ�������Ϊ��ʼ��)
uint32 CReplaySystem::getCurrentTime()
{
    if (m_initTime)
        return OS::GetTimeMS() - m_initTime;
    
    return 0;
}

// �����ص����ؽӿ�, ¼��ģʽ����Ҫ��¼�´�����Ϣ
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
        // ��ӡLOG����
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

// ��������
bool CReplaySystem::sendPacketCallback(INetPacket* pNetPacket)
{
    // �ط�ģʽ��, ���Ƚ��ͻ��˵ķ���ͳͳѹ��һ������(����У�鴦��), �������ϲ�ֱ�ӷ���, ���οͻ��˵ķ���
    if (isOnReplayMode()) 
    {
        CSendPacketOpcode::pushCheckOpcode(pNetPacket->GetOpcode());
        return false;
    }

    // ¼��ģʽ��, ��¼��ǰ�ķ�������
    if (isOnRecordMode())
        IReplay.addOpcode(new CSendPacketOpcode(getCurrentTime(), pNetPacket));

    return true;
}

// �հ�����
bool CReplaySystem::recvPacketCallback(INetPacket* pNetPacket)
{
    // ¼��ģʽ��, ��¼�µ�ǰ���հ����� 
    if (isOnRecordMode())
        IReplay.addOpcode(new CRecvPacketOpcode(getCurrentTime(), pNetPacket));

    return true;
}

// ϵͳ�˳�
bool CReplaySystem::exit()
{
    // ��¼ģʽ��Ҫ�������ļ�
    if (IReplaySystem.isOnRecordMode())
        IReplay.saveToFile(REPLAY_FILE);

    return true;
}

// �Ƿ���¼��ģʽ
bool CReplaySystem::isOnRecordMode()
{
    return m_nState == E_STATE_RECORD;
}

// �Ƿ��ǻط�ģʽ
bool CReplaySystem::isOnReplayMode()
{
    return m_nState == E_STATE_REPLAY;
}

// ��LUA�ж�ȡ���� ��ʼ��ϵͳģʽ
bool CReplaySystem::initFromLua()
{
    int nMode = IScriptEngine->GetGlobalVal("REPLAY_MODE");
    // �ط�ģʽ
    if (nMode == 1)
        init(E_STATE_RECORD);
    else if (nMode == 2)
        init(E_STATE_REPLAY);

    return true;
}

// ��ʼ��
bool CReplaySystem::init(int nState, const char* szPath /* = NULL */)
{
    // ���ؽű��߼��ļ�
    CCLuaEngine* pEngine    = CCLuaEngine::defaultEngine();
    std::string  replayPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("lua/replay/replay.lua");
    pEngine->executeScriptFile(replayPath.c_str());


    m_pTouchDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    m_nState           = nState; 

    // ��ʼ��ϵͳʱ��
    m_initTime         = OS::GetTimeMS();
    
    // ��¼ģʽ�����ô������ػص�
    if (m_nState == E_STATE_RECORD)
    {
        m_pTouchDispatcher->setRecordTouchCallback(touchCallback);
        IReplay.init(REPLAY_FILE, false);
    }
    else if (m_nState == E_STATE_REPLAY)
    {
        // �ط�ģʽ
        m_pTouchDispatcher->setRecordTouchCallback(touchCallback);

        // ���δ����¼�
        m_pTouchDispatcher->setDispatchEvents(false);

        // ���ô������ػص�
        CTouchOpcode::setTouchDispatcher(m_pTouchDispatcher);

        // ����¼���ļ�
        IReplay.init(REPLAY_FILE, true);

        //CCScheduler* m_scheduler = new CCScheduler();

        // ע��һ��ȫ�ֶ�ʱ��(��¼��ϵͳ����ѭ������)
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
    }

    return true;
}

// ���ζ�ȡģ��ڵ�, ��ʼ����¼���ļ�
void CReplaySystem::update(float dt)
{
    IReplay.excute();
}

// �л�ǰ��̨����
bool CReplaySystem::activeCallback(bool bActive)
{
    // �ط�ģʽ��, ��Ҫ���е��л�ǰ��̨����(�������ϲ�����CCDirector::sharedDirector()->stopAnimation() ���������Ϸѭ����ֹ, ����¼��Ķ�ʱ������, �޷�����)
    if (IReplaySystem.isOnReplayMode())
        return false;

    // ¼��ģʽ��, ��¼���л�ǰ��̨�Ĳ���
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
    // �ط�ģʽ�� ��Ҫ�����ϲ������/�Ͽ��߼����� (
    if (IReplaySystem.isOnReplayMode())
        return false;

    if (IReplaySystem.isOnRecordMode())
        IReplay.addOpcode(new CReconnectOpcode(getCurrentTime(), nType, nErrorCode));

    return true;
}