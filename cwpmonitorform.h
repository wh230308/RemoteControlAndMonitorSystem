#ifndef CWPMONITORFORM_H
#define CWPMONITORFORM_H

#include <QWidget>

class CWPMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    CWPNumber = 12
};

public:
    explicit CWPMonitorForm(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // CWPMONITORFORM_H
