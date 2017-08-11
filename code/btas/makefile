CXX=g++
INCLUDES=-I/home/yang/BTAS/ -I/usr/opt/mkl/include
FLAGS=-std=c++11
PROG=tucker_decomp_btas
BLAS_LIBS=-Wl,--start-group /usr/opt/mkl/lib/intel64/libmkl_intel_lp64.a /usr/opt/mkl/lib/intel64/libmkl_intel_thread.a /usr/opt/mkl/lib/intel64/libmkl_core.a -Wl,--end-group -L"/opt/intel/composer_xe_2015.2.164/compiler/lib/intel64/" -liomp5 -lpthread -ldl -lm

svdtest: tucker_decomp_btas.cpp
	$(CXX) -D_HAS_INTEL_MKL $(INCLUDES) $(FLAGS) $(BLAS_LIBS) tucker_decomp_btas.cpp -o $(PROG)

clean:
	rm $(PROG) *.txt *~

run:
	nohup ./$(PROG) > out.txt & 
