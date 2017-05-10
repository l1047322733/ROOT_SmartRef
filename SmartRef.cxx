#include"SmartRef.h"
#include"TObject.h"
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
	*this =obj;
}

SmartRef::SmartRef(const SmartRef &sref)
{
	*this =sref;
}

void SmartRef::operator=(TObject *obj)
{
	TRef::operator= (obj);
	SmartRefManager::GetManager(1);
}

SmartRef&	SmartRef::operator=(const SmartRef &sref)
{
	TRef::operator=(sref);
	///do nothing else
	return *this;
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
