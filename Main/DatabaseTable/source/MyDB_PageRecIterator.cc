#include "MyDB_PageRecIterator.h"

void MyDB_PageRecIterator :: getNext () {
    PageHeader * header = (PageHeader *)handle->getBytes();
    char * next = (char *)record->fromBinary((char *)handle->getBytes() + this->current);
    this->current = (next - (char *)header);
}

// return true iff there is another record in the file/page
bool MyDB_PageRecIterator :: hasNext ()  {
    PageHeader * header = (PageHeader *)handle->getBytes();
    return header->nextFreeByte > this->current;
}

// destructor and contructor
MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_RecordPtr ptr, MyDB_PageHandle handle, int offset) {
    this->handle = handle;
    this->record = ptr;
    this->current = offset;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator () {
    
}