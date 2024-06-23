#include "qinputreceiver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time [hh:mm:ss.zzz]} %{message}");

    QApplication a(argc, argv);
    QInputReceiver w;
    w.show();
    return a.exec();
}
