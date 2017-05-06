#include"SmartRefTable.h"
#include<mutex>
#include<boost/thread/shared_mutex.hpp>
#include <algorithm>

ClassImp(SmartRefItem);


SmartRefItem::SmartRefItem(TProcessID* fP,UInt_t uid,TKey* key)
{
	fPID=fP;
	fUID=uid;
	fKey=key;
}

Bool_t SmartRefItem::CheckID(TProcessID* fP,UInt_t uid)
{
	if(fP!=fPID)	return kFALSE;
	if(uid!=fUID)	return kFALSE;
	return kTRUE;
}

bool	SmartRefItem::CheckKey(TKey* key)
{
	if(key==fKey)
		return true;
	else 
		return false;
}

TKey* SmartRefItem::GetKey()
{
	return fKey;
}



ClassImp(SmartRefTable);

SmartRefTable::SmartRefTable()
{
	fFile=0;
}

SmartRefTable::SmartRefTable(TFile* file)
{
	fFile=file;
}

void		SmartRefTable::AddItem(TProcessID* fP,UInt_t uid,TKey* key)
{
	SmartRefItem item(fP,uid,key);
	std::lock_guard<boost::shared_mutex> lk(Table_mutex);
	mSRefTable.push_back(item);
}

void		SmartRefTable::DeleteItem(TKey* key)
{
	vector<SmartRefItem>::iterator it;
	boost::shared_lock<boost::sharde_mutex> lk(Table_mutex);
	vector<SmartRefItem>::iterator end=mSRefTable.end();
	for(it =mSRefTable.begin();it!=end;it++)
		if((*it).CheckKey(key))
			break;
	if(it!=end)
	{
		lk.unlock();
		std::lock_guard<boost::shared_mutex> lk2(Table_mutex);
		mSRefTable.erase(it);
	}
}

TKey*		SmartRefTable::FindItem(TProcessID* fP,UInt_t uid)
{
	vector<SmartRefItem>::iterator it;
	boost::shared_lock<boost::sharde_mutex> lk(Table_mutex);
	vector<SmartRefItem>::iterator end=mSRefTable.end();
	for(it =mSRefTable.begin();it!=end;it++)
		if((*it).CheckID(fP,uid))
			break;
	TKey* key=0;
	if(it!=end)
		key=(*it).GetKey();
	return key;
}

void 		SmartRefTable::SetFile(SmartFile* file)
{
	std::lock_guard<boost::shared_mutex> lk(Table_mutex);
	fFile=file;
}

Bool_t		SmartRefTable::CheckFile(TFile* file)
{
	boost::shared_lock<boost::sharde_mutex> lk(Table_mutex);
	return fFile==file;
}
