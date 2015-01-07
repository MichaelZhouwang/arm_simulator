AM_CFLAGS=-g -mbig-endian
AM_CCASFLAGS=-mbig-endian
AM_LDFLAGS=-nostdlib -T $(srcdir)/linker_script -n --entry main
AM_LDFLAGS+=-Wl,-EB

if HAVE_ARM_COMPILER
noinst_PROGRAMS=example1 example2 example3 example4 insertion_sort exampleLDR exampleLDRH exampleLDRB exampleMRS exampleADC exampleSTR exampleSTRB exampleSTRH exampleLDM exampleSTM

all-am: $(PROGS)
endif

example1_SOURCES=example1.s
example2_SOURCES=example2.s
example3_SOURCES=example3.s
example4_SOURCES=example4.s
insertion_sort_SOURCES=insertion_sort.c
exampleLDR_SOURCES=exampleLDR.s
exampleLDRB_SOURCES=exampleLDRB.s
exampleLDRH_SOURCES=exampleLDRH.s
exampleMRS_SOURCES=exampleMRS.s
exampleADC_SOURCES=exampleADC.s
exampleSTR_SOURCES=exampleSTR.s
exampleSTRB_SOURCES=exampleSTRB.s
exampleSTRH_SOURCES=exampleSTRH.s
exampleLDM_SOURCES=exampleLDM.s
exampleSTM_SOURCES=exampleSTM.s


EXTRA_DIST=linker_script

clean-local:
	$(RM) $(PROGS)
