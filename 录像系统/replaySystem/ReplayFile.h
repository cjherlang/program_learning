/********************************************************************
    Baina@copyright 2013
作用: 录像文件基本指令类定义
作者: Geek_Tiger
时间: 2014/7/21
---------------------------------------------------------------------        
备注: 录像文件类, 将模拟事件以单链表形式保存, 方便录像的回放与录制
*********************************************************************/
#pragma once

#include "Opcode.h"

#define IReplay     CReplayFile::Instance()

// 这个类负责录像文件的录制 与 加载
class CReplayFile
{
    struct stNode
    {
        CReplayOpcode*  m_pData;
        stNode*         m_pNext;

        stNode(CReplayOpcode* pData)
        {
            m_pData = pData;
            m_pNext = NULL;
        }
    };

    stNode*     m_pHead;            // 头结点
    stNode*     m_pTail;            // 尾节点
    uint32      m_initTime;         // 录像文件加载完成时间
    int         m_nNumOpcode;       // 操作码总数
    bool        m_bReplay;          // 是否为回放模式
    
    //COpcode*    GenerateTouchOpcode()
public:
    static CReplayFile& Instance()
    {
        static CReplayFile opcodeManager;
        return opcodeManager;
    }
    CReplayFile();
    ~CReplayFile();
    bool init(const char* szFile, bool bReplay = false);    // 初始化
    bool excute();                          // 回放录像
    void addOpcode(CReplayOpcode* pNew);    // 添加一个操作码
    bool saveToFile(const char* szFile);    // 存入文件
};