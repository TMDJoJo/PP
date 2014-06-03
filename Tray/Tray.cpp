#include "Tray.h"
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QImage>

#include "Configure.h"

#define MAX_DOUBLE_PRESS_TIME_MSEC  500

Tray::Tray(QObject *parent) :
    QSystemTrayIcon(parent),
    short_cut_(NULL)
{
    Init();
}

Tray::~Tray(){

    if(short_cut_)
        delete short_cut_,short_cut_ = NULL;
    if(tray_menu_)
        delete tray_menu_,tray_menu_ = NULL;
//    if(setting_)
//        delete setting_,setting_ = NULL;
    if(transfer_)
        delete transfer_,transfer_ = NULL;

}

void Tray::Init(){
    last_press_time_ = QTime::currentTime();

    setIcon(QIcon("Resource/tray_icon.png"));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    short_cut_ = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::Key_P),this);
    Q_ASSERT(short_cut_);
    connect(short_cut_,SIGNAL(activated()),
            this,SLOT(ShortCutActivated()));

    tray_menu_ = new TrayIconMenu;
    Q_ASSERT(tray_menu_);
    setting_ = new Setting(tray_menu_);
    Q_ASSERT(setting_);
    //setting_->Init();
    transfer_ = new Transfer();
    Q_ASSERT(transfer_);

    this->setContextMenu(tray_menu_);

    connect(tray_menu_, SIGNAL(Setting()),
            setting_, SLOT(show()));

}

void Tray::GoAhead(const QString& file_name){
    if(g_configuer->in_or_out_ == "in"){
        transfer_->DownLoad();
    }
    else if(g_configuer->in_or_out_ == "out"){
        transfer_->UpLoad(file_name);
    }
    else{}
}

void Tray::Upload(){
    QClipboard *board = QApplication::clipboard();
    const QMimeData* copy_data  = board->mimeData();

    QDir dir("./");
    if(!dir.exists(TEMP_DIR_NAME)){
        if(!dir.mkdir(TEMP_DIR_NAME)){
            return ;
        }
    }
    QString file_name;
    if(copy_data->hasUrls()){
        QUrl url = copy_data->urls().front();
        if(url.isLocalFile()){
            QFileInfo file_info(url.toLocalFile());
            if(file_info.isFile()&&file_info.size() <= MAX_FILE_SIZE){
                file_name = url.toLocalFile();
            }
            else{
                transfer_->MessagePop("biger than 11M...");
            }
        }
        else{
            file_name = TEMP_DIR_NAME + "/" + TEMP_TEXT_FILE_NAME;
            QFile file(file_name);
            file.open(QIODevice::WriteOnly);
            file.write(copy_data->text().toLocal8Bit());
            file.close();
        }
    }
    else if(copy_data->hasImage()){
        file_name = TEMP_DIR_NAME + "/" + TEMP_IMG_FILE_NAME;
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        QImage image = qvariant_cast<QImage>(copy_data->imageData());
        image.save(&file,"PNG");
        file.close();
    }
    else if(copy_data->hasHtml()){
        file_name = TEMP_DIR_NAME + "/" + TEMP_HTML_FILE_NAME;
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        file.write(copy_data->html().toLocal8Bit());
        file.close();
    }
    else if(copy_data->hasText()){
        file_name = TEMP_DIR_NAME + "/" + TEMP_TEXT_FILE_NAME;
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        file.write(copy_data->text().toLocal8Bit());
        file.close();
    }
    else{
//            QFile text_file("file_name");
//            text_file.open(QIODevice::WriteOnly);
//            text_file.write(copy_data->html().toLocal8Bit());
//            text_file.close();
    }

    if(!file_name.isEmpty()){
        transfer_->UpLoad(file_name);
    }
}

void Tray::Download(){
    transfer_->DownLoad();
}

void Tray::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        emit Show();
    default:
        ;
    }
}

void Tray::ShortCutActivated(){

    if(last_press_time_.msecsTo(QTime::currentTime()) < MAX_DOUBLE_PRESS_TIME_MSEC){
        if(g_configuer->in_or_out_ == "in"){
            Download();
        }
        else if(g_configuer->in_or_out_ == "out"){
            Upload();
        }
        else{

        }
    }
    else{
        last_press_time_ = QTime::currentTime();
    }
}


