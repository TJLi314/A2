
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (int pageSize, MyDB_SchemaPtr schema, MyDB_PageHandle handle) {
    this->pageSize = pageSize;
    this->handle = handle;
    this->schema = schema;
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter () {
    this->pageSize = 0;
    this->handle = nullptr;
    this->schema = nullptr;
}

void MyDB_PageReaderWriter :: clear () {
    PageHeader * header = (PageHeader *)handle->getBytes();
    header->type = MyDB_PageType::RegularPage;
    header->recordCount = 0;
    header->recordSize = 0;
    header->totalRecords = -1;
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
    if (header->totalRecords == -1) {
        header->recordSize = rec->getBinarySize();
        header->pageSize = pageSize;
        header->nextFreeByte = (char *)header + sizeof(PageHeader);
        header->totalRecords = (pageSize - sizeof(PageHeader)) / header->recordSize;
    }

    if (header->recordCount == header->totalRecords) {
        std::cout << "Not enough space on page to append record" << std::endl;
        return false;
    }

    void * toWrite = header->nextFreeByte;
    rec->toBinary(toWrite);
    header->nextFreeByte = (char *)toWrite + header->recordSize;
    header->recordCount++;
    handle->wroteBytes();
    return true;
}

bool MyDB_PageReaderWriter :: isLast(int current) {
    PageHeader * header = (PageHeader *)this->handle->getBytes();
    return current == header->recordCount;
}

#endif
