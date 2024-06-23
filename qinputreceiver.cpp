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
    static POINT lastMousePos = { -1, -1 };
    static bool stateChanged = false;

    MSG *msg = static_cast<MSG*>(message);
    if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP ||
        msg->message == WM_LBUTTONDOWN || msg->message == WM_LBUTTONUP ||
        msg->message == WM_RBUTTONDOWN || msg->message == WM_RBUTTONUP ||
        msg->message == WM_MBUTTONDOWN || msg->message == WM_MBUTTONUP ||
        msg->message == WM_XBUTTONDOWN || msg->message == WM_XBUTTONUP ||
        msg->message == WM_MOUSEMOVE || msg->message == WM_MOUSEWHEEL ||
        msg->message == WM_MOUSEHWHEEL)
    {
        // 检查是否是 WM_MOUSEMOVE 消息
        if (msg->message == WM_MOUSEMOVE) {
            POINT currentMousePos = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };

            // 如果坐标相同且状态没有变化，忽略消息
            if (currentMousePos.x == lastMousePos.x && currentMousePos.y == lastMousePos.y && !stateChanged) {
                return false; // 忽略消息
            }

            // 更新上一次的鼠标位置和状态
            lastMousePos = currentMousePos;
            stateChanged = false; // 根据实际情况更新状态变化
        }

        // 处理其他消息
        QString messageText = translateMessage(msg->message, msg->wParam, msg->lParam);

        // 获取当前时间并格式化为字符串
        QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss.zzz]");
        // 将时间戳添加到消息前面
        messageText.prepend(timestamp + " ");

        ui->messageDisplay->append(messageText);
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}

QString QInputReceiver::translateMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN: return QString("Key Down: %1").arg(wParam);
    case WM_KEYUP: return QString("Key Up: %1").arg(wParam);
    case WM_LBUTTONDOWN:
        return QString("Left Mouse Button Down at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_LBUTTONUP:
        return QString("Left Mouse Button Up at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_RBUTTONDOWN:
        return QString("Right Mouse Button Down at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_RBUTTONUP:
        return QString("Right Mouse Button Up at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MBUTTONDOWN:
        return QString("Middle Mouse Button Down at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MBUTTONUP:
        return QString("Middle Mouse Button Up at (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_XBUTTONDOWN:
        if (HIWORD(wParam) == XBUTTON1)
            return QString("X1 Mouse Button Down at (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
        else
            return QString("X2 Mouse Button Down at (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_XBUTTONUP:
        if (HIWORD(wParam) == XBUTTON1)
            return QString("X1 Mouse Button Up at (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
        else
            return QString("X2 Mouse Button Up at (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
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
