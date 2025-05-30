TARGET  = libexpat.a
DEST    = LIBS:

CC      = m68k-amigaos-gcc
AR      = m68k-amigaos-ar
STRIP   = m68k-amigaos-strip

CDEFS   = -DUSE_INLINE_STDARG
CWARN   = -Wall \
          -Wno-missing-braces \
          -Wno-parentheses \
          -Wno-switch \
          -Wstrict-prototypes \
          -Werror-implicit-function-declaration \
          -Wpointer-arith

CFLAGS  = $(CDEFS) $(CWARN) -O2 -fomit-frame-pointer -fstrict-aliasing \
          -noixemul -I../include/ -m68020-60

OBJS = init.o loc.o utils.o xmlparse.o xmlrole.o xmltok.o

$(TARGET): $(OBJS)
	$(AR) rcs $@ $(OBJS)
	$(STRIP) --strip-unneeded --remove-section=.comment $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	@copy $(TARGET) $(DEST)
	@delete $(TARGET)
	@avail flush

clean:
	rm -f *.o
	rm -f $(TARGET)
