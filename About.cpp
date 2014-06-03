#include "About.h"
#include "ui_About.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include "Configure.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("About PP"));
}

About::~About()
{
    delete ui;
}

void About::closeEvent(QCloseEvent* evt){
    this->hide();
    evt->ignore();
}

void About::on_lbe_copyright_linkActivated(const QString &)
{

		QString url = "weibo.com/tmdjojo";
    QDesktopServices::openUrl(QUrl(url));
}
