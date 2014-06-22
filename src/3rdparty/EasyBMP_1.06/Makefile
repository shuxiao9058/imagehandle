LIBCOMMAND = ar cr
RANLIB = ranlib

#
# Object files...
#
OBJS	=	EasyBMP.o

LIBJPEG	=	./libEasyBMP.a

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

