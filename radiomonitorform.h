#ifndef RADIOMONITORFORM_H
#define RADIOMONITORFORM_H

#include <QWidget>

class RadioMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    RadioNumber = 12
};

public:
    explicit RadioMonitorForm(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // RADIOMONITORFORM_H
