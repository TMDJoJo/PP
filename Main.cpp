#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QList>

#include "Tray/Tray.h"
#include "Configure.h"

#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

    g_configuer =  new Configure;
    g_configuer->Init();

    QFile file("Resource/QSS/Theme.css");
    if(file.open(QIODevice::ReadOnly)){
        app.setStyleSheet(file.readAll());
    }

    file.close();

    Tray* tray = new Tray(&app);

    tray->setToolTip("PP");
    tray->show();
    //tray->showMessage("ii","start!",QSystemTrayIcon::NoIcon,1000);

    int re = app.exec();

    delete tray;
    tray = NULL;

    return re;
}
