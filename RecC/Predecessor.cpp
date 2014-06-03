#include "Predecessor.h"

#include <string>
#include <stdlib.h>
#include <QFile>
#include <QDebug>

Predecessor::Predecessor()
{
    g_pRecvPacket = NULL;
}

myStru* Predecessor::makeInitPacket(){
    myStru * prt = (myStru*)malloc(sizeof(myStru));
    memset( prt, 0, sizeof(myStru) );
    prt->len = DEF_LEN;
    prt->ver = 1;
    prt->flag = REQ_INIT;
    prt->exdata = 0x67;
    return prt;
}


myStru* Predecessor::makeListPacket(const char* path){
    if (path == NULL)
        return NULL;
    int pathlen = strlen(path);
    myStru* prt = (myStru*)malloc(sizeof(myStru));
    memset(prt, 0, sizeof(myStru) );
    prt->len = (unsigned int)(DEF_LEN + pathlen);
    prt->ver = 1;
    prt->flag = REQ_LIST;
    prt->nextlen = pathlen;
    if (pathlen > 0)
    {
        prt->data = (unsigned char *)malloc(pathlen);
        memcpy( prt->data, path, pathlen );
    }
    return prt;
}

myStru* Predecessor::makeFileIdPacket(const char * path){
    if(path == NULL)
        return NULL;
    int pathlen = strlen(path);
    myStru * prt = (myStru *)malloc(sizeof(myStru));
    memset( prt, 0, sizeof(myStru) );
    prt->len = (unsigned int)(DEF_LEN + pathlen);
    prt->ver = 1;
    prt->flag = REQ_FILEID;
    prt->exdata = 1;
    prt->nextlen = pathlen;
    if(pathlen > 0){
        prt->data = (unsigned char *)malloc(pathlen);
        memcpy(prt->data, path, pathlen);
    }
    return prt;
}

myStru* Predecessor::makeDataPacket(unsigned int fileid){
    myStru* prt = (myStru *)malloc(sizeof(myStru));
    memset(prt, 0, sizeof(myStru));
    prt->len = DEF_LEN;
    prt->ver = 1;
    prt->flag = REQ_DATA;
    prt->b_or_e = fileid;
    return prt;
}

myStru* Predecessor::makeRecvPacket(unsigned int packet_len){
    if(packet_len < DEF_LEN)
       return NULL; //
    myStru * prt = (myStru *)malloc(sizeof(myStru));
    memset(prt, 0, sizeof(myStru));
    return prt;
}

void Predecessor::clearRecvPacket(myStru* pPacket){
    if (pPacket == NULL)
       return;
    if (pPacket->data)
    free(pPacket->data);
    memset(pPacket, 0, sizeof(myStru));
}

void Predecessor::freeSendPacket(myStru* p){
    if(p){
        if(p->data){
            delete p->data;
            p->data = NULL;
        }
        delete p;
        p = NULL;
    }
}

int Predecessor::writePacket(myStru * pPacket,EvBuffer* out){
    if(NULL == pPacket
             ||NULL == out){
         return -1;
     }
     int needlen = pPacket->len + 4 + pPacket->nextlen;
     if(out->buffer_free_size() < needlen){
         return -1;
     }
     out->Write((const char*)pPacket,DEF_LEN+4);
     if(pPacket->nextlen > 0 && pPacket->data != NULL){
         out->Write((const char*)pPacket->data,pPacket->nextlen);
     }
     return 0;
}

myStru* Predecessor::processRecvBuff(EvBuffer* in_buf){
    myStru * return_packet = NULL;
    if(in_buf == NULL){
        return return_packet;
    }

    if ( g_pRecvPacket == NULL )
    {
        g_pRecvPacket = makeRecvPacket( DEF_LEN );
    }
    do{
        if ( in_buf->buffer_size() <= 0 )
            break;
        int all_packet_len = 0;
        if(0 != in_buf->Peek((char *)(&all_packet_len),4)){

        }
        else{
            if(all_packet_len <= in_buf->buffer_size()){
                in_buf->Read((char *)g_pRecvPacket,DEF_LEN + 4);
                unsigned char* data = new unsigned char[g_pRecvPacket->nextlen + 1];
                memset(data,0,g_pRecvPacket->nextlen + 1);
                in_buf->Read((char*)data,g_pRecvPacket->nextlen);
                g_pRecvPacket->data = data;
                return_packet = g_pRecvPacket;
            }
        }
    }while(0);
    return return_packet;
}

void Predecessor::ReceiveData(int len){
    data_len_ = len;
    is_recving_data_ = true;
}
