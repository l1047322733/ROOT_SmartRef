#include "SmartFile.h"
#include "SmartRefManager.h"
#include "SmartRefTable.h"
#include "TObject.h"
#include <string>
#include <cstdio>

class SmartRefManager;
class SmartRefTable;


ClassImp(SmartFile)

SmartFile::SmartFile():TFile()
{
	SmartRefManager::GetManager(1);
}

SmartFile::SmartFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress):TFile(fname, option, ftitle, compress)
{
	TIter next(GetListOfKeys());
	TKey* key;
	const char* target="SmartRefTable";
	while((key=(TKey*)next() ))
	{
		if(!strcmp(key->GetClassName(),target))
		{
			SmartRefTable* SRT=(SmartRefTable*)(key->ReadObj());
			SmartRefManager* SRM=SmartRefManager::GetManager(1);
			SRT->SetFile(this);
			SRM->AddTable(SRT);
		}
	}
}

SmartFile::~SmartFile()
{
	if(gDebug>0) printf("dtor of SmartFile called\n");
	Close();
}

void SmartFile::Close()
{
	if(gDebug>0) printf("SmartFile::Close() called\n");
	TFile::Close();
}

void SmartFile::Delete(const char *namecycle)
{
	if(gDebug>0) printf("SmartFile::Delete() called\n");
	TObject* dobj=Get(namecycle);
	if(dobj->TestBit(kIsReferenced))
	{
		const size_t Max=2048;
		Short_t cycle;
		char name[Max];
		DecodeNameCycle(namecycle,name,cycle,Max);
		SmartRefManager* SRM=SmartRefManager::GetManager(0);
		if(SRM)
		{
			SRM->DeleteObject(this,name);
		}
	}
	TFile::Delete(namecycle);
}

void	SmartFile::WriteStreamerInfo()
{
	if(gDebug>0) printf("SmartFile::WriteStreamerInfo() called\n");
	WriteSmartRefTable();
	TFile::WriteStreamerInfo();
}

void	SmartFile::WriteSmartRefTable()
{
	if(gDebug>0) printf("SmartFile::WriteSmartRefTable() called\n");
	SmartRefManager* SRM=SmartRefManager::GetManager(0);
	if(SRM)
	{
		if(gDebug>0)	printf("Get SRM\n");
		SmartRefTable* SRT=SRM->RemoveTable(this);
		if(SRT)
		{
			if(gDebug>0)	printf("Get SRT\n");
			WriteTObject(SRT,"SmartRefTable","WriteDelete");
			if(gDebug>0) 	printf("Write SRT to file success\n");
			delete SRT;
		}
	}
}

TKey*	SmartFile::CreateKey(TDirectory* mother, const TObject* obj, const char* name, Int_t bufsize)
{
	TKey* skey=new TKey(obj, name, bufsize, mother);
	if(gDebug>0) printf("Create Key\n");
	if(!(obj->TestBit(kIsReferenced)))	return skey;
	if(gDebug>0) printf("IsReferenced\n");
	SmartRefManager* SRM=SmartRefManager::GetManager(1);
	if(!SRM) return skey;
	
	UInt_t uid=obj->GetUniqueID();
 	TProcessID *pid = TProcessID::GetProcessWithUID(uid,obj);
	if(gDebug>0) printf("Start to AddItem from File\n");
	SRM->AddItemToTable(this,pid,uid,name);
	if(gDebug>0) printf("AddItem success from File\n");
	return skey;
}

TKey*	SmartFile::CreateKey(TDirectory* mother, const void* obj, const TClass* cl, const char* name, Int_t bufsize)
{
	printf("TClass object cannot use delay-reading of SmartRef\n");
	return new TKey(obj, cl, name, bufsize, mother);
}
