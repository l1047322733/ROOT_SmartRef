# Makefile with boost

INCLUDE:=/home/Liang/boost/boost_1_64_0
LINK:=/home/Liang/boost/boost_1_64_0/stage/lib

ROOTFLAG=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)

DEF:=SmartRef.h SmartFile.h SmartRefTable.h SmartRefManager.h
IMP:=SmartRef.cxx SmartFile.cxx SmartRefTable.cxx SmartRefManager.cxx

libSmartRef.so: SmartRef_dict.cxx $(IMP)
	g++ -fPIC -shared $(ROOTFLAG) -I$(INCLUDE) -L$(LINK) $(ROOTLIBS) -o libSmartRef.so  SmartRef_dict.cxx $(IMP) -lboost_system -lboost_thread

SmartRef_dict.cxx: $(DEF) LinkDef.h
	rootcling -f SmartRef_dict.cxx -rml libSmartRef.so -rmf libSmartRef.rootmap $(DEF) LinkDef.h

.PHONY: clean
clean:
	rm SmartRef_dict* libSmartRef.*
