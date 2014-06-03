#include "TrayIconMenu.h"
#include <QAction>
#include <QApplication>
#include "About.h"

TrayIconMenu::TrayIconMenu(QWidget *parent) :
    QMenu(parent)
{
    Init();
}

void TrayIconMenu::Init(){

    QAction* open_action = new QAction(tr("PP"),this);
    this->addAction(open_action);
    open_action->setEnabled(false);
    QAction* login_about = new QAction(tr("关于"),this);
    this->addAction(login_about);
    connect(login_about,SIGNAL(triggered()),this,SLOT(OnAbout()));
//    login_action->setEnabled(false);
    QAction* set_action = new QAction(tr("设置"),this);
    this->addAction(set_action);
//    set_action->setEnabled(false);
    connect(set_action,SIGNAL(triggered()),this,SLOT(OnSetting()));

    this->addSeparator();

    QAction* quit_Action = new QAction(tr("退出"),this);
    this->addAction(quit_Action);
    connect(quit_Action,SIGNAL(triggered()),qApp,SLOT(quit()));
}

void TrayIconMenu::OnSetting(){
    emit Setting();
}

void TrayIconMenu::OnAbout(){
    About about;
    about.exec();
}
