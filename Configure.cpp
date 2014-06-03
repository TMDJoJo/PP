#include "Configure.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QApplication>

Configure* g_configuer = NULL;
Configure::Configure()
{
    local_name_ = QProcessEnvironment::systemEnvironment().value("USERNAME");
    is_auto_start_ = true;
}

bool Configure::Init(){

    QSettings setting("./config.ini",QSettings::IniFormat);
    setting.beginGroup("MAIN");
    in_or_out_ = setting.value("i_or_o").toString();

    is_auto_start_ = setting.value("is_auto_start",true).toBool();
    SetAutoStart(is_auto_start_);

    auto_get_host_name_ = setting.value("auto_get_host_name",false).toBool();
    if(auto_get_host_name_){
        user_name_ = local_name_;
    }
    else{
        user_name_ = setting.value("user_name",local_name_).toString();
    }


    if(user_name_.isEmpty()){
        user_name_ = local_name_;
    }
    cbox_key_ = setting.value("cbox","").toString();
    setting.endGroup();

    setting.beginGroup("FTP");
    QStringList ftp_all_keys =setting.allKeys();
    QStringList::iterator ftp_it = ftp_all_keys.begin();
    while(ftp_it != ftp_all_keys.end()){

        FtpConfigure ftp;
        ftp.key_name_ = *ftp_it;
        ftp.value_ = setting.value(*ftp_it).toString();
        ftp_list_.push_back(ftp);
        if(ftp.key_name_ == cbox_key_){
            current_ftp_ = ftp;
        }
        ++ftp_it;
    }
    setting.endGroup();

    setting.beginGroup("RedC");
    QStringList red_c_all_keys =setting.allKeys();
    QStringList::iterator red_c_it = red_c_all_keys.begin();
    while(red_c_it != red_c_all_keys.end()){

        FtpConfigure red_c;
        red_c.key_name_ = *red_c_it;
        red_c.value_ = setting.value(*red_c_it).toString();
        red_c_list_.push_back(red_c);
        if(red_c.key_name_ == cbox_key_){
            current_red_c_ = red_c;
        }
        ++red_c_it;
    }
    setting.endGroup();

    desktop_ = QApplication::desktop();
    desk_rect_ = desktop_->availableGeometry();

    return true;
}

void Configure::SetAutoStart(bool is_auto_start){
    QString application_name = QApplication::applicationName();
    QSettings* settings = new QSettings(REG_RUN,QSettings::NativeFormat);
    Q_ASSERT(settings);
    is_auto_start_ = is_auto_start;
    if(is_auto_start){
        settings->setValue(application_name,QApplication::applicationFilePath());
    }
    else{
        settings->remove(application_name);
    }
}

void Configure::Save(){
    QSettings setting("./config.ini",QSettings::IniFormat);
    setting.beginGroup("MAIN");
    setting.setValue("i_or_o",in_or_out_);
    setting.setValue("is_auto_start",is_auto_start_);
    setting.setValue("user_name",user_name_);
    setting.setValue("auto_get_host_name",auto_get_host_name_);
    setting.setValue("cbox",cbox_key_);
    setting.endGroup();
}
