
#ifndef PAGE_REC_ITER_H
#define PAGE_REC_ITER_H

#include <memory>
#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
using namespace std;

// This pure virtual class is used to iterate through the records in a page or file
// Instances of this class will be created via calls to MyDB_PageReaderWriter.getIterator ()
// or MyDB_FileReaderWriter.getIterator ().
//
class MyDB_PageRecIterator;
typedef shared_ptr <MyDB_PageRecIterator> MyDB_PageRecIteratorPtr;

class MyDB_PageRecIterator: public MyDB_RecordIterator {

public:

	// put the contents of the next record in the file/page into the iterator record
	// this should be called BEFORE the iterator record is first examined
	void getNext () override;

	// return true iff there is another record in the file/page
	bool hasNext () override;

	// destructor and contructor
	MyDB_PageRecIterator (MyDB_RecordPtr ptr, std::function<bool(int)> isLast, std::function<void*()> getBytes, int offset);

	~MyDB_PageRecIterator () override;

private:

    std::function<bool(int)> isLast;
    std::function<void*()> getBytes;
    MyDB_RecordPtr record;
    int current;
};

#endif
