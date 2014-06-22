LIBCOMMAND = ar cr
RANLIB = ranlib

#
# Object files...
#
OBJS	=	adler32.o compress.o crc32.o uncompr.o deflate.o \
		trees.o zutil.o inflate.o inftrees.o inffast.o \
		gzclose.o gzlib.o gzread.o gzwrite.o infback.o

LIBJPEG	=	./libfltk_zlib.a

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

