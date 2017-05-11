#ifndef 	_SmartFile_h_
#define	_SmartFile_h_

#include "TFile.h"
#include "TKey.h"

class TKey;

class SmartFile:public TFile {
	friend class TDirectoryFile;
	friend class TFilePrefetch;
#ifdef R__USE_IMT
	friend class TBasket;
#endif

private:
	SmartFile(const SmartFile & );	//cannot be copied
	void 	operator=(const SmartFile &);
public:
	SmartFile();
	SmartFile(const char *fname, Option_t *option="", const char *ftitle="", Int_t compress=1);
	virtual ~SmartFile();
	virtual void 	Close();
	virtual void 	Delete(const char *namecycle="");
	virtual void	WriteStreamerInfo();
	virtual void	WriteSmartRefTable();
	TKey*	CreateKey(TDirectory* mother, const TObject* obj, const char* name, Int_t bufsize);
	TKey*	CreateKey(TDirectory* mother, const void* obj, const TClass* cl, const char* name, Int_t bufsize);


ClassDef(SmartFile,0);
};

#endif
