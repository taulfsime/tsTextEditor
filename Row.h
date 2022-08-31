#pragma once
#include "DefStructs.h"

Row* createRow(RowContent* content);
Row* init();
Row* getRowHead(Row* row);
Row* getRowTail(Row* row);
Row* getRowByIndex(Row* row, unsigned long index);
void appendNewRow(Row* row, RowContent* content);
unsigned long getRowsCount(Row* row);
void insertNewRow(Row* row, unsigned long index, RowContent* content);
void setRowContent(Row* row, RowContent* content);
void clearContent(RowContent* content);
RowContent* createContent(char str[]);
RowContent* createSingleContent(char ch);
void clearRowAndContent(Row* row);
void clearAllRows(Row* row);
void insertIntoRowContentByIndex(Row* row, unsigned long index, RowContent* content);
RowContent* getContentByIndex(RowContent* content, unsigned long index);
unsigned long getContentSize(RowContent* content);
void appendContent(RowContent* dest, RowContent* content);
RowContent* getSubcontent(RowContent* content, unsigned long startIndex, unsigned long endIndex);
void removeContentFromRow(Row* row, unsigned long startIndex, unsigned long endIndex);
void removeSingleContentFromRow(Row* row, unsigned long index);
RowContent* getContentFromRow(Row* row);
void removeRow(Row* head, unsigned long index);
void setSymbolInContent(RowContent* cnt, unsigned long index, char ch);
unsigned long findInContent(RowContent* cnt, char ch);
short contentToInt(RowContent* cnt, int* num);

//FUNCTIONS

short contentToInt(RowContent* cnt, int* num) {
    if (cnt == NULL) {
        return 0;
    }

    unsigned long size = getContentSize(cnt);
    if (size == 0) {
        return 0;
    }

    *num = 0;
    int sign = getContentByIndex(cnt, 0)->symbol == '-' ? -1 : 1;
    int endIndex = getContentByIndex(cnt, 0)->symbol == '-' ? 1 : 0;
    int mpt = 0;
    
    for (int i = size - 1; i >= endIndex; i--) {
        int numCh = 0;
        char ch = getContentByIndex(cnt, (unsigned long)i)->symbol;

        switch (ch) {
        case '0': numCh = 0; break;
        case '1': numCh = 1; break;
        case '2': numCh = 2; break;
        case '3': numCh = 3; break;
        case '4': numCh = 4; break;
        case '5': numCh = 5; break;
        case '6': numCh = 6; break;
        case '7': numCh = 7; break;
        case '8': numCh = 8; break;
        case '9': numCh = 9; break;
        default: return 0;
        }

        if (mpt == 0) {
            *num = numCh;
            mpt = 1;
        }
        else {
            *num += (mpt * 10 * numCh);
            mpt *= 10;
        }
    }

    *num *= sign;

    return 1;
}

unsigned long findInContent(RowContent* cnt, char ch) {
    if (cnt == NULL) {
        return 0;
    }

    for (unsigned long i = 0; i < getContentSize(cnt); i++) {
        if (getContentByIndex(cnt, i)->symbol == ch) {
            return i;
        }
    }

    return 0;
}

void setSymbolInContent(RowContent* cnt, unsigned long index, char ch) {
    unsigned long cntSize = getContentSize(cnt);
    if (index >= cntSize) {
        return;
    }

    RowContent* part = getContentByIndex(cnt, index);
    if (part != NULL) {
        part->symbol = ch;
    }
}

void removeRow(Row* head, unsigned long index) {
    head = getRowHead(head);

    unsigned long rowsSize = getRowsCount(head);
    if (index >= rowsSize) {
        return;
    }

    Row* row = getRowByIndex(head, index);
    if (row->nextRow != NULL) {
        row->nextRow->prevRow = row->prevRow;
    }

    if (row->prevRow != NULL) {
        row->prevRow->nextRow = row->nextRow;
    }

    clearRowAndContent(row);
}

RowContent* getContentFromRow(Row* row) {
    if (row == NULL) {
        return  NULL;
    }

    RowContent* cnt = row->content;
    row->content = NULL;

    return cnt;
}

void removeSingleContentFromRow(Row* row, unsigned long index) {
    if (row == NULL) {
        return;
    }

    if (row->content == NULL) {
        return;
    }

    unsigned long size = getContentSize(row->content);
    if (index >= size) {
        return;
    }

    RowContent* cnt = getContentByIndex(row->content, index);
    if (index == 0) {
        row->content = cnt->next;

        if (cnt->next != NULL) {
            cnt->next->prev = NULL;
        }
    }
    else {
        if (cnt->next != NULL) {
            cnt->next->prev = cnt->prev;
        }

        if (cnt->prev != NULL) {
            cnt->prev->next = cnt->next;
        }
    }

    cnt->prev = NULL;
    cnt->next = NULL;

    clearContent(cnt);
}

void removeContentFromRow(Row* row, unsigned long startIndex, unsigned long endIndex) {
    if (row == NULL) {
        return;
    }

    if (row->content == NULL) {
        return;
    }

    unsigned long size = getContentSize(row->content);
    if (startIndex >= size && endIndex >= size) {
        return;
    }

    if (endIndex == 0) {
        endIndex = size - 1;
    }

    RowContent* first = getContentByIndex(row->content, startIndex);
    RowContent* last = getContentByIndex(row->content, endIndex);

    if (first != NULL && last != NULL) {

        if (startIndex == 0) {
            row->content = last->next;
        }
        else {
            if (first->prev != NULL) {
                (first->prev)->next = last->next;
            }

            if (last->next != NULL) {
                (last->next)->prev = first->prev;
            }
        }

        first->prev = NULL;
        last->next = NULL;

        clearContent(first);
    }
}

RowContent* getSubcontent(RowContent* content, unsigned long startIndex, unsigned long endIndex) {
    unsigned long size = getContentSize(content);
    if (startIndex >= size || endIndex >= size) {
        return NULL;
    }

    if (endIndex == 0) {
        endIndex = size - 1;
    }

    RowContent* first = NULL;
    RowContent* temp = NULL;

    for (startIndex; startIndex <= endIndex; startIndex++) {
        RowContent* newContent = createSingleContent(getContentByIndex(content, startIndex)->symbol);
        if (newContent == NULL) {
            break;
        }

        if (first == NULL) {
            first = newContent;
            temp = first;
        }
        else {
            temp->next = newContent;
            newContent->prev = temp;
            temp = temp->next;
        }
    }

    return first;
}

void appendContent(RowContent* dest, RowContent* content) {
    if (content == NULL) {
        return;
    }

    if (dest == NULL) {
        return;
    }

    while (dest->next != NULL) {
        dest = dest->next;
    }

    dest->next = content;
    content->prev = dest;
}

unsigned long getContentSize(RowContent* content) {
    if (content == NULL) {
        return 0;
    }

    unsigned long count = 0;
    while (content != NULL) {
        content = content->next;
        count += 1;
    }

    return count;
}

RowContent* getContentByIndex(RowContent* content, unsigned long index) {
    if (content == NULL) {
        return NULL;
    }

    if (index >= getContentSize(content)) {
        return NULL;
    }

    for (unsigned long i = 0; i < index; i++) {
        if (content == NULL) {
            break;
        }

        content = content->next;
    }

    return content;
}

void insertIntoRowContentByIndex(Row* row, unsigned long index, RowContent* content) {
    if (row == getRowHead(row)) {
        return;
    }

    if (index >= getContentSize(row->content)) {
        return;
    }

    if (content == NULL) {
        return;
    }

    RowContent* contentLast = content;
    while (contentLast->next != NULL) {
        contentLast = contentLast->next;
    }

    RowContent* dest = getContentByIndex(row->content, index);
    if (dest != NULL) {
        if (index == 0) {
            row->content = content;
        }
        else {
            dest->prev->next = content;
        }
        content->prev = dest->prev;

        contentLast->next = dest;
        dest->prev = contentLast;
    }
}

void clearAllRows(Row* row) {
    if (row != getRowHead(row)) {
        return;
    }

    row = row->nextRow;
    while (row != NULL) {
        Row* temp = row;
        row = row->nextRow;
        clearRowAndContent(temp);
    }
}

void clearRowAndContent(Row* row) {
    if (row == getRowHead(row)) {
        return;
    }

    clearContent(row->content);
    if (row->prevRow != NULL) {
        row->prevRow->nextRow = row->nextRow;
    }
    if (row->nextRow != NULL) {
        row->nextRow->prevRow = row->prevRow;
    }

    free(row);
}

RowContent* createSingleContent(char ch) {
    RowContent* cnt = (RowContent*)(malloc(sizeof(RowContent)));

    if (cnt != NULL) {
        cnt->symbol = ch;
        cnt->next = NULL;
        cnt->prev = NULL;
    }

    return cnt;
}

RowContent* createContent(char str[]) {
    unsigned int index = 0;
    char ch;

    RowContent* first = NULL;
    RowContent* temp = NULL;

    while ((ch = str[index++]) != '\0') {
        if (first == NULL) {
            first = createSingleContent(ch);
            if (first == NULL) {
                break;
            }

            temp = first;
        }
        else {
            RowContent* newContent = createSingleContent(ch);
            if (newContent == NULL) {
                break;
            }

            newContent->prev = temp;
            newContent->next = NULL;

            temp->next = newContent;
            temp = temp->next;
        }
    }

    return first;
}

void clearContent(RowContent* content) {
    RowContent* cnt = content;
    while (cnt != NULL) {
        RowContent* temp = cnt;
        cnt = cnt->next;

        free(temp);
    }
}

void setRowContent(Row* row, RowContent* content) {
    if (row == NULL) {
        return;
    }

    if (row->content != NULL) {
        clearContent(row->content);
    }

    row->content = content;
}

Row* getRowByIndex(Row* row, unsigned long index) {
    if (index >= getRowsCount(row)) {
        return NULL;
    }

    row = getRowTail(row);
    for (unsigned long i = 0; i < index; i++) {
        if (row == NULL) {
            break;
        }

        row = row->nextRow;
    }

    return row;
}

void insertNewRow(Row* row, unsigned long index, RowContent* content) {
    row = getRowByIndex(getRowHead(row), index);
    if (row == NULL) {
        return;
    }

    Row* newRow = createRow(row->content);

    if (newRow != NULL) {
        row->content = content;

        newRow->nextRow = row->nextRow;

        if (row->nextRow != NULL) {
            row->nextRow->prevRow = newRow;
        }

        newRow->prevRow = row;
        row->nextRow = newRow;
    }
}

unsigned long getRowsCount(Row* row) {
    row = getRowTail(row);
    if (row == NULL) {
        return 0;
    }

    unsigned long count = 0;
    while (row != NULL) {
        row = row->nextRow;
        count += 1;
    }

    return count;
}

void appendNewRow(Row* row, RowContent* content) {
    row = getRowHead(row);
    if (row == NULL) {
        return;
    }

    while (row->nextRow != NULL) {
        row = row->nextRow;
    }

    Row* newRow = createRow(content);
    row->nextRow = newRow;
    newRow->prevRow = row;
}

Row* getRowTail(Row* row) {
    row = getRowHead(row);
    if (row == NULL) {
        return NULL;
    }

    return row->nextRow;
}

Row* getRowHead(Row* row) {
    if (row == NULL) {
        return NULL;
    }

    if (row->prevRow == NULL) {
        return row;
    }

    while (row->prevRow != NULL) {
        row = row->prevRow;
    }

    return row;
}

Row* init() {
    return createRow(NULL);
}

Row* createRow(RowContent* content) {
    Row* row = (Row*)(malloc(sizeof(Row)));

    if (row != NULL) {
        row->content = content;
        row->nextRow = NULL;
        row->prevRow = NULL;
    }

    return row;
}