#include "RedC.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include "Configure.h"
#include "Tray/Tray.h"

const QString IN_HOST_PATH = "Users\\PP\\";

RedC::RedC(QObject *parent):
    QThread(parent)
{
    is_shutdown_ = true;
    port_ = 0;

    in_buffer_ = NULL;
    out_buffer_ = NULL;

    file_total_len_ = 0;
    file_id_ = 0;
}

int RedC::Init(){
    in_buffer_ = new EvBuffer(MAX_BUFFER_SIZE);
    out_buffer_ = new EvBuffer(MAX_BUFFER_SIZE);

    connect(&time_out_,SIGNAL(timeout()),
            this,SLOT(Exit()));

    return 0;
}

int RedC::DownLoad(const QString ip,int port){
    if(!this->isRunning()){
        if(!socket_.Initialize()){
            emit TransferError("initialize fail...");
            return -1;
        }
        if(!socket_.SetNonblocking()){
            emit TransferError("set nonblocking fail...");
            return -1;
        }
        emit TransferError("initialize ok...");
        host_ip_ = ip;
        port_ = port;
        is_shutdown_ = false;
        start();
        return 0;
    }
    emit TransferError("downloading...");
    return -1;
}

void RedC::run(){
    time_out_.start(2000);

    if(!socket_.Open((const unsigned char*)host_ip_.toStdString().c_str(),
                    port_)){
        emit TransferError("connect fail...");
        socket_.Close();
        time_out_.stop();
        return ;
    }
    emit TransferError("connect ok...");
    myStru * pInit = predecessor_.makeInitPacket();
    predecessor_.writePacket(pInit,out_buffer_);
    predecessor_.freeSendPacket( pInit );

    while(!is_shutdown_){
        if(!socket_.Select(1,0)){
            continue;
        }
        if (socket_.IsReadReady()){
            if(in_buffer_->buffer_free_size() >0 ){
                int len = socket_.Receive( in_buffer_->buffer_free_size() );
                if(len < 0
                        &&socket_.GetSocketError() == CSimpleSocket::SocketEwouldblock){

                }
                else if(len == 0){
                    is_shutdown_ = true;
                    break;
                }
                else if(len > 0){
                    in_buffer_->Write((char*)socket_.GetData(),len);
                }
                else{
                    is_shutdown_ = true;
                    break;
                }
            }
        }
        if(socket_.IsWriteReady()){
            int buf_size = 0;
            while( (buf_size = out_buffer_->buffer_size()) >0 ){
                char buf[2048] = {0};
                int peek_size = buf_size > sizeof(buf)?sizeof(buf):buf_size;
                if(0 != out_buffer_->Peek(buf,peek_size)){
                    is_shutdown_ = true;
                    break;
                }
                int send_len = socket_.Send((const unsigned char*)buf,peek_size);
                if(out_buffer_->Read(buf,send_len) != 0){
                    is_shutdown_ = true;
                    break;
                }
            }////end of while(...
        }
        Process();
    }////endof while(!is_shutdown_){

    ReSet();
}

void RedC::Exit(){
    ReSet();
    quit();
}

int RedC::Process(){
    myStru * pPacket = NULL;
    while( (pPacket = predecessor_.processRecvBuff(in_buffer_)) != NULL ){
        time_out_.start(2000);
        if ( pPacket->flag == RESP_INIT ){
            qDebug()<<"send list msg";
            QString user_host_path = IN_HOST_PATH + g_configuer->user_name_ + "\\";
            myStru * pList = predecessor_.makeListPacket(user_host_path.toStdString().c_str());
            predecessor_.writePacket(pList,out_buffer_);
            predecessor_.freeSendPacket(pList);
        }
        else if(pPacket->flag == RESP_LIST){
            ProcessLisg(pPacket);
        }
        else if ( pPacket->flag == RESP_FILEID ){
            ProcessFileId(pPacket);
        }
        else if ( pPacket->flag == RESP_DATA ){
            ProcessFileData(pPacket);
        }
        else{
            is_shutdown_ = true;
        }
        predecessor_.clearRecvPacket(pPacket);

    }
    return 0;
}
void RedC::ProcessLisg(myStru* pPacket){
    if(NULL == pPacket){
        is_shutdown_ = true;
        return;
    }
    if(pPacket->b_or_e == 1){
        //开始
    }
    else if(pPacket->b_or_e == 2){
        //结束
        if(file_name_.isEmpty()){
            emit TransferError("no file...");
            is_shutdown_ = true;
        }
        else{

            emit TransferError("get " + file_name_ + "...");
            QString file_path = QString(IN_HOST_PATH + g_configuer->user_name_) + "\\" + file_name_;
            myStru * pFileId = predecessor_.makeFileIdPacket(file_path.toStdString().c_str());
            predecessor_.writePacket(pFileId,out_buffer_);
            predecessor_.freeSendPacket(pFileId);
        }
    }
    else if(pPacket->b_or_e == 0){
        //正常
        myData* pData = (myData *)(pPacket->data);
        if(pData){
            if( pData->f_or_d == 0x10 ){
                //目录

            }
            else if( pData->f_or_d == 0x20 ){
                //文件
                file_name_ = pData->path;
            }
            else{
                //其他未知情况
            }
        }
    }
    else{
        is_shutdown_ = true;
    }
}

void RedC::ProcessFileId(myStru* pPacket){
    if ( pPacket->isfileid == 1 )
    { // 有文件id
        if ( pPacket->data != NULL )
        {
        }
    }
    else if ( pPacket->b_or_e > 0 )
    {
        if ( pPacket->data != NULL )
        {
            file_id_ = pPacket->b_or_e;

            myData * pdata = (myData *)pPacket->data;
            file_total_len_ = pdata->nouse[7];
            file_done_len_ = 0;

            QFile file(TEMP_DIR_NAME + "/" +file_name_);
            if(file.exists()){
                file.remove();
            }

            myStru * pReqData = predecessor_.makeDataPacket(pPacket->b_or_e);
            predecessor_.writePacket(pReqData,out_buffer_);
            predecessor_.freeSendPacket(pReqData);

        }
    }
}

void RedC::ProcessFileData(myStru* pPacket){
    file_done_len_ += pPacket->nextlen;
    QDir dir("./");
    if(!dir.exists(TEMP_DIR_NAME)){
        if(!dir.mkdir(TEMP_DIR_NAME)){
            is_shutdown_ = true;
            return ;
        }
    }
    QFile file(TEMP_DIR_NAME + "/" +file_name_);
    file.open(QIODevice::Append);
    file.write((char*)pPacket->data,pPacket->nextlen);
    emit DataTransferProgress(file_done_len_,file_total_len_);
    file.close();
    if ( file_total_len_ <= file_done_len_ ){
        emit TransferError("get ok...");
        emit DownLoadFinish(TEMP_DIR_NAME + "/" +file_name_);
        is_shutdown_ = true;
    }
    else{
        //继续请求数据
        myStru * pReqData = predecessor_.makeDataPacket(file_id_);
        predecessor_.writePacket(pReqData,out_buffer_);
        predecessor_.freeSendPacket(pReqData);
    }
}

void RedC::ReSet(){
    socket_.Close();
    is_shutdown_ = true;
    host_ip_ = "";
    port_ = 0;

    if(in_buffer_){
        in_buffer_->Clean();
    }
    if(out_buffer_){
        out_buffer_->Clean();
    }
    time_out_.stop();
    file_name_ = "";
    file_total_len_ = 0;
    file_done_len_ = 0;
    file_id_ = 0;
}
