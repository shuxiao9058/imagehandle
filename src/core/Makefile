ifeq ($(OS),Windows_NT)
	CC="C:\Program Files (x86)\Dev-Cpp\MinGW32\bin\gcc"
	CXX="C:\Program Files (x86)\Dev-Cpp\MinGW32\bin\g++"
	SHIT=\\
	DEL=del
	INCS=-I./inc -I"C:/Program Files (x86)/opencv/include"
	LIBS=-L"C:/Program Files (x86)/Dev-Cpp/MinGW32/lib" -L"C:/Program Files (x86)/opencv/mingw/lib"

	CFLAGS=
	LDFLAGS=-lopencv_calib3d241 -lopencv_contrib241 -lopencv_core241 -lopencv_features2d241 -lopencv_flann241 -lopencv_gpu241 -lopencv_highgui241 -lopencv_imgproc241 -lopencv_legacy241 -lopencv_ml241 -lopencv_nonfree241 -lopencv_objdetect241 -lopencv_photo241 -lopencv_stitching241 -lopencv_ts241 -lopencv_video241 -lopencv_videostab241 
	PROGRAM=$(OUTDIR)$(SHIT)demo.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CC=gcc
		CXX=g++
		SHIT=/
		DEL=rm -f
	endif

	ifeq ($(UNAME_S),Darwin)
		CC=gcc
		CXX=g++
		SHIT=/
		DEL=rm -f

		INCS=-I./inc
		LIBS=./libfltk_jpeg.a ./libfltk_zlib.a ./libfltk_png.a ./libEasyBMP.a
	endif

	CFLAGS=`pkg-config --cflags opencv`
	LDFLAGS=`pkg-config --libs opencv`
	PROGRAM=$(OUTDIR)$(SHIT)example
endif

# The source file types (headers excluded).
# .c indicates C source files, and others C++ ones.
SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp

# The header file types.
HDREXTS = .h .H .hh .hpp .HPP .h++ .hxx .hp

CFLAGS+=-O2 -Wall -g -fpermissive
LDFLAGS+=-lm

SRCDIRS=
OUTDIR=.

ifeq ($(PROGRAM),)
  CUR_PATH_NAMES = $(subst /,$(SPACE),$(subst $(SPACE),_,$(CURDIR)))
  PROGRAM = $(word $(words $(CUR_PATH_NAMES)),$(CUR_PATH_NAMES))
  ifeq ($(PROGRAM),)
    PROGRAM = a.out
  endif
endif

ifeq ($(SRCDIRS),)
  SRCDIRS = .
endif

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
HEADERS = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))
SRC_CXX = $(filter-out %.c,$(SOURCES))
OBJS    = $(addsuffix .o, $(basename $(SOURCES)))

## Define some useful variables.
COMPILE.c   = $(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -c
COMPILE.cxx = $(CXX) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(INCS) -c
LINK.c      = $(CC) $(CFLAGS) $(CPPFLAGS) 
LINK.cxx    = $(CXX) $(CXXFLAGS) $(CPPFLAGS) 

# Rules for generating the executable.
#-------------------------------------
all:DEFS=-DIS_ALL=1
all:$(OBJS)
ifeq ($(SRC_CXX),)              # C program
	$(LINK.c) $(OBJS) $(LIBS) $(LDFLAGS) -o $(PROGRAM)
else	                   # C++ program
	$(LINK.cxx) $(OBJS) $(LIBS) $(LDFLAGS) -o $(PROGRAM)
endif

# Rules for generating object files (.o).
#----------------------------------------
objs:$(OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@

%.o:%.C
	$(COMPILE.cxx) $< -o $@

%.o:%.cc
	$(COMPILE.cxx) $< -o $@

%.o:%.cpp
	$(COMPILE.cxx) $< -o $@

%.o:%.CPP
	$(COMPILE.cxx) $< -o $@

%.o:%.c++
	$(COMPILE.cxx) $< -o $@

%.o:%.cp
	$(COMPILE.cxx) $< -o $@

%.o:%.cxx
	$(COMPILE.cxx) $< -o $@

check-syntax:
	$(CXXCOMPILE) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

clean:
	$(DEL) $(SRCDIRS)$(SHIT)*.o
	$(DEL) $(SRCDIRS)$(SHIT)*.~
	$(DEL) $(OUTDIR)$(SHIT)*.~
	$(DEL) $(OUTDIR)$(SHIT)$(PROGRAM)

rebuild:clean all

