#ifndef	_SmartRefTable_h_
#define	_SmartRefTable_h_

#include "TProcessID.h"
#include "TObject.h"
#include "TKey.h"
#include "TFile.h"
#include "TString.h"
#include "SmartFile.h"

#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>

class TProcessID;
class TKey;
class TFile;
class SmartFile;

class SmartRefItem:public TObject{
private:
	TProcessID*	fPID;
	UInt_t	fUID;
	TString	fName;
public:
	SmartRefItem(){fPID=0;fUID=0;}
	SmartRefItem(TProcessID* fP,UInt_t uid,const char* name);
	~SmartRefItem(){};
	Bool_t 	CheckID(TProcessID* fP,UInt_t uid);
	bool		CheckName(TString name);
	const char* 	GetName();

ClassDef(SmartRefItem,1);
};


class SmartRefTable: public TObject{
private:
	SmartFile* fFile; //!
	std::vector<SmartRefItem> mSRefTable;
	mutable boost::shared_mutex Table_mutex; //!
public:
	SmartRefTable();
	SmartRefTable(SmartFile* file);
	~SmartRefTable(){};
	void 		AddItem(TProcessID* fP,UInt_t uid,const char* name);
	void 		DeleteItem(const char* name);
	TKey* 	FindItem(TProcessID* fP,UInt_t uid);
	void 		SetFile(SmartFile* file);
	Bool_t 	CheckFile(SmartFile* file);

ClassDef(SmartRefTable,1);
};




#endif
