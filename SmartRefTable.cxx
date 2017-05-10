#include"SmartRefTable.h"
#include"TString.h"
#include<mutex>
#include<boost/thread/shared_mutex.hpp>
#include <algorithm>

ClassImp(SmartRefItem)

SmartRefItem::SmartRefItem(TProcessID* fP,UInt_t uid,const char* name)
{
	fPID=fP;
	fUID=uid;
	fName=name;
}

Bool_t SmartRefItem::CheckID(TProcessID* fP,UInt_t uid)
{
	if(fP!=fPID)	return kFALSE;
	if(uid!=fUID)	return kFALSE;
	return kTRUE;
}

bool	SmartRefItem::CheckName(TString t_name)
{
	if(!t_name.CompareTo(fName))
		return true;
	else 
		return false;
}

const char* SmartRefItem::GetName()
{
	return fName.Data();
}

/*
void SmartRefItem::Streamer(TBuffer &R__b)
{
	if(gDebug>0)
		printf("Start to streamer SRItem\n");
	UShort_t pidf;
	if(R__b.IsReading())
	{
		TObject::Streamer(R__b);
		R__b>>fUID;
		R__b>>fKey;
		R__b>>pidf;
		pidf += R__b.GetPidOffset();
		fPID = R__b.ReadProcessID(pidf);
		Int_t execid = R__b.GetTRefExecId();
		if (execid) SetBit(execid<<16);
		if(gDebug>0) printf("Read SRItem success\n");
	}
	else
	{
		TObject::Streamer(R__b);
		pidf=R__b.WriteProcessID(fPID);
		R__b<<pidf;
		R__b<<fKey;
		R__b<<fUID;
		if(gDebug>0) printf("Write SRItem Success\n");
	}
}*/
void SmartRefItem::Streamer(TBuffer &R__b)
{
   // Stream an object of class SmartRefItem.
   UShort_t pidf;
   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
	R__b>>pidf;
	pidf += R__b.GetPidOffset();
	fPID = R__b.ReadProcessID(pidf);
	Int_t execid = R__b.GetTRefExecId();
	if (execid) SetBit(execid<<16);
	R__b >> fUID;
      fName.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, SmartRefItem::IsA());
   } else {
      R__c = R__b.WriteVersion(SmartRefItem::IsA(), kTRUE);
      TObject::Streamer(R__b);
      pidf=R__b.WriteProcessID(fPID);
	R__b << pidf;
      R__b << fUID;
      fName.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

ClassImp(SmartRefTable)

SmartRefTable::SmartRefTable()
{
	fFile=0;
}

SmartRefTable::SmartRefTable(SmartFile* file)
{
	fFile=file;
}

void		SmartRefTable::AddItem(TProcessID* fP,UInt_t uid,const char* name)
{
	SmartRefItem item(fP,uid,name);
	std::lock_guard<boost::shared_mutex> lk(Table_mutex);
	mSRefTable.push_back(item);
}

void		SmartRefTable::DeleteItem(const char* name)
{
	std::vector<SmartRefItem>::iterator it;
	TString t_name=name;
	boost::shared_lock<boost::shared_mutex> lk(Table_mutex);
	std::vector<SmartRefItem>::iterator end=mSRefTable.end();
	for(it =mSRefTable.begin();it!=end;it++)
		if((*it).CheckName(t_name))
			break;
	if(it==end)
		return;

	lk.unlock();
	std::lock_guard<boost::shared_mutex> lk2(Table_mutex);
	mSRefTable.erase(it);
}

TKey*		SmartRefTable::FindItem(TProcessID* fP,UInt_t uid)
{
	std::vector<SmartRefItem>::iterator it;
	boost::shared_lock<boost::shared_mutex> lk(Table_mutex);
	std::vector<SmartRefItem>::iterator end=mSRefTable.end();
	for(it =mSRefTable.begin();it!=end;it++)
		if((*it).CheckID(fP,uid))
			break;
	const char* c_name;
	if(it!=end)
		c_name=(*it).GetName();
	return fFile->FindKey(c_name);
}

void 		SmartRefTable::SetFile(SmartFile* file)
{
	std::lock_guard<boost::shared_mutex> lk(Table_mutex);
	fFile=file;
}

Bool_t		SmartRefTable::CheckFile(SmartFile* file)
{
	boost::shared_lock<boost::shared_mutex> lk(Table_mutex);
	return fFile==file;
}
