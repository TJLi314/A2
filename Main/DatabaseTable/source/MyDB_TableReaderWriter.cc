
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_TableRecIterator.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;
	this->myBuffer = myBuffer;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	if (i >= pages.size()) {
        std::cout << "Page index out of range, creating new pages" << std::endl;

		// Create empty pages up to and including the requested page
		for (int index = 1; index <= i - pages.size() + 1; i++) {
			pages.push_back(MyDB_PageReaderWriter(myBuffer->getPageSize()));
		}
    }
	return pages[i];
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared <MyDB_Record>(myTable->getSchema());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	if (pages.empty()) {
        std::cout << "Table has no pages" << std::endl;
		
		// Create an empty page to return
		pages.push_back(MyDB_PageReaderWriter(myBuffer->getPageSize()));
    }
    return pages.back(); 
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// TODO: Go through every page and try to append

	// I'm appending to the last page of the table for now
	if (!last().append(appendMe)) {
		pages.push_back(MyDB_PageReaderWriter(myBuffer->getPageSize()));

		// This should not happen
		if (!last().append(appendMe)) {
			std::cout << "Failed to append record to newly created last page" << std::endl;
		}
	}
}

void MyDB_TableReaderWriter :: loadFromTextFile (string) {
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return make_shared <MyDB_TableRecIterator>(shared_from_this(), iterateIntoMe);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string) {
}

#endif

