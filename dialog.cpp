#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    fileTransferClient()
{
    ui->setupUi(this);
    ui->remoteIpAddressLineEdit->setText(
                QString::fromStdString(fileTransferClient.DEFAULT_REMOTE_IP_ADDRESS));
    ui->portLineEdit->setText(
                QString::fromStdString(std::to_string(fileTransferClient.DEFAULT_PORT)));
    ui->receivingDirLineEdit->setText(
                QString::fromStdString(fileTransferClient.DEFAULT_RECEIVING_DIR));
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_connectButton_clicked()
{
    std::string remoteIpAddress = ui->remoteIpAddressLineEdit->text().toStdString();
    int port = std::stoi(ui->portLineEdit->text().toStdString());
    fileTransferClient.connect(remoteIpAddress, port);
    fileTransferClient.receiveFileList();
}

void Dialog::on_receiveButton_clicked()
{
    std::string receivingDir = ui->receivingDirLineEdit->text().toStdString();
    fileTransferClient.setReceivingDir(receivingDir);
    fileTransferClient.receiveFiles();
}
