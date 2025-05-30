
CFLAGS = -I../include
OBJS   = minicheck.o runtests.o chardata.o

test-mos: $(OBJS)
	ppc-morphos-gcc  -o $@ -I../include -noixemul $(OBJS)
	ppc-morphos-strip --strip-unneeded --remove-section=.comment $@

clean:
	rm *.o minicheck.o runtests.o chardata.o test-mos

minicheck.o: minicheck.c minicheck.h
runtests.o: runtests.c chardata.h minicheck.h
chardata.o: chardata.c minicheck.h chardata.h

