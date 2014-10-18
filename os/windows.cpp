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

	INPUT input = {0};
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

	input.type = INPUT_MOUSE;
	input.mi.dwFlags = eventType;
	SendInput(1, &input, sizeof(INPUT));
}

void Os::moveMouse(QPoint const &point, QString const &windowTitle)
{
	QPoint const windowPosition = windowTitle.isEmpty()
			? windowPos(GetActiveWindow())
			: windowPos(windowTitle);

	QPoint const windowPoint = point + windowPosition;
	SetCursorPos(windowPoint.x(), windowPoint.y());
}

void Os::releaseMouse(QPoint const &point, Qt::MouseButton button
		, QString const &windowTitle, QSize const &windowSize)
{
	resizeWindow(windowTitle, windowSize);
	moveMouse(point, windowTitle);

	INPUT input = {0};
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

	input.type = INPUT_MOUSE;
	input.mi.dwFlags = eventType;
	SendInput(1, &input, sizeof(INPUT));
}

void Os::simulateWheel(int deltaX, int deltaY, QPoint const &point
		, QString const &windowTitle, QSize const &windowSize)
{
}

void Os::pressKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
}

void Os::releaseKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
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

void Os::pressKey(Qt::Key key)
{
}

void Os::releaseKey(Qt::Key key)
{
}

HWND Os::windowHandle(QString const &title)
{
	HWND const handle = FindWindowW(0, title.toStdWString().c_str());
	if (!handle) {
		PRINT(QString("Cannot find handle of \"%1\"! Error code: %2\n").arg(title, QString::number(GetLastError())));
	}

	return handle;
}

QPoint Os::windowPos(HWND handle)
{
	RECT windowRectandle;
	GetWindowRect(handle, &windowRectandle);
	return QPoint(windowRectandle.left, windowRectandle.top);
}

QPoint Os::windowPos(QString const &title)
{
	return windowPos(windowHandle(title));
}
