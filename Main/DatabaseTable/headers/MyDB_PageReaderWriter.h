
#ifndef PAGE_RW_H
#define PAGE_RW_H

#include "MyDB_PageType.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_Schema.h"
#include "MyDB_Record.h"
#include "MyDB_PageHandle.h"

typedef struct PageHeader {
    MyDB_PageType type;
    int pageSize;
    int recordCount;
    int totalRecords;
    size_t recordSize;
    void * nextFreeByte;
} PageHeader;

class MyDB_PageReaderWriter {

    

public:

    MyDB_PageReaderWriter (int pageSize, MyDB_SchemaPtr schema, MyDB_PageHandle handle);

	// ANY OTHER METHODS YOU WANT HERE

	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an itrator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// appends a record to this page... return false if the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an ennumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);
	
private:

    MyDB_PageHandle handle;
    MyDB_SchemaPtr schema;
    int pageSize;

	// ANYTHING ELSE YOU WANT HERE

    bool isLast(int current);
};

#endif
