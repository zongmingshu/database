#include "BTreeNode.h"
#include <iostream>

using namespace std;

BTLeafNode::BTLeafNode(PageId pid)
:m_pid(pid)
{
	setKeyCount(0);
}

PageId BTLeafNode::getCurrentPid()
{
	return m_pid;
}

void BTLeafNode::setCurrentPid(PageId pid)
{
	m_pid = pid;
}

void BTLeafNode::printNode()
{
	for(int i = 0; i < this-> getKeyCount(); i++)
	{
		int m_key;
		RecordId m_rid;
		readEntry(i, m_key, m_rid);
		cout<< "Entry " << i << " is: "<< m_rid.pid << " " << m_rid.sid << " " << m_key << endl;  
	}
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	//RC is int type
	RC rc;
	//BTNode class has a private member called buffer
	//read() is a member function of PageFile
	rc = pf.read(pid, this->buffer);

	return rc; 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 
	//Todo: Write a new node?
	//RC is int type
	RC rc;
	//BTNode class has a private member called buffer
	//write() is a member function of PageFile
	rc = pf.write(pid, this->buffer);
	return rc; 
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
//pp: starting from offset 4 is KeyCount
int BTLeafNode::getKeyCount()
{ 	
	//KeyCound is located at second 4-byte block of buffer
	char* keycountaddress = buffer + sizeof(PageId);
	int keycount = *((int*)keycountaddress); 

	return keycount; 
}

/**
* Set the number of keys stored in the node.
* @Set the number of keys in the node
*/
void BTLeafNode::setKeyCount(const int keycount)
{
	char* keycountaddress =  buffer + sizeof(PageId);
	memcpy(keycountaddress, &keycount, sizeof(int));
}


/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	RC rc;
	//Check if node is full
	if (getKeyCount() >= MAXLEAFNODESIZE)
		return RC_NODE_FULL;
	//Insertion begin
	int eid;
	rc = locate(key, eid);
	char * init = buffer + sizeof(PageId) + sizeof(int);
	if (rc == RC_NO_SUCH_RECORD)
	{
		//Inserted (key, rid) is at the end of the node
		//cout << eid << endl;
		//Initial pos began after pageid and keycount
		//init = buffer + sizeof(PageId) + sizeof(int);
		//Insert rid.pid and rid.sid
		//*(int*)(init + eid * LEAFNODEOFFSET) = rid.pid;
		//(char*) (init + eid * LEAFNODEOFFSET) = (char*) &(rid.pid);
		memcpy(init + eid * LEAFNODEOFFSET, (char*) &(rid.pid), sizeof(int));
		//*(int*)(init + eid * LEAFNODEOFFSET + sizeof(int)) = rid.sid;
		//(char*) (init + eid * LEAFNODEOFFSET + sizeof(int)) = (char*) &(rid.sid);
		memcpy(init + eid * LEAFNODEOFFSET + sizeof(int), (char*) &(rid.sid), sizeof(int));
		//Insert key
		//*(int*)(init + eid * LEAFNODEOFFSET + sizeof(RecordId)) = key;
		//(char*) (init + eid * LEAFNODEOFFSET + sizeof(RecordId)) = (char*) &key;
		memcpy(init + eid * LEAFNODEOFFSET + sizeof(RecordId), (char*) &key, sizeof(int));
	}
	else
	{
		int temp_size = LEAFNODEOFFSET*(getKeyCount() - eid);
		char* temp = new char[temp_size];
		//
		memcpy((char*)temp, (char*)(init + eid * LEAFNODEOFFSET), temp_size);
		memcpy((char*)(init + (eid + 1) * LEAFNODEOFFSET ), (char*)temp, temp_size);
		
		memcpy(init + eid * LEAFNODEOFFSET, (char*) &(rid.pid), sizeof(int));
		memcpy(init + eid * LEAFNODEOFFSET + sizeof(int), (char*) &(rid.sid), sizeof(int));
		memcpy(init + eid * LEAFNODEOFFSET + sizeof(RecordId), (char*) &key, sizeof(int));
		delete [] temp;

	}
	//Inserted (key, rid) is at the mid of the node
	setKeyCount(getKeyCount() + 1);

	return rc;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	int left_half_size = MAXLEAFNODESIZE / 2;
	//
	int right_half_size = MAXLEAFNODESIZE - left_half_size;
	bool left_insert = ( key < buffer[left_half_size] );
	char * init = buffer + sizeof(PageId) + sizeof(int);
	memcpy((char*)(sibling.buffer + sizeof(PageId) + sizeof(int)), init + left_half_size * LEAFNODEOFFSET, right_half_size * LEAFNODEOFFSET);
	this->setKeyCount(left_half_size);
	sibling.setKeyCount(right_half_size);
	if (left_insert)
	{
		this->insert(key, rid);
	}
	else
	{
		sibling.insert(key, rid);
	}
	RecordId m_rid;
	sibling.readEntry(0, siblingKey, m_rid);

	//set the pid
	PageId temp_pid = getNextNodePtr();
	sibling.setNextNodePtr(temp_pid);
	setCurrentPid(sibling.getCurrentPid());

	return 0;

}

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remeber that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equalty to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */

//
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	//initial pos began after pageid and keycount
	char* init = buffer + sizeof(PageId) + sizeof(int);

	for (int i=0; i<this->getKeyCount(); i++)
	{
		//init + 0*12 +8, init + 1*12 +8, ...
		if (*(int*)(init + i * LEAFNODEOFFSET + sizeof(RecordId)) < searchKey)
		{
			continue;
		}
		else
		{
			//Return i to eid, return 0
			eid = i;
			return 0;
		}
	}
	//Not found, return error code
	eid = getKeyCount();
	return RC_NO_SUCH_RECORD; 
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
//pp: readEntry, starting from 0 is NextNode PageId, starting from 4 is KeyCount
//pp: starting from 8 is entry 0, each entry contains 12 bytes, first 8 is for RecordID, next 4 is for Key
//pp: in RecordID, first 4 is pid and next 4 is sid
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
	char* RecordIdAddress_pid = buffer + sizeof(PageId) + sizeof(int) + eid * LEAFNODEOFFSET;
	rid.pid = *((int*)RecordIdAddress_pid);
	char* RecordIdAddress_sid = RecordIdAddress_pid + sizeof(int);
	rid.sid = *((int*)RecordIdAddress_sid);
	char* KeyAddress = RecordIdAddress_sid + sizeof(int);
	key = *((int*)KeyAddress);  

	return 0; 
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
//pp: starting from offset 0 is NextNode PageId
PageId BTLeafNode::getNextNodePtr()
{
	char* NextNodeAddress = buffer;
	int NextPtr = *((int*)NextNodeAddress); 
	return NextPtr; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
	memcpy(this->buffer, (char*) &pid, sizeof(int));
	return 0; 
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return 0; }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
