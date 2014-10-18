#include "windows.h"

#include "defines.h"

BOOL CALLBACK onWindow(HWND hwnd, LPARAM lParam)
{
	Q_UNUSED(lParam)

	wchar_t className[80];
	wchar_t title[80];

	GetClassNameW(hwnd, className, sizeof(className));
	GetWindowTextW(hwnd, title, sizeof(title));

	PRINT("Window title: " + QString::fromStdWString(title) + "\n");
	PRINT("Class name: " + QString::fromStdWString(className) + "\n");

	return TRUE;
}

void debugWindowTitles()
{
	EnumWindows(onWindow, 0);
}

void Os::pressMouse(QPoint const &point, Qt::MouseButton button
		, QString const &windowTitle, QSize const &windowSize)
{
	resizeWindow(windowTitle, windowSize);
	moveMouse(point, windowTitle);

	int eventType;
	switch(button) {
	case Qt::LeftButton:
		eventType = MOUSEEVENTF_LEFTDOWN;
		break;
	case Qt::RightButton:
		eventType = MOUSEEVENTF_RIGHTDOWN;
		break;
	case Qt::MiddleButton:
		eventType = MOUSEEVENTF_MIDDLEDOWN;
		break;
	default:
		eventType = MOUSEEVENTF_XDOWN;
		break;
	}

	sendMouseEvent(eventType);
}

void Os::moveMouse(QPoint const &point, QString const &windowTitle)
{
	QPoint const screenPosition = windowTitle.isEmpty()
			? screenPos(GetForegroundWindow(), point)
			: screenPos(windowTitle, point);

	SetCursorPos(screenPosition.x(), screenPosition.y());
}

void Os::releaseMouse(QPoint const &point, Qt::MouseButton button
		, QString const &windowTitle, QSize const &windowSize)
{
	resizeWindow(windowTitle, windowSize);
	moveMouse(point, windowTitle);

	int eventType;
	switch(button) {
	case Qt::LeftButton:
		eventType = MOUSEEVENTF_LEFTUP;
		break;
	case Qt::RightButton:
		eventType = MOUSEEVENTF_RIGHTUP;
		break;
	case Qt::MiddleButton:
		eventType = MOUSEEVENTF_MIDDLEUP;
		break;
	default:
		eventType = MOUSEEVENTF_XUP;
		break;
	}

	sendMouseEvent(eventType);
}

void Os::simulateWheel(int deltaX, int deltaY, QPoint const &point
		, QString const &windowTitle, QSize const &windowSize)
{
	Q_UNUSED(deltaY)

	resizeWindow(windowTitle, windowSize);
	moveMouse(point, windowTitle);

	sendWheelEvent(MOUSEEVENTF_WHEEL, deltaX);
}

void Os::pressKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
	pressModifiers(modifiers);
	pressKey(key);
}

void Os::releaseKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
	releaseKey(key);
	releaseModifiers(modifiers);
}

void Os::pressKey(Qt::Key key)
{
	sendKeyEvent(qtKeyToNative(key), 0);
}

void Os::releaseKey(Qt::Key key)
{
	sendKeyEvent(qtKeyToNative(key), KEYEVENTF_KEYUP);
}

void Os::pressModifiers(Qt::KeyboardModifiers modifiers)
{
	for (Qt::Key key : modifiersToKeys(modifiers)) {
		pressKey(key);
	}
}

void Os::releaseModifiers(Qt::KeyboardModifiers modifiers)
{
	for (Qt::Key key : modifiersToKeys(modifiers)) {
		releaseKey(key);
	}
}

QList<Qt::Key> Os::modifiersToKeys(Qt::KeyboardModifiers modifiers)
{
	QList<Qt::Key> result;
	if (modifiers & Qt::ShiftModifier) {
		result << Qt::Key_Shift;
	}
	if (modifiers & Qt::ControlModifier) {
		result << Qt::Key_Control;
	}
	if (modifiers & Qt::AltModifier) {
		result << Qt::Key_Alt;
	}
	return result;
}

void Os::resizeWindow(QString const &windowTitle, QSize const &windowSize)
{
	HWND const handle = windowHandle(windowTitle);
	SetActiveWindow(handle);
	BringWindowToTop(handle);
	RECT windowRect { 0, 0, windowSize.width(), windowSize.height() };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, true);
	SetWindowPos(handle, 0, 0, 0, windowRect.right - windowRect.left
			, windowRect.bottom - windowRect.top, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

HWND Os::windowHandle(QString const &title)
{
	HWND const handle = FindWindowW(0, title.toStdWString().c_str());
	if (!handle) {
		PRINT(QString("Cannot find handle of \"%1\"! Error code: %2\n").arg(title, QString::number(GetLastError())));
	}

	return handle;
}

QPoint Os::screenPos(HWND handle, QPoint const &windowPoint)
{
	POINT pos = { windowPoint.x(), windowPoint.y() };
	ClientToScreen(handle, &pos);
	return QPoint(pos.x, pos.y);
}

QPoint Os::screenPos(QString const &title, QPoint const &windowPoint)
{
	return screenPos(windowHandle(title), windowPoint);
}

void Os::sendMouseEvent(int type)
{
	INPUT input = { 0, 0, 0, 0, 0, 0, 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = type;
	SendInput(1, &input, sizeof(INPUT));
}

void Os::sendWheelEvent(int type, int delta)
{
	INPUT input = { 0, 0, 0, 0, 0, 0, 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = type;
	input.mi.mouseData = delta;
	SendInput(1, &input, sizeof(INPUT));
}

void Os::sendKeyEvent(int key, int keypress)
{
	INPUT input = { 0, 0, 0, 0, 0, 0, 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = key;
	input.ki.dwFlags = keypress;
	SendInput(1, &input, sizeof(INPUT));
}

int Os::qtKeyToNative(Qt::Key key)
{
	switch(key) {
	case Qt::Key_Escape:
		return VK_ESCAPE;
	case Qt::Key_Tab:
		return VK_TAB;
	case Qt::Key_Backspace:
		return VK_BACK;
	case Qt::Key_Return:
	case Qt::Key_Enter:
		return VK_RETURN;
	case Qt::Key_Insert:
		return VK_INSERT;
	case Qt::Key_Delete:
		return VK_DELETE;
	case Qt::Key_Pause:
		return VK_PAUSE;
	case Qt::Key_Print:
		return VK_PRINT;
	case Qt::Key_Clear:
		return VK_CLEAR;
	case Qt::Key_Home:
		return VK_HOME;
	case Qt::Key_End:
		return VK_END;
	case Qt::Key_Left:
		return VK_LEFT;
	case Qt::Key_Up:
		return VK_UP;
	case Qt::Key_Right:
		return VK_RIGHT;
	case Qt::Key_Down:
		return VK_DOWN;
	case Qt::Key_PageUp:
		return VK_PRIOR;
	case Qt::Key_PageDown:
		return VK_NEXT;
	case Qt::Key_Shift:
		return VK_SHIFT;
	case Qt::Key_Control:
		return VK_CONTROL;
	case Qt::Key_Alt:
		return VK_MENU;
	case Qt::Key_CapsLock:
		return VK_CAPITAL;
	case Qt::Key_NumLock:
		return VK_NUMLOCK;
	case Qt::Key_ScrollLock:
		return VK_SCROLL;
	case Qt::Key_F1:
		return VK_F1;
	case Qt::Key_F2:
		return VK_F2;
	case Qt::Key_F3:
		return VK_F3;
	case Qt::Key_F4:
		return VK_F4;
	case Qt::Key_F5:
		return VK_F5;
	case Qt::Key_F6:
		return VK_F6;
	case Qt::Key_F7:
		return VK_F7;
	case Qt::Key_F8:
		return VK_F8;
	case Qt::Key_F9:
		return VK_F9;
	case Qt::Key_F10:
		return VK_F10;
	case Qt::Key_F11:
		return VK_F11;
	case Qt::Key_F12:
		return VK_F12;
	case Qt::Key_F13:
		return VK_F13;
	case Qt::Key_F14:
		return VK_F14;
	case Qt::Key_F15:
		return VK_F15;
	case Qt::Key_F16:
		return VK_F16;
	case Qt::Key_F17:
		return VK_F17;
	case Qt::Key_F18:
		return VK_F18;
	case Qt::Key_F19:
		return VK_F19;
	case Qt::Key_F20:
		return VK_F20;
	case Qt::Key_F21:
		return VK_F21;
	case Qt::Key_F22:
		return VK_F22;
	case Qt::Key_F23:
		return VK_F23;
	case Qt::Key_F24:
		return VK_F24;
	case Qt::Key_Space:
		return VK_SPACE;
	case Qt::Key_QuoteDbl:
		return VK_OEM_7;
	case Qt::Key_Asterisk:
		return VK_MULTIPLY;
	case Qt::Key_Plus:
		return VK_ADD;
	case Qt::Key_Comma:
		return VK_OEM_COMMA;
	case Qt::Key_Minus:
		return VK_OEM_MINUS;
	case Qt::Key_Period:
		return VK_OEM_PERIOD;
	case Qt::Key_Slash:
		return VK_DIVIDE;
	case Qt::Key_0:
		return 0x30;
	case Qt::Key_1:
		return 0x31;
	case Qt::Key_2:
		return 0x32;
	case Qt::Key_3:
		return 0x33;
	case Qt::Key_4:
		return 0x34;
	case Qt::Key_5:
		return 0x35;
	case Qt::Key_6:
		return 0x36;
	case Qt::Key_7:
		return 0x37;
	case Qt::Key_8:
		return 0x38;
	case Qt::Key_9:
		return 0x39;
	case Qt::Key_Colon:
	case Qt::Key_Semicolon:
		return VK_OEM_1;
	case Qt::Key_Question:
		return VK_OEM_2;
	case Qt::Key_A:
		return 0x41;
	case Qt::Key_B:
		return 0x42;
	case Qt::Key_C:
		return 0x43;
	case Qt::Key_D:
		return 0x44;
	case Qt::Key_E:
		return 0x45;
	case Qt::Key_F:
		return 0x46;
	case Qt::Key_G:
		return 0x47;
	case Qt::Key_H:
		return 0x48;
	case Qt::Key_I:
		return 0x49;
	case Qt::Key_J:
		return 0x4A;
	case Qt::Key_K:
		return 0x4B;
	case Qt::Key_L:
		return 0x4C;
	case Qt::Key_M:
		return 0x4D;
	case Qt::Key_N:
		return 0x4E;
	case Qt::Key_O:
		return 0x4F;
	case Qt::Key_P:
		return 0x50;
	case Qt::Key_Q:
		return 0x51;
	case Qt::Key_R:
		return 0x52;
	case Qt::Key_S:
		return 0x53;
	case Qt::Key_T:
		return 0x54;
	case Qt::Key_U:
		return 0x55;
	case Qt::Key_V:
		return 0x56;
	case Qt::Key_W:
		return 0x57;
	case Qt::Key_X:
		return 0x58;
	case Qt::Key_Y:
		return 0x59;
	case Qt::Key_Z:
		return 0x5A;
	case Qt::Key_Backslash:
		return VK_OEM_102;
	case Qt::Key_QuoteLeft:
		return VK_OEM_7;
	case Qt::Key_AsciiTilde:
		return VK_OEM_3;
	default:
		return 0;
	}
}
