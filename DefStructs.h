#pragma once

#include "Constants.h"

typedef struct ScreenSize {
    unsigned long width;
    unsigned long height;
} ScreenSize;

typedef struct ConsolePoint {
    unsigned long x;
    unsigned long y;
} ConsolePoint;

typedef struct Change {
	short command;
	char symbol;
	ConsolePoint cursorPos;
    short typeMode;
} Change;

typedef struct RowContent {
    char symbol;
    struct RowContent* next;
    struct RowContent* prev;
} RowContent;

typedef struct Row {
    RowContent* content;
    struct Row* prevRow;
    struct Row* nextRow;
} Row;

typedef struct TypeCommand {
    char cmdStr[COMMANDMODE_COMMANDMAXSIZE];
    short (*cmdFunc)(RowContent* args);
    char feedbackTexts[COMMANDMODE_FEEDBACKSIZE][COMMANDMODE_FEEDBACKMAXSIZE];
    char cmdHelp[COMMANDMODE_FEEDBACKMAXSIZE];
} TypeCommand;

HANDLE consoleHandle;
ScreenSize consoleSize;
ScreenSize visibleSize;
Row* DATA;
ConsolePoint scrollOffset = { 0, 0 };
ConsolePoint maxScrollOffset = { 0, 0 };
ConsolePoint cursor = { 0, 0 };
short typeMode = INSERT;
char lastInput;
short editorMode = MODE_COMMANDS;
short loop = 1;
Change changes[CHANGESSIZE];
int changesCount = 0;
Row* enterCmd;
char loadedFilePath[FILEPATHMAXSIZE];
RowContent* lastFeedback = NULL;