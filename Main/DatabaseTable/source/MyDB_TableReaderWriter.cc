
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <unistd.h>     // for write, close
#include <fcntl.h>      // for open
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include <fcntl.h>

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;
	this->myBuffer = myBuffer;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	if (i >= pages.size()) {
        std::cout << "Page index out of range, creating new pages" << std::endl;

		// Create empty pages up to and including the requested page
		for (size_t index = 1; index <= i - pages.size() + 1; i++) {
			pages.push_back(MyDB_PageReaderWriter(myBuffer->getPage(myTable, i), myBuffer->getPageSize(), myTable->getSchema()));
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
		pages.push_back(MyDB_PageReaderWriter(myBuffer->getPage(myTable, 0), myBuffer->getPageSize(), myTable->getSchema()));
    }
    return pages.back(); 
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// TODO: Go through every page and try to append

	// I'm appending to the last page of the table for now
	if (!last().append(appendMe)) {
		MyDB_PageReaderWriter newPageRW = MyDB_PageReaderWriter(myBuffer->getPage(myTable, pages.size()), myBuffer->getPageSize(), myTable->getSchema());
		pages.push_back(newPageRW);

		// This should not happen
		if (!last().append(appendMe)) {
			std::cout << "Failed to append record to newly created last page" << std::endl;
		}
	}
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	// Clear existing pages
	for (MyDB_PageReaderWriter pageRW: pages) {
		pageRW.clear();
	}
	// Do we have to do this? Is there anything we have to do in the MyDB_Table code?
	myTable->setLastPage(-1);
	
	// I don't know if this correctly discards of all the pageReaderWriter objects
	pages.resize(0);

	ifstream inFile(fromMe);
    if (!inFile.is_open()) {
        cerr << "Could not open file in loadFromTextFile" << endl;
        return;
    }

	string line;
    while (getline(inFile, line)) {
        if (line.empty()) {
            continue; 
        }

        MyDB_RecordPtr rec = getEmptyRecord();
        rec->fromText(line);

        append(rec);
    }

    inFile.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return make_shared <MyDB_TableRecIterator>(shared_from_this(), iterateIntoMe);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	int fd = open(toMe.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
        perror("open failed");
        return;
    }

	MyDB_RecordPtr rec = getEmptyRecord();
	for (MyDB_PageReaderWriter pageRW : pages) {
		MyDB_RecordIteratorPtr it = pageRW.getIterator(rec);

        while (it->hasNext()) {
            it->getNext();

            // convert record to string
			std::stringstream ss;
            ss << rec;
            string line = ss.str() + "\n";

            // write to file
            ssize_t res = write(fd, line.c_str(), line.size());
            if (res < 0) {
                perror("write failed");
                close(fd);
                return;
            }
        }
	}

	close(fd);
}

#endif

