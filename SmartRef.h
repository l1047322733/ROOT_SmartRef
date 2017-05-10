#ifndef _SmartRef_h_
#define _SmartRef_h_

#include"TRef.h"
#include"TObject.h"

class SmartRef:public TRef  {

public:
	SmartRef();
	SmartRef(TObject* obj);
	SmartRef(const SmartRef &sref);
	~SmartRef(){};

	void 		operator=(TObject *obj);
	SmartRef& 	operator=(const SmartRef &sref);

	TObject*	GetObject() const;
	

ClassDef(SmartRef,1);
};

#endif
