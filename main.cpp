#include <QApplication>

#include "mainwindow.h"
#include "log.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    base::log::set_directory(QObject::tr("d:\\log").toStdString().c_str());
    base::log::set_level(base::log::kLevelMask & ~base::log::kDebug);
    base::log::set_prefix_name(QObject::tr("rcms_").toStdString().c_str());
    base::log::set_storage_type(base::log::kDiskFile);
    base::log::set_reserve_days(7);

    LOG_STARTUP("remote control and monitor system is running...");
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
