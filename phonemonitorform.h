#ifndef PHONEMONITORFORM_H
#define PHONEMONITORFORM_H

#include <QWidget>

class PhoneMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    PhoneNumber = 12
};

public:
    explicit PhoneMonitorForm(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // PHONEMONITORFORM_H
