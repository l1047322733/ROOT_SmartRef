#ifndef 	_SmartRefManager_h_
#define	_SmartRefManager_h_


#include"SmartRefTable.h"
#include"SmartFile.h"
#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>

class SmartRefTable;
class SmartRile;

class SmartRefManager:public TObject{
private:
	static SmartRefManager* 	mManager;
	static std::once_flag 	ini_flag;
	std::vector<SmartRefTable*> 	mSRefList;
	
	mutable boost::shared_mutex	Manager_mutex;

public:
	~SmartRefManager();
	static SmartRefManager* 	GetManager(Int_t option=0);
	void 				AddTable(SmartRefTable* table);
	void				AddItemToTable(SmartFile* file,TProcessID* fP,UInt_t uid,const char* name);
	SmartRefTable*		RemoveTable(SmartFile* file);
	TObject*			GetObject(TProcessID* fP,UInt_t uid);
	void				DeleteObject(SmartFile* file,const char* name);
private:
	SmartRefManager();
	std::vector<SmartRefTable*>::iterator 	FindTableWithFile(SmartFile* file);

ClassDef(SmartRefManager,0);
};


#endif
