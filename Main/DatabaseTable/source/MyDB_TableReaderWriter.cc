
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
			pages.push_back(make_shared<MyDB_PageReaderWriter>(myBuffer->getPageSize(), myBuffer->getPage(myTable, i)));
		}
    }
	return *pages[i];
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared <MyDB_Record>(myTable->getSchema());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	if (pages.empty()) {
        std::cout << "Table has no pages" << std::endl;
		
		// Create an empty page to return
		pages.push_back(make_shared<MyDB_PageReaderWriter>(myBuffer->getPageSize(), myBuffer->getPage(myTable, 0)));
    }
    return *pages.back(); 
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// TODO: Go through every page and try to append

	// I'm appending to the last page of the table for now
	if (!last().append(appendMe)) {
		MyDB_PageReaderWriterPtr newPageRW = make_shared<MyDB_PageReaderWriter>(myBuffer->getPageSize(), myBuffer->getPage(myTable, pages.size()));
		pages.push_back(newPageRW);

		// This should not happen
		if (!last().append(appendMe)) {
            std::cout << "happened anyways" << std::endl;
		}
	}
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	// Clear existing pages
    std::cout << "Clearing existing pages..." << std::endl;
	for (MyDB_PageReaderWriterPtr pageRW: pages) {
		pageRW->clear();
	}
	// Do we have to do this? Is there anything we have to do in the MyDB_Table code?
	myTable->setLastPage(-1);
    std::cout << "1" << std::endl;
	
	// I don't know if this correctly discards of all the pageReaderWriter objects
	pages.resize(0);
    std::cout << "2" << std::endl;

	ifstream inFile(fromMe);
    if (!inFile.is_open()) {
        cerr << "Could not open file in loadFromTextFile" << endl;
        return;
    }
    std::cout << "3" << std::endl;

	string line;
    int lines = 0;
    while (getline(inFile, line)) {
        if (lines % 100 == 0) {
            std::cout << "Read " << lines << " lines" << std::endl;
        }
        lines++;
        if (line.empty()) {
            continue; 
        }

        MyDB_RecordPtr rec = getEmptyRecord();
        rec->fromString(line);

        append(rec);
    }
    std::cout << "4" << std::endl;

    inFile.close();

    std::cout << "Finished loading from text file" << std::endl;
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
	for (MyDB_PageReaderWriterPtr pageRW : pages) {
		MyDB_RecordIteratorPtr it = pageRW->getIterator(rec);

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

