#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
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

private slots:
    void on_connectButton_clicked();
    void on_receiveButton_clicked();

private:
    Ui::Dialog *ui;
    FileTransferClient fileTransferClient;
};

#endif // DIALOG_H
