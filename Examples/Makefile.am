AM_CFLAGS=-g -mbig-endian
AM_CCASFLAGS=-mbig-endian
AM_LDFLAGS=-nostdlib -T $(srcdir)/linker_script -n --entry main
AM_LDFLAGS+=-Wl,-EB

if HAVE_ARM_COMPILER

noinst_PROGRAMS=example1 example2 example3 example4 insertion_sort exampleB_BL exampleLDM_STM exampleLDR exampleLDRB exampleLDRH exampleLDRSH_LDRSB_LDRD_STRD exampleMRS exampleMSR exampleMOV_MVN example_operations example_operations_bin exampleSTR exampleSTRB exampleSTRH exampleTST_TEQ_CMP_CMN exampleHandler presentation tri_insertion demoIntteruption

all-am: $(PROGS)
endif

example1_SOURCES=example1.s
example2_SOURCES=example2.s
example3_SOURCES=example3.s
example4_SOURCES=example4.s
insertion_sort_SOURCES=insertion_sort.c
exampleB_BL_SOURCES=exampleB_BL.s
exampleLDM_STM_SOURCES=exampleLDM_STM.s
exampleLDR_SOURCES=exampleLDR.s
exampleLDRB_SOURCES=exampleLDRB.s
exampleLDRH_SOURCES=exampleLDRH.s
exampleLDRSH_LDRSB_LDRD_STRD_SOURCES=exampleLDRSH_LDRSB_LDRD_STRD.s
exampleMRS_SOURCES=exampleMRS.s
exampleMSR_SOURCES=exampleMSR.s
exampleMOV_MVN_SOURCES=exampleMOV_MVN.s
example_operations_SOURCES=example_operations.s
example_operations_bin_SOURCES=example_operations_bin.s
exampleSTR_SOURCES=exampleSTR.s
exampleSTRB_SOURCES=exampleSTRB.s
exampleSTRH_SOURCES=exampleSTRH.s
exampleTST_TEQ_CMP_CMN_SOURCES=exampleTST_TEQ_CMP_CMN.s
exampleHandler_SOURCES=exampleHandler.s
presentation_SOURCES=presentation.s
tri_insertion_SOURCES=tri_insertion.s
demoIntteruption_SOURCES=demoIntteruption.s

EXTRA_DIST=linker_script

clean-local:
	$(RM) $(PROGS)
