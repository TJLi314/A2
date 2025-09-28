#include "MyDB_PageRecIterator.h"

void MyDB_PageRecIterator :: getNext () {
    void * bytes = this->getBytes();
    this->record->fromBinary(bytes + this->offset + this->current * this->record->getBinarySize());
    this->current++;
}

// return true iff there is another record in the file/page
bool MyDB_PageRecIterator :: hasNext ()  {
    return !this->isLast(this->current);
}

// destructor and contructor
MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_RecordPtr ptr, std::function<bool(int)> isLast, std::function<void*()> getBytes, int offset) {
    this->isLast = isLast;
    this->getBytes = getBytes;
    this->record = ptr;
    this->current = 0;
    this->offset = offset;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator () {
    this->record = nullptr;
}