
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include <memory>

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (int pageSize, MyDB_PageHandle handle) {
    this->pageSize = pageSize;
    this->handle = handle;
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
	MyDB_PageRecIteratorPtr itr =  std::make_shared<MyDB_PageRecIterator>(ptr, [this](int current) { return this->isLast(current); }, [this]() { return this->getBytes(); }, sizeof(PageHeader));
	return itr;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType type) {
    void * buf = (PageHeader *)handle->getBytes();
    ((PageHeader *)buf)->type = type;
    handle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr rec) {
    PageHeader * header = (PageHeader *)handle->getBytes();
    if (header->constructed == 0) {
        header->pageSize = pageSize;
        header->nextFreeByte = sizeof(PageHeader);
        header->constructed = 1;
        handle->wroteBytes();
        std::cout << "new page constructed" << std::endl;
    } else {
        std::cout << "page already constructed, nextFreeByte: " << header->nextFreeByte << std::endl;
    }

    if (header->nextFreeByte + sizeof(size_t) + rec->getBinarySize() > header->pageSize) {
        std::cout << "Not enough space to append record of size " << rec->getBinarySize() << " to page of size " << header->pageSize << " nextFreeByte: " << header->nextFreeByte << std::endl;
        return false;
    }


    void * toWrite = (char *)header + header->nextFreeByte;
    size_t size = rec->getBinarySize();
    memcpy(toWrite, &size, sizeof(size_t));
    toWrite = (char *)toWrite + sizeof(size_t);
    rec->toBinary(toWrite);
    header->nextFreeByte = header->nextFreeByte + rec->getBinarySize() + sizeof(size_t);
    handle->wroteBytes();
    return true;
}

bool MyDB_PageReaderWriter :: isLast(int current) {
    PageHeader * header = (PageHeader *)this->getBytes();
    return (char *)header + current == (char *)header->nextFreeByte;
}

MyDB_PageReaderWriter :: ~MyDB_PageReaderWriter () {
    std::cout << "Destroying PageReaderWriter" << std::endl;
}

void * MyDB_PageReaderWriter :: getBytes() {
    return this->handle->getBytes();
}

#endif
