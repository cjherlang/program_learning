/********************************************************************
    Baina@copyright 2013
����: ¼���ļ�����ָ���ඨ��
����: Geek_Tiger
ʱ��: 2014/7/21
---------------------------------------------------------------------        
��ע: ������ģ����, �����������˰��������Լ��շ�����ģ�����
      1 ¼��ģʽ��, ������ڿͻ����ϲ��ȡ����, �շ�����Ϣ����¼���ļ���
      2 �ط�ģʽ��, ���¼���ļ��ж�ȡ�����շ���ָ����ģ�����Щ���������ͻ��˴���
*********************************************************************/
#pragma once

#include <queue>
#include "cocos2d.h"
#include "typeDef.h"
#include "OS.h"
#include "NetPacket.h"

//USING_NS_CC;

// ģ��Ĳ����붨��
enum
{
    E_OPCODE_NULL       = 0,
    E_OPCODE_TOUCH,
    E_OPCODE_SEND,
    E_OPCODE_RECV,
    E_OPCODE_ACTIVE,    
    E_OPCODE_RECONNECT,
};

enum
{
    E_ACTIVE_NULL   = 0,
    E_ACTIVE_TRUE,
    E_ACTIVE_FALSE,
};

enum
{
    E_NET_NULL      = 0,
    E_NET_RECONNECT,
    E_NET_DISCONNECT,
};

// ���������
// ps: ���ж����ʾ���ڴ��е����ݽṹ, ����¼���ļ��� 
// ͨ���麯��save���ݵ�ǰ���ݽṹ���������ı���Ϣд���ļ�
// ¼��ط�ʱ���α���������ڵ� ��˳��ִ��execute����.
class CReplayOpcode
{
protected:
    int     m_nType;        // ָ������
    int     m_nExcuteTime;  // ����ʱ��

    bool    isOnExcute();   // �Ƿ��˴���ʱ��

public:
    CReplayOpcode(int nType, uint32 time): m_nType(nType), m_nExcuteTime(time) {}
    virtual     ~CReplayOpcode() {}
    virtual     bool  save(int nIndex)            = 0;  // ¼��ģʽ��, ���浱ǰ�Ĳ�����
    virtual     bool  execute(int currentTime) = 0;  // �ط�ģʽ��, ģ�⵱ǰ�Ĳ�����
};

// ���������� (���㴥�����㴥��ģ����)
class CTouchOpcode: public CReplayOpcode
{
    struct stTouch
    {
        int     m_nID;
        int     m_nPosX;
        int     m_nPosY;

        stTouch(): m_nID(0), m_nPosX(0), m_nPosY(0) {}
        stTouch(int nID, int nPosX, int nPosY): m_nID(nID), m_nPosX(nPosX), m_nPosY(nPosY) {}
    };

    static  cocos2d::CCTouch                m_touch;                // ���㴥�����ݶ���
    static  cocos2d::CCSet                  m_touchSet;             // ����������
    static  cocos2d::CCTouchDispatcher*     m_pTouchDispatcher;     // ������ǲ��

    typedef std::vector<stTouch*>::iterator itor;
    std::vector<stTouch*> m_vecTouch;                               // ���ڱ����㴥����Ϣ       
    int                   m_nTouchType;                             // ��������
            
    const char*  getTouchText();                                    // ȡ�õ��㴥�ص����ݸ�ʽ(����д��¼���ļ�)

public:
    CTouchOpcode(uint32 time, int nTouchType);
    ~CTouchOpcode();
    void         add(int nID, int nPosX, int nPosY);                // ���һ������������Ϣ(��㴥��)
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
    static  void setTouchDispatcher(cocos2d::CCTouchDispatcher* pTouchDispatcher) {m_pTouchDispatcher = pTouchDispatcher;}  // ���ô�����ǲ��
};

// ����������
class CSendPacketOpcode: public CReplayOpcode
{
    static vector<int>  m_vecCheckOpcode;       // ����У����
    INetPacket*         m_pNetPacket;           // ��������ඨ��

public:
    CSendPacketOpcode(uint32 time, INetPacket* pNetPacket);
    CSendPacketOpcode(uint32 time, int nOpcode, const char* szData, int nSize);
    ~CSendPacketOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
    static void  pushCheckOpcode(int nOpcode);      // ѹ��һ��У����
    static int   getCheckOpcode();                  // ȡ��һ��У����
    static void  popCheckOpcode();                  // ����һ��У����
};

// �հ�������
class CRecvPacketOpcode: public CReplayOpcode
{
    INetPacket*  m_pNetPacket;

public:
    CRecvPacketOpcode(uint32 time, INetPacket* pNetPacket);
    CRecvPacketOpcode(uint32 time, int nOpcode, const char* szData, int nSize);
    ~CRecvPacketOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
};

// �л�ǰ��̨������
class CActiveOpcode: public CReplayOpcode
{
    int    m_nActive;

public:
    CActiveOpcode(uint32 time, int nActive);
    ~CActiveOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
};


// ��������������
class CReconnectOpcode: public CReplayOpcode
{
    int             m_nConnectType;        // ���߻�������?
    int             m_nErrorCode;          // ������

public:
    CReconnectOpcode(uint32 time, int nConnectType, int nErrorCode);
    ~CReconnectOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
};

