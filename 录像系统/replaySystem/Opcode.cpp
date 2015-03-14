#include <stdio.h>  
#include <string.h>
#include "../Game/Game.h"
#include "Opcode.h"
#include "../scriptEngine/ScriptSystem.h"

USING_NS_CC;

#define     MAX_PACKET_SIZE     1024

CCTouch            CTouchOpcode::m_touch;
CCSet              CTouchOpcode::m_touchSet;
CCTouchDispatcher* CTouchOpcode::m_pTouchDispatcher = NULL;

// ����������תΪ���ַ���
int bytes2String(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen)     
{     
    if (pDst != NULL)  
    {  
        *pDst = 0;
    }  

    if (pSrc == NULL || nSrcLen <= 0 || pDst == NULL || nDstMaxLen <= nSrcLen*2)
    {  
        return 0;
    }  

    const char szTable[] = "0123456789ABCDEF";      
    for(int i=0; i<nSrcLen; i++)     
    {     
        *pDst++ = szTable[pSrc[i] >> 4];     
        *pDst++ = szTable[pSrc[i] & 0x0f];     
    }     
 
    *pDst = '\0';     
    return  nSrcLen * 2;     
}   

// �ַ���ת��Ϊ������
int string2Bytes(unsigned char* szSrc, unsigned char* pDst, int nDstMaxLen)     
{  
    if(szSrc == NULL)  
    {  
        return 0;  
    }  
    int iLen = strlen((char *)szSrc);  
    if (iLen <= 0 || iLen%2 != 0 || pDst == NULL || nDstMaxLen < iLen/2)  
    {  
        return 0;  
    }  

    iLen /= 2;  
    //strupr((char *)szSrc);  
    for (int i=0; i<iLen; i++)  
    {  
        int iVal = 0;  
        unsigned char *pSrcTemp = szSrc + i*2;  
        sscanf((char *)pSrcTemp, "%02x", &iVal);  
        pDst[i] = (unsigned char)iVal;  
    }  

    return iLen;  
}

// �Զ����ʽ���ı���, ������������һ����д��lua��text, 
class CRecordText
{
    char szText[MAX_PACKET_SIZE];

public:
    // ֵ = �ַ���
    const char* getText(const char* szVar, const char* szVal)
    {
        sprintf(szText, "\n\t%s = %s,", szVar, szVal);
        return szText;
    }
    // ֵ = �޷�������
    const char* getText(const char* szVar, uint32 nVal)
    {
        sprintf(szText, "\n\t%s = %u,", szVar, nVal);
        return szText;
    }
    // ֵ = ����(������ʮ������)
    const char* getText(const char* szVar, int nVal, bool bHex = false)
    {
        if (bHex)
            sprintf(szText, "\n\t%s = 0X%04x,", szVar, nVal);
        else
            sprintf(szText, "\n\t%s = %d,", szVar, nVal);

        return szText;
    }

    // ֵ = ������
    const char* getText(const char* szVar, float fVal)
    {
        sprintf(szText, "\n\t%s = %f,", szVar, fVal);
        return szText;
    }

    // ֵ = ������������
    const char* getText(const char* szVar, void* pData, int nSize)
    {
        char szData[MAX_PACKET_SIZE];
        memset(szData, 0, sizeof(char) * MAX_PACKET_SIZE);
        int nLen = bytes2String((unsigned char*)pData, nSize, (unsigned char*)szData, MAX_PACKET_SIZE);
        sprintf(szText, "\n\t%s = \"%s\",", szVar, szData);
        return szText;
    }
};

CTouchOpcode::CTouchOpcode(uint32 time, int nTouchType): CReplayOpcode(E_OPCODE_TOUCH, time)
{
    m_nTouchType = nTouchType;
}

CTouchOpcode::~CTouchOpcode()
{
    for (itor it = m_vecTouch.begin(); it != m_vecTouch.end(); it++)
        delete (*it);

    m_vecTouch.clear();
}

// ���ض�㴥�ص� ����text
const char* CTouchOpcode::getTouchText()
{
    static char szBuffer[1024];
    CRecordText id, posX, posY;
    // CRecordText touch;
    sprintf(szBuffer, "\n\ttouch = {\n");

    for (itor it = m_vecTouch.begin(); it != m_vecTouch.end(); it++)
    {
        strcat(szBuffer, "\t{");
        strcat(szBuffer, id.getText("id",       (*it)->m_nID));
        strcat(szBuffer, posX.getText("posX",   (*it)->m_nPosX));
        strcat(szBuffer, posY.getText("posY",   (*it)->m_nPosY));
        //strcat(szBuffer, type.getText("type",   (*it)->m_nTouchType));
        strcat(szBuffer, "\n\t},");
    }

    strcat(szBuffer,  "\n\t}");

    return szBuffer;
}

// ���һ��������Ϣ
void CTouchOpcode::add(int nID, int nPosX, int nPosY)
{
    m_vecTouch.push_back(new stTouch(nID, nPosX, nPosY));
}

// ģ�ⴥ��( ����������Ϣ �ָ��õ�һ��touchSet���� �ڶ���cocos2dx�ײ㴦��)
bool CTouchOpcode::execute(int currentTime)
{
    // ����ʱ��Ŵ���
    if (currentTime >= m_nExcuteTime)
    {
        m_touchSet.removeAllObjects();

        for (itor it = m_vecTouch.begin(); it != m_vecTouch.end(); it++)
        {
            m_touch.setTouchInfo((*it)->m_nID, (*it)->m_nPosX, (*it)->m_nPosY);
            m_touchSet.addObject(&m_touch);

            // printf("CTouchOpcode excute! time: %d, posX: %d, posY: %d, type: %d\n", m_nExcuteTime, (*it)->m_nPosX, (*it)->m_nPosY, m_nTouchType);
        }

        m_pTouchDispatcher->touches(&m_touchSet, NULL, m_nTouchType);
        return true;
    }

    return false;
}

// ����
bool CTouchOpcode::save(int nIndex)
{
    static char szBuffer[1024];
    CRecordText type, time, opType;

    sprintf(szBuffer,
        "Replay[%d] =\n{%s%s%s%s\n}\n",
        nIndex,
        opType.getText("opType", m_nType),
        time.getText("time",     m_nExcuteTime),
        type.getText("type",     m_nTouchType),
        getTouchText());

    IScriptEngine->CallLua("writeToReplayFile", "s", szBuffer);
    return true;
}

vector<int>  CSendPacketOpcode::m_vecCheckOpcode;

// ���ڴ��г�ʼ��һ����������(¼��ģʽ��ʹ��)
CSendPacketOpcode::CSendPacketOpcode(uint32 time, INetPacket* pNetPacket): CReplayOpcode(E_OPCODE_SEND, time)
{
    m_pNetPacket = pNetPacket->CopyNew();
}

// �������г�ʼ��һ������(�ط�ģʽʹ��)
CSendPacketOpcode::CSendPacketOpcode(uint32 time, int nOpcode, const char* szData, int nSize): CReplayOpcode(E_OPCODE_SEND, time)
{
    char szBuffer[MAX_PACKET_SIZE];
    // ע�������½�һ��opcode�Ŀշ��, ������������׷������漴�ɵõ�֮ǰ�ķ��(�ǲ��Ǻܼ�:)
    m_pNetPacket = new NetPacket(nOpcode);
    string2Bytes((unsigned char*)szData, (unsigned char*)szBuffer, nSize);
    m_pNetPacket->Append((const uint8*)szBuffer, nSize);
}

CSendPacketOpcode::~CSendPacketOpcode()
{
    if (m_pNetPacket)
    {
        delete m_pNetPacket;
        m_pNetPacket = NULL;
    }
}

bool CSendPacketOpcode::save(int nIndex)
{
    static char szBuffer[1024];
    CRecordText time, opType, data, size, opcode;
    sprintf(szBuffer, 
            "Replay[%d] =\n{%s%s%s%s%s\n}\n",
            nIndex,
            opType.getText("opType",m_nType),
            time.getText("time",    m_nExcuteTime),
            opcode.getText("opcode",m_pNetPacket->GetOpcode(), true),
            data.getText("data",    m_pNetPacket->Content(), m_pNetPacket->Size()),
            size.getText("size",    m_pNetPacket->Size()));

    IScriptEngine->CallLua("writeToReplayFile", "s", szBuffer);
    return true;
}

// ģ�ⷢ��
bool CSendPacketOpcode::execute(int currentTime)
{
    // ����ʱ��Ŵ���
    if (currentTime >= m_nExcuteTime)
    {
        // ��¼���еı���ķ���opcode = ¼��ģʽ��ʵ�����ص��ķ���opcode, ��ģ�⵱ǰ�ķ�������
        if (m_pNetPacket->GetOpcode() == getCheckOpcode())
        {
            popCheckOpcode();
            printf("CSendPacketOpcode: 0X%04X time: %d]\n", m_pNetPacket->GetOpcode(), m_nExcuteTime);
            Game::sharedGame()->_SendPacket(m_pNetPacket);
            return true;
        }
    }

    return false;
}

// ѹ��һ����У��ķ���opcode
void CSendPacketOpcode::pushCheckOpcode(int nOpcode)
{
    m_vecCheckOpcode.insert(m_vecCheckOpcode.begin(), nOpcode);
}

// ����һ��opcode
void CSendPacketOpcode::popCheckOpcode()
{
    m_vecCheckOpcode.pop_back();
}

// ȡ�õ�ǰ��ҪУ���opcode
int CSendPacketOpcode::getCheckOpcode()
{
    if (m_vecCheckOpcode.size() > 0)
        return m_vecCheckOpcode.back();
    
    return 0;
}

// �հ��뷢����������
CRecvPacketOpcode::CRecvPacketOpcode(uint32 time, INetPacket* pNetPacket): CReplayOpcode(E_OPCODE_RECV, time)
{
    m_pNetPacket = pNetPacket->CopyNew();
}

CRecvPacketOpcode::CRecvPacketOpcode(uint32 time, int nOpcode, const char* szData, int nSize): CReplayOpcode(E_OPCODE_SEND, time)
{
    char szBuffer[MAX_PACKET_SIZE];
    m_pNetPacket = new NetPacket(nOpcode);
    string2Bytes((unsigned char*)szData, (unsigned char*)szBuffer, nSize);
    m_pNetPacket->Append((const uint8*)szBuffer, nSize);
}

CRecvPacketOpcode::~CRecvPacketOpcode()
{
    if (m_pNetPacket)
    {
        // TO FIX: �����ͷ��ڴ����һ������crash, �������Ͽ�����֮ǰ����ǰ�ͷ���.
        //delete m_pNetPacket;
        //m_pNetPacket = NULL;
    }
}

bool CRecvPacketOpcode::save(int nIndex)
{
    static char szBuffer[1024];
    CRecordText time, opType, data, size, opcode;
    sprintf(szBuffer, 
        "Replay[%d] =\n{%s%s%s%s%s\n}\n",
        nIndex,
        opType.getText("opType",m_nType),
        time.getText("time",    m_nExcuteTime),
        opcode.getText("opcode",m_pNetPacket->GetOpcode(), true),
        data.getText("data",    m_pNetPacket->Content(), m_pNetPacket->Size()),
        size.getText("size",    m_pNetPacket->Size()));

    IScriptEngine->CallLua("writeToReplayFile", "s", szBuffer);
    return true;
}

// ģ���հ�(ֱ�Ӷ����ͻ��˴���ǰ���)
bool CRecvPacketOpcode::execute(int currentTime)
{
    if (currentTime >= m_nExcuteTime)
    {
        printf("CRecvPacketOpcode: 0X%04X time: %d\n", m_pNetPacket->GetOpcode(), m_nExcuteTime);
        Game::sharedGame()->HandlePacket(m_pNetPacket);
        return true;
    }

    return false;
}

CActiveOpcode::CActiveOpcode(uint32 time, int nActive): CReplayOpcode(E_OPCODE_ACTIVE, time)
{
    m_nActive = nActive;
}

CActiveOpcode::~CActiveOpcode()
{

}

bool CActiveOpcode::save(int nIndex)
{
    static char szBuffer[1024];
    CRecordText time, opType, type;
    sprintf(szBuffer, 
        "Replay[%d] =\n{%s%s%s\n}\n",
        nIndex,
        opType.getText("opType",    m_nType),
        time.getText("time",        m_nExcuteTime),
        type.getText("active",      m_nActive));

    IScriptEngine->CallLua("writeToReplayFile", "s", szBuffer);
    return true;
}

// �ͻ�������ǰ��̨����
bool CActiveOpcode::execute(int currentTime)
{
    if (currentTime >= m_nExcuteTime)
    {
        if (m_nActive == E_ACTIVE_TRUE)
        {
            CCApplication::sharedApplication()->applicationWillEnterForeground();
        }
        else if (m_nActive == E_ACTIVE_FALSE)
        {
            CCApplication::sharedApplication()->applicationDidEnterBackground();
        }

        return true;
    }
    
    return false;
}

CReconnectOpcode::CReconnectOpcode(uint32 time, int nConnectType, int nErrorCode): CReplayOpcode(E_OPCODE_RECONNECT, time)
{
    m_nConnectType  = nConnectType;
    m_nErrorCode    = nErrorCode;
}

CReconnectOpcode::~CReconnectOpcode()
{

}

bool CReconnectOpcode::save(int nIndex)
{
    static char szBuffer[1024];
    CRecordText time, opType, type, errorCode;
    sprintf(szBuffer, 
        "Replay[%d] =\n{%s%s%s%s\n}\n",
        nIndex,
        opType.getText("opType",        m_nType),
        time.getText("time",            m_nExcuteTime),
        type.getText("ConnectType",     m_nConnectType),
        errorCode.getText("errorCode",  m_nErrorCode));

    IScriptEngine->CallLua("writeToReplayFile", "s", szBuffer);

    return true;
}

// ģ��ͻ��˶Ͽ�/�������
bool CReconnectOpcode::execute(int currentTime)
{
    //printf("currentTime: %d: , m_nExcuteTime %d\n", currentTime, m_nExcuteTime);
    if (currentTime >= m_nExcuteTime)
    {
        if (m_nConnectType == E_NET_RECONNECT)
            Game::sharedGame()->_OnConnect(m_nErrorCode);
        else if (m_nConnectType == E_NET_DISCONNECT)
            Game::sharedGame()->_OnDisconnect(m_nErrorCode);

        return true;
    }
    
    return false;
}