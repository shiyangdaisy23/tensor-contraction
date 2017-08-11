EXEC = $(basename $(wildcard *.cpp) $(wildcard *.cu))

include Makefile.inc

# Extra dependencies for executables

BLAS_DIR = /opt/intel/mkl
BLAS_INC = -I$(BLAS_DIR)/include
BLAS_LIBS =  -Wl,--start-group $(BLAS_DIR)/lib/intel64/libmkl_intel_lp64.a $(BLAS_DIR)/lib/intel64/libmkl_intel_thread.a $(BLAS_DIR)/lib/intel64/libmkl_core.a -Wl,--end-group -L"/opt/intel/composer_xe_2015.2.164/compiler/lib/intel64/" -liomp5 -lpthread -ldl -lm

INCLUDES = $(BLAS_INC)
LIBS = $(BLAS_LIBS)


# test: LDFLAGS += # $(INCLUDES)
# test: LDLIBS += # $(LIBS)

# test2: LDFLAGS += # $(INCLUDES)
# test2: LDLIBS += # $(LIBS)


# test4: LDFLAGS += # $(INCLUDES)
# test4: LDLIBS += # $(LIBS)
# test4float: LDFLAGS += # $(INCLUDES)
# test4float: LDLIBS += # $(LIBS)

# test5: LDFLAGS += # $(INCLUDES)
# test5: LDLIBS += # $(LIBS)

tucker_decomp_kernel: LDFLAGS += # $(INCLUDES)
tucker_decomp_kernel: LDLIBS += # $(LIBS)


# transtiming: LDFLAGS += # $(INCLUDES)
# transtiming: LDLIBS += # $(LIBS)

# test: LDFLAGS += -L/home/yang/openblas/lib/ # openblas/lib/
# test: LDLIBS += -lopenblas
