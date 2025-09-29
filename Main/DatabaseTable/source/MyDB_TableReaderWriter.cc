
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

    // Set the last page of the table
    int tableLastPage = forMe->lastPage();
    if (tableLastPage != -1) {
        // When creating first page, need to create specific first page, call getbytes(), write to it and initialize it
        this->lastPageIndex = tableLastPage;
        this->lastPageRW = MyDB_PageReaderWriter(myBuffer->getPageSize(), myBuffer, forMe, tableLastPage, false);
    } else {
        this->lastPageIndex = 0;
        this->lastPageRW = MyDB_PageReaderWriter(myBuffer->getPageSize(), myBuffer, forMe, 0, true);
    }
}

MyDB_TableReaderWriter :: ~MyDB_TableReaderWriter() {
    std::cout << "Destroying TableReaderWriter" << std::endl;
    // for (auto &p : pages) {
    //     if (p) {
    //         p->clear();   // if PageReaderWriter has clear; otherwise just reset pointer
    //         p.reset();
    //     }
    // }
    // pages.clear();
    // std::cout << "TableReaderWriter destroyed" << std::endl;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	if (i > lastPageIndex) {

		// Create empty pages up to and including the requested page
        while (lastPageIndex < i) {
            lastPageIndex += 1;
            myBuffer->getPage(myTable, lastPageIndex);
            myTable->setLastPage(lastPageIndex);
        }

        lastPageRW = MyDB_PageReaderWriter(myBuffer->getPageSize(), myBuffer, myTable, i, true);
        return lastPageRW;
    }

    return MyDB_PageReaderWriter(myBuffer->getPageSize(), myBuffer, myTable, i, false);
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared <MyDB_Record>(myTable->getSchema());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
    return lastPageRW; 
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	if (!last().append(appendMe)) {
        // std::cout << "Creating new page for append" << std::endl;
        lastPageIndex++;
		lastPageRW = MyDB_PageReaderWriter(myBuffer->getPageSize(), myBuffer, myTable, lastPageIndex, true);
        myTable->setLastPage(lastPageIndex);

		// This should not happen
		if (!last().append(appendMe)) {
            std::cout << "happened anyways" << std::endl;
		}
	}
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	ifstream inFile(fromMe);
    if (!inFile.is_open()) {
        cerr << "Could not open file in loadFromTextFile" << endl;
        return;
    }

	string line;
    int lines = 0;
    while (getline(inFile, line)) {
        std::cout << "Read line: " + line << std::endl;
        // if (lines % 100 == 0) {
        //     std::cout << "Read " << lines << " lines" << std::endl;
        // }
        lines++;
        if (line.empty()) {
            continue; 
        }

        MyDB_RecordPtr rec = getEmptyRecord();
        rec->fromString(line);

        append(rec);
    }

    inFile.close();

    std::cout << "Finished loading from text file" << std::endl;
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return make_shared <MyDB_TableRecIterator>(this, iterateIntoMe);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	int fd = open(toMe.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
        perror("open failed");
        return;
    }

	MyDB_RecordPtr rec = getEmptyRecord();
    MyDB_RecordIteratorPtr iterator = getIterator(rec);
    while (iterator->hasNext()) {
        iterator->getNext();

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

	close(fd);
}

#endif

