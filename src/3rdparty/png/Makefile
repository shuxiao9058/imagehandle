LIBCOMMAND = ar cr
RANLIB = ranlib

#
# Object files...
#
OBJS	=	png.o pngset.o pngget.o pngrutil.o pngtrans.o pngwutil.o \
		pngread.o pngrio.o pngwio.o pngwrite.o pngrtran.o \
		pngwtran.o pngmem.o pngerror.o pngpread.o

LIBJPEG	=	./libfltk_png.a

#
# Make all targets...
#

all:	$(LIBJPEG)


#
# Clean all targets and object files...
#

clean:
	$(RM) $(OBJS)
	$(RM) $(LIBJPEG)

#
# libfltk_jpeg.a
#

$(LIBJPEG):	$(OBJS)
	echo Archiving $@...
#	$(RM) $@
	$(LIBCOMMAND) $@ $(OBJS)
	$(RANLIB) $@

