#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDialog>
#include <QFtp>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include "RecC/RedC.h"

const QString HOST_NAME = "PP";
namespace Ui {
class Transfer;
}

class Transfer : public QDialog
{
    enum FtpStep{
        IN_ROOT   = 0,          //in ftp /
        IN_HOST         ,       //in ftp /PP
        IN_USER_NAME            //in ftp /PP/username
    };
    Q_OBJECT
    
public:
    explicit Transfer(QWidget *parent = 0);
    ~Transfer();
    void UpLoad(const QString& file_name);
    void DownLoad();
    void MessagePop(const QString& msg);

protected:
    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);

private slots:
    void OnFtpCommandFinished(int id,bool error);
    void OnListInfo(const QUrlInfo& i);
    void OnDataTransferProgress(qint64 done,qint64 total);

    void OnDownLoadError(const QString&);
    void OnDownLoadProgress(qint64 done,qint64 total);
    void OnDownLoadFinish(const QString&);
private:


    void OnListFinish(bool error);
    void OnCdFinish(bool error);
    void OnMkdirFinish(bool error);
    void OnRmFileFinish(bool error);

    void PutFile();
    void ReSet();
private:
    Ui::Transfer *ui;

    bool is_transfering_;

    QFtp ftp_;

    QString upload_file_;
    QString upload_file_name_;
    bool have_host_;
    bool have_user_dir_;
    bool have_no_file_;
    int list_step_;

    QFile file_;
    QTimer hide_time_;
    //QPropertyAnimation* animation_;

    RedC* redc_;
};

#endif // TRANSFER_H
