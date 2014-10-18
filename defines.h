#pragma once

#include <stdio.h>

#define PRINT(x) printf(qPrintable(x))

#define VERBOSING

#ifdef VERBOSING
#define VERBOSE(x) PRINT(x)
#else
#define VERBOSE(x)
#endif

/// Represents the speed of the user actions reproducing.
enum class Speed
{
	/// The actions are reproduced with just the same intervals between them as user made.
	original = 0
	/// The actions are reproduced faster then user made them, but they still observable.
	, fast
	/// The actions are reproduced very fast, the intervals between them are just milliseconds.
	, fastest
};
