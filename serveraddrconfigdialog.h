#ifndef SERVERADDRCONFIGDIALOG_H
#define SERVERADDRCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ServerAddrConfigDialog;
}

class ServerAddrConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerAddrConfigDialog(QWidget *parent = nullptr);
    ~ServerAddrConfigDialog();

signals:
    void serverAddrChanged(const QString &svr1Ip, ushort svr1Port,
                             const QString &svr2Ip, ushort svr2Port);

private slots:
    void onSaveConfig();
    void onCloseDialog();

private:
    void loadServerAddrConfig();

private:
    Ui::ServerAddrConfigDialog *ui;
};

#endif // SERVERADDRCONFIGDIALOG_H
