#ifndef TABLE_REC_ITER_H
#define TABLE_REC_ITER_H

#include <memory>
#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

class MyDB_TableRecIterator : public MyDB_RecordIterator {

public:

    MyDB_TableRecIterator(MyDB_TableReaderWriter* myTableRW, MyDB_RecordPtr record);

    bool hasNext() override;

    void getNext() override;

private:

    MyDB_TableReaderWriter* myTableRW;
    MyDB_RecordPtr myRecord;
    size_t currentPageIndex;
    MyDB_RecordIteratorPtr currentPageIter;
};

#endif
