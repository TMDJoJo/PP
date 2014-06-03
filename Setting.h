#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT
    
public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();
    void Init();
protected:
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);
private slots:
    void on_pbtn_accept_clicked();

    void on_pbtn_reject_clicked();

    void on_rbtn_in_clicked();

    void on_rbtn_out_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
