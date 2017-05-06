#ifdef 	_SmartFile_h_
#define	_SmartFile_h_

#include "TFile.h"


class SmartFile: public TFile {
	friend class TDirectoryFile;
	friend class TFilePrefetch;
#ifdef R__USE_IMT
	friend class TBasket;
#endif

private:
	SmartFile(const SmartFile & );	//cannot be copied
	void 	operator=(const TFile &);
public:
	SmartFile();
	SmartFile(const char *fname, Option_t *option="", const char *ftitle="", Int_t compress=1);
	~SmartFile();
	void 	Close();
	void 	Delete(const char *namecycle="");
	TKey*	CreateKey(TDirectory* mother, const TObject* obj, const char* name, Int_t bufsize);
	TKey*	CreateKey(TDirectory* mother, const void* obj, const TClass* cl, const char* name, Int_t bufsize);


ClassDef(SmartFile,1)
};

#endif





}

#endif
