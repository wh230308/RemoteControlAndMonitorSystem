#ifndef SVRADDRCONFIGDIALOG_H
#define SVRADDRCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class SvrAddrConfigDialog;
}

class SvrAddrConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SvrAddrConfigDialog(QWidget *parent = nullptr);
    ~SvrAddrConfigDialog();

signals:
    void svrAddrChanged(const QString &svr1Ip, ushort svr1Port,
                             const QString &svr2Ip, ushort svr2Port);

private slots:
    void onSaveConfig();
    void onCloseDialog();

private:
    void loadSvrAddrConfig();

private:
    Ui::SvrAddrConfigDialog *ui;
};

#endif // SVRADDRCONFIGDIALOG_H
