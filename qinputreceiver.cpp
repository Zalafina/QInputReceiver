#include "qinputreceiver.h"
#include "ui_qinputreceiver.h"

// 四种常见等宽字体的定义
inline constexpr const char FONT_CONSOLAS[]         = "Consolas";
inline constexpr const char FONT_LUCIDA_CONSOLE[]   = "Lucida Console";
inline constexpr const char FONT_COURIER_NEW[]      = "Courier New";
inline constexpr const char FONT_COURIER[]          = "Courier";


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
    ui->messageDisplay->setFont(QFont(FONT_CONSOLAS, 11));

    initVirtualKeyNameMap();
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

        if (!messageText.isEmpty()) {
            // 获取当前时间并格式化为字符串
            QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss.zzz]");
            // 将时间戳添加到消息前面
            messageText.prepend(timestamp + " ");

            ui->messageDisplay->append(messageText);
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}

QString QInputReceiver::translateMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Extract Extended-key flag from lParam (bit 24)
    bool isExtendedKey = (lParam & 0x1000000) != 0;

    switch (msg)
    {
    case WM_KEYDOWN:
    {
        QString keyName = getKeyName(wParam, isExtendedKey);
        QString hexCode = QString("%1").arg(wParam, 2, 16, QChar('0')).toUpper();
        if (isExtendedKey) {
            hexCode += "+E";
        }
        return QString("Keyboard Down : %1 (0x%2)").arg(keyName, hexCode);
    }
    case WM_KEYUP:
    {
        QString keyName = getKeyName(wParam, isExtendedKey);
        QString hexCode = QString("%1").arg(wParam, 2, 16, QChar('0')).toUpper();
        if (isExtendedKey) {
            hexCode += "+E";
        }
        return QString("Keyboard Up   : %1 (0x%2)").arg(keyName, hexCode);
    }
    case WM_LBUTTONDOWN:
        return QString("Mouse-L  Down : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_LBUTTONUP:
        return QString("Mouse-L  Up   : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_RBUTTONDOWN:
        return QString("Mouse-R  Down : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_RBUTTONUP:
        return QString("Mouse-R  Up   : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MBUTTONDOWN:
        return QString("Mouse-M  Down : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MBUTTONUP:
        return QString("Mouse-M  Up   : (%1, %2)")
            .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_XBUTTONDOWN:
        if (HIWORD(wParam) == XBUTTON1)
            return QString("Mouse-X1 Down : (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
        else
            return QString("Mouse-X2 Down : (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_XBUTTONUP:
        if (HIWORD(wParam) == XBUTTON1)
            return QString("Mouse-X1 Up   : (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
        else
            return QString("Mouse-X2 Up   : (%1, %2)")
                .arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
    case WM_MOUSEMOVE:
        if (ui->showMouseMoveCheckBox->isChecked()) {
            return QString("Mouse Move    : (%1, %2)").arg(GET_X_LPARAM(lParam)).arg(GET_Y_LPARAM(lParam));
        }
        break;
    case WM_MOUSEWHEEL:
        return QString("Mouse Wheel   : %1").arg(GET_WHEEL_DELTA_WPARAM(wParam));
    case WM_MOUSEHWHEEL:
        return QString("Mouse H-Wheel : %1").arg(GET_WHEEL_DELTA_WPARAM(wParam));
    default: return QString("Unknown Message");
    }

    return QString();
}

void QInputReceiver::initVirtualKeyNameMap()
{
    // US 104 Keyboard Main Area
    // Row 1
    m_VirtualKeyNameMap.insert(0xC0, "`");          // VK_OEM_3
    m_VirtualKeyNameMap.insert(0x31, "1");          // VK_1
    m_VirtualKeyNameMap.insert(0x32, "2");          // VK_2
    m_VirtualKeyNameMap.insert(0x33, "3");          // VK_3
    m_VirtualKeyNameMap.insert(0x34, "4");          // VK_4
    m_VirtualKeyNameMap.insert(0x35, "5");          // VK_5
    m_VirtualKeyNameMap.insert(0x36, "6");          // VK_6
    m_VirtualKeyNameMap.insert(0x37, "7");          // VK_7
    m_VirtualKeyNameMap.insert(0x38, "8");          // VK_8
    m_VirtualKeyNameMap.insert(0x39, "9");          // VK_9
    m_VirtualKeyNameMap.insert(0x30, "0");          // VK_0
    m_VirtualKeyNameMap.insert(0xBD, "-");          // VK_OEM_MINUS
    m_VirtualKeyNameMap.insert(0xBB, "=");          // VK_OEM_PLUS
    m_VirtualKeyNameMap.insert(0x08, "Backspace");  // VK_BACK
    // Row 2
    m_VirtualKeyNameMap.insert(0x09, "Tab");        // VK_TAB
    m_VirtualKeyNameMap.insert(0x51, "Q");          // VK_Q
    m_VirtualKeyNameMap.insert(0x57, "W");          // VK_W
    m_VirtualKeyNameMap.insert(0x45, "E");          // VK_E
    m_VirtualKeyNameMap.insert(0x52, "R");          // VK_R
    m_VirtualKeyNameMap.insert(0x54, "T");          // VK_T
    m_VirtualKeyNameMap.insert(0x59, "Y");          // VK_Y
    m_VirtualKeyNameMap.insert(0x55, "U");          // VK_U
    m_VirtualKeyNameMap.insert(0x49, "I");          // VK_I
    m_VirtualKeyNameMap.insert(0x4F, "O");          // VK_O
    m_VirtualKeyNameMap.insert(0x50, "P");          // VK_P
    m_VirtualKeyNameMap.insert(0xDB, "[");          // VK_OEM_4
    m_VirtualKeyNameMap.insert(0xDD, "]");          // VK_OEM_6
    m_VirtualKeyNameMap.insert(0xDC, "\\");        // VK_OEM_5
    // Row 3
    m_VirtualKeyNameMap.insert(0x14, "CapsLock");   // VK_CAPITAL
    m_VirtualKeyNameMap.insert(0x41, "A");          // VK_A
    m_VirtualKeyNameMap.insert(0x53, "S");          // VK_S
    m_VirtualKeyNameMap.insert(0x44, "D");          // VK_D
    m_VirtualKeyNameMap.insert(0x46, "F");          // VK_F
    m_VirtualKeyNameMap.insert(0x47, "G");          // VK_G
    m_VirtualKeyNameMap.insert(0x48, "H");          // VK_H
    m_VirtualKeyNameMap.insert(0x4A, "J");          // VK_J
    m_VirtualKeyNameMap.insert(0x4B, "K");          // VK_K
    m_VirtualKeyNameMap.insert(0x4C, "L");          // VK_L
    m_VirtualKeyNameMap.insert(0xBA, ";");          // VK_OEM_1
    m_VirtualKeyNameMap.insert(0xDE, "'");          // VK_OEM_7
    m_VirtualKeyNameMap.insert(0x0D, "Enter");      // VK_RETURN (also NumEnter with +E)
    // Row 4
    m_VirtualKeyNameMap.insert(0xA0, "L-Shift");    // VK_LSHIFT
    m_VirtualKeyNameMap.insert(0x5A, "Z");          // VK_Z
    m_VirtualKeyNameMap.insert(0x58, "X");          // VK_X
    m_VirtualKeyNameMap.insert(0x43, "C");          // VK_C
    m_VirtualKeyNameMap.insert(0x56, "V");          // VK_V
    m_VirtualKeyNameMap.insert(0x42, "B");          // VK_B
    m_VirtualKeyNameMap.insert(0x4E, "N");          // VK_N
    m_VirtualKeyNameMap.insert(0x4D, "M");          // VK_M
    m_VirtualKeyNameMap.insert(0xBC, ",");          // VK_OEM_COMMA
    m_VirtualKeyNameMap.insert(0xBE, ".");          // VK_OEM_PERIOD
    m_VirtualKeyNameMap.insert(0xBF, "/");          // VK_OEM_2
    m_VirtualKeyNameMap.insert(0xA1, "R-Shift");    // VK_RSHIFT (requires Extended flag)
    // Row 5
    m_VirtualKeyNameMap.insert(0xA2, "L-Ctrl");     // VK_LCONTROL
    m_VirtualKeyNameMap.insert(0x5B, "L-Win");      // VK_LWIN (requires Extended flag)
    m_VirtualKeyNameMap.insert(0xA4, "L-Alt");      // VK_LMENU
    m_VirtualKeyNameMap.insert(0x20, "Space");      // VK_SPACE
    m_VirtualKeyNameMap.insert(0xA5, "R-Alt");      // VK_RMENU (requires Extended flag)
    m_VirtualKeyNameMap.insert(0x5D, "Application"); // VK_APPS (requires Extended flag)
    m_VirtualKeyNameMap.insert(0xA3, "R-Ctrl");     // VK_RCONTROL (requires Extended flag)
    m_VirtualKeyNameMap.insert(0x5C, "R-Win");      // VK_RWIN (requires Extended flag)

    // Generic modifier keys (0x10, 0x11, 0x12)
    m_VirtualKeyNameMap.insert(0x10, "Shift");      // VK_SHIFT (generic)
    m_VirtualKeyNameMap.insert(0x11, "Ctrl");       // VK_CONTROL (generic)
    m_VirtualKeyNameMap.insert(0x12, "Alt");        // VK_MENU (generic)

    // Function Keys
    m_VirtualKeyNameMap.insert(0x1B, "Esc");        // VK_ESCAPE
    m_VirtualKeyNameMap.insert(0x70, "F1");         // VK_F1
    m_VirtualKeyNameMap.insert(0x71, "F2");         // VK_F2
    m_VirtualKeyNameMap.insert(0x72, "F3");         // VK_F3
    m_VirtualKeyNameMap.insert(0x73, "F4");         // VK_F4
    m_VirtualKeyNameMap.insert(0x74, "F5");         // VK_F5
    m_VirtualKeyNameMap.insert(0x75, "F6");         // VK_F6
    m_VirtualKeyNameMap.insert(0x76, "F7");         // VK_F7
    m_VirtualKeyNameMap.insert(0x77, "F8");         // VK_F8
    m_VirtualKeyNameMap.insert(0x78, "F9");         // VK_F9
    m_VirtualKeyNameMap.insert(0x79, "F10");        // VK_F10
    m_VirtualKeyNameMap.insert(0x7A, "F11");        // VK_F11
    m_VirtualKeyNameMap.insert(0x7B, "F12");        // VK_F12
    m_VirtualKeyNameMap.insert(0x7C, "F13");        // VK_F13
    m_VirtualKeyNameMap.insert(0x7D, "F14");        // VK_F14
    m_VirtualKeyNameMap.insert(0x7E, "F15");        // VK_F15
    m_VirtualKeyNameMap.insert(0x7F, "F16");        // VK_F16
    m_VirtualKeyNameMap.insert(0x80, "F17");        // VK_F17
    m_VirtualKeyNameMap.insert(0x81, "F18");        // VK_F18
    m_VirtualKeyNameMap.insert(0x82, "F19");        // VK_F19
    m_VirtualKeyNameMap.insert(0x83, "F20");        // VK_F20
    m_VirtualKeyNameMap.insert(0x84, "F21");        // VK_F21
    m_VirtualKeyNameMap.insert(0x85, "F22");        // VK_F22
    m_VirtualKeyNameMap.insert(0x86, "F23");        // VK_F23
    m_VirtualKeyNameMap.insert(0x87, "F24");        // VK_F24

    m_VirtualKeyNameMap.insert(0x2C, "PrintScrn");  // VK_SNAPSHOT (requires Extended flag)
    m_VirtualKeyNameMap.insert(0x91, "ScrollLock");   // VK_SCROLL
    m_VirtualKeyNameMap.insert(0x13, "Pause");        // VK_PAUSE

    m_VirtualKeyNameMap.insert(0x2D, "Insert");     // VK_INSERT (Extended for main, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x2E, "Delete");     // VK_DELETE (Extended for main, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x24, "Home");       // VK_HOME (Extended for main, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x23, "End");        // VK_END (Extended for main, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x21, "PageUp");     // VK_PRIOR (Extended for main, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x22, "PageDown");   // VK_NEXT (Extended for main, non-Extended for numpad)

    m_VirtualKeyNameMap.insert(0x26, "Up");         // VK_UP (Extended for arrow, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x28, "Down");       // VK_DOWN (Extended for arrow, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x25, "Left");       // VK_LEFT (Extended for arrow, non-Extended for numpad)
    m_VirtualKeyNameMap.insert(0x27, "Right");      // VK_RIGHT (Extended for arrow, non-Extended for numpad)

    // NumberPad Keys
    m_VirtualKeyNameMap.insert(0x90, "NumLock");    // VK_NUMLOCK (requires Extended flag)
    m_VirtualKeyNameMap.insert(0x6F, "Num/");       // VK_DIVIDE (requires Extended flag)
    m_VirtualKeyNameMap.insert(0x6A, "Num*");       // VK_MULTIPLY
    m_VirtualKeyNameMap.insert(0x6D, "Num-");       // VK_SUBTRACT
    m_VirtualKeyNameMap.insert(0x6B, "Num+");       // VK_ADD
    m_VirtualKeyNameMap.insert(0x6E, "Num.");       // VK_DECIMAL
    m_VirtualKeyNameMap.insert(0x60, "Num0");       // VK_NUMPAD0
    m_VirtualKeyNameMap.insert(0x61, "Num1");       // VK_NUMPAD1
    m_VirtualKeyNameMap.insert(0x62, "Num2");       // VK_NUMPAD2
    m_VirtualKeyNameMap.insert(0x63, "Num3");       // VK_NUMPAD3
    m_VirtualKeyNameMap.insert(0x64, "Num4");       // VK_NUMPAD4
    m_VirtualKeyNameMap.insert(0x65, "Num5");       // VK_NUMPAD5
    m_VirtualKeyNameMap.insert(0x66, "Num6");       // VK_NUMPAD6
    m_VirtualKeyNameMap.insert(0x67, "Num7");       // VK_NUMPAD7
    m_VirtualKeyNameMap.insert(0x68, "Num8");       // VK_NUMPAD8
    m_VirtualKeyNameMap.insert(0x69, "Num9");       // VK_NUMPAD9
    m_VirtualKeyNameMap.insert(0x0C, "Num5(NumOFF)"); // VK_CLEAR

    // MultiMedia keys (all require Extended flag)
    m_VirtualKeyNameMap.insert(0xAD, "VolumeMute");       // VK_VOLUME_MUTE
    m_VirtualKeyNameMap.insert(0xAE, "VolumeDown");       // VK_VOLUME_DOWN
    m_VirtualKeyNameMap.insert(0xAF, "VolumeUp");         // VK_VOLUME_UP
    m_VirtualKeyNameMap.insert(0xB0, "MediaNext");        // VK_MEDIA_NEXT_TRACK
    m_VirtualKeyNameMap.insert(0xB1, "MediaPrev");        // VK_MEDIA_PREV_TRACK
    m_VirtualKeyNameMap.insert(0xB2, "MediaStop");        // VK_MEDIA_STOP
    m_VirtualKeyNameMap.insert(0xB3, "MediaPlayPause");   // VK_MEDIA_PLAY_PAUSE
    m_VirtualKeyNameMap.insert(0xB4, "LaunchMail");       // VK_LAUNCH_MAIL
    m_VirtualKeyNameMap.insert(0xB5, "SelectMedia");      // VK_LAUNCH_MEDIA_SELECT
    m_VirtualKeyNameMap.insert(0xB6, "LaunchApp1");       // VK_LAUNCH_APP1
    m_VirtualKeyNameMap.insert(0xB7, "LaunchApp2");       // VK_LAUNCH_APP2

    // Browser keys (all require Extended flag)
    m_VirtualKeyNameMap.insert(0xA6, "BrowserBack");      // VK_BROWSER_BACK
    m_VirtualKeyNameMap.insert(0xA7, "BrowserForward");   // VK_BROWSER_FORWARD
    m_VirtualKeyNameMap.insert(0xA8, "BrowserRefresh");   // VK_BROWSER_REFRESH
    m_VirtualKeyNameMap.insert(0xA9, "BrowserStop");      // VK_BROWSER_STOP
    m_VirtualKeyNameMap.insert(0xAA, "BrowserSearch");    // VK_BROWSER_SEARCH
    m_VirtualKeyNameMap.insert(0xAB, "BrowserFavorites"); // VK_BROWSER_FAVORITES
    m_VirtualKeyNameMap.insert(0xAC, "BrowserHome");      // VK_BROWSER_HOME

    // Additional keys from Microsoft documentation
    m_VirtualKeyNameMap.insert(0x03, "Cancel");      // VK_CANCEL
    m_VirtualKeyNameMap.insert(0x0C, "Clear");       // VK_CLEAR
    m_VirtualKeyNameMap.insert(0x5F, "Sleep");       // VK_SLEEP
}

QString QInputReceiver::getKeyName(int virtualKeyCode, bool isExtended)
{
    // For some keys, we need to distinguish based on the Extended flag
    // NumPad keys (without Extended) vs Navigation/Arrow keys (with Extended)
    if (!isExtended) {
        // NumPad navigation keys when NumLock is OFF (non-Extended)
        if (virtualKeyCode == 0x2D) return "Num0(NumOFF)";  // Insert
        if (virtualKeyCode == 0x2E) return "Num.(NumOFF)";  // Delete
        if (virtualKeyCode == 0x23) return "Num1(NumOFF)";  // End
        if (virtualKeyCode == 0x28) return "Num2(NumOFF)";  // Down
        if (virtualKeyCode == 0x22) return "Num3(NumOFF)";  // PageDown
        if (virtualKeyCode == 0x25) return "Num4(NumOFF)";  // Left
        if (virtualKeyCode == 0x0C) return "Num5(NumOFF)";  // Clear
        if (virtualKeyCode == 0x27) return "Num6(NumOFF)";  // Right
        if (virtualKeyCode == 0x24) return "Num7(NumOFF)";  // Home
        if (virtualKeyCode == 0x26) return "Num8(NumOFF)";  // Up
        if (virtualKeyCode == 0x21) return "Num9(NumOFF)";  // PageUp
    }

    // For Enter key, distinguish between main Enter and NumEnter
    if (virtualKeyCode == 0x0D) {
        return isExtended ? "NumEnter" : "Enter";
    }

    if (m_VirtualKeyNameMap.contains(virtualKeyCode)) {
        return m_VirtualKeyNameMap.value(virtualKeyCode);
    }

    return QString::number(virtualKeyCode);
}

void InputReceiverTextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void InputReceiverTextEdit::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}
