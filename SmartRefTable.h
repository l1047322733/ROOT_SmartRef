#ifdef	_SmartRefTable_h_
#define	_SmartRefTable_h_

#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>


class SmartRefItem:public TObject{
private:
	TProcessID*	fPID;
	UInt_t	fUID;
	TKey*		fKey;
public:
	SmartRefItem(){fPID=0;fUID=0;fKey=0;}
	SmartRefItem(TProcessID* fP,UInt_t uid,TKey* key);
	~SmartRefItem(){};
	Bool_t 	CheckID(TProcessID* fP,UInt_t uid);
	bool		CheckKey(TKey* key);
	TKey* 	GetKey();

ClassDef(SmartRefItem,1);
};


class SmartRefTable: public TObject{
private:
	TFile* fFile; //!
	vector<SmartRefItem> mSRefTable;
	mutable boost::shared_mutex Table_mutex;
public:
	SmartRefTable();
	SmartRefTable(TFile* file);
	~SmartRefTable(){};
	void 		AddItem(TProcessID* fP,UInt_t uid,TKey* key);
	void 		DeleteItem(TKey* key);
	TKey* 	FindItem(TProcessID* fP,UInt_t uid);
	void 		SetFile(SmartFile* file);
	Bool_t 	CheckFile(TFile* file);

ClassDef(SmartRefTable,1);
};




#endif
