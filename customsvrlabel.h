#ifndef CUSTOMSVRLABEL_H
#define CUSTOMSVRLABEL_H

#include <QLabel>

class CustomSvrLabel : public QLabel
{
    Q_OBJECT

enum
{
    kSvrLabelWidth = 165,
    kSvrLabelHeight = 165,
};

public:
    explicit CustomSvrLabel(const QString &svrName, QWidget *parent = nullptr);

    void updateRunningState(int runningState);
    void updateEthPortsState(int port1State, int port2State);

private:
    void initContentsLayout(const QString &svrName);

private:
    bool isRunning_ = false;
    bool isDoubleEths_ = true;
};

#endif // CUSTOMSVRLABEL_H
