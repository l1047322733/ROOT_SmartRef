#ifdef 	_SmartRefManager_h_
#define	_SmartRefManager_h_


#include"SmartRefTable.h"
#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>

class SmartRefTable;

class SmartRefManager:public TObject{
private:
	static SmartRefManager* 	mManager;
	static std::once_flag 	ini_flag;
	vector<SmartRefTable*> 	mSRefList;
	
	boost::shared_mutex	Manager_mutex;

public:
	~SmartRefManager();
	static SmartRefManager* 	GetManager(Int_t option=0);
	void 		AddTable(SmartRefTable* table);
	void		AddItemToTable(TFile* file,TProcessID* fP,UInt_t uid,TKey* key);
	SmartRefTable*		RemoveTable(TFile* file);
	TObject*	GetObject(TProcessID* fP,UInt_t uid);
	void		DeleteObject(SmartFile* file,TKey* key);
private:
	SmartRefManager();
	vector<SmartRefTable*>::iterator FindTableWithFile(TFile* file);

ClassDef(SmartRefManager,1);
};


#endif
