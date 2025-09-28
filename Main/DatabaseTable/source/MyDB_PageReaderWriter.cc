
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
	MyDB_PageRecIteratorPtr itr =  std::make_shared<MyDB_PageRecIterator>(ptr, [this](int current) { return this->isLast(current); }, [this]() { return this->handle->getBytes(); }, sizeof(PageHeader));
	return itr;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType type) {
    void * buf = (PageHeader *)handle->getBytes();
    ((PageHeader *)buf)->type = type;
    handle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr rec) {
    PageHeader * header = (PageHeader *)handle->getBytes();
    std::cout << "Appending record of size " << rec->getBinarySize() << " to page with page size " << pageSize << std::endl;
    std::cout << "rec at " << &rec << std::endl;
    if (header->constructed == 0) {
        header->pageSize = pageSize;
        header->nextFreeByte = (char *)header + sizeof(PageHeader);
        std::cout << "first free bytes: " << header->nextFreeByte << std::endl;
        header->constructed = 1;
        handle->wroteBytes();
        std::cout << "Initialized page header" << std::endl;
    }


    if ((char *)header->nextFreeByte + sizeof(size_t) + rec->getBinarySize() > (char *)header + header->pageSize) {
        std::cout << "Not enough space on page to append record" << std::endl;
        return false;
    }

    std::cout << "page is not full, appending record, size of page header: " << sizeof(PageHeader) << std::endl;

    void * toWrite = header->nextFreeByte;
    size_t size = rec->getBinarySize();
    std::cout << "Writing size_t at " << toWrite << " with header at " << header << std::endl;
    memcpy(toWrite, &size, sizeof(size_t));
    toWrite = (char *)toWrite + sizeof(size_t);
    std::cout << "Writing record at " << toWrite<< std::endl;
    rec->toBinary(toWrite);
    header->nextFreeByte = (char *)toWrite + rec->getBinarySize();
    handle->wroteBytes();
    std::cout << "Appended record, next free byte is at " << header->nextFreeByte << std::endl;
    return true;
}

bool MyDB_PageReaderWriter :: isLast(int current) {
    PageHeader * header = (PageHeader *)this->handle->getBytes();
    return (char *)header + current == (char *)header->nextFreeByte;
}

#endif
