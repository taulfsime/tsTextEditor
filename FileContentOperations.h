#pragma once

#include "DefStructs.h"
#include "Constants.h"
#include "Row.h"
#include <stdio.h>
#include <windows.h>

short setCursor(unsigned long newX, unsigned long newY, short redraw);
short moveCursorX(long addX, short redraw);
short moveCursorY(long addY, short redraw);
unsigned long getLongestVisibleLineSize();
char writeSymbol(char sym, short moveCursor);
void goToHome();
void goToEnd();
void insertNewLine();
char deleteBefore();
char deleteAfter();
short moveCursorToFileEnd();
void setScrollOffsetY(unsigned long y);
void setScrollOffsetX(unsigned long x);
void updateMaxScrollY();
void updateMaxScrollX();
void redrawText(long line, short mode);
void drawBorder(short drawAllSides);

void redrawText(long line, short mode) {
    //draw text
    unsigned long rowsLength = getRowsCount(DATA);

    for (unsigned long y = 0; y < visibleSize.height; y++) {
        unsigned long rowsCount = scrollOffset.y + y;

        if (line != -1 && line != rowsCount) {
            continue;
        }

        RowContent* lineCnt = NULL;
        if (rowsCount < rowsLength) {
            lineCnt = getRowByIndex(DATA, rowsCount)->content;
        }

        COORD p;

        //draw line number
        if (cursor.y == rowsCount) {
            SetConsoleTextAttribute(consoleHandle, 143 - 16);
        }
        else {
            SetConsoleTextAttribute(consoleHandle, 143);
        }
        p.X = 1;

        p.Y = (short)y + 1;
        SetConsoleCursorPosition(consoleHandle, p);
        if (rowsCount < rowsLength) {
            printf("%4d ", rowsCount + 1);
        }
        else {
            printf("   ~ ");
        }

        unsigned long lineSize = getContentSize(lineCnt);

        if (scrollOffset.x > lineSize) {
            continue;
        }

        //write text
        for (unsigned long x = 0; (x <= lineSize || x < 1 || mode == ALLLINE) && x < visibleSize.width; x++) {
            unsigned long symbolsCount = scrollOffset.x + x;

            if (cursor.x == symbolsCount && cursor.y == rowsCount) {
                SetConsoleTextAttribute(consoleHandle, 100);
            }
            else {
                SetConsoleTextAttribute(consoleHandle, 10);
            }

            p.X = (short)x + 1 + 5 ;
            SetConsoleCursorPosition(consoleHandle, p);

            if (rowsCount > rowsLength) {
                break;
            }

            if (rowsCount >= rowsLength || symbolsCount >= lineSize) {
                printf(" ");   
            }
            else {
                printf("%c", getContentByIndex(lineCnt, symbolsCount)->symbol);
                symbolsCount += 1;
            }
        }
    }
}

void drawBorder(short drawAllSides) {
    char ch = '*';

    SetConsoleTextAttribute(consoleHandle, 79);

    unsigned long yEnd = visibleSize.height + 1;

    COORD p;
    if (drawAllSides == ALLSIDES) {
        //left
        for (unsigned long i = 0; i < yEnd; i++) {
            p.X = 0;
            p.Y = (short)i;
            SetConsoleCursorPosition(consoleHandle, p);
            putchar(ch);
        }

        //right
        for (unsigned long i = 0; i < yEnd; i++) {
            p.X = (short)consoleSize.width - 1;
            p.Y = (short)i;
            SetConsoleCursorPosition(consoleHandle, p);
            printf("%c", ch);
        }

        //top
        p.Y = 0;
        p.X = 0;
        SetConsoleCursorPosition(consoleHandle, p);

        for (unsigned long i = 0; i < consoleSize.width; i++) {
            printf("%c", ch);
        }
    }

    //bottom
    p.Y = (short)yEnd;
    p.X = 0;
    SetConsoleCursorPosition(consoleHandle, p);

    for (unsigned long i = 0; i < 10; i++) {
        printf("%c", ch);
    }
    p.X += 10;

    p.X += printf(" ");

    //draw cursor pos
    SetConsoleCursorPosition(consoleHandle, p);
    p.X += printf("Cursor: ");
    p.X += printf("%d/%d", cursor.x + 1, cursor.y + 1);

    p.X += printf("   ");

    //draw scroll offset
    p.X += printf("Scroll: ");
    p.X += printf("%d/%d:%d/%d", scrollOffset.x, maxScrollOffset.x, scrollOffset.y, maxScrollOffset.y);

    //draw scroll offset
    p.X += printf("   ");
    p.X += printf("LineSize: ");
    unsigned long size = 0;
    if (getRowsCount(DATA) > cursor.y) {
        size = getContentSize(getRowByIndex(DATA, cursor.y)->content);
    }

    p.X += printf("%d", size);

    if (editorMode == MODE_EDIT) {
        //draw type mode
        p.X += printf("   ");
        p.X += printf("TypeMode: ");
        p.X += printf("%s", typeMode == OVERWRITE ? "OVERWRITE" : "INSERT");

        //draw number of undos
        p.X += printf("   ");
        p.X += printf("UNDO: ");
        p.X += printf("%d", changesCount + 1);
    }
    
    p.X += printf(" ");

    for (unsigned long i = 0; i < consoleSize.width - p.X; i++) {
        printf("%c", ch);
    }

    SetConsoleTextAttribute(consoleHandle, 10);
}

void updateMaxScrollY() {
    unsigned long rowsCount = getRowsCount(DATA);
    if (rowsCount > 0) {
        maxScrollOffset.y = rowsCount - 1;
    }
    else {
        maxScrollOffset.y = 0;
    }
}

void updateMaxScrollX() {
    unsigned long lognestSize = getLongestVisibleLineSize();

    if (lognestSize <= visibleSize.width) {
        maxScrollOffset.x = lognestSize;
    }
    else {
        maxScrollOffset.x = lognestSize - visibleSize.width;
    }
}


void goToHome() {
    if (cursor.y >= getRowsCount(DATA)) {
        return;
    }

    //find first non space char
    unsigned long nonSpaceIndex = 0;

    Row* currentRow = getRowByIndex(DATA, cursor.y);
    if (currentRow != NULL) {
        unsigned long rowSize = getContentSize(currentRow->content);
        for (unsigned long i = 0; i < rowSize; i++) {
            if (getContentByIndex(currentRow->content, i)->symbol != ' ') {
                nonSpaceIndex = i;
                break;
            }
        }
    }

    setCursor(cursor.x == nonSpaceIndex ? 0 : nonSpaceIndex, cursor.y, NOREDRAW);
    redrawText(cursor.y, ROWSIZE);
}

void goToEnd() {
    unsigned long lastIndex = 0;

    Row* currentRow = getRowByIndex(DATA, cursor.y);

    if (currentRow != NULL) {
        lastIndex = getContentSize(currentRow->content);
    }

    setCursor(lastIndex, cursor.y, NOREDRAW);
    redrawText(cursor.y, ROWSIZE);
}

void setScrollOffsetY(unsigned long y) {
    if (y > maxScrollOffset.y) {
        return;
    }

    scrollOffset.y = y;
    updateMaxScrollX();

    redrawText(-1, ALLLINE);
}

void setScrollOffsetX(unsigned long x) {
    if (x > maxScrollOffset.x) {
        return;
    }

    scrollOffset.x = x;
    redrawText(-1, ALLLINE);
}

short moveCursorToFileEnd() {
    unsigned long rowsCount = getRowsCount(DATA);

    if (rowsCount > 0) {
        return setCursor(getContentSize(getRowByIndex(DATA, rowsCount - 1)->content), rowsCount - 1, REDRAW);
    }

    return CM_ERROR;
}

void insertNewLine() {
    unsigned long rowsSize = getRowsCount(DATA);

    Row* currentRow = getRowByIndex(DATA, cursor.y);
    if (currentRow != NULL) {
        unsigned long contentSize = getContentSize(currentRow->content);
        RowContent* afterCursor = NULL;

        //end of the text
        if (cursor.x == contentSize) {
            if (cursor.y == rowsSize - 1) {
                appendNewRow(DATA, NULL);
            }
            else {
                insertNewRow(DATA, cursor.y + 1, NULL);
            }
        }
        else {
            afterCursor = getSubcontent(currentRow->content, cursor.x, 0);
            removeContentFromRow(currentRow, cursor.x, 0);
            if (cursor.y == rowsSize - 1) {
                appendNewRow(DATA, afterCursor);
            }
            else {
                insertNewRow(DATA, cursor.y + 1, afterCursor);
            }
        }

        setCursor(0, cursor.y + 1, NOREDRAW);
        redrawText(-1, ALLLINE);
        updateMaxScrollY();
    }
}

char deleteBefore() {
    if (cursor.x == 0) {
        Row* currentRow = getRowByIndex(DATA, cursor.y);

        if (currentRow != NULL && cursor.y != 0) {
            RowContent* cnt = getContentFromRow(currentRow);

            removeRow(DATA, cursor.y);

            if (moveCursorX(-1, NOREDRAW) == CM_SUCCESS) {
                unsigned long newLineSize = getContentSize(getRowByIndex(DATA, cursor.y)->content);

                if (cnt != NULL) {
                    Row* row = getRowByIndex(DATA, cursor.y);
                    if (row->content == NULL) {
                        setRowContent(row, cnt);
                    }
                    else {
                        appendContent(row->content, cnt);
                    }
                }

                redrawText(-1, ALLLINE);
            }
            updateMaxScrollY();

            return 13;
        }
    }
    else {
        Row* currentRow = getRowByIndex(DATA, cursor.y);
        if (currentRow != NULL) {
            unsigned long contentSize = getContentSize(currentRow->content);
            char removedChar = '\0';

            if (moveCursorX(-1, NOREDRAW) == CM_SUCCESS) {
                removedChar = getContentByIndex(currentRow->content, cursor.x)->symbol;
                removeSingleContentFromRow(currentRow, cursor.x);
                redrawText(cursor.y, ALLLINE);
            }

            return removedChar;
        }
    }

    return '\0';
}

char deleteAfter() {
    Row* currentRow = getRowByIndex(DATA, cursor.y);
    if (currentRow == NULL) {
        return '\0';
    }

    unsigned long contentSize = getContentSize(currentRow->content);
    if (cursor.x == contentSize) {
        unsigned long rowsSize = getRowsCount(DATA);
        if (cursor.y == rowsSize - 1) {
            return 0;
        }

        RowContent* cnt = getContentFromRow(getRowByIndex(DATA, cursor.y + 1));
        removeRow(DATA, cursor.y + 1);
        if (currentRow->content == NULL) {
            setRowContent(currentRow, cnt);
        }
        else {
            appendContent(currentRow->content, cnt);
        }

        redrawText(-1, ALLLINE);
        updateMaxScrollY();

        return 13;
    }
    else {
        char charToRemove = getContentByIndex(currentRow->content, cursor.x)->symbol;
        removeSingleContentFromRow(currentRow, cursor.x);
        redrawText(cursor.y, ALLLINE);

        return charToRemove;
    }
}

char writeSymbol(char sym, short moveCursor) {
    if (cursor.y >= getRowsCount(DATA)) {
        return '\0';
    }

    char replacedChar = '\0';

    Row* currentRow = getRowByIndex(DATA, cursor.y);
    unsigned long lineSize = getContentSize(currentRow->content);

    if (cursor.x < lineSize) {
        if (typeMode == INSERT) {
            insertIntoRowContentByIndex(currentRow, cursor.x, createSingleContent(sym));
        }
        else {
            replacedChar = getContentByIndex(currentRow->content, cursor.x)->symbol;
            setSymbolInContent(currentRow->content, cursor.x, sym);
        }
    }
    else {
        if (currentRow->content == NULL) {
            setRowContent(currentRow, createSingleContent(sym));
        }
        else {
            appendContent(currentRow->content, createSingleContent(sym));
        }
    }

    if (moveCursor == CM_MOVE) {
        moveCursorX(1, REDRAW);
    }
    else if(moveCursor == CM_NOMOVE){
        redrawText(cursor.y, ALLLINE);
    }

    return replacedChar;
}

unsigned long getLongestVisibleLineSize() {
    unsigned long longestLineSize = 0;

    for (unsigned long i = 0; i < visibleSize.height && i < getRowsCount(DATA) - scrollOffset.y; i++) {
        unsigned long lineSize = getContentSize(getRowByIndex(DATA, i + scrollOffset.y)->content);
        if (longestLineSize < lineSize) {
            longestLineSize = lineSize;
        }
    }

    return longestLineSize;
}

short setCursor(unsigned long newX, unsigned long newY, short redraw) {
    long y = (long)(newY - cursor.y);

    if (moveCursorY(y, redraw) != CM_SUCCESS) {
        return CM_ERROR;
    }

    long x = (long)(newX - cursor.x);

    if (moveCursorX(x, redraw) != CM_SUCCESS) {
        return CM_ERROR;
    }

    return CM_SUCCESS;
}

short moveCursorX(long addX, short redraw) {
    if (addX != 0) {

        Row* currentRow = NULL;
        unsigned long rowSize;
        unsigned long newX = cursor.x + addX;

        if (addX < 0 && cursor.x == 0) {
            if (moveCursorY(-1, NOREDRAW) != CM_SUCCESS) {
                return CM_ERROR;
            }

            currentRow = getRowByIndex(DATA, cursor.y);
            if (currentRow == NULL) {
                return CM_ERROR;
            }
            else {
                rowSize = getContentSize(currentRow->content);
                if (rowSize == 0) {
                    newX = 0;
                }
                else if (newX >= rowSize) {
                    newX = rowSize;
                }
            }
        }
        else {
            currentRow = getRowByIndex(DATA, cursor.y);
            if (currentRow == NULL) {
                return CM_ERROR;
            }
            else {
                rowSize = getContentSize(currentRow->content);
                if (newX > rowSize) {
                    if (moveCursorY(1, NOREDRAW) != CM_SUCCESS) {
                        return CM_ERROR;
                    }

                    newX = 0;
                }
            }
        }

        if (newX < scrollOffset.x) {
            setScrollOffsetX(newX);
        }
        else {
            unsigned long screenEndX = scrollOffset.x + visibleSize.width - 1;
            if (newX > screenEndX) {
                setScrollOffsetX(scrollOffset.x + (newX - screenEndX));
            }
        }

        cursor.x = newX;
    }

    if (redraw == REDRAW) {
        redrawText(cursor.y, ALLLINE);
        redrawText(cursor.y + (addX > 0 ? -1 : 1), ALLLINE);
    }

    return CM_SUCCESS;
}

short moveCursorY(long addY, short redraw) {
    if (addY == 0) {
        return CM_SUCCESS;
    }

    unsigned long newY = cursor.y + addY;
    unsigned long rowsCount = getRowsCount(DATA);

    if (newY >= rowsCount || cursor.y > rowsCount) {
        return CM_ERROR;
    }

    if ((addY < 0 && cursor.y == 0) || (newY >= rowsCount)) {
        return CM_ERROR;
    }

    if (newY < scrollOffset.y) {
        setScrollOffsetY(newY);
    }
    else {
        unsigned long screenEndY = scrollOffset.y + visibleSize.height - 1;
        if (newY > screenEndY) {
            setScrollOffsetY(scrollOffset.y + (newY - screenEndY));
        }
    }

    Row* currentRow = getRowByIndex(DATA, newY);
    if (currentRow == NULL) {
        return CM_ERROR;
    }
    else {
        unsigned long rowSize = getContentSize(currentRow->content);
        if (rowSize == 0) {
            cursor.x = 0;
        }
        else if (cursor.x >= rowSize) {
            cursor.x = rowSize;
        }
    }

    unsigned long oldY = cursor.y;
    cursor.y = newY;

    if (redraw == REDRAW) {
        redrawText(oldY, ROWSIZE);
        redrawText(newY, ROWSIZE);
    }

    return CM_SUCCESS;
}