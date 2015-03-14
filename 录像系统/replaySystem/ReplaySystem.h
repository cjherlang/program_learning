/********************************************************************
    Baina@copyright 2013
����: ¼���ļ�����ָ���ඨ��
����: Geek_Tiger
ʱ��: 2014/7/21
---------------------------------------------------------------------        
��ע: ¼��ϵͳ�� ����¼��ϵͳ�ϲ����.
*********************************************************************/
#pragma once

#include "cocos2d.h"
#include "typeDef.h"
#include "NetPacket.h"
#include "Opcode.h"
 

#define IReplaySystem       CReplaySystem::Instance()

enum
{
    E_STATE_NULL    = 0,
    E_STATE_RECORD,         // ��¼
    E_STATE_REPLAY,         // �ط�
};

// ¼��ϵͳ�ඨ��
class CReplaySystem: public cocos2d::CCObject
{
    int                             m_nState;              // ��ǰ״̬
    cocos2d::CCTouchDispatcher*     m_pTouchDispatcher;    // ������ǲ���
    uint32                          m_initTime;            // ϵͳ��ʼ��ʱ��

    CReplaySystem(): m_nState(0), m_pTouchDispatcher(0), m_initTime(0) {}

public:
    static CReplaySystem& Instance()
    {
        static CReplaySystem replaySystem;
        return replaySystem;
    }

    bool            isOnReplayMode();   // �Ƿ��ǻط�ģʽ
    bool            isOnRecordMode();   // �Ƿ���¼��ģʽ
    bool            initFromLua();      // �ӽű����ó�ʼ����ϵͳ
    bool            init(int nState, const char* szPath = NULL);    
    bool            exit();             // �˳�ϵͳ
    void            update(float dt);   // ����ϵͳ
    uint32          getCurrentTime();   // ȡ�õ�ǰϵͳ�ľ���ʱ��
    bool            sendPacketCallback(INetPacket* pNetPacket);     // �������ؽӿ�
    bool            recvPacketCallback(INetPacket* pNetPacket);     // �հ����ؽӿ�
    bool            connectCallback(int nType, int nErrorCode);     // �������ؽӿ�
    bool            activeCallback(bool bActive);                   // �л���̨���ؽӿ�
    static void     touchCallback(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent, unsigned int uIndex);   // �������ؽӿ�
};