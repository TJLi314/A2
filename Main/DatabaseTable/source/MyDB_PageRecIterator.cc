#include "MyDB_PageRecIterator.h"

void MyDB_PageRecIterator :: getNext () {
    void * bytes = this->getBytes();
    int next = this->current + *((size_t *)((char *)bytes + this->current)) + sizeof(size_t);
    this->record->fromBinary((char *)bytes + this->current);
    this->current = next;
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
    this->current = offset;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator () {
    this->record = nullptr;
}