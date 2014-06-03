#ifndef TRAY_H
#define TRAY_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <qDebug>
#include <QTime>
#include "qxtglobalshortcut.h"
#include "Tray/TrayIconMenu.h"
#include "Setting.h"
#include "Transfer.h"

const QString TEMP_DIR_NAME = "temp";
const QString TEMP_TEXT_FILE_NAME = "-_-!!!.txt";
const QString TEMP_HTML_FILE_NAME = "-_-!!!.html";
const QString TEMP_IMG_FILE_NAME = "-_-!!!.png";

#define MAX_FILE_SIZE   (11*1024*1024)

class Tray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit Tray(QObject *parent = 0);
    ~Tray();

private:
    void Init();
    void GoAhead(const QString& file_name);
    void Upload();
    void Download();
private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void ShortCutActivated();

signals:
    void Show();
public slots:

private:
    QxtGlobalShortcut* short_cut_;
    QTime last_press_time_;

    TrayIconMenu* tray_menu_;
    Setting* setting_;

    Transfer* transfer_;

};

#endif // TRAY_H
