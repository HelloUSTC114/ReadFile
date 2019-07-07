# This makefile aims at procesing all file
# First, TestDict is dictionary that should be generated by root cling.
# Then, all .o file must be generated in order to reduce compile time
# .so library is final aim if no specific excutable file is needed.

DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib

# Search all .cpp files in this directory(main.cpp) and src directory
SRC = $(wildcard ${DIR_SRC}/*.cpp)
# Search all .h files in include directroy
HEADERPRE = $(wildcard ${DIR_INC}/*.h)
HEADER = ${subst ${DIR_INC}/LinkDef.h, ,$(HEADERPRE)}
# substitute all .cpp in $(SRC) with .o, and add related directory path
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}) )

CXXFLAGS = -g -Wall -I${DIR_INC} -I. `root-config --cflags`


LIB = ReadFile
LIBFULL = $(patsubst %,${DIR_LIB}/lib%.so,$(LIB))

DIR_LINK = ./lkdef
LINKDEF = ${DIR_INC}/LinkDef.h
DICTCXX = ${DIR_LINK}/$(LIB)Dict.cxx

Excutable = test


$(LIBFULL): $(OBJ) $(DICTCXX)
	`root-config --cxx`	-fPIC	-shared	-o	$@	$(OBJ)	$(DICTCXX)	`root-config --libs`  $(CXXFLAGS)

${DIR_OBJ}/%.o: ${DIR_SRC}/%.cpp ${DIR_INC}/%.h
	`root-config --cxx `	-o	$@	-c	$<	-fPIC $(CXXFLAGS)

$(DICTCXX): $(HEADER)   $(LINKDEF)
	rootcling	-f	$@	-c	$(HEADER)	$(LINKDEF)



clean:
	-rm $(OBJ) $(DIR_LINK)/*.cxx
distclean:
	-rm *.so *.pcm *.o *.root *.pdf $(Excutable) *Dict* filelist


test:  test.cpp $(LIBFULL)
	`root-config --cxx `	-o	$@	$<	-Llib	-l$(LIB)	`root-config --libs` $(CXXFLAGS)
	echo $(LIBFULL)



all: $(Excutable)

install: $(LIBFULL)
	-mkdir ../Excutable
	-cp $(Excutable) ../Excutable
	-cp $(LIBFULL) ../Excutable
	-cp *.pcm ../Excutable
	-cp rootlogon.C ../Excutable
	-cp ConfigFile ../Excutable