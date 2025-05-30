
all: elements-mos outline-mos

elements-mos: elements.c
	gcc elements.c -I../include -oelements-mos -noixemul
	ppc-morphos-strip --strip-unneeded --remove-section=.comment elements-mos

outline-mos: outline.c
	gcc outline.c -I../include -ooutline-mos -noixemul
	ppc-morphos-strip --strip-unneeded --remove-section=.comment outline-mos

