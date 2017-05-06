#include"SmartRef.h"
#include"SmartRefManager.h"
#include"TRef.h"
#include"TROOT.h"


ClassImp(SmartRef)

SmartRef::SmartRef():TRef()
{
	SmartRefManager::GetManager(1);
}

SmartRef::SmartRef(TObject* obj)
{
	*this=obj;
}

SmartRef::SmartRef(const SmartRef &sref):TRef(sref)
{
	*this=sref;
}

void SmartRef::operator=(TObject *obj):TRef(obj)
{
	SmartRefManager::GetManager(1);
}

SmartRef& 	SmartRef::operator=(SmartRef &sref):TRef(sref)
{
	
}

TObject*	SmartRef::GetObject() const
{
	TObject* obj=TRef::GetObject();
	if(!obj)
	{
		if(TestBit(kHasUUID))	return obj;
		
		SmartRefManager* SRM=SmartRefManager::GetManager(0);
		if(!SRM)	return obj;
		
		TProcessID*	pid=GetPID();
		UInt_t	uid=GetUniqueID();
		obj=SRM->GetObject(pid,uid);
	}
	return obj;
}
