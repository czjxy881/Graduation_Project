EXECEXT=
#ifeq (${OSTYPE}, cygwin)
ifeq (${OS},Windows_NT)
	CPPFLAGS1=-DHAVE_CYGWIN 
	CC=gcc -g
	EXECEXT=.exe
else 
	CPPFLAGS1=-DHAVE_UNIX
#	CC=cc -g -xCC
	CC=gcc -g -m32
endif

INSTALL_DIR = ${HOME}/bin
CPPFLAGS=${CPPFLAGS1} -DDEBUGNO

SOURCEC= graph.c stringset.c path.c loadfinger.c  filter.c query.c parse.c update.c debug.c table.c dataoperation.c
OBJC=graph.o stringset.o path.o loadfinger.o  filter.o query.o parse.o update.o debug.o table.o dataoperation.o


all: graphbuild graphgrep

graphbuild:${OBJC} graphbuild.o
	${CC} -o graphbuild graphbuild.o ${OBJC}; \
	mv graphbuild${EXECEXT} ../

graphgrep:${OBJC} graphgrep.o
	${CC} -o graphgrep graphgrep.o ${OBJC}; \
	mv graphgrep${EXECEXT} ../

depend:${SOURCEC} common.h
	/bin/rm -rf Makedepend
	gcc -MM ${SOURCEC} > Makedepend

include Makedepend# DO NOT DELETE



# DO NOT DELETE
