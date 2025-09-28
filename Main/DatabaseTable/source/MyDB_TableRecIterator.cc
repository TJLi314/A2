#include "MyDB_TableRecIterator.h"

MyDB_TableRecIterator::MyDB_TableRecIterator(
    MyDB_TableReaderWriterPtr myTableRW,
    MyDB_RecordPtr record
) {
    this->myTableRW = myTableRW;
    this->myRecord = record;
    this->currentPageIndex = 0;
    MyDB_PageReaderWriter currentPageRW = (*myTableRW)[0];
    this->currentPageIter = currentPageRW.getIterator(myRecord);
} 

bool MyDB_TableRecIterator::hasNext() {
    // Current page has more records
    if (currentPageIter && currentPageIter->hasNext()) {
        return true;
    }

    // Get the next page iterator
    currentPageIndex++;
    MyDB_PageReaderWriter currentPageRW = (*myTableRW)[currentPageIndex];   
    currentPageIter = currentPageRW.getIterator(myRecord);  

    if (currentPageIter->hasNext()) {
        return true;
    }

    return false;
}

void MyDB_TableRecIterator::getNext() {
    if (!currentPageIter->hasNext()) {
        if (!hasNext()) {  // hasNext in this case will move to the next page if there is one
            std::cout << "No more records in table" << std::endl;
        }
    }

    currentPageIter->getNext();
}
