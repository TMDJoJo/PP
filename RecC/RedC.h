#ifndef REDC_H
#define REDC_H

#include <QString>
#include <QThread>
#include <QTimer>
#include "ev_buffer.h"
#include "../Socket/ActiveSocket.h"
#include "Predecessor.h"

#define MAX_BUFFER_SIZE (1024*1024)

class RedC :public QThread
{
    Q_OBJECT
public:
    explicit RedC(QObject *parent = 0);

    int Init();
    int DownLoad(const QString ip,int port);
protected:
    virtual void run();
signals:
    void initalize(bool);
    void DataTransferProgress(qint64 done,qint64 total);
    void TransferError(const QString& err_str);
    void DownLoadFinish(const QString& err_str);

private slots:
    void Exit();
private:
    int Process();
    void ProcessLisg(myStru* pPacket);
    void ProcessFileId(myStru* pPacket);
    void ProcessFileData(myStru* pPacket);

    void ReSet();

private:
    Predecessor predecessor_;

    CActiveSocket socket_;
    bool is_shutdown_;
    QString host_ip_;
    int port_;

    EvBuffer* in_buffer_;
    EvBuffer* out_buffer_;

    QString file_name_;
    int file_total_len_;
    int file_done_len_;
    int file_id_;

    QTimer time_out_;

};

#endif // REDC_H
