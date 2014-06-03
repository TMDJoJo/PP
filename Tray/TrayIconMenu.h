#ifndef TRAYICONMENU_H
#define TRAYICONMENU_H

#include <QMenu>
#include "Setting.h"

class TrayIconMenu : public QMenu
{
    Q_OBJECT
public:
    explicit TrayIconMenu(QWidget *parent = 0);
    
private:
    void Init();

signals:
    void Setting();
public slots:
    void OnSetting();
    void OnAbout();
private:
};

#endif // TRAYICONMENU_H
