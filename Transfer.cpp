#include "Transfer.h"
#include "ui_Transfer.h"

#include <QDebug>
#include <QString>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QClipboard>
#include <QDir>
#include <QUrl>
#include <QFileDialog>

#include "Configure.h"
#include "Tray/Tray.h"

#define VELOCITY    0.3   ////0.3pix/msec
#define MESSAGE_SHOW_TIME    3000 ////msec

Transfer::Transfer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Transfer)
{
    ui->setupUi(this);

    connect(&ftp_,SIGNAL(commandFinished(int,bool)),
            this,SLOT(OnFtpCommandFinished(int,bool)));
    connect(&ftp_,SIGNAL(listInfo(const QUrlInfo&)),
            this,SLOT(OnListInfo(const QUrlInfo&)));

    connect(&ftp_,SIGNAL(dataTransferProgress(qint64,qint64)),
            this,SLOT(OnDataTransferProgress(qint64,qint64)));

    setWindowTitle(tr("PP´«ÊäÖÐ"));
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
//    this->setAutoFillBackground(false);

//    animation_ = new QPropertyAnimation(this,"pos");
//    connect(animation_,SIGNAL(finished()),
//            this,SLOT(OnAnimationFinished()));
    connect(&hide_time_,SIGNAL(timeout()),
            this,SLOT(hide()));
    this->setObjectName("sld_rate");

    ReSet();

    redc_ = new RedC;
    redc_->Init();

    connect(redc_,SIGNAL(TransferError(const QString&)),
            this,SLOT(OnDownLoadError(const QString&)));
    connect(redc_,SIGNAL(DataTransferProgress(qint64,qint64)),
            this,SLOT(OnDownLoadProgress(qint64,qint64)));
    connect(redc_,SIGNAL(DownLoadFinish(const QString&)),
            this,SLOT(OnDownLoadFinish(const QString&)));

}

Transfer::~Transfer()
{
    delete ui;
}

void Transfer::UpLoad(const QString& file_name){
    ReSet();

    if(is_transfering_){
        return;
    }

    MessagePop("start upload!");

    upload_file_ = file_name;
    upload_file_name_ = QFileInfo(upload_file_).fileName();
    QStringList split_at = g_configuer->current_ftp_.value_.split("@");
    if(split_at.size() != 2){
        return;
    }
    QStringList host_port = split_at[0].split(":");
    if(host_port.size() != 2){
        return;
    }
    QStringList user_passwd = split_at[1].split(":");
    if(user_passwd.size() != 2){
        return;
    }

    qDebug()<<host_port[0]<<host_port[1].toInt();

    ftp_.connectToHost(host_port[0],host_port[1].toInt());
    ftp_.login(user_passwd[0],user_passwd[1]);
    list_step_ = IN_ROOT;
    ftp_.list();
}

void Transfer::DownLoad(){

    if(is_transfering_){
        return;
    }
    QStringList value_list = g_configuer->current_red_c_.value_.split(":");
    if(value_list.size() == 2){
        redc_->DownLoad(value_list[0],value_list[1].toInt());
    }
    else{
        MessagePop("rec c fail");
    }
}

void Transfer::MessagePop(const QString& msg){

    if(this->isHidden()){
        this->show();
        this->move(
                    QPoint(g_configuer->desk_rect_.width() - width()-2,
                           g_configuer->desk_rect_.height() - height() - 2)
                    );
        ui->lbe_info->setText(msg);
        hide_time_.start(MESSAGE_SHOW_TIME);
    }
    else{
        hide_time_.stop();
        hide_time_.start(MESSAGE_SHOW_TIME);
        ui->lbe_info->setText(msg);
    }
}

void Transfer::showEvent(QShowEvent* evt){
    evt->ignore();
    QDesktopWidget *desktop = QApplication::desktop();
    QRect desk_rect = desktop->availableGeometry();

    move(desk_rect.width() - rect().width() - 2,
         desk_rect.height());
    //QDialog::showEvent(evt);
}

void Transfer::closeEvent(QCloseEvent* evt){
    this->hide();
    evt->ignore();
}

void Transfer::OnFtpCommandFinished(int,bool error){

    switch(ftp_.currentCommand()){
    case QFtp::ConnectToHost:{
        if(error){
            MessagePop("connect fail...");
        }
        else{
            MessagePop("connect ok...");
        }
    }break;
    case QFtp::Login:{
        if(error){
            MessagePop("login fail...");
        }
        else{
            MessagePop("login ok...");
        }
    }break;
    case QFtp::Close:{
        if(error){
            MessagePop("close fail...");
        }
    }break;
    case QFtp::List:{
        if(error){
            MessagePop("list fail...");
        }
        else{
            MessagePop("list ok...");
        }
        OnListFinish(error);
    }break;
    case QFtp::Cd:{
        if(error){
            MessagePop("cd fail...");
        }
        else{
            MessagePop("cd ok...");
        }
        OnCdFinish(error);
    }break;
    case QFtp::Put:
        if(error){
            MessagePop("put fail...");
        }
        else{
            MessagePop("put ok...");
        }
        ReSet();
        break;
    case QFtp::Remove:
        if(error){
            MessagePop("rm fail...");
        }
        else{
            MessagePop("rm ok...");
        }
        OnRmFileFinish(error);
        break;
    case QFtp::Mkdir:{
        if(error){
            MessagePop("mkdir fail...");
        }
        else{
            MessagePop("mkdir ok...");
        }
        OnMkdirFinish(error);
    } break;
    default:
        if(error){
            MessagePop(QString("op[%d] fail...").arg(ftp_.currentCommand()));
        }

        break;
    }
}

void Transfer::OnListInfo(const QUrlInfo& info){

    switch(list_step_){
    case IN_ROOT:{
        if(info.isDir()
                &&info.name() == HOST_NAME){
            have_host_ = true;
        }
    }break;
    case IN_HOST:{
        if(info.isDir()
                &&info.name() == g_configuer->user_name_){
            have_user_dir_ = true;
        }
    }break;
    case IN_USER_NAME:{

        if(info.isFile()){
            have_no_file_ = false;
            ftp_.remove(info.name());
        }
    }break;
    default:
        break;
    }
}

void Transfer::OnDataTransferProgress(qint64 done,qint64 total){

    double per = (double)done*100/(double)total;
    MessagePop(QString::number(per,'f',2)+"%");
    ui->sld_rate->setMaximum(total);
    ui->sld_rate->setValue(done);
}

void Transfer::OnDownLoadError(const QString& err_str){
    MessagePop(err_str);
}

void Transfer::OnDownLoadProgress(qint64 done,qint64 total){
    double per = (double)done*100/(double)total;
    MessagePop(QString::number(per,'f',2)+"%");
    ui->sld_rate->setMaximum(total);
    ui->sld_rate->setValue(done);
}

void Transfer::OnDownLoadFinish(const QString& file_path){
    QClipboard *board = QApplication::clipboard();
    QDir dir("./");
    if(!dir.exists(TEMP_DIR_NAME)){
        if(!dir.mkdir(TEMP_DIR_NAME)){
            return ;
        }
    }
    if(QFile::exists(file_path)){
        QFileInfo file_info(file_path);
        QString file_name = file_info.fileName();

        if(file_name == TEMP_TEXT_FILE_NAME){
            QFile file(file_path);
            if(file.open(QIODevice::ReadOnly)){
                board->setText(file.readAll());
            }
            file.close();
        }
        else if(file_name == TEMP_HTML_FILE_NAME){
            QString save_path = QFileDialog::getSaveFileName(
                        NULL,
                        QObject::tr("Save Html"),
                        file_name,
                        QObject::tr("*.html"));
            if(!save_path.isEmpty()){
                QFile file(save_path);
                if(file.exists()){
                    file.remove();
                }
                file.setFileName(file_path);
                if(file.open(QIODevice::ReadOnly)){
                    file.copy(save_path);
                }
                file.close();
            }
        }
        else if(file_name == TEMP_IMG_FILE_NAME){
            QImage image(file_path);
            board->setImage(image);
        }
        else{
            QString save_path = QFileDialog::getSaveFileName(
                        NULL,
                        QObject::tr("Save File"),
                        file_name,
                        QObject::tr("*.*"));
            if(!save_path.isEmpty()){
                QFile file(save_path);
                if(file.exists()){
                    file.remove();
                }
                file.setFileName(file_path);
                if(file.open(QIODevice::ReadOnly)){
                    file.copy(save_path);
                }
                file.close();
            }
        }
    }
    is_transfering_ = false;
}

void Transfer::OnListFinish(bool error){
    if(error){
        ReSet();
        return ;
    }

    switch(list_step_){
    case IN_ROOT:{
        if(have_host_){
            ftp_.cd(HOST_NAME);
        }
        else{
            ftp_.mkdir(HOST_NAME);
        }
    }break;
    case IN_HOST:{
        if(have_user_dir_){
            ftp_.cd(g_configuer->user_name_);
        }
        else{
            ftp_.mkdir(g_configuer->user_name_);
        }
    }break;
    case IN_USER_NAME:{
        if(have_no_file_){
            PutFile();
        }
        else{
            have_no_file_ = true;
            ftp_.list();
        }

    }break;
    default:
        break;
    }
}

void Transfer::OnCdFinish(bool error){

    if(!error){
        ftp_.list();
        list_step_++;
    }
    else{
        ReSet();
    }
}

void Transfer::OnMkdirFinish(bool error){
    if(error){
        ReSet();
    }
    switch(list_step_){
    case IN_ROOT:{
        ftp_.cd(HOST_NAME);
    }break;
    case IN_HOST:{
        ftp_.cd(g_configuer->user_name_);
    }break;
    default:
        ReSet();
        break;
    }
}

void Transfer::OnRmFileFinish(bool error){
    if(error){
        ReSet();
    }
//    if(have_no_file_)
//        PutFile();
//    else
//        ftp_.list();
}

void Transfer::PutFile(){

    file_.setFileName(upload_file_);
    if(file_.open(QIODevice::ReadOnly)){
        ftp_.put(&file_,upload_file_name_.toUtf8());
    }
}

void Transfer::ReSet(){
    if(ftp_.state() == QFtp::Connecting
            ||ftp_.state() == QFtp::Connected
            ||ftp_.state() == QFtp::LoggedIn){
        ftp_.close();
    }
    if(file_.isOpen()){
        file_.close();
    }
    have_host_ = false;
    have_user_dir_ = false;
    have_no_file_ = false;
    list_step_ = IN_ROOT;
    have_no_file_ = true;

    is_transfering_ = false;

}
