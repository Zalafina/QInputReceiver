#include "qinputreceiver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QInputReceiver w;
    w.show();
    return a.exec();
}
