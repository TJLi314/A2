#include "MyDB_PageRecIterator.h"

void MyDB_PageRecIterator :: getNext () {
    void * bytes = handle->getBytes();
    int next = this->current + *((size_t *)((char *)bytes + this->current)) + sizeof(size_t);
    this->record->fromBinary((char *)bytes + this->current);
    this->current = next;
}

// return true iff there is another record in the file/page
bool MyDB_PageRecIterator :: hasNext ()  {
    PageHeader * header = (PageHeader *)handle->getBytes();
    std::cout << "last: " << header->nextFreeByte << " current: " << this->current << " bytes: " << header->nextFreeByte << std::endl;
    return header->nextFreeByte == this->current;
}

// destructor and contructor
MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_RecordPtr ptr, MyDB_PageHandle handle, int offset) {
    this->handle = handle;
    this->record = ptr;
    this->current = offset;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator () {
    
}