#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtGui>
#include "FileTransferClient.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void setStatus(const string& status);
    void setFileList(const vector<FileInfo>& fileList);

private slots:
    void on_connectButton_clicked();
    void on_receiveButton_clicked();
    void setProgress(const long long soFar, const long long size);

private:
    Ui::Dialog *ui;
    QStandardItemModel *model;
    FileTransferClient *fileTransferClient;
};

#endif // DIALOG_H
