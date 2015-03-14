/********************************************************************
    Baina@copyright 2013
作用: 录像文件基本指令类定义
作者: Geek_Tiger
时间: 2014/7/21
---------------------------------------------------------------------        
备注: 录像系统类 负责录像系统上层管理.
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
    E_STATE_RECORD,         // 记录
    E_STATE_REPLAY,         // 回放
};

// 录像系统类定义
class CReplaySystem: public cocos2d::CCObject
{
    int                             m_nState;              // 当前状态
    cocos2d::CCTouchDispatcher*     m_pTouchDispatcher;    // 按键派遣句柄
    uint32                          m_initTime;            // 系统初始化时间

    CReplaySystem(): m_nState(0), m_pTouchDispatcher(0), m_initTime(0) {}

public:
    static CReplaySystem& Instance()
    {
        static CReplaySystem replaySystem;
        return replaySystem;
    }

    bool            isOnReplayMode();   // 是否是回放模式
    bool            isOnRecordMode();   // 是否是录像模式
    bool            initFromLua();      // 从脚本配置初始化心系统
    bool            init(int nState, const char* szPath = NULL);    
    bool            exit();             // 退出系统
    void            update(float dt);   // 更新系统
    uint32          getCurrentTime();   // 取得当前系统的绝对时间
    bool            sendPacketCallback(INetPacket* pNetPacket);     // 发包拦截接口
    bool            recvPacketCallback(INetPacket* pNetPacket);     // 收包拦截接口
    bool            connectCallback(int nType, int nErrorCode);     // 重连拦截接口
    bool            activeCallback(bool bActive);                   // 切换后台拦截接口
    static void     touchCallback(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent, unsigned int uIndex);   // 输入拦截接口
};