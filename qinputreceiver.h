#ifndef QINPUTRECEIVER_H
#define QINPUTRECEIVER_H

#include <windows.h>
#include <windowsx.h>
#include <QMainWindow>
#include <QDateTime>
#include <QMap>
// #include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui {
class QInputReceiver;
}
QT_END_NAMESPACE

class QInputReceiver : public QMainWindow
{
    Q_OBJECT

public:
    QInputReceiver(QWidget *parent = nullptr);
    ~QInputReceiver();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    QString translateMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void initVirtualKeyNameMap();
    QString getKeyName(int virtualKeyCode, bool isExtended);

private:
    Ui::QInputReceiver *ui;
    QMap<int, QString> m_VirtualKeyNameMap;
};
#endif // QINPUTRECEIVER_H
