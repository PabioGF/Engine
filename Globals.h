#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <memory>


extern std::unique_ptr<std::vector<std::string>> logBuffer;

#define LOG(format, ...) engine_log(__FILE__, __LINE__, format, __VA_ARGS__);

void engine_log(const char file[], int line, const char* format, ...);
void ClearLogBuffer();

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Super Awesome Engine"