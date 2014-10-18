#include "entryProcessor.h"

#include <QtCore/QRegExp>
#include <QtCore/QPoint>
#include <QtCore/QSize>

#include "defines.h"
#include "os.h"

LogEntry::Level const userActionsLogLevel = LogEntry::Level::trace;

EntryProcessor::EntryProcessor(LogEntry const &entry)
	: mEntry(entry)
{
}

void EntryProcessor::process()
{
	if (mEntry.level() != userActionsLogLevel) {
		// Got message of the other level
		return;
	}

	if (mEntry.message().startsWith("Mouse")) {
		processMouseEntry();
	} else if (mEntry.message().startsWith("Wheel")) {
		processWheelEntry();
	} else if (mEntry.message().startsWith("Key")) {
		processKeyEntry();
	}

	// Else it is not user action message, ignoring it
}

void EntryProcessor::processMouseEntry()
{
	if (mEntry.message().startsWith("Mouse gesture movement")) {
		processGestureMovement();
		return;
	}

	QRegExp const regexp("Mouse (\\w+) in QPoint\\((\\d+),(\\d+)\\) with (\\d+)"\
			" target \"(.*)\" QSize\\((\\d+), (\\d+)\\)");
	if (!regexp.exactMatch(mEntry.message())) {
		return;
	}

	QString const action = regexp.cap(1);

	int const x = regexp.cap(2).toInt();
	int const y = regexp.cap(3).toInt();

	Qt::MouseButton const button = static_cast<Qt::MouseButton>(regexp.cap(4).toInt());

	QString const title = regexp.cap(5);

	int const width = regexp.cap(6).toInt();
	int const height = regexp.cap(7).toInt();

	VERBOSE(QString("Simulating mouse %1, pos=(%2,%3), button=%4, title=%5, size=(%6,%7)...\n")
			.arg(action, QString::number(x), QString::number(y), QString::number(static_cast<int>(button))
					, title, QString::number(width), QString::number(height)));

	if (action == "press") {
//		Os::pressMouse(QPoint(x, y), button, title, QSize(width, height));
	} else if (action == "release") {
//		Os::releaseMouse(QPoint(x, y), button, title, QSize(width, height));
	}
}

void EntryProcessor::processWheelEntry()
{
	QRegExp const regexp("Wheel with delta QPoint\\(([0-9|\\-]+),([0-9|\\-]+)\\) in"\
			" QPoint\\(([0-9|\\-]+),([0-9|\\-]+)\\) target \"(.*)\" QSize\\(([0-9|\\-]+), ([0-9|\\-]+)\\)\n");
	if (!regexp.exactMatch(mEntry.message())) {
		return;
	}

	int const deltaX = regexp.cap(1).toInt();
	int const deltaY = regexp.cap(2).toInt();

	int const x = regexp.cap(3).toInt();
	int const y = regexp.cap(4).toInt();

	QString const title = regexp.cap(5);

	int const width = regexp.cap(6).toInt();
	int const height = regexp.cap(7).toInt();

	VERBOSE(QString("Simulating wheel event, delta=(%1,%2), pos=(%3,%4), title=%5, size=(%6,%7)...\n")
			.arg(QString::number(deltaX), QString::number(deltaY), QString::number(x), QString::number(y)
					, title, QString::number(width), QString::number(height)));

	//Os::simulateWheel(deltaX, deltaY, QPoint(x, y), title, QSize(width, height));
}

void EntryProcessor::processKeyEntry()
{
	QRegExp const regexp("Key (\\w+) with (\\d+) modifiers QFlags\\(0x(\\d+)\\)");
	if (!regexp.exactMatch(mEntry.message())) {
		return;
	}

	QString const action = regexp.cap(1);

	Qt::Key const key = static_cast<Qt::Key>(regexp.cap(2).toInt());
	Qt::KeyboardModifiers const modifiers = static_cast<Qt::KeyboardModifiers>(regexp.cap(3).toInt(0, 16));

	VERBOSE(QString("Simulating keyboard %1, key=%2, modifiers=%3...\n")
			.arg(action, QString::number(static_cast<int>(key)), QString::number(static_cast<int>(modifiers), 16)));

	if (action == "press") {
//		Os::pressKey(key, modifiers);
	} else if (action == "release") {
//		Os::releaseKey(key, modifiers);
	}
}

void EntryProcessor::processGestureMovement()
{
	QRegExp const regexp("Mouse gesture movement to QPoint\\(([0-9|\\-]+),([0-9|\\-]+)\\)");
	if (!regexp.exactMatch(mEntry.message())) {
		return;
	}

	int const x = regexp.cap(1).toInt();
	int const y = regexp.cap(2).toInt();

	VERBOSE(QString("Moving mouse to pos=(%1,%2)...\n").arg(QString::number(x), QString::number(y)));

//	Os::moveMouse(QPoint(x, y), QString());
}
