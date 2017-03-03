#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    fileTransferClient(new FileTransferClient(this))
{
    ui->setupUi(this);

    ui->remoteIpAddressLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_REMOTE_IP_ADDRESS));
    ui->portLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_PORT));
    ui->receivingDirLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_RECEIVE_DIR));
    ui->receiveButton->setEnabled(false);
}

Dialog::~Dialog()
{
    delete ui;
    delete fileTransferClient;
}

void Dialog::on_connectButton_clicked()
{
    std::string remoteIpAddress = ui->remoteIpAddressLineEdit->text().toStdString();
    int port = std::stoi(ui->portLineEdit->text().toStdString());

    ui->connectButton->setEnabled(false);
    fileTransferClient->connect(remoteIpAddress, port);
}

void Dialog::setStatus(const string& status)
{
    ui->statusLabel->setText(QString::fromStdString(status));
}

void Dialog::setFileList(const vector<FileInfo>& fileList)
{
    model = new QStandardItemModel(fileList.size(), 1, this);
    ui->listView->setModel(model);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (unsigned int row = 0; row < fileList.size(); ++row ) {
        QModelIndex index = model->index(row, 0);
        model->setData(index, QString::fromStdString(fileList[row].getName()));
    }
}

void Dialog::setProgress(const long long soFar, const long long size)
{
    int progress = soFar / (float)size * 100;
    ui->receivedBytes->setText(
        QString::fromStdString(to_string(soFar) + " B / " + to_string(size) + " B"));
    ui->progressBar->valueChanged(progress);
}

void Dialog::on_receiveButton_clicked()
{
    QString buttonText = ui->receiveButton->text();
    if (buttonText == "Close") {
        QCoreApplication::exit();
    } else {
        ui->receiveButton->setEnabled(false);
        std::string receiveDir = ui->receivingDirLineEdit->text().toStdString();
        fileTransferClient->receiveFiles(receiveDir);
    }
}
