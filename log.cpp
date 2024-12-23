#pragma once
#include "Globals.h"
#include <vector>
#include <string>
#include <cstdarg>
#include <cstdio>

std::unique_ptr<std::vector<std::string>> logBuffer = std::make_unique<std::vector<std::string>>();

void engine_log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	logBuffer->push_back(tmp_string2);

	std::memset(tmp_string, 0, sizeof(tmp_string));
	std::memset(tmp_string2, 0, sizeof(tmp_string2));

	const size_t max_logs = 500;
	if (logBuffer->size() > max_logs) {
		logBuffer->erase(logBuffer->begin());
	}
}

void ClearLogBuffer()
{
	logBuffer->clear();
	logBuffer->shrink_to_fit();
}