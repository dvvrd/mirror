#pragma once

#include <windows.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QList>

/// Windows API communicator.
class Os
{
public:
	/// Simulates mouse press using Windows API.
	/// @param point: The coordinates of the mouse cursor in window coordinate system.
	/// @param button: The mouse button which was pressed by the user.
	/// @param windowTitle: The title that the window had at the moment of the action.
	/// @param windowSize: The size in px that the window had at the moment of the action.
	static void pressMouse(QPoint const &point, Qt::MouseButton button
			, QString const &windowTitle, QSize const &windowSize);

	/// Simulates mouse cursor movement using Windows API.
	/// If window title is empty active one will be used.
	/// @param point: The coordinates of the mouse cursor in window coordinate system.
	/// @param windowTitle: The title that the window had at the moment of the action.
	static void moveMouse(QPoint const &point, QString const &windowTitle);

	/// Simulates mouse release using Windows API.
	/// @param point: The coordinates of the mouse cursor in window coordinate system.
	/// @param button: The mouse button which was released by the user.
	/// @param windowTitle: The title that the window had at the moment of the action.
	/// @param windowSize: The size in px that the window had at the moment of the action.
	static void releaseMouse(QPoint const &point, Qt::MouseButton button
			, QString const &windowTitle, QSize const &windowSize);

	/// Simulates
	/// @param deltaX: Wheel ticks by the x-axis.
	/// @param deltaY: Wheel ticks by the y-axis.
	/// @param point: The coordinates of the mouse cursor in window coordinate system.
	/// @param windowTitle: The title that the window had at the moment of the action.
	/// @param windowSize: The size in px that the window had at the moment of the action.
	static void simulateWheel(int deltaX, int deltaY, QPoint const &point
			, QString const &windowTitle, QSize const &windowSize);

	/// Simulates key press using Windows API.
	/// @param key: The keyboard key that was pressed by user.
	/// @param modifiers: The keyboard modifiers (alt, ctrl buttons and so on) that were pressed at that moment.
	static void pressKey(Qt::Key key, Qt::KeyboardModifiers modifiers);

	/// Simulates key release using Windows API.
	/// @param key: The keyboard key that was relseased by user.
	/// @param modifiers: The keyboard modifiers (alt, ctrl buttons and so on) that were pressed at that moment.
	static void releaseKey(Qt::Key key, Qt::KeyboardModifiers modifiers);

private:
	static void pressKey(Qt::Key key);
	static void releaseKey(Qt::Key key);
	static void pressModifiers(Qt::KeyboardModifiers modifiers);
	static void releaseModifiers(Qt::KeyboardModifiers modifiers);
	static QList<Qt::Key> modifiersToKeys(Qt::KeyboardModifiers modifiers);

	static void resizeWindow(QString const &windowTitle, QSize const &windowSize);

	static HWND windowHandle(QString const &title);
	static QPoint screenPos(HWND handle, QPoint const &windowPoint);
	static QPoint screenPos(QString const &title, QPoint const &windowPoint);

	static void sendMouseEvent(int type);
	static void sendWheelEvent(int type, int delta);
	static void sendKeyEvent(int key, int keypress);

	static int qtKeyToNative(Qt::Key key);
};
