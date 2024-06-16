#include "qinputreceiver.h"
#include "ui_qinputreceiver.h"

QInputReceiver::QInputReceiver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QInputReceiver)
{
    ui->setupUi(this);

    setWindowTitle("Input Message Receiver");
    // setCentralWidget(m_MessageDisplayLabel);
    // QVBoxLayout *layout = new QVBoxLayout();
    // layout->addWidget(m_MessageDisplayLabel);
    // centralWidget()->setLayout(layout);
    // m_MessageDisplayLabel->setWordWrap(true);
}

QInputReceiver::~QInputReceiver()
{
    delete ui;
}

bool QInputReceiver::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG*>(message);
    if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP ||
        msg->message == WM_LBUTTONDOWN || msg->message == WM_LBUTTONUP ||
        msg->message == WM_RBUTTONDOWN || msg->message == WM_RBUTTONUP ||
        msg->message == WM_MBUTTONDOWN || msg->message == WM_MBUTTONUP ||
        msg->message == WM_XBUTTONDOWN || msg->message == WM_XBUTTONUP ||
        msg->message == WM_MOUSEMOVE || msg->message == WM_MOUSEWHEEL ||
        msg->message == WM_MOUSEHWHEEL)
    {
        QString messageText = translateMessage(msg->message, msg->wParam, msg->lParam);
        ui->messageDisplay->append(messageText);
        // ui->messageDisplay->verticalScrollBar()->setValue(ui->messageDisplay->verticalScrollBar()->maximum());
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

QString QInputReceiver::translateMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN: return QString("Key Down: %1").arg(wParam);
    case WM_KEYUP: return QString("Key Up: %1").arg(wParam);
    case WM_LBUTTONDOWN: return QString("Left Mouse Button Down");
    case WM_LBUTTONUP: return QString("Left Mouse Button Up");
    case WM_RBUTTONDOWN: return QString("Right Mouse Button Down");
    case WM_RBUTTONUP: return QString("Right Mouse Button Up");
    case WM_MBUTTONDOWN: return QString("Middle Mouse Button Down");
    case WM_MBUTTONUP: return QString("Middle Mouse Button Up");
    case WM_XBUTTONDOWN:
        return QString("X Mouse Button Down: %1")
            .arg(HIWORD(wParam) == XBUTTON1 ? "XButton1" : "XButton2");
    case WM_XBUTTONUP:
        return QString("X Mouse Button Up: %1")
            .arg(HIWORD(wParam) == XBUTTON1 ? "XButton1" : "XButton2");
    case WM_MOUSEMOVE:
        return QString("Mouse Move: (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MOUSEWHEEL:
        return QString("Mouse Wheel: %1").arg(GET_WHEEL_DELTA_WPARAM(wParam));
    case WM_MOUSEHWHEEL:
        return QString("Mouse Horizontal Wheel: %1").arg(GET_WHEEL_DELTA_WPARAM(wParam));
    default: return QString("Unknown Message");
    }
}
