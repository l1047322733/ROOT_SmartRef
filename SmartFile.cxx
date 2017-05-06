#include "SmartFile.h"
#include <string>
#include <cstdio>
ClassImp(SmartFile)

SmartFile::SmartFile():TFile()
{
	SmartRefManager::GetManager(1);
}

SmartFile::SmartFile(const char *fname, `Option_t *option="", const char *ftitle="", Int_t compress=1):TFile(const char *fname, Option_t *option="", const char *ftitle="", Int_t compress=1)
{
	if(TestBit(kHasReferences))
	{
		TIter next(GetListOfKeys());
		TKey* key;
		const char* target="SmartRefTable";
		While((key=(TKey*)next() ))
		{
			if(!scrcmp(key->GetClaassName(),target))
			{
				SmartRefTable* SRT=(SmartRefTable*) key->GetObj();
				SmartRefManager* SRM=SmartRefManager::GetManager(1);
				SRT->SetFile(this);
				SRM->AddTable(*SRT);
			}
		}
	}
}

SmartFile::~SmartFile()
{
	Close();
}

void SmartFile::Close()
{
	SmartRefManager* SRM=SmartRefManager::GetManager(0);
	if(SRM)
	{
		SmartRefTable* table=SRM->RemoveTable(this);
		if(table)
			WriteTObject(table,"SmartRefTable","WriteDelete");
	}
	TFile::Close();
}

void SmartFile::Delete(const char *namecycle="")
{
	TObject* dobj=Get(namecycle);
	if(dobj->TestBit(kIsReferenced))
	{
		TKey* key=FineKey(namecycle);
		SmartRefManager* SRM=SmartRefManager::GetManager(0);
		if(SRM)
		{
			SRM->DeleteObject(this,key);
		}
	}
	TFile::Delete(namecycle);
}

TKey*	SmartFile::CreateKey(TDirectory* mother, const TObject* obj, const char* name, Int_t bufsize)
{
	TKey* skey=new TKey(obj, cl, name, bufsize, mother);
	if(!(obj->TestBit(kIsReferenced)))	return;
	SmartRefManager* SRM=SmartRefManager::GetManager(1);
	if(!SRM) return;
	
	UInt_t uid=obj->GetUniqueID();
 	TProcessID *pid = TProcessID::GetProcessWithUID(uid,obj);
	SRM->AddItemToTable(this,pid,uid,skey);
	return skey;
}

TKey*	SmartFile::CreateKey(TDirectory* mother, const void* obj, const TClass* cl, const char* name, Int_t bufsize)
{
	printf("TClass object cannot use delay-reading of SmartRef\n");
	return new TKey(obj, cl, name, bufsize, mother);
}
