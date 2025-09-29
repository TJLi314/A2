
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include <memory>

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (int pageSize, MyDB_BufferManagerPtr mgr, MyDB_TablePtr table, int index) {
    this->pageSize = pageSize;
    this->handle = mgr->getPage(table, index);
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter () {
    this->pageSize = 0;
    this->handle = nullptr;
}

void MyDB_PageReaderWriter :: clear () {
    PageHeader * header = (PageHeader *)handle->getBytes();
    memset(header, 0, pageSize);
    header->type = RegularPage;
    handle->wroteBytes();
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	return ((PageHeader *)handle->getBytes())->type;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr ptr) {
    void * header = handle->getBytes();
    std::cout << "nextfreebyte: " << ((PageHeader *)header)->nextFreeByte << std::endl; 
	MyDB_PageRecIteratorPtr itr =  std::make_shared<MyDB_PageRecIterator>(ptr, this->handle, sizeof(PageHeader));
	return itr;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType type) {
    void * buf = (PageHeader *)handle->getBytes();
    ((PageHeader *)buf)->type = type;
    handle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr rec) {
    PageHeader * header = (PageHeader *)handle->getBytes();
    std::cout << "Appending record of size: " << rec->getBinarySize() << " at " << header << std::endl;
    if (header->constructed == 0) {
        header->pageSize = pageSize;
        header->nextFreeByte = sizeof(PageHeader);
        header->constructed = 1;
        handle->wroteBytes();
    }

    if (header->nextFreeByte + sizeof(size_t) + rec->getBinarySize() > header->pageSize) {
        return false;
    }

    void * toWrite = (char *)header + header->nextFreeByte;
    size_t size = rec->getBinarySize();
    memcpy(toWrite, &size, sizeof(size_t));
    toWrite = (char *)toWrite + sizeof(size_t);
    rec->toBinary(toWrite);
    header->nextFreeByte = header->nextFreeByte + rec->getBinarySize() + sizeof(size_t);
    handle->wroteBytes();
    std::cout << "header at " << header << ", nextfreebyte at: " << &header->nextFreeByte << std::endl;
    std::cout << "Appended record, next free byte at: " << *reinterpret_cast<size_t*>(header) << " nextfreeByte: " << header->nextFreeByte << std::endl;
    return true;
}

MyDB_PageReaderWriter :: ~MyDB_PageReaderWriter () {
}

#endif
