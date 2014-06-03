#ifndef CONFIGURE_H
#define CONFIGURE_H
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QRect>
#include <QDesktopWidget>
#include <QDebug>

#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

struct FtpConfigure{
    QString key_name_;
    QString value_;
};

class Configure
{
public:
    Configure();
    bool Init();

    void SetAutoStart(bool );
    void Save();
public:
    QString local_name_;
    QString in_or_out_;
    bool is_auto_start_;
    QString user_name_;
    bool auto_get_host_name_;
    QString cbox_key_;

    FtpConfigure current_ftp_;
    QList<FtpConfigure> ftp_list_;

    FtpConfigure current_red_c_;
    QList<FtpConfigure> red_c_list_;

    QDesktopWidget* desktop_;
    QRect desk_rect_;
};

extern Configure* g_configuer;

#endif // CONFIGURE_H
