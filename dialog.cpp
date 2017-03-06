#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    fileTransferClient(new FileTransferClient(this))
{
    ui->setupUi(this);
    qRegisterMetaType<string>("string");
    setStatus(FileTransferClient::NO_CONNECTION);
    connect(fileTransferClient, SIGNAL(changeProgress(const long long, const long long)),
            this, SLOT(setProgress(const long long, const long long)));
    connect(fileTransferClient, SIGNAL(changeStatus(const string&)),
            this, SLOT(setStatus(const string&)));

    ui->remoteIpAddressLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_REMOTE_IP_ADDRESS));
    ui->portLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_PORT));
    ui->receivingDirLineEdit->setText(
        QString::fromStdString(FileTransferClient::DEFAULT_RECEIVE_DIR));
}

Dialog::~Dialog()
{
    delete ui;
    delete fileTransferClient;
}

void Dialog::on_connectButton_clicked()
{
    string remoteIpAddress = ui->remoteIpAddressLineEdit->text().toStdString();
    int port = std::stoi(ui->portLineEdit->text().toStdString());

    fileTransferClient->connect(remoteIpAddress, port);
}

void Dialog::setStatus(const string& status)
{
    ui->statusLabel->setText(QString::fromStdString(status));
    if (status == FileTransferClient::NO_CONNECTION) {
        ui->connectButton->setEnabled(true);
        ui->receiveButton->setEnabled(false);
        ui->cancelButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
    } else if (status == FileTransferClient::CONNECTING) {
        ui->progressBar->setMaximum(0);
        ui->connectButton->setEnabled(false);
        ui->cancelButton->setEnabled(true);
        ui->closeButton->setEnabled(false);
    } else if (status == FileTransferClient::RECEIVED_FILE_LIST) {
        ui->progressBar->setMaximum(100);
        ui->receiveButton->setEnabled(true);
        ui->cancelButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
    } else if (status == FileTransferClient::RECEIVING_FILES) {
        ui->receiveButton->setEnabled(false);
        ui->cancelButton->setEnabled(true);
        ui->closeButton->setEnabled(false);
    } else if (status == FileTransferClient::DONE) {
        ui->cancelButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
    } else if (status == FileTransferClient::CONNECTION_FAILED) {
        ui->progressBar->setMaximum(100);
        ui->connectButton->setEnabled(false);
        ui->receiveButton->setEnabled(false);
        ui->cancelButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
    }
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

void Dialog::setReceiveFileName(const string& fileName)
{
    ui->fileNameLabel->setText(QString::fromStdString(fileName));
}

void Dialog::setProgress(const long long soFar, const long long size)
{
    int progress = soFar / (float)size * 100;
    ui->receivedBytes->setText(
        QString::fromStdString(to_string(soFar) + " B / " + to_string(size) + " B"));
    ui->progressBar->setValue(progress);
}

void Dialog::on_receiveButton_clicked()
{
    ui->receiveButton->setEnabled(false);

    std::string receiveDir = ui->receivingDirLineEdit->text().toStdString();
    fileTransferClient->receiveFiles(receiveDir);
}

void Dialog::on_cancelButton_clicked()
{
    ui->closeButton->setEnabled(!ui->closeButton->isEnabled());
}

void Dialog::on_closeButton_clicked()
{
    QCoreApplication::exit();
}
