
benchmark-mos: benchmark.c
	gcc benchmark.c -I../../include -obenchmark-mos -noixemul
	ppc-morphos-strip --strip-unneeded --remove-section=.comment benchmark-mos

