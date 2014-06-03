#include "Setting.h"
#include "ui_Setting.h"
#include <QDebug>
#include <QCloseEvent>
#include <QProcess>
#include <QProcessEnvironment>

#include "Configure.h"

Setting::Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("PPÉèÖÃ"));
    ui->tab_widget->setTabEnabled(1,false);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::Init(){

    ui->auto_get_host_name->setChecked(g_configuer->auto_get_host_name_);
    ui->ledt_user_name->setText(g_configuer->user_name_);

    if(g_configuer->in_or_out_ == "in"){
        ui->rbtn_in->setChecked(true);
        ui->rbtn_out->setChecked(false);

        on_rbtn_in_clicked();
    }
    else if(g_configuer->in_or_out_ == "out"){
        ui->rbtn_in->setChecked(false);
        ui->rbtn_out->setChecked(true);

        on_rbtn_out_clicked();
    }
    else{
        if(g_configuer->user_name_ == "Administration"){
            ui->rbtn_in->setChecked(true);
            ui->rbtn_out->setChecked(false);
            g_configuer->in_or_out_ = "in";

            on_rbtn_in_clicked();
        }
        else{
            ui->rbtn_in->setChecked(false);
            ui->rbtn_out->setChecked(true);
            g_configuer->in_or_out_ = "out";

            on_rbtn_out_clicked();
        }
    }

    if(!g_configuer->is_auto_start_){
        ui->cbox_is_auto_start->setChecked(false);
    }
    else{
        ui->cbox_is_auto_start->setChecked(true);
        g_configuer->is_auto_start_ = true;
    }
}

void Setting::closeEvent(QCloseEvent* evt){
    this->hide();
    evt->ignore();
}

void Setting::showEvent(QShowEvent *evt){
    Init();
    QDialog::showEvent(evt);
}

void Setting::on_pbtn_accept_clicked()
{
    ////save configure
    g_configuer->user_name_ = ui->ledt_user_name->text();

    if(ui->auto_get_host_name->isChecked()){
        g_configuer->auto_get_host_name_ = true;
    }
    else{
        g_configuer->auto_get_host_name_ = false;
    }

    if(ui->rbtn_in->isChecked()){
        g_configuer->in_or_out_ = "in";
        g_configuer->current_red_c_.key_name_ = ui->cbox_ftps->currentText();
        g_configuer->current_red_c_.value_ = ui->cbox_ftps->itemData(
                    ui->cbox_ftps->currentIndex()).toString();
    }
    else{
        g_configuer->in_or_out_ = "out";
        g_configuer->current_ftp_.key_name_ = ui->cbox_ftps->currentText();
        g_configuer->current_ftp_.value_ = ui->cbox_ftps->itemData(
                    ui->cbox_ftps->currentIndex()).toString();
    }
    if(ui->cbox_is_auto_start->isChecked()){
        g_configuer->SetAutoStart(true);
    }
    else{
        g_configuer->SetAutoStart(false);
    }
    g_configuer->cbox_key_ = ui->cbox_ftps->currentText();



    g_configuer->Save();

    this->hide();
}

void Setting::on_pbtn_reject_clicked()
{
    this->hide();
}



void Setting::on_rbtn_in_clicked()
{
    ui->auto_get_host_name->setEnabled(false);
    ui->lbe_cbox->setText(tr("RedC£º"));
    ui->cbox_ftps->clear();
    QList<FtpConfigure>::iterator it = g_configuer->red_c_list_.begin();
    while(it != g_configuer->red_c_list_.end()){
        ui->cbox_ftps->addItem(it->key_name_,it->value_);
        ++it;
    }
    ui->cbox_ftps->setCurrentIndex(
                ui->cbox_ftps->findText(g_configuer->cbox_key_));
}

void Setting::on_rbtn_out_clicked()
{
    ui->auto_get_host_name->setEnabled(true);
    ui->lbe_cbox->setText(tr("FTP £º"));
    ui->cbox_ftps->clear();
    QList<FtpConfigure>::iterator it = g_configuer->ftp_list_.begin();
    while(it != g_configuer->ftp_list_.end()){
        ui->cbox_ftps->addItem(it->key_name_,it->value_);
        ++it;
    }
    ui->cbox_ftps->setCurrentIndex(
                ui->cbox_ftps->findText(g_configuer->cbox_key_));
}
