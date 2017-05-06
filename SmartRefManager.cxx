#include"SmartRefManager.h"
#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>

SmartRefManager* 	SmartRefManager::mManager=0;
std::once_flag 	SmartRefManager::ini_flag;

SmartRefManager::~SmartRefManager()
{
	delete mManager;
	
}

static SmartRefManager* 	SmartRefManager::GetManager(Int_t option=0)
{
	if(mManager==0)
		if(option==1)
			call_once(ini_flag,[]{mManager=new SmartRefManager;});
	return mManager;
}

void 	SmartRefManager::AddTable(SmartRefTable* table)
{
	std::lock_guard<boost::shared_mutex> lk(Manager_mutex);
	mSRefList.push_back(table);
}

void	SmartRefManager::AddItemToTable(TFile* file,TProcessID* fP,UInt_t uid,TKey* key)
{
	boost::shared_lock<boost::sharde_mutex> lk(Manager_mutex);
	vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	lk.unlock();
	std::lock_guard<boost::shared_mutex> lk(Manager_mutex);
	if(iter_SRT==mSRefList.end())
	{
		mSRefList.push_back(new SmartRefTable(file));
		iter_SRT=mSRefList.end()-1;
	}
	
	(*iter_SRT)->AddItem(fP, uid, key);
}

SmartRefTable*	SmartRefManager::RemoveTable(TFile* file)
{
	SmartRefTable* ptr_SRT=nullptr;
	boost::shared_lock<boost::sharde_mutex> lk(Manager_mutex);
	vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	if(iter_SRT==mSRefList.end())
		return ptr_SRT;

	ptr_SRT=*iter_SRT;
	lk.unlock();
	std::lock_guard<boost::shared_mutex> lk(Manager_mutex);
	mSRefList.erase(iter_SRT);

	return ptr_SRT;
}

TObject*	SmartRefManager::GetObject(TProcessID* fP,UInt_t uid)
{
	TKey* obj_key=0;
	boost::shared_lock<boost::sharde_mutex> lk(Manager_mutex);
	vector<SmartRefTable*>::iterator iter_SRT;
	vector<SmartRefTable*>::iterator itend=mSRefList.end();
	for(iter_SRT=mSRefList.begin();iter_SRT!=itend;iter_SRT++)
	{
		obj_key=(*iter_SRT)->FindItem(fP,uid);
		if(obj_key)
			return obj_key->ReadObj();
	}
	return 0;
}

void		SmartRefManager::DeleteObject(SmartFile* file,TKey* key)
{
	boost::shared_lock<boost::sharde_mutex> lk(Manager_mutex);
	vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	if(iter_SRT==mSRefList.end())
		return;
		
	(*iter_SRT)->DeleteItem(TKey* key);
}

SmartRefManager::SmartRefManager()
{
	///does nothing
}


vector<SmartRefTable*>::iterator FindTableWithFile(TFile* file)
{
	vector<SmartRefTable*>::iterator iter_SRT;
	vector<SmartRefTable*>::iterator end=mSRefList.end();
	for(iter_SRT=mSRefList.begin();iter_SRT!=end;++iter_SRT)
	{
		if((*iter_SRT)->CheckFile(file))
			break;
	}
	return iter_SRT;
	
}

