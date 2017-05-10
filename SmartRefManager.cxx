#include"SmartRefManager.h"
#include<vector>
#include<mutex>
#include<boost/thread/shared_mutex.hpp>

SmartRefManager* 	SmartRefManager::mManager=0;
std::once_flag 	SmartRefManager::ini_flag;

ClassImp(SmartRefManager)

SmartRefManager::~SmartRefManager()
{
	std::lock_guard<boost::shared_mutex> lk(Manager_mutex);
	
	std::vector<SmartRefTable*>::iterator iter_SRT;
	std::vector<SmartRefTable*>::iterator end=mSRefList.end();
	for(iter_SRT=mSRefList.begin();iter_SRT!=end;++iter_SRT)
		delete *iter_SRT;

	delete mManager;	
}

SmartRefManager* 	SmartRefManager::GetManager(Int_t option)
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

void	SmartRefManager::AddItemToTable(SmartFile* file,TProcessID* fP,UInt_t uid, const char* name)
{
	boost::shared_lock<boost::shared_mutex> lk(Manager_mutex);
	std::vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	lk.unlock();
	std::lock_guard<boost::shared_mutex> lk2(Manager_mutex);
	if(iter_SRT==mSRefList.end())
	{
		mSRefList.push_back(new SmartRefTable(file));
		iter_SRT=mSRefList.end()-1;
	}
	
	(*iter_SRT)->AddItem(fP, uid, name);
}

SmartRefTable*	SmartRefManager::RemoveTable(SmartFile* file)
{
	SmartRefTable* ptr_SRT=nullptr;
	boost::shared_lock<boost::shared_mutex> lk(Manager_mutex);
	std::vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	if(iter_SRT==mSRefList.end())
		return ptr_SRT;

	ptr_SRT=*iter_SRT;
	lk.unlock();
	std::lock_guard<boost::shared_mutex> lk2(Manager_mutex);
	mSRefList.erase(iter_SRT);

	return ptr_SRT;
}

TObject*	SmartRefManager::GetObject(TProcessID* fP,UInt_t uid)
{
	TKey* obj_key=0;
	boost::shared_lock<boost::shared_mutex> lk(Manager_mutex);
	std::vector<SmartRefTable*>::iterator iter_SRT;
	std::vector<SmartRefTable*>::iterator itend=mSRefList.end();
	for(iter_SRT=mSRefList.begin();iter_SRT!=itend;iter_SRT++)
	{
		obj_key=(*iter_SRT)->FindItem(fP,uid);
		if(obj_key)
			return obj_key->ReadObj();
	}
	return 0;
}

void		SmartRefManager::DeleteObject(SmartFile* file,const char* name)
{
	boost::shared_lock<boost::shared_mutex> lk(Manager_mutex);
	std::vector<SmartRefTable*>::iterator iter_SRT=FindTableWithFile(file);
	if(iter_SRT==mSRefList.end())
		return;
		
	(*iter_SRT)->DeleteItem(name);
}

SmartRefManager::SmartRefManager()
{
	///does nothing
}


std::vector<SmartRefTable*>::iterator SmartRefManager::FindTableWithFile(SmartFile* file)
{
	std::vector<SmartRefTable*>::iterator iter_SRT;
	std::vector<SmartRefTable*>::iterator end=mSRefList.end();
	boost::shared_lock<boost::shared_mutex> lk(Manager_mutex);
	for(iter_SRT=mSRefList.begin();iter_SRT!=end;++iter_SRT)
	{
		if((*iter_SRT)->CheckFile(file))
			break;
	}
	return iter_SRT;
	
}

