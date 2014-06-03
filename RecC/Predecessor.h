#ifndef PREDECESSOR_H
#define PREDECESSOR_H

#include "ev_buffer.h"

enum PACKET_FLAG
{
    REQ_LIST = 1, // 请求列表
    RESP_LIST = 2, // 回复列表

    REQ_FILEID = 3,
    RESP_FILEID = 4, // 请求,返回文件id

    REQ_DATA = 5,
    RESP_DATA = 6, // 请求,返回文件数据

    REQ_INIT = 7,
    RESP_INIT = 8 // 请求,返回初始化
};

#define DEF_LEN  (0x28) // 默认的整包长度
#define MAX_PATH_LEN    (256)

typedef struct _myStru
{ // 消息包
    unsigned int len; // 整包长度
    unsigned int nouse_0;
    unsigned int nouse_1;
    unsigned short nouse_2;
    unsigned short ver; // 永远是1
    unsigned int flag; // 12345678
    unsigned int exdata; // 扩展数据
    unsigned int b_or_e; // 开始还是结束
    unsigned int isfileid; // 0是正常的,1表示后面有个包是文件id

    unsigned int nouse_3;
    unsigned int nouse_4;
    unsigned int nextlen; // 后面接续的长度
    unsigned char * data; // 数据
} myStru;

typedef struct _myData
{ // 接收用
    unsigned int f_or_d; // 0x10--目录;0x20--文件
    unsigned int nouse[10]; // 没有用
    char path[MAX_PATH_LEN]; // 路径
    char sm_name[16]; // small name,简单名
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
    myStru * g_pRecvPacket; // 当前正在处理的消息包
    bool is_recving_data_;

    int data_len_;

};

#endif // PREDECESSOR_H
