/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include <cstring>
#include <iostream>
using namespace std;

void setBuffer(char* buff, int value, int pos)
{
	char* position = buff + sizeof(int)*pos;
	memcpy(position, (char*) &value, 4);
}

void test()
{
	BTLeafNode test1;

	int m_PageId = 10;
	int m_KeyCount = 1;
	//memcpy(test1.buffer, (char*)(&m_PageId), 4);
	memcpy(test1.buffer+4, (char*)(&m_KeyCount), 4);	

	setBuffer(test1.buffer, 100 ,0);

	// cout << "next ptr should be 100: " << test1.getNextNodePtr() << endl;
	// cout << "key count should be 1: " << test1.getKeyCount() << endl;

	// test1.setNextNodePtr(5);
	// cout << "new next ptr should be 5: " << test1.getNextNodePtr() << endl;


}

void test2()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 3, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	//cout << test1.getKeyCount() << endl;

	setBuffer(test1.buffer, 5, 5);
	setBuffer(test1.buffer, 6, 6);
	setBuffer(test1.buffer, 7, 7);

	setBuffer(test1.buffer, 8, 8);
	setBuffer(test1.buffer, 9, 9);
	setBuffer(test1.buffer, 10, 10);

	RecordId id;
	int key;

	test1.readEntry(0, key, id);

	cout << "key is: " << key << endl;
	cout << "pid is: "<< id.pid << " sid is: " << id.sid << endl;

	int eid;
	test1.locate(7, eid);

	cout << "it should be 2: " << eid << endl;
}

void test3()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	// setBuffer(test1.buffer, 5, 5);
	// setBuffer(test1.buffer, 6, 6);
	// setBuffer(test1.buffer, 7, 7);

	// setBuffer(test1.buffer, 8, 8);
	// setBuffer(test1.buffer, 9, 9);
	// setBuffer(test1.buffer, 10, 10);

	RecordId testid;
	testid.pid = 6;
	testid.sid = 7;

	test1.insert(8, testid);
	//setBuffer(test1.buffer, 2, 1);
	test1.setKeyCount(2);
	RecordId id;
	int key;

	test1.readEntry(1, key, id);
	cout << "key should be 8: " << key << endl;
	cout << "pid should be 6: " << id.pid << " sid should be 7: " << id.sid << endl;

}

void test4()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	int eid;
	test1.locate(5, eid);

	cout << "it should be 1: " << eid << endl;
}

void test5()
{
	RecordId id;
	int key;

	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	RecordId testid1;
	testid1.pid = 9;
	testid1.sid = 10;
	test1.insert(11, testid1);
	test1.readEntry(1, key, id);
	cout << "key should be 11: " << key << endl;
	cout << "pid should be 9: " << id.pid << " sid should be 10: " << id.sid << endl;


	RecordId testid2;
	testid2.pid = 6;
	testid2.sid = 7;
	test1.insert(8, testid2);
	test1.readEntry(1, key, id);
	cout << "key should be 8: " << key << endl;
	cout << "pid should be 6: " << id.pid << " sid should be 7: " << id.sid << endl;

	RecordId testid3;
	testid3.pid = 5;
	testid3.sid = 5;
	test1.insert(5, testid3);
	test1.readEntry(1, key, id);
	cout << "key should be 5: " << key << endl;
	cout << "pid should be 5: " << id.pid << " sid should be 5: " << id.sid << endl;
}


int main()
{
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);

  	//test();
  	//test2();
  	//test3();
	//test4();
	test5();
  return 0;
}
