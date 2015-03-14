/********************************************************************
    Baina@copyright 2013
����: ¼���ļ�����ָ���ඨ��
����: Geek_Tiger
ʱ��: 2014/7/21
---------------------------------------------------------------------        
��ע: ¼���ļ���, ��ģ���¼��Ե�������ʽ����, ����¼��Ļط���¼��
*********************************************************************/
#pragma once

#include "Opcode.h"

#define IReplay     CReplayFile::Instance()

// ����ฺ��¼���ļ���¼�� �� ����
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

    stNode*     m_pHead;            // ͷ���
    stNode*     m_pTail;            // β�ڵ�
    uint32      m_initTime;         // ¼���ļ��������ʱ��
    int         m_nNumOpcode;       // ����������
    bool        m_bReplay;          // �Ƿ�Ϊ�ط�ģʽ
    
    //COpcode*    GenerateTouchOpcode()
public:
    static CReplayFile& Instance()
    {
        static CReplayFile opcodeManager;
        return opcodeManager;
    }
    CReplayFile();
    ~CReplayFile();
    bool init(const char* szFile, bool bReplay = false);    // ��ʼ��
    bool excute();                          // �ط�¼��
    void addOpcode(CReplayOpcode* pNew);    // ���һ��������
    bool saveToFile(const char* szFile);    // �����ļ�
};