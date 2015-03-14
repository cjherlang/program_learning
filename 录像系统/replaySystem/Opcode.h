/********************************************************************
    Baina@copyright 2013
作用: 录像文件基本指令类定义
作者: Geek_Tiger
时间: 2014/7/21
---------------------------------------------------------------------        
备注: 操作码模拟类, 这里的类抽象了按键输入以及收发包的模拟操作
      1 录制模式下, 会根据在客户端上层截取按键, 收发包信息存入录像文件中
      2 回放模式下, 会从录像文件中读取按键收发包指令再模拟出这些操作丢给客户端处理
*********************************************************************/
#pragma once

#include <queue>
#include "cocos2d.h"
#include "typeDef.h"
#include "OS.h"
#include "NetPacket.h"

//USING_NS_CC;

// 模拟的操作码定义
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

// 操作码基类
// ps: 类中定义表示在内存中的数据结构, 存入录像文件中 
// 通过虚函数save根据当前数据结构定义生成文本信息写入文件
// 录像回放时依次遍历操作码节点 按顺序执行execute函数.
class CReplayOpcode
{
protected:
    int     m_nType;        // 指令类型
    int     m_nExcuteTime;  // 触发时间

    bool    isOnExcute();   // 是否到了触发时间

public:
    CReplayOpcode(int nType, uint32 time): m_nType(nType), m_nExcuteTime(time) {}
    virtual     ~CReplayOpcode() {}
    virtual     bool  save(int nIndex)            = 0;  // 录制模式下, 保存当前的操作码
    virtual     bool  execute(int currentTime) = 0;  // 回放模式下, 模拟当前的操作码
};

// 触摸操作码 (单点触控与多点触控模拟类)
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

    static  cocos2d::CCTouch                m_touch;                // 单点触摸数据定义
    static  cocos2d::CCSet                  m_touchSet;             // 触摸集合类
    static  cocos2d::CCTouchDispatcher*     m_pTouchDispatcher;     // 触摸派遣类

    typedef std::vector<stTouch*>::iterator itor;
    std::vector<stTouch*> m_vecTouch;                               // 用于保存多点触控信息       
    int                   m_nTouchType;                             // 触摸类型
            
    const char*  getTouchText();                                    // 取得单点触控的数据格式(用于写入录像文件)

public:
    CTouchOpcode(uint32 time, int nTouchType);
    ~CTouchOpcode();
    void         add(int nID, int nPosX, int nPosY);                // 添加一个触摸坐标信息(多点触控)
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
    static  void setTouchDispatcher(cocos2d::CCTouchDispatcher* pTouchDispatcher) {m_pTouchDispatcher = pTouchDispatcher;}  // 设置触摸派遣类
};

// 发包操作码
class CSendPacketOpcode: public CReplayOpcode
{
    static vector<int>  m_vecCheckOpcode;       // 发包校验码
    INetPacket*         m_pNetPacket;           // 封包数据类定义

public:
    CSendPacketOpcode(uint32 time, INetPacket* pNetPacket);
    CSendPacketOpcode(uint32 time, int nOpcode, const char* szData, int nSize);
    ~CSendPacketOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
    static void  pushCheckOpcode(int nOpcode);      // 压入一个校验码
    static int   getCheckOpcode();                  // 取得一个校验码
    static void  popCheckOpcode();                  // 弹出一个校验码
};

// 收包操作码
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

// 切换前后台操作码
class CActiveOpcode: public CReplayOpcode
{
    int    m_nActive;

public:
    CActiveOpcode(uint32 time, int nActive);
    ~CActiveOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
};


// 断线重连操作码
class CReconnectOpcode: public CReplayOpcode
{
    int             m_nConnectType;        // 断线还是重练?
    int             m_nErrorCode;          // 错误码

public:
    CReconnectOpcode(uint32 time, int nConnectType, int nErrorCode);
    ~CReconnectOpcode();
    virtual bool save(int nIndex);
    virtual bool execute(int currentTime);
};

