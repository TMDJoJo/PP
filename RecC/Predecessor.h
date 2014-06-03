#ifndef PREDECESSOR_H
#define PREDECESSOR_H

#include "ev_buffer.h"

enum PACKET_FLAG
{
    REQ_LIST = 1, // �����б�
    RESP_LIST = 2, // �ظ��б�

    REQ_FILEID = 3,
    RESP_FILEID = 4, // ����,�����ļ�id

    REQ_DATA = 5,
    RESP_DATA = 6, // ����,�����ļ�����

    REQ_INIT = 7,
    RESP_INIT = 8 // ����,���س�ʼ��
};

#define DEF_LEN  (0x28) // Ĭ�ϵ���������
#define MAX_PATH_LEN    (256)

typedef struct _myStru
{ // ��Ϣ��
    unsigned int len; // ��������
    unsigned int nouse_0;
    unsigned int nouse_1;
    unsigned short nouse_2;
    unsigned short ver; // ��Զ��1
    unsigned int flag; // 12345678
    unsigned int exdata; // ��չ����
    unsigned int b_or_e; // ��ʼ���ǽ���
    unsigned int isfileid; // 0��������,1��ʾ�����и������ļ�id

    unsigned int nouse_3;
    unsigned int nouse_4;
    unsigned int nextlen; // ��������ĳ���
    unsigned char * data; // ����
} myStru;

typedef struct _myData
{ // ������
    unsigned int f_or_d; // 0x10--Ŀ¼;0x20--�ļ�
    unsigned int nouse[10]; // û����
    char path[MAX_PATH_LEN]; // ·��
    char sm_name[16]; // small name,����
} myData;

class Predecessor
{
public:
    Predecessor();
    myStru* makeInitPacket();
    myStru* makeListPacket(const char* path);
    myStru* makeFileIdPacket(const char * path);
    myStru* makeDataPacket(unsigned int fileid);
    myStru* makeRecvPacket(unsigned int packet_len);
    void clearRecvPacket( myStru * pPacket );
    void freeSendPacket( myStru * p );
    int writePacket(myStru * pPacket,EvBuffer* out);
    myStru* processRecvBuff(EvBuffer* in_buf);

    void ReceiveData(int len);

private:
    myStru * g_pRecvPacket; // ��ǰ���ڴ������Ϣ��
    bool is_recving_data_;

    int data_len_;

};

#endif // PREDECESSOR_H
