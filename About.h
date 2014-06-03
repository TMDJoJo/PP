#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT
    
public:
    explicit About(QWidget *parent = 0);
    ~About();
protected:
    void closeEvent(QCloseEvent *);
private slots:

    void on_lbe_copyright_linkActivated(const QString &link);

private:
    Ui::About *ui;
};

#endif // ABOUT_H
