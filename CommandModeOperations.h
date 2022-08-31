#pragma once
#include "DefStructs.h"
#include "Constants.h"

short checkSymbol(char ch);
char toLowerCase(char ch);
short compareContentToStr(RowContent* cnt, char str[], short strMaxLen);
void redrawCmdText();

short cmFileTop(RowContent* args);
short cmLoadFile(RowContent* args);
short cmFileBottom(RowContent* args);
short cmMoveLine(RowContent* args);
short cmSetLine(RowContent* args);
short cmMoveCursor(RowContent* args);
short cmSetCursor(RowContent* args);
short cmDeleteChars(RowContent* args);
short cmDeleteLines(RowContent* args);
short cmEditMode(RowContent* args);
short cmSaveFile(RowContent* args);
short cmSaveAsFile(RowContent* args);
short cmQuit(RowContent* args);
short cmExit(RowContent* args);

//error success sintax
TypeCommand COMMANDMODE_COMMANDS[COMMANDMODE_COMMANDSIZE] = {
    { "filetop", cmFileTop , { "Cursor can not be moved", "Cursor was successfully moved", "No arguments are required" }, "" },
    { "filebottom", cmFileBottom, { "Cursor can not be moved", "Cursor was successfully moved", "No arguments are required" }, "" },
    { "loadfile", cmLoadFile, { "File cannot be found", "File is successfully loaded", "Invalid filename" }, "[An absolute file path]" },
    { "editmode", cmEditMode, { "", "The editor mode was successfully changed", "No arguments are required" }, "" },
    { "moveline", cmMoveLine, { "", "Current line was successfully moved", "Only integers are allowed" }, "[Amount of lines to move]" },
    { "setline", cmSetLine, { "The line cannot be found", "Current line was successfully changed", "Only integers are allowed" }, "[Number of line]" },
    { "movecursor", cmMoveCursor, { "", "Cursor was successfully moved", "Only integers are allowed" }, "[Amount of chars to move]" },
    { "setcursor", cmSetCursor, { "Invalid cursor position", "Cursor was successfully changed", "Only integers are allowed" }, "[Number of char]" },
    { "deletechars", cmDeleteChars, { "There is no chars to delete", "Chars was successfully deleted", "Only integers are allowed" }, "[Amount of chars to delete]" }, //????
    { "deletelines", cmDeleteLines, { "There is no lines to be deleted", "Lines was successfully deleted", "Only integers are allowed" }, "[Amount of lines to delete]" }, //????
    { "save", cmSaveFile, { "File cannot be saved", "File was successfully saved", "No arguments are required" }, "" },
    { "saveas", cmSaveAsFile, { "File cannot be saved", "File was successfully saved", "Invalid filename" }, "[An absolute file path]" },
    { "quit", cmQuit, { "", "", "No arguments are required" }, "" },
    { "exit", cmExit, { "", "", "Add <yes/no> at the end" }, "[Save the file:<yes/no>]" }
};

short cmExit(RowContent* args) {
    if (args == NULL) {
        return COMMANDMODE_SINTAX;
    }

    if (compareContentToStr(args, "yes", 0)) {
        saveFile(NULL);
    }
    else if (!compareContentToStr(args, "no", 0)) {
        return COMMANDMODE_SINTAX;
    }

    loop = 0;
    return COMMANDMODE_SUCCESS;
}

short cmQuit(RowContent* args) {
    if (args != NULL) {
        return COMMANDMODE_SINTAX;
    }

    loop = 0;
    return COMMANDMODE_SUCCESS;
}

short cmSaveFile(RowContent* args) {
    if (args != NULL) {
        return COMMANDMODE_SINTAX;
    }

    if (saveFile(NULL) == SAVEFILESUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmSaveAsFile(RowContent* args) {
    if (args == NULL) {
        return COMMANDMODE_SINTAX;
    }

    unsigned long size = getContentSize(args);
    if (size > FILEPATHMAXSIZE - 1) {
        return COMMANDMODE_SINTAX;
    }

    if (findInContent(args, ' ') != 0) {
        return COMMANDMODE_SINTAX;
    }

    if (findInContent(args, '.') == 0) {
        return COMMANDMODE_SINTAX;
    }

    char path[FILEPATHMAXSIZE];
    unsigned long index = 0;
    for (index = 0; index < size; index++) {
        path[index] = getContentByIndex(args, index)->symbol;
    }
    path[index] = '\0';

    if (saveFile(path) == SAVEFILESUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmEditMode(RowContent* args) {
    toggleMode();

    return COMMANDMODE_SUCCESS;
}

short cmDeleteLines(RowContent* args) {
    if (getRowsCount(DATA) == 0) {
        return COMMANDMODE_ERROR;
    }

    int amount;

    if (args == NULL)
    {
        amount = 1;
    }
    else {
        if (contentToInt(args, &amount) == 0) {
            return COMMANDMODE_SINTAX;
        }
    }

    int sign = (amount < 0 ? -1 : 1);
    amount *= sign;

    unsigned long rowsSize = getRowsCount(DATA);
    unsigned long delRows = (sign == 1 ? rowsSize - cursor.y : cursor.y);

    if (sign == -1) {
        if (moveCursorY(-1 * (amount - 1), REDRAW) == CM_ERROR) {
            if (setCursor(0, 0, REDRAW) == CM_ERROR) {
                return COMMANDMODE_ERROR;
            }
        }
    }

    
    if (setCursor(0, cursor.y, REDRAW) == CM_ERROR) {
        return COMMANDMODE_ERROR;
    }

    short oldTypeMode = typeMode;
    typeMode = OVERWRITE;
    for (unsigned long i = 0; i < delRows && i < (unsigned long)amount; i++) {
        unsigned long cntSize = getContentSize(getRowByIndex(DATA, cursor.y)->content);
        for (unsigned long j = 0; j <= cntSize; j++) {
            callCommand(CMD_DELETEAFTERCHAR, 0, 1);
        }
    }
    typeMode = oldTypeMode;

    if (sign == 1 && cursor.y == getRowsCount(DATA) - 1) {
        callCommand(CMD_DELETEBEFORECHAR, 0, 1);
    }

    return COMMANDMODE_SUCCESS;
}

short cmDeleteChars(RowContent* args) {
    if (getRowsCount(DATA) == 0) {
        return COMMANDMODE_ERROR;
    }

    int amount;

    if (args == NULL)
    {
        amount = 1;
    }
    else {
        if (contentToInt(args, &amount) == 0) {
            return COMMANDMODE_SINTAX;
        }
    }

    int sign = (amount < 0 ? -1 : 1);
    amount *= sign;

    unsigned long lineSize = getContentSize(getRowByIndex(DATA, cursor.y)->content);
    unsigned long delChars = (sign == 1 ? (lineSize - cursor.x) : cursor.x);
    short oldTypeMode = typeMode;
    typeMode = OVERWRITE;
    for (unsigned long i = 0; i < delChars && i < (unsigned int)amount; i++) {
        callCommand(sign == 1 ? CMD_DELETEAFTERCHAR : CMD_DELETEBEFORECHAR, 0, 1);
    }
    typeMode = oldTypeMode;

    return COMMANDMODE_SUCCESS;
}

short cmSetCursor(RowContent* args) {
    if (args == NULL)
    {
        return COMMANDMODE_SINTAX;
    }

    int coord;
    if (contentToInt(args, &coord) == 0) {
        return COMMANDMODE_SINTAX;
    }

    if ((unsigned long)coord < 1 || (unsigned long)coord > getContentSize(getRowByIndex(DATA, cursor.y)->content) + 1) {
        return COMMANDMODE_ERROR;
    }

    if (setCursor((long)(coord - 1), cursor.y, REDRAW) == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmMoveCursor(RowContent* args) {
    if (args == NULL)
    {
        return COMMANDMODE_SINTAX;
    }

    int amount;
    if (contentToInt(args, &amount) == 0) {
        return COMMANDMODE_SINTAX;
    }

    int sign = (amount < 0 ? -1 : 1);
    amount *= sign;

    unsigned long size = getContentSize(getRowByIndex(DATA, cursor.y)->content);
    if (sign == 1 && cursor.x + (unsigned long)amount > size) {
        amount = (int)(size - cursor.x);
    }
    else if (sign == -1 && (unsigned long)amount > cursor.x) {
        amount = (int)cursor.x;
    }

    if (moveCursorX((long)(amount * sign), REDRAW) == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmMoveLine(RowContent* args) {
    if (args == NULL)
    {
        return COMMANDMODE_SINTAX;
    }

    int amount;
    if (contentToInt(args, &amount) == 0) {
        return COMMANDMODE_SINTAX;
    }

    int sign = (amount < 0 ? -1 : 1);
    amount *= sign;

    unsigned long size = getRowsCount(DATA);
    if (sign == 1 && cursor.y + (unsigned long)amount > size) {
        amount = (int)(size - cursor.y - 1);
    }
    else if (sign == -1 && (unsigned long)amount > cursor.y) {
        amount = (int)cursor.y;
    }

    if (moveCursorY((long)(amount * sign), REDRAW) == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmSetLine(RowContent* args) {
    if (args == NULL)
    {
        return COMMANDMODE_SINTAX;
    }

    int coord;
    if (contentToInt(args, &coord) == 0) {
        return COMMANDMODE_SINTAX;
    }

    if ((unsigned long) coord < 1 || (unsigned long)coord > getRowsCount(DATA)) {
        return COMMANDMODE_ERROR;
    }

    if (setCursor(cursor.x, (unsigned long)(coord - 1), REDRAW) == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmFileBottom(RowContent* args) {
    if (args != NULL) {
        return COMMANDMODE_SINTAX;
    }

    if (moveCursorToFileEnd() == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmLoadFile(RowContent* args) {
    if (args == NULL) {
        return COMMANDMODE_SINTAX;
    }

    unsigned long size = getContentSize(args);
    if (size > FILEPATHMAXSIZE - 1) {
        return COMMANDMODE_SINTAX;
    }

    if (findInContent(args, '.') == 0) {
        return COMMANDMODE_SINTAX;
    }

    char path[FILEPATHMAXSIZE];
    unsigned long index = 0;
    for (index = 0; index < size; index++) {
        path[index] = getContentByIndex(args, index)->symbol;
    }
    path[index] = '\0';

    if (loadFile(path) == LOADFILESUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short cmFileTop(RowContent* args) {
    if (args != NULL) {
        return COMMANDMODE_SINTAX;
    }

    if (setCursor(0, 0, REDRAW) == CM_SUCCESS) {
        return COMMANDMODE_SUCCESS;
    }

    return COMMANDMODE_ERROR;
}

short compareContentToStr(RowContent* cnt, char str[], short strMaxLen) {
    if (strMaxLen == 0) {
        strMaxLen = COMMANDMODE_COMMANDMAXSIZE;
    }

    short strLen = 0;
    for (strLen = 0; strLen < strMaxLen; strLen++) {
        if (str[strLen] == '\0') {
            break;
        }
    }

    short size = (short)getContentSize(cnt);
    if (size != strLen) {
        return 0;
    }

    for (short i = 0; i < strLen; i++) {
        if (toLowerCase(getContentByIndex(cnt, i)->symbol) != toLowerCase(str[i])) {
            return 0;
        }
    }

    return 1;
}

short checkSymbol(char ch) {
    for (short i = 0; i < COMMANDMODE_ALLOWEDSYMBOLSSIZE; i++) {
        if (toLowerCase(ch) == COMMANDMODE_ALLOWEDSYMBOLS[i]) {
            return 1;
        }
    }

    return 0;
}

char toLowerCase(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ch + ('a' - 'A');
    }

    return ch;
}

void redrawCmdText() {
    COORD p;
    p.Y = (short)visibleSize.height + 3;
    p.X = 3;
    SetConsoleCursorPosition(consoleHandle, p);
    SetConsoleTextAttribute(consoleHandle, 10);
    p.X += printf("COMMAND: ");

    unsigned long contentSize = getContentSize(enterCmd->content);
    for (unsigned long i = 0; i < consoleSize.width - 20; i++) {
        SetConsoleCursorPosition(consoleHandle, p);

        SetConsoleTextAttribute(consoleHandle, i == contentSize ? 22 : 10);

        if (i < contentSize) {
            printf("%c", getContentByIndex(enterCmd->content, i)->symbol);
        }
        else {
            printf(" ");
        }
        p.X++;
    }

    p.Y += 1;
    p.X = 7;
    SetConsoleCursorPosition(consoleHandle, p);
    p.X += printf("TIP: ");
    SetConsoleCursorPosition(consoleHandle, p);
    short printedSize = 0;
    if (lastFeedback == NULL) {
        printedSize += printf("...");
    }
    else {
        short size = (short)getContentSize(lastFeedback);
        for (printedSize = 0; printedSize < size; printedSize++) {
            printf("%c", getContentByIndex(lastFeedback, (unsigned long)printedSize)->symbol);
        }
    }

    for (printedSize; printedSize < COMMANDMODE_FEEDBACKMAXSIZE; printedSize++) {
        printf(" ");
    }
}