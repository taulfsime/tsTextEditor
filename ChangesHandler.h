#pragma once
#include "DefStructs.h"
#include "Constants.h"

void addChange(ConsolePoint pos, short cmd, char symbol);
void moveChangesBack();
Change getLastChange();
short getOppositeCommand(short cmd);

//FUNCTIONS

short getOppositeCommand(short cmd) {
	const short COMMANDS[][2] = {
		{ CMD_INSERTCHAR, CMD_DELETEAFTERCHAR },
		{ CMD_DELETEAFTERCHAR, CMD_INSERTCHARAFTER },
		{ CMD_DELETEBEFORECHAR, CMD_INSERTCHAR },
		{ CMD_OVERWRITECHAR, CMD_OVERWRITECHAR }
	};

	for (int i = 0; i < 4; i++) {
		if (COMMANDS[i][0] == cmd) {
			return COMMANDS[i][1];
		}
	}

	return CMD_NONE;
}

void addChange(ConsolePoint pos, short cmd, char symbol) {
	if (cmd == CMD_UNDO || cmd == CMD_REDO || cmd == CMD_NONE) {
		return;
	}

	if (changesCount < 0) {
		changesCount = 0;
	}

	if (changesCount == CHANGESSIZE - 1) {
		moveChangesBack();
	}

	changes[changesCount].command = cmd;
	changes[changesCount].symbol = symbol;
	changes[changesCount].cursorPos = pos;

	if (changesCount != CHANGESSIZE - 1) {
		changesCount += 1;
	}
}

Change getLastChange() {
	if (changesCount >= 0) {
		return changes[--changesCount];
	}

	Change empty;
	empty.command = CMD_NONE;
	return empty;
}

void moveChangesBack() {
	for (int i = 0; i < changesCount; i++) {
		changes[i] = changes[i + 1];
	}
}

