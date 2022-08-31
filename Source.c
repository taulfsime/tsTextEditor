#define _CRT_SECURE_NO_WARNINGS

#include <windows.h> 
#include <stdio.h>
#include <winuser.h>
#include "Constants.h"
#include "DefStructs.h"
#include "Row.h"
#include "FileContentOperations.h"
#include "ChangesHandler.h"

void handleScreenSize();
short loadFile(char path[]);
short saveFile(char path[]);
void handleEditMode(char ch);
void handleCmdMode(char ch);
void handleInputCommandKeys(char type);
void callCommand(short cmd, char symbol, short add);
void toggleMode();
void setFilepath(char path[]);
void updateVisible(short cls);

#include "CommandModeOperations.h"

int main() {
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    DATA = init();
    enterCmd = createRow(NULL);

    while (loop == 1) {
        handleScreenSize();

        COORD p = { 0, 0 };
        SetConsoleCursorPosition(consoleHandle, p);

        if (kbhit()) {
            char ch = getch();
            if (editorMode == MODE_EDIT) {
                handleEditMode(ch);
            }
            else {
                handleCmdMode(ch);
            }

            drawBorder(ONLYINFO);
        }
    }

    clearRowAndContent(enterCmd);
    clearAllRows(DATA);
    free(DATA);

    return 0;
}

void handleCmdMode(char ch) {
    switch (ch) {
    case 8: //backspace
        if (enterCmd->content != NULL) {
            unsigned long size = getContentSize(enterCmd->content);
            if (size > 0) {
                removeSingleContentFromRow(enterCmd, size - 1);

                if (size == 1) {
                    clearContent(lastFeedback);
                    lastFeedback = NULL;
                }
            }
        }
        break;

    case 13: {
        RowContent* cmdText = NULL;
        RowContent* cmdArgs = NULL;

        for (short i = 0; i < COMMANDMODE_COMMANDSIZE; i++) {
            unsigned long cmdTextEndIndex = findInContent(enterCmd->content, ' ');
            cmdText = getSubcontent(enterCmd->content, 0, cmdTextEndIndex - 1);
            cmdArgs = (cmdTextEndIndex == 0 ? NULL : getSubcontent(enterCmd->content, cmdTextEndIndex + 1, 0));

            if (compareContentToStr(cmdText == NULL ? enterCmd->content : cmdText, COMMANDMODE_COMMANDS[i].cmdStr, 0) == 1) {
                short feedback = COMMANDMODE_COMMANDS[i].cmdFunc(cmdArgs);

                clearContent(lastFeedback);
                lastFeedback = createContent(COMMANDMODE_COMMANDS[i].feedbackTexts[feedback]);
                break;
            }
        }

        clearContent(cmdText);
        clearContent(cmdArgs);
        clearContent(enterCmd->content);
        enterCmd->content = NULL;

        break;
    }

    case -32:
    case 0:
        getch();
        break;

    case '\t':
        for (short i = 0; i < COMMANDMODE_COMMANDSIZE; i++) {
            unsigned long cmdTextEndIndex = findInContent(enterCmd->content, ' ');
            RowContent* cmdText = getSubcontent(enterCmd->content, 0, cmdTextEndIndex == 0 ? 0 : cmdTextEndIndex - 1);
            short cmdTextSize = (short)getContentSize(cmdText);

            if (cmdText != NULL) {
                if (compareContentToStr(cmdText, COMMANDMODE_COMMANDS[i].cmdStr, cmdTextSize) == 1) {
                    setRowContent(enterCmd, createContent(COMMANDMODE_COMMANDS[i].cmdStr));
                    break;
                }
                else {
                    clearContent(cmdText);
                }
            }
        }
        break;

    default:
        if (checkSymbol(ch) == 1) {
            if (enterCmd->content == NULL) {
                setRowContent(enterCmd, createSingleContent(ch));
            }
            else {
                appendContent(enterCmd->content, createSingleContent(ch));
            }
        }
        break;
    }

    for (short i = 0; i < COMMANDMODE_COMMANDSIZE; i++) {
        unsigned long cmdTextEndIndex = findInContent(enterCmd->content, ' ');
        RowContent* cmdText = getSubcontent(enterCmd->content, 0, cmdTextEndIndex == 0 ? 0 : cmdTextEndIndex - 1);
        short cmdTextSize = (short)getContentSize(cmdText);

        if (cmdText != NULL) {
            if (compareContentToStr(cmdText, COMMANDMODE_COMMANDS[i].cmdStr, cmdTextSize) == 1) {
                char feedbackText[COMMANDMODE_FEEDBACKMAXSIZE];
                short feedbackIndex = 0;

                for (int j = 0; COMMANDMODE_COMMANDS[i].cmdStr[j] != '\0' && feedbackIndex < COMMANDMODE_FEEDBACKMAXSIZE; j++) {
                    feedbackText[feedbackIndex++] = COMMANDMODE_COMMANDS[i].cmdStr[j];
                }

                feedbackText[feedbackIndex++] = ' ';

                for (int j = 0; COMMANDMODE_COMMANDS[i].cmdHelp[j] != '\0' && feedbackIndex < COMMANDMODE_FEEDBACKMAXSIZE; j++) {
                    feedbackText[feedbackIndex++] = COMMANDMODE_COMMANDS[i].cmdHelp[j];
                }

                feedbackText[feedbackIndex] = '\0';

                clearContent(lastFeedback);
                lastFeedback = createContent(feedbackText);
                break;
            }
            else {
                clearContent(lastFeedback);
                lastFeedback = createContent("Unknown command");
            }
        }

        clearContent(cmdText);
    }
    redrawCmdText();
}

void handleEditMode(char ch) {
    switch (ch) {
    case 0: //special keys
    case -32: //special keys
        handleInputCommandKeys(getch());
        break;

    case 27: //escape
        toggleMode();
        break;

    case 8: //backspace
        callCommand(CMD_DELETEBEFORECHAR, 0, 1);
        break;

    case 19://ctrl+s 
        saveFile(NULL);
        break;

    case 26://ctrl+z
        callCommand(CMD_UNDO, 0, 0);
        break;

    case '\n': //enter
    case '\t': //tab
    default: //write symbol
        if (getRowsCount(DATA) == 0) {
            appendNewRow(DATA, NULL);
        }

        callCommand(CMD_INSERTCHAR, ch, 1);
        break;
    }
}

void handleInputCommandKeys(char type) {
    switch (type) {
    case 72://arrow up
        moveCursorY(-1, REDRAW);
        break;

    case 77://arrow right
        moveCursorX(1, REDRAW);
        break;

    case 80://arrow down
        moveCursorY(1, REDRAW);
        break;

    case 75://arrow left
        moveCursorX(-1, REDRAW);
        break;

    case 82: //insert
        typeMode = (typeMode == INSERT ? OVERWRITE : INSERT);
        break;

    case 83://delete
        callCommand(CMD_DELETEAFTERCHAR, 0, 1);
        break;

    case 73://pageup
        if (lastInput == 73) {
            setCursor(0, 0, REDRAW);
        }
        else {
            setScrollOffsetY(scrollOffset.y - visibleSize.height);
            moveCursorY(-1 * visibleSize.height, REDRAW);
        }
        break;

    case 81://pagedown
        if (lastInput == 81) {
            moveCursorToFileEnd();
        }
        else {
            setScrollOffsetY(scrollOffset.y + visibleSize.height);
            moveCursorY(visibleSize.height, REDRAW);
        }
        break;

    case 71: //home
        goToHome();
        break;

    case 79://end
        goToEnd();
        break;
    }

    lastInput = type;
}

void toggleMode() {
    editorMode = (editorMode == MODE_COMMANDS ? MODE_EDIT : MODE_COMMANDS);
    updateVisible(CLS);

    if (editorMode == MODE_COMMANDS) {
        redrawCmdText();
    }
}

void callCommand(short cmd, char symbol, short add) {
    ConsolePoint cursorPos = cursor;

    switch (cmd) {
    case CMD_INSERTCHAR:
    case CMD_INSERTCHARAFTER:
    case CMD_OVERWRITECHAR:
        if (symbol == 13) { //enter
            insertNewLine();
            symbol = 13;
        }
        else if (symbol == '\t') {//tab
            char replacedChar;
            for (int i = 0; i < 3; i++) {
                replacedChar = writeSymbol(' ', (cmd == CMD_INSERTCHAR ? CM_MOVE : CM_NOMOVE));
            }

            if (typeMode == OVERWRITE) {
                symbol = replacedChar;
            }
        }
        else {
            char replacedChar = writeSymbol(symbol, (cmd == CMD_INSERTCHAR ? CM_MOVE : CM_NOMOVE));
            if (typeMode == OVERWRITE) {
                symbol = replacedChar;
            }
        }

        if (typeMode == OVERWRITE) {
            cmd = (symbol == '\0' ? CMD_INSERTCHAR : CMD_OVERWRITECHAR);
        }
        break;

    case CMD_DELETEAFTERCHAR:
        if (symbol == '\t') {
            for (int i = 0; i < 3; i++) {
                deleteAfter();
            }
        }
        else {
            symbol = deleteAfter();
        }
        cursorPos = cursor; //update Cursor pos
        break;

    case CMD_DELETEBEFORECHAR:
        if (symbol == '\t') {
            for (int i = 0; i < 3; i++) {
                deleteBefore();
            }
        }
        else {
            symbol = deleteBefore();
        }
        cursorPos = cursor; //update Cursor pos
        break;

    case CMD_UNDO: {
        Change lastChange = getLastChange();

        if (lastChange.command != CMD_NONE) {
            if (setCursor(lastChange.cursorPos.x, lastChange.cursorPos.y, REDRAW) == CM_SUCCESS) {
                short oldTypeMode = typeMode;
                if (lastChange.command == CMD_OVERWRITECHAR) {
                    typeMode = OVERWRITE;
                }

                short oppositeCmd = getOppositeCommand(lastChange.command);

                if (oppositeCmd != CMD_NONE) {
                    callCommand(oppositeCmd, lastChange.symbol, 0);
                }

                typeMode = oldTypeMode;
            }
        }
        break;
    }
    }

    if (add == 1) {
        addChange(cursorPos, cmd, symbol);
    }
}

void handleScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBuffer;
    GetConsoleScreenBufferInfo(consoleHandle, &consoleScreenBuffer);

    ScreenSize newSize;
    newSize.width = consoleScreenBuffer.srWindow.Right - consoleScreenBuffer.srWindow.Left + 1;
    newSize.height = consoleScreenBuffer.srWindow.Bottom - consoleScreenBuffer.srWindow.Top + 1;

    if (newSize.width != consoleSize.width || newSize.height != consoleSize.height) {
        consoleSize.width = newSize.width;
        consoleSize.height = newSize.height;

        updateVisible(CLS);
    }
}

void updateVisible(short cls) {
    //6 -> left +  1 -> right = 7
    //1 -> top +  1 -> bottom = 2

    int menuHeight = (editorMode == MODE_EDIT ? 0 : COMMANDMODE_MENUHEIGHT);

    visibleSize.width = consoleSize.width - 7;
    visibleSize.height = consoleSize.height - 2 - menuHeight;

    updateMaxScrollY();
    updateMaxScrollX();

    if (cls == CLS) {
        system("cls");
    }
    drawBorder(ALLSIDES);
    redrawText(-1, ALLLINE);
    if (editorMode == MODE_COMMANDS) {
        redrawCmdText();
    }
}


short loadFile(char path[]) {
    FILE* test = fopen(path, "r");
    if (test == NULL) {
        return LOADFILEERROR;
    }

    setFilepath(path);

    clearAllRows(DATA);
    appendNewRow(DATA, NULL);

    unsigned long cLine = 0;
    char ch = ' ';
    while (fscanf(test, "%c", &ch) >= 0) {
        if (ch == '\n') {
            appendNewRow(DATA, NULL);
            cLine += 1;
        }
        else {
            Row* row = getRowByIndex(DATA, cLine);
            if (row->content == NULL) {
                setRowContent(row, createSingleContent(ch));
            }
            else {
                appendContent(row->content, createSingleContent(ch));
            }
        }
    }

    updateVisible(TXT);

    fclose(test);
    return LOADFILESUCCESS;
}

short saveFile(char path[]) {
    if (path == NULL) {
        path = loadedFilePath;
    }

    FILE* file = fopen(path, "w+");
    if (file == NULL) {
        return SAVEFILEERROR;
    }

    for (unsigned long y = 0; y < getRowsCount(DATA); y++) {
        Row* row = getRowByIndex(DATA, y);
        for (unsigned long x = 0; x < getContentSize(row->content); x++) {
            fprintf(file, "%c", getContentByIndex(row->content, x)->symbol);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    return SAVEFILESUCCESS;
}

void setFilepath(char path[]) {
    for (int i = 0; i < FILEPATHMAXSIZE; i++) {
        loadedFilePath[i] = path[i];
    }
}