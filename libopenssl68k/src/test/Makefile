#
# test/Makefile
#

DIR=		test
TOP=		..
CC=		cc
INCLUDES=	-I$(TOP) -I../include $(KRB5_INCLUDES)
CFLAG=		-g
MAKEDEPEND=	$(TOP)/util/domd $(TOP) -MD $(MAKEDEPPROG)
PERL=		perl
# KRB5 stuff
KRB5_INCLUDES=
LIBKRB5=

PEX_LIBS=
EX_LIBS= #-lnsl -lsocket

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile maketests.com \
	tests.com testenc.com tx509.com trsa.com tcrl.com tsid.com treq.com \
	tpkcs7.com tpkcs7d.com tverify.com testgen.com testss.com testssl.com \
	testca.com VMSca-response.1 VMSca-response.2

DLIBCRYPTO= ../libcrypto.a
DLIBSSL= ../libssl.a
LIBCRYPTO= -L.. -lcrypto
LIBSSL= -L.. -lssl

BNTEST=		bntest
ECTEST=		ectest
ECDSATEST=	ecdsatest
ECDHTEST=	ecdhtest
EXPTEST=	exptest
IDEATEST=	ideatest
SHATEST=	shatest
SHA1TEST=	sha1test
SHA256TEST=	sha256t
SHA512TEST=	sha512t
MDC2TEST=	mdc2test
RMDTEST=	rmdtest
MD2TEST=	md2test
MD4TEST=	md4test
MD5TEST=	md5test
HMACTEST=	hmactest
WPTEST=		wp_test
RC2TEST=	rc2test
RC4TEST=	rc4test
RC5TEST=	rc5test
BFTEST=		bftest
CASTTEST=	casttest
DESTEST=	destest
RANDTEST=	randtest
DHTEST=		dhtest
DSATEST=	dsatest
METHTEST=	methtest
SSLTEST=	ssltest
RSATEST=	rsa_test
ENGINETEST=	enginetest
EVPTEST=	evp_test
IGETEST=	igetest
JPAKETEST=	jpaketest
SRPTEST=	srptest
ASN1TEST=	asn1test
HEARTBEATTEST=  heartbeat_test
CONSTTIMETEST=  constant_time_test

TESTS=		alltests

EXE=	$(BNTEST)$(EXE_EXT) $(ECTEST)$(EXE_EXT)  $(ECDSATEST)$(EXE_EXT) $(ECDHTEST)$(EXE_EXT) $(IDEATEST)$(EXE_EXT) \
	$(MD2TEST)$(EXE_EXT)  $(MD4TEST)$(EXE_EXT) $(MD5TEST)$(EXE_EXT) $(HMACTEST)$(EXE_EXT) $(WPTEST)$(EXE_EXT) \
	$(RC2TEST)$(EXE_EXT) $(RC4TEST)$(EXE_EXT) $(RC5TEST)$(EXE_EXT) \
	$(DESTEST)$(EXE_EXT) $(SHATEST)$(EXE_EXT) $(SHA1TEST)$(EXE_EXT) $(SHA256TEST)$(EXE_EXT) $(SHA512TEST)$(EXE_EXT) \
	$(MDC2TEST)$(EXE_EXT) $(RMDTEST)$(EXE_EXT) \
	$(RANDTEST)$(EXE_EXT) $(DHTEST)$(EXE_EXT) $(ENGINETEST)$(EXE_EXT) \
	$(BFTEST)$(EXE_EXT) $(CASTTEST)$(EXE_EXT) $(SSLTEST)$(EXE_EXT) $(EXPTEST)$(EXE_EXT) $(DSATEST)$(EXE_EXT) $(RSATEST)$(EXE_EXT) \
	$(EVPTEST)$(EXE_EXT) $(IGETEST)$(EXE_EXT) $(JPAKETEST)$(EXE_EXT) $(SRPTEST)$(EXE_EXT) \
	$(ASN1TEST)$(EXE_EXT) $(HEARTBEATTEST)$(EXE_EXT) $(CONSTTIMETEST)$(EXE_EXT)

# $(METHTEST)$(EXE_EXT)

OBJ=	$(BNTEST).o $(ECTEST).o  $(ECDSATEST).o $(ECDHTEST).o $(IDEATEST).o \
	$(MD2TEST).o $(MD4TEST).o $(MD5TEST).o \
	$(HMACTEST).o $(WPTEST).o \
	$(RC2TEST).o $(RC4TEST).o $(RC5TEST).o \
	$(DESTEST).o $(SHATEST).o $(SHA1TEST).o $(SHA256TEST).o $(SHA512TEST).o \
	$(MDC2TEST).o $(RMDTEST).o \
	$(RANDTEST).o $(DHTEST).o $(ENGINETEST).o $(CASTTEST).o \
	$(BFTEST).o  $(SSLTEST).o  $(DSATEST).o  $(EXPTEST).o $(RSATEST).o \
	$(EVPTEST).o $(IGETEST).o $(JPAKETEST).o $(ASN1TEST).o \
	$(HEARTBEATTEST).o $(CONSTTIMETEST).o

SRC=	$(BNTEST).c $(ECTEST).c  $(ECDSATEST).c $(ECDHTEST).c $(IDEATEST).c \
	$(MD2TEST).c  $(MD4TEST).c $(MD5TEST).c \
	$(HMACTEST).c $(WPTEST).c \
	$(RC2TEST).c $(RC4TEST).c $(RC5TEST).c \
	$(DESTEST).c $(SHATEST).c $(SHA1TEST).c $(MDC2TEST).c $(RMDTEST).c \
	$(RANDTEST).c $(DHTEST).c $(ENGINETEST).c $(CASTTEST).c \
	$(BFTEST).c  $(SSLTEST).c $(DSATEST).c   $(EXPTEST).c $(RSATEST).c \
	$(EVPTEST).c $(IGETEST).c $(JPAKETEST).c $(SRPTEST).c $(ASN1TEST).c \
	$(HEARTBEATTEST).c $(CONSTTIMETEST).c

EXHEADER= 
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ..; $(MAKE) DIRS=$(DIR) TESTS=$(TESTS) all)

all:	exe

exe:	$(EXE) dummytest$(EXE_EXT)

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:

generate: $(SRC)
$(SRC):
	@sh $(TOP)/util/point.sh dummytest.c $@

errors:

install:

tags:
	ctags $(SRC)

tests:	exe apps $(TESTS)

apps:
	@(cd ..; $(MAKE) DIRS=apps all)

alltests: \
	test_des test_idea test_sha test_md4 test_md5 test_hmac \
	test_md2 test_mdc2 test_wp \
	test_rmd test_rc2 test_rc4 test_rc5 test_bf test_cast test_aes \
	test_rand test_bn test_ec test_ecdsa test_ecdh \
	test_enc test_x509 test_rsa test_crl test_sid \
	test_gen test_req test_pkcs7 test_verify test_dh test_dsa \
	test_ss test_ca test_engine test_evp test_ssl test_tsa test_ige \
	test_jpake test_srp test_cms test_heartbeat test_constant_time

test_evp:
	../util/shlib_wrap.sh ./$(EVPTEST) evptests.txt

test_des:
	../util/shlib_wrap.sh ./$(DESTEST)

test_idea:
	../util/shlib_wrap.sh ./$(IDEATEST)

test_sha:
	../util/shlib_wrap.sh ./$(SHATEST)
	../util/shlib_wrap.sh ./$(SHA1TEST)
	../util/shlib_wrap.sh ./$(SHA256TEST)
	../util/shlib_wrap.sh ./$(SHA512TEST)

test_mdc2:
	../util/shlib_wrap.sh ./$(MDC2TEST)

test_md5:
	../util/shlib_wrap.sh ./$(MD5TEST)

test_md4:
	../util/shlib_wrap.sh ./$(MD4TEST)

test_hmac:
	../util/shlib_wrap.sh ./$(HMACTEST)

test_wp:
	../util/shlib_wrap.sh ./$(WPTEST)

test_md2:
	../util/shlib_wrap.sh ./$(MD2TEST)

test_rmd:
	../util/shlib_wrap.sh ./$(RMDTEST)

test_bf:
	../util/shlib_wrap.sh ./$(BFTEST)

test_cast:
	../util/shlib_wrap.sh ./$(CASTTEST)

test_rc2:
	../util/shlib_wrap.sh ./$(RC2TEST)

test_rc4:
	../util/shlib_wrap.sh ./$(RC4TEST)

test_rc5:
	../util/shlib_wrap.sh ./$(RC5TEST)

test_rand:
	../util/shlib_wrap.sh ./$(RANDTEST)

test_enc:
	@sh ./testenc

test_x509:
	echo test normal x509v1 certificate
	sh ./tx509 2>/dev/null
	echo test first x509v3 certificate
	sh ./tx509 v3-cert1.pem 2>/dev/null
	echo test second x509v3 certificate
	sh ./tx509 v3-cert2.pem 2>/dev/null

test_rsa: $(RSATEST)$(EXE_EXT)
	@sh ./trsa 2>/dev/null
	../util/shlib_wrap.sh ./$(RSATEST)

test_crl:
	@sh ./tcrl 2>/dev/null

test_sid:
	@sh ./tsid 2>/dev/null

test_req:
	@sh ./treq 2>/dev/null
	@sh ./treq testreq2.pem 2>/dev/null

test_pkcs7:
	@sh ./tpkcs7 2>/dev/null
	@sh ./tpkcs7d 2>/dev/null

test_bn:
	@echo starting big number library test, could take a while...
	@../util/shlib_wrap.sh ./$(BNTEST) >tmp.bntest
	@echo quit >>tmp.bntest
	@echo "running bc"
	@<tmp.bntest sh -c "`sh ./bctest ignore`" | $(PERL) -e '$$i=0; while (<STDIN>) {if (/^test (.*)/) {print STDERR "\nverify $$1";} elsif (!/^0\r?$$/) {die "\nFailed! bc: $$_";} else {print STDERR "."; $$i++;}} print STDERR "\n$$i tests passed\n"'
	@echo 'test a^b%c implementations'
	../util/shlib_wrap.sh ./$(EXPTEST)

test_ec:
	@echo 'test elliptic curves'
	../util/shlib_wrap.sh ./$(ECTEST)

test_ecdsa:
	@echo 'test ecdsa'
	../util/shlib_wrap.sh ./$(ECDSATEST)

test_ecdh:
	@echo 'test ecdh'
	../util/shlib_wrap.sh ./$(ECDHTEST)

test_verify:
	@echo "The following command should have some OK's and some failures"
	@echo "There are definitly a few expired certificates"
	../util/shlib_wrap.sh ../apps/openssl verify -CApath ../certs/demo ../certs/demo/*.pem

test_dh:
	@echo "Generate a set of DH parameters"
	../util/shlib_wrap.sh ./$(DHTEST)

test_dsa:
	@echo "Generate a set of DSA parameters"
	../util/shlib_wrap.sh ./$(DSATEST)
	../util/shlib_wrap.sh ./$(DSATEST) -app2_1

test_gen:
	@echo "Generate and verify a certificate request"
	@sh ./testgen

test_ss keyU.ss certU.ss certCA.ss certP1.ss keyP1.ss certP2.ss keyP2.ss \
		intP1.ss intP2.ss: testss
	@echo "Generate and certify a test certificate"
	@sh ./testss
	@cat certCA.ss certU.ss > intP1.ss
	@cat certCA.ss certU.ss certP1.ss > intP2.ss

test_engine: 
	@echo "Manipulate the ENGINE structures"
	../util/shlib_wrap.sh ./$(ENGINETEST)

test_ssl: keyU.ss certU.ss certCA.ss certP1.ss keyP1.ss certP2.ss keyP2.ss \
		intP1.ss intP2.ss
	@echo "test SSL protocol"
	@if [ -n "$(FIPSCANLIB)" ]; then \
	  sh ./testfipsssl keyU.ss certU.ss certCA.ss; \
	fi
	../util/shlib_wrap.sh ./$(SSLTEST) -test_cipherlist
	@sh ./testssl keyU.ss certU.ss certCA.ss
	@sh ./testsslproxy keyP1.ss certP1.ss intP1.ss
	@sh ./testsslproxy keyP2.ss certP2.ss intP2.ss

test_ca:
	@if ../util/shlib_wrap.sh ../apps/openssl no-rsa; then \
	  echo "skipping CA.sh test -- requires RSA"; \
	else \
	  echo "Generate and certify a test certificate via the 'ca' program"; \
	  sh ./testca; \
	fi

test_aes: #$(AESTEST)
#	@echo "test Rijndael"
#	../util/shlib_wrap.sh ./$(AESTEST)

test_tsa:
	@if ../util/shlib_wrap.sh ../apps/openssl no-rsa; then \
	  echo "skipping testtsa test -- requires RSA"; \
	else \
	  sh ./testtsa; \
	fi

test_ige: $(IGETEST)$(EXE_EXT)
	@echo "Test IGE mode"
	../util/shlib_wrap.sh ./$(IGETEST)

test_jpake: $(JPAKETEST)$(EXE_EXT)
	@echo "Test JPAKE"
	../util/shlib_wrap.sh ./$(JPAKETEST)

test_cms:
	@echo "CMS consistency test"
	$(PERL) cms-test.pl

test_srp: $(SRPTEST)$(EXE_EXT)
	@echo "Test SRP"
	../util/shlib_wrap.sh ./srptest

test_heartbeat: $(HEARTBEATTEST)$(EXE_EXT)
	../util/shlib_wrap.sh ./$(HEARTBEATTEST)

test_constant_time: $(CONSTTIMETEST)$(EXE_EXT)
	@echo "Test constant time utilites"
	../util/shlib_wrap.sh ./$(CONSTTIMETEST)

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	@if [ -z "$(THIS)" ]; then \
	    $(MAKE) -f $(TOP)/Makefile reflect THIS=$@; \
	else \
	    $(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(SRC); \
	fi

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)
	rm -f $(SRC) $(SHA256TEST).c $(SHA512TEST).c evptests.txt newkey.pem testkey.pem \
			testreq.pem

clean:
	rm -f .rnd tmp.bntest tmp.bctest *.o *.obj *.dll lib tags core .pure .nfs* *.old *.bak fluff $(EXE) *.ss *.srl log dummytest

$(DLIBSSL):
	(cd ..; $(MAKE) DIRS=ssl all)

$(DLIBCRYPTO):
	(cd ..; $(MAKE) DIRS=crypto all)

BUILD_CMD=shlib_target=; if [ -n "$(SHARED_LIBS)" ]; then \
		shlib_target="$(SHLIB_TARGET)"; \
	fi; \
	LIBRARIES="$(LIBSSL) $(LIBCRYPTO) $(LIBKRB5)"; \
	$(MAKE) -f $(TOP)/Makefile.shared -e \
		CC="$${CC}" APPNAME=$$target$(EXE_EXT) OBJECTS="$$target.o" \
		LIBDEPS="$(PEX_LIBS) $$LIBRARIES $(EX_LIBS)" \
		link_app.$${shlib_target}

FIPS_BUILD_CMD=shlib_target=; if [ -n "$(SHARED_LIBS)" ]; then \
		shlib_target="$(SHLIB_TARGET)"; \
	fi; \
	LIBRARIES="$(LIBSSL) $(LIBCRYPTO) $(LIBKRB5)"; \
	if [ -z "$(SHARED_LIBS)" -a -n "$(FIPSCANLIB)" ] ; then \
		FIPSLD_CC="$(CC)"; CC=$(FIPSDIR)/bin/fipsld; export CC FIPSLD_CC; \
	fi; \
	$(MAKE) -f $(TOP)/Makefile.shared -e \
		CC="$${CC}" APPNAME=$$target$(EXE_EXT) OBJECTS="$$target.o" \
		LIBDEPS="$(PEX_LIBS) $$LIBRARIES $(EX_LIBS)" \
		link_app.$${shlib_target}

BUILD_CMD_STATIC=shlib_target=; \
	LIBRARIES="$(DLIBSSL) $(DLIBCRYPTO) $(LIBKRB5)"; \
	$(MAKE) -f $(TOP)/Makefile.shared -e \
		APPNAME=$$target$(EXE_EXT) OBJECTS="$$target.o" \
		LIBDEPS="$(PEX_LIBS) $$LIBRARIES $(EX_LIBS)" \
		link_app.$${shlib_target}

$(RSATEST)$(EXE_EXT): $(RSATEST).o $(DLIBCRYPTO)
	@target=$(RSATEST); $(BUILD_CMD)

$(BNTEST)$(EXE_EXT): $(BNTEST).o $(DLIBCRYPTO)
	@target=$(BNTEST); $(BUILD_CMD)

$(ECTEST)$(EXE_EXT): $(ECTEST).o $(DLIBCRYPTO)
	@target=$(ECTEST); $(BUILD_CMD)

$(EXPTEST)$(EXE_EXT): $(EXPTEST).o $(DLIBCRYPTO)
	@target=$(EXPTEST); $(BUILD_CMD)

$(IDEATEST)$(EXE_EXT): $(IDEATEST).o $(DLIBCRYPTO)
	@target=$(IDEATEST); $(BUILD_CMD)

$(MD2TEST)$(EXE_EXT): $(MD2TEST).o $(DLIBCRYPTO)
	@target=$(MD2TEST); $(BUILD_CMD)

$(SHATEST)$(EXE_EXT): $(SHATEST).o $(DLIBCRYPTO)
	@target=$(SHATEST); $(BUILD_CMD)

$(SHA1TEST)$(EXE_EXT): $(SHA1TEST).o $(DLIBCRYPTO)
	@target=$(SHA1TEST); $(BUILD_CMD)

$(SHA256TEST)$(EXE_EXT): $(SHA256TEST).o $(DLIBCRYPTO)
	@target=$(SHA256TEST); $(BUILD_CMD)

$(SHA512TEST)$(EXE_EXT): $(SHA512TEST).o $(DLIBCRYPTO)
	@target=$(SHA512TEST); $(BUILD_CMD)

$(RMDTEST)$(EXE_EXT): $(RMDTEST).o $(DLIBCRYPTO)
	@target=$(RMDTEST); $(BUILD_CMD)

$(MDC2TEST)$(EXE_EXT): $(MDC2TEST).o $(DLIBCRYPTO)
	@target=$(MDC2TEST); $(BUILD_CMD)

$(MD4TEST)$(EXE_EXT): $(MD4TEST).o $(DLIBCRYPTO)
	@target=$(MD4TEST); $(BUILD_CMD)

$(MD5TEST)$(EXE_EXT): $(MD5TEST).o $(DLIBCRYPTO)
	@target=$(MD5TEST); $(BUILD_CMD)

$(HMACTEST)$(EXE_EXT): $(HMACTEST).o $(DLIBCRYPTO)
	@target=$(HMACTEST); $(BUILD_CMD)

$(WPTEST)$(EXE_EXT): $(WPTEST).o $(DLIBCRYPTO)
	@target=$(WPTEST); $(BUILD_CMD)

$(RC2TEST)$(EXE_EXT): $(RC2TEST).o $(DLIBCRYPTO)
	@target=$(RC2TEST); $(BUILD_CMD)

$(BFTEST)$(EXE_EXT): $(BFTEST).o $(DLIBCRYPTO)
	@target=$(BFTEST); $(BUILD_CMD)

$(CASTTEST)$(EXE_EXT): $(CASTTEST).o $(DLIBCRYPTO)
	@target=$(CASTTEST); $(BUILD_CMD)

$(RC4TEST)$(EXE_EXT): $(RC4TEST).o $(DLIBCRYPTO)
	@target=$(RC4TEST); $(BUILD_CMD)

$(RC5TEST)$(EXE_EXT): $(RC5TEST).o $(DLIBCRYPTO)
	@target=$(RC5TEST); $(BUILD_CMD)

$(DESTEST)$(EXE_EXT): $(DESTEST).o $(DLIBCRYPTO)
	@target=$(DESTEST); $(BUILD_CMD)

$(RANDTEST)$(EXE_EXT): $(RANDTEST).o $(DLIBCRYPTO)
	@target=$(RANDTEST); $(BUILD_CMD)

$(DHTEST)$(EXE_EXT): $(DHTEST).o $(DLIBCRYPTO)
	@target=$(DHTEST); $(BUILD_CMD)

$(DSATEST)$(EXE_EXT): $(DSATEST).o $(DLIBCRYPTO)
	@target=$(DSATEST); $(BUILD_CMD)

$(METHTEST)$(EXE_EXT): $(METHTEST).o $(DLIBCRYPTO)
	@target=$(METHTEST); $(BUILD_CMD)

$(SSLTEST)$(EXE_EXT): $(SSLTEST).o $(DLIBSSL) $(DLIBCRYPTO)
	@target=$(SSLTEST); $(FIPS_BUILD_CMD)

$(ENGINETEST)$(EXE_EXT): $(ENGINETEST).o $(DLIBCRYPTO)
	@target=$(ENGINETEST); $(BUILD_CMD)

$(EVPTEST)$(EXE_EXT): $(EVPTEST).o $(DLIBCRYPTO)
	@target=$(EVPTEST); $(BUILD_CMD)

$(ECDSATEST)$(EXE_EXT): $(ECDSATEST).o $(DLIBCRYPTO)
	@target=$(ECDSATEST); $(BUILD_CMD)

$(ECDHTEST)$(EXE_EXT): $(ECDHTEST).o $(DLIBCRYPTO)
	@target=$(ECDHTEST); $(BUILD_CMD)

$(IGETEST)$(EXE_EXT): $(IGETEST).o $(DLIBCRYPTO)
	@target=$(IGETEST); $(BUILD_CMD)

$(JPAKETEST)$(EXE_EXT): $(JPAKETEST).o $(DLIBCRYPTO)
	@target=$(JPAKETEST); $(BUILD_CMD)

$(ASN1TEST)$(EXE_EXT): $(ASN1TEST).o $(DLIBCRYPTO)
	@target=$(ASN1TEST); $(BUILD_CMD)

$(SRPTEST)$(EXE_EXT): $(SRPTEST).o $(DLIBCRYPTO)
	@target=$(SRPTEST); $(BUILD_CMD)

$(HEARTBEATTEST)$(EXE_EXT): $(HEARTBEATTEST).o $(DLIBCRYPTO)
	@target=$(HEARTBEATTEST); $(BUILD_CMD_STATIC)

$(CONSTTIMETEST)$(EXE_EXT): $(CONSTTIMETEST).o
	@target=$(CONSTTIMETEST) $(BUILD_CMD)

#$(AESTEST).o: $(AESTEST).c
#	$(CC) -c $(CFLAGS) -DINTERMEDIATE_VALUE_KAT -DTRACE_KAT_MCT $(AESTEST).c

#$(AESTEST)$(EXE_EXT): $(AESTEST).o $(DLIBCRYPTO)
#	if [ "$(SHLIB_TARGET)" = "hpux-shared" -o "$(SHLIB_TARGET)" = "darwin-shared" ] ; then \
#	  $(CC) -o $(AESTEST)$(EXE_EXT) $(CFLAGS) $(AESTEST).o $(PEX_LIBS) $(DLIBCRYPTO) $(EX_LIBS) ; \
#	else \
#	  $(CC) -o $(AESTEST)$(EXE_EXT) $(CFLAGS) $(AESTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS) ; \
#	fi

dummytest$(EXE_EXT): dummytest.o $(DLIBCRYPTO)
	@target=dummytest; $(BUILD_CMD)

# DO NOT DELETE THIS LINE -- make depend depends on it.

asn1test.o: ../include/openssl/asn1.h ../include/openssl/asn1_mac.h
asn1test.o: ../include/openssl/bio.h ../include/openssl/buffer.h
asn1test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
asn1test.o: ../include/openssl/ec.h ../include/openssl/ecdh.h
asn1test.o: ../include/openssl/ecdsa.h ../include/openssl/evp.h
asn1test.o: ../include/openssl/lhash.h ../include/openssl/obj_mac.h
asn1test.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
asn1test.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
asn1test.o: ../include/openssl/pkcs7.h ../include/openssl/safestack.h
asn1test.o: ../include/openssl/sha.h ../include/openssl/stack.h
asn1test.o: ../include/openssl/symhacks.h ../include/openssl/x509.h
asn1test.o: ../include/openssl/x509_vfy.h asn1test.c
bftest.o: ../e_os.h ../include/openssl/blowfish.h ../include/openssl/e_os2.h
bftest.o: ../include/openssl/opensslconf.h bftest.c
bntest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
bntest.o: ../include/openssl/bn.h ../include/openssl/buffer.h
bntest.o: ../include/openssl/crypto.h ../include/openssl/dh.h
bntest.o: ../include/openssl/dsa.h ../include/openssl/e_os2.h
bntest.o: ../include/openssl/ec.h ../include/openssl/ecdh.h
bntest.o: ../include/openssl/ecdsa.h ../include/openssl/err.h
bntest.o: ../include/openssl/evp.h ../include/openssl/lhash.h
bntest.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
bntest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
bntest.o: ../include/openssl/ossl_typ.h ../include/openssl/pkcs7.h
bntest.o: ../include/openssl/rand.h ../include/openssl/rsa.h
bntest.o: ../include/openssl/safestack.h ../include/openssl/sha.h
bntest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
bntest.o: ../include/openssl/x509.h ../include/openssl/x509_vfy.h bntest.c
casttest.o: ../e_os.h ../include/openssl/cast.h ../include/openssl/e_os2.h
casttest.o: ../include/openssl/opensslconf.h casttest.c
constant_time_test.o: ../crypto/constant_time_locl.h ../e_os.h
constant_time_test.o: ../include/openssl/e_os2.h
constant_time_test.o: ../include/openssl/opensslconf.h constant_time_test.c
destest.o: ../include/openssl/des.h ../include/openssl/des_old.h
destest.o: ../include/openssl/e_os2.h ../include/openssl/opensslconf.h
destest.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
destest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
destest.o: ../include/openssl/ui.h ../include/openssl/ui_compat.h destest.c
dhtest.o: ../e_os.h ../include/openssl/bio.h ../include/openssl/bn.h
dhtest.o: ../include/openssl/crypto.h ../include/openssl/dh.h
dhtest.o: ../include/openssl/e_os2.h ../include/openssl/err.h
dhtest.o: ../include/openssl/lhash.h ../include/openssl/opensslconf.h
dhtest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
dhtest.o: ../include/openssl/rand.h ../include/openssl/safestack.h
dhtest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h dhtest.c
dsatest.o: ../e_os.h ../include/openssl/bio.h ../include/openssl/bn.h
dsatest.o: ../include/openssl/crypto.h ../include/openssl/dh.h
dsatest.o: ../include/openssl/dsa.h ../include/openssl/e_os2.h
dsatest.o: ../include/openssl/err.h ../include/openssl/lhash.h
dsatest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
dsatest.o: ../include/openssl/ossl_typ.h ../include/openssl/rand.h
dsatest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
dsatest.o: ../include/openssl/symhacks.h dsatest.c
ecdhtest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
ecdhtest.o: ../include/openssl/bn.h ../include/openssl/crypto.h
ecdhtest.o: ../include/openssl/e_os2.h ../include/openssl/ec.h
ecdhtest.o: ../include/openssl/ecdh.h ../include/openssl/err.h
ecdhtest.o: ../include/openssl/lhash.h ../include/openssl/obj_mac.h
ecdhtest.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
ecdhtest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
ecdhtest.o: ../include/openssl/rand.h ../include/openssl/safestack.h
ecdhtest.o: ../include/openssl/sha.h ../include/openssl/stack.h
ecdhtest.o: ../include/openssl/symhacks.h ecdhtest.c
ecdsatest.o: ../include/openssl/asn1.h ../include/openssl/bio.h
ecdsatest.o: ../include/openssl/bn.h ../include/openssl/crypto.h
ecdsatest.o: ../include/openssl/e_os2.h ../include/openssl/ec.h
ecdsatest.o: ../include/openssl/ecdsa.h ../include/openssl/err.h
ecdsatest.o: ../include/openssl/evp.h ../include/openssl/lhash.h
ecdsatest.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
ecdsatest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
ecdsatest.o: ../include/openssl/ossl_typ.h ../include/openssl/rand.h
ecdsatest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
ecdsatest.o: ../include/openssl/symhacks.h ecdsatest.c
ectest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
ectest.o: ../include/openssl/bn.h ../include/openssl/crypto.h
ectest.o: ../include/openssl/e_os2.h ../include/openssl/ec.h
ectest.o: ../include/openssl/err.h ../include/openssl/lhash.h
ectest.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
ectest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
ectest.o: ../include/openssl/ossl_typ.h ../include/openssl/rand.h
ectest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
ectest.o: ../include/openssl/symhacks.h ectest.c
enginetest.o: ../include/openssl/e_os2.h ../include/openssl/opensslconf.h
enginetest.o: enginetest.c
evp_test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
evp_test.o: ../include/openssl/conf.h ../include/openssl/crypto.h
evp_test.o: ../include/openssl/e_os2.h ../include/openssl/err.h
evp_test.o: ../include/openssl/evp.h ../include/openssl/lhash.h
evp_test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
evp_test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
evp_test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
evp_test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h evp_test.c
exptest.o: ../e_os.h ../include/openssl/bio.h ../include/openssl/bn.h
exptest.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
exptest.o: ../include/openssl/err.h ../include/openssl/lhash.h
exptest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
exptest.o: ../include/openssl/ossl_typ.h ../include/openssl/rand.h
exptest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
exptest.o: ../include/openssl/symhacks.h exptest.c
heartbeat_test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
heartbeat_test.o: ../include/openssl/buffer.h ../include/openssl/comp.h
heartbeat_test.o: ../include/openssl/crypto.h ../include/openssl/dsa.h
heartbeat_test.o: ../include/openssl/dtls1.h ../include/openssl/e_os2.h
heartbeat_test.o: ../include/openssl/ec.h ../include/openssl/ecdh.h
heartbeat_test.o: ../include/openssl/ecdsa.h ../include/openssl/err.h
heartbeat_test.o: ../include/openssl/evp.h ../include/openssl/hmac.h
heartbeat_test.o: ../include/openssl/kssl.h ../include/openssl/lhash.h
heartbeat_test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
heartbeat_test.o: ../include/openssl/opensslconf.h
heartbeat_test.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
heartbeat_test.o: ../include/openssl/pem.h ../include/openssl/pem2.h
heartbeat_test.o: ../include/openssl/pkcs7.h ../include/openssl/pqueue.h
heartbeat_test.o: ../include/openssl/rsa.h ../include/openssl/safestack.h
heartbeat_test.o: ../include/openssl/sha.h ../include/openssl/srtp.h
heartbeat_test.o: ../include/openssl/ssl.h ../include/openssl/ssl2.h
heartbeat_test.o: ../include/openssl/ssl23.h ../include/openssl/ssl3.h
heartbeat_test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
heartbeat_test.o: ../include/openssl/tls1.h ../include/openssl/x509.h
heartbeat_test.o: ../include/openssl/x509_vfy.h ../ssl/ssl_locl.h
heartbeat_test.o: ../test/testutil.h heartbeat_test.c
hmactest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
hmactest.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
hmactest.o: ../include/openssl/evp.h ../include/openssl/hmac.h
hmactest.o: ../include/openssl/md5.h ../include/openssl/obj_mac.h
hmactest.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
hmactest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
hmactest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
hmactest.o: ../include/openssl/symhacks.h hmactest.c
ideatest.o: ../e_os.h ../include/openssl/e_os2.h ../include/openssl/idea.h
ideatest.o: ../include/openssl/opensslconf.h ideatest.c
igetest.o: ../include/openssl/aes.h ../include/openssl/e_os2.h
igetest.o: ../include/openssl/opensslconf.h ../include/openssl/ossl_typ.h
igetest.o: ../include/openssl/rand.h igetest.c
jpaketest.o: ../include/openssl/buffer.h ../include/openssl/crypto.h
jpaketest.o: ../include/openssl/e_os2.h ../include/openssl/opensslconf.h
jpaketest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
jpaketest.o: ../include/openssl/safestack.h ../include/openssl/stack.h
jpaketest.o: ../include/openssl/symhacks.h jpaketest.c
md2test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
md2test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
md2test.o: ../include/openssl/evp.h ../include/openssl/md2.h
md2test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
md2test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
md2test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
md2test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h md2test.c
md4test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
md4test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
md4test.o: ../include/openssl/evp.h ../include/openssl/md4.h
md4test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
md4test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
md4test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
md4test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h md4test.c
md5test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
md5test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
md5test.o: ../include/openssl/evp.h ../include/openssl/md5.h
md5test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
md5test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
md5test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
md5test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h md5test.c
mdc2test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
mdc2test.o: ../include/openssl/crypto.h ../include/openssl/des.h
mdc2test.o: ../include/openssl/des_old.h ../include/openssl/e_os2.h
mdc2test.o: ../include/openssl/evp.h ../include/openssl/mdc2.h
mdc2test.o: ../include/openssl/obj_mac.h ../include/openssl/objects.h
mdc2test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
mdc2test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
mdc2test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
mdc2test.o: ../include/openssl/ui.h ../include/openssl/ui_compat.h mdc2test.c
randtest.o: ../e_os.h ../include/openssl/e_os2.h
randtest.o: ../include/openssl/opensslconf.h ../include/openssl/ossl_typ.h
randtest.o: ../include/openssl/rand.h randtest.c
rc2test.o: ../e_os.h ../include/openssl/e_os2.h
rc2test.o: ../include/openssl/opensslconf.h ../include/openssl/rc2.h rc2test.c
rc4test.o: ../e_os.h ../include/openssl/e_os2.h
rc4test.o: ../include/openssl/opensslconf.h ../include/openssl/rc4.h
rc4test.o: ../include/openssl/sha.h rc4test.c
rc5test.o: ../e_os.h ../include/openssl/e_os2.h
rc5test.o: ../include/openssl/opensslconf.h ../include/openssl/rc5.h rc5test.c
rmdtest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
rmdtest.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
rmdtest.o: ../include/openssl/evp.h ../include/openssl/obj_mac.h
rmdtest.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
rmdtest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
rmdtest.o: ../include/openssl/ripemd.h ../include/openssl/safestack.h
rmdtest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h rmdtest.c
rsa_test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
rsa_test.o: ../include/openssl/bn.h ../include/openssl/crypto.h
rsa_test.o: ../include/openssl/e_os2.h ../include/openssl/err.h
rsa_test.o: ../include/openssl/lhash.h ../include/openssl/opensslconf.h
rsa_test.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
rsa_test.o: ../include/openssl/rand.h ../include/openssl/rsa.h
rsa_test.o: ../include/openssl/safestack.h ../include/openssl/stack.h
rsa_test.o: ../include/openssl/symhacks.h rsa_test.c
sha1test.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
sha1test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
sha1test.o: ../include/openssl/evp.h ../include/openssl/obj_mac.h
sha1test.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
sha1test.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
sha1test.o: ../include/openssl/safestack.h ../include/openssl/sha.h
sha1test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h sha1test.c
shatest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
shatest.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
shatest.o: ../include/openssl/evp.h ../include/openssl/obj_mac.h
shatest.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
shatest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
shatest.o: ../include/openssl/safestack.h ../include/openssl/sha.h
shatest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h shatest.c
srptest.o: ../include/openssl/bio.h ../include/openssl/bn.h
srptest.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
srptest.o: ../include/openssl/err.h ../include/openssl/lhash.h
srptest.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
srptest.o: ../include/openssl/ossl_typ.h ../include/openssl/rand.h
srptest.o: ../include/openssl/safestack.h ../include/openssl/srp.h
srptest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h srptest.c
ssltest.o: ../e_os.h ../include/openssl/asn1.h ../include/openssl/bio.h
ssltest.o: ../include/openssl/bn.h ../include/openssl/buffer.h
ssltest.o: ../include/openssl/comp.h ../include/openssl/conf.h
ssltest.o: ../include/openssl/crypto.h ../include/openssl/dh.h
ssltest.o: ../include/openssl/dsa.h ../include/openssl/dtls1.h
ssltest.o: ../include/openssl/e_os2.h ../include/openssl/ec.h
ssltest.o: ../include/openssl/ecdh.h ../include/openssl/ecdsa.h
ssltest.o: ../include/openssl/err.h ../include/openssl/evp.h
ssltest.o: ../include/openssl/hmac.h ../include/openssl/kssl.h
ssltest.o: ../include/openssl/lhash.h ../include/openssl/obj_mac.h
ssltest.o: ../include/openssl/objects.h ../include/openssl/opensslconf.h
ssltest.o: ../include/openssl/opensslv.h ../include/openssl/ossl_typ.h
ssltest.o: ../include/openssl/pem.h ../include/openssl/pem2.h
ssltest.o: ../include/openssl/pkcs7.h ../include/openssl/pqueue.h
ssltest.o: ../include/openssl/rand.h ../include/openssl/rsa.h
ssltest.o: ../include/openssl/safestack.h ../include/openssl/sha.h
ssltest.o: ../include/openssl/srp.h ../include/openssl/srtp.h
ssltest.o: ../include/openssl/ssl.h ../include/openssl/ssl2.h
ssltest.o: ../include/openssl/ssl23.h ../include/openssl/ssl3.h
ssltest.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
ssltest.o: ../include/openssl/tls1.h ../include/openssl/x509.h
ssltest.o: ../include/openssl/x509_vfy.h ../include/openssl/x509v3.h ssltest.c
wp_test.o: ../include/openssl/crypto.h ../include/openssl/e_os2.h
wp_test.o: ../include/openssl/opensslconf.h ../include/openssl/opensslv.h
wp_test.o: ../include/openssl/ossl_typ.h ../include/openssl/safestack.h
wp_test.o: ../include/openssl/stack.h ../include/openssl/symhacks.h
wp_test.o: ../include/openssl/whrlpool.h wp_test.c
