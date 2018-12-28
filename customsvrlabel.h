#ifndef CUSTOMSVRLABEL_H
#define CUSTOMSVRLABEL_H

#include <QLabel>

class CustomSvrLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CustomSvrLabel(int width, int height, const QString &svrName, QWidget *parent = nullptr);

    void updateRunningState(int runningState);
    void updateEthPortsState(int port1State, int port2State);

private:
    void initContentsLayout(const QString &svrName);

private:
    int svrLabelWidth_;
    int svrLabelHeight_;
    bool isRunning_ = false;
    bool isDualEths_ = true;
};

#endif // CUSTOMSVRLABEL_H
