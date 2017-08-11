#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include "mkl_trans.h"
#include <omp.h>
#include "cblas_batch_gemm.hpp"
//#define DOM_DEBUG

using namespace std;

void print_mat(int I, int J, double *mat_ptr) // number of rows, columns, pointer to the column-major buffer
{
  int i, j;
  for(i=0; i<I; i++) {
    for(j=0; j<J; j++) printf("%lf ", *(mat_ptr+(j*I+i))); //printf("%2.1f ", *(mat_ptr+(j*I+i))); // use if decimal display precision needs to be specified
    printf("\n");
  }
}

void print_ten(int I, int J, int K, double *ten_ptr) // tensor
{
  int i, j, k;
  for(k=0; k<K; k++) {
    printf("Slice %d:\n", k);
    for(i=0; i<I; i++) {
      for(j=0; j<J; j++) printf("%lf ", *(ten_ptr+(k*I*J+j*I+i)));
      printf("\n");
    }
  }
}


#define TIME_ME(NAME, CODE)                                             \
  h_A = (double*)malloc(M*K*sd); h_B = (double*)malloc(K*N*P*sd); h_C = (double*)malloc(M*N*P*sd); \
for(i=0; i<M*K; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX; \
for(i=0; i<K*N*P; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX; \
for(i=0; i<M*N*P; ++i) h_C[i] = 0;                                      \
if(dom_debug) printf("NAME");                                           \
CODE ;   \
gettimeofday(&tv1, NULL);                       \
for (i = 0; i < 10; ++i)     \
CODE ;                                      \
gettimeofday(&tv2, NULL);                                               \
time_##NAME = double(tv2.tv_usec-tv1.tv_usec)/1000 + double(tv2.tv_sec-tv1.tv_sec)*1000; \
time_##NAME /= 10;                                                      \
if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}                   \
free(h_A); free(h_B); free(h_C);

int main()
{
  int M, N, P, K, t_size=2, i, dom_debug=1;
  size_t sd = sizeof(double); // size of datatype used in gemm calls - single or double precision
  double alpha = 1.0f, beta = 0.0f, *h_A, *h_B, *h_C, *h_tmp_B;
  double time_11_b_p=0, time_11_b_n=0, time_11_f=0, time_12_b_p=0, time_12_b_n=0, time_21_b_p=0, time_21_b_n=0, time_21_f=0, time_22_b_p=0, time_22_b_n=0, time_13_b_p=0, time_13_geam=0, time_15_b_p=0, time_15_f=0, time_51_b_n=0, time_51_f=0, time_61_b_n=0, time_61_f=0;
  timeval tv1, tv2;
  ofstream fout("cpu_timing_dom_2_serial_gtod.txt",ios::out|ios::app);
  fout << "t_size" << '\t' << "time_11_b_p" << '\t' << "time_11_b_n" << '\t' << "time_11_f" << '\t' << "time_12_b_p" << '\t' << "time_12_b_n" << '\t' << "time_21_b_p" << '\t' << "time_21_b_n" << '\t' << "time_21_f" << '\t' << "time_22_b_p" << '\t' << "time_22_b_n" << '\t' << "time_13_b_p" << '\t' << "time_13_geam" << '\t' << "time_15_b_p" << '\t' << "time_15_f" << '\t' << "time_51_b_n" << '\t' << "time_51_f" << '\t' << "time_61_b_n" << '\t' << "time_61_f" << '\t';

  fout << "time_11_pbyn" << '\t' << "time_11_nbyp" << '\t' << "time_12_pbyn" << '\t' << "time_12_nbyp" << '\t' << "time_21_pbyn" << '\t' << "time_21_nbyp" << '\t' << "time_22_pbyn" << '\t' << "time_22_nbyp" << '\t' << "time_11_fbyn" << '\t' << "time_11_nbyf" << '\t' << "time_11_fbyp" << '\t' << "time_11_pbyf" << '\t' << "time_21_fbyn" << '\t' << "time_21_nbyf" << '\t' << "time_21_fbyp" << '\t' << "time_21_pbyf" << '\t' << "time_15_fbyp" << '\t' << "time_15_pbyf" << '\t' << "time_51_fbyn" << '\t' << "time_51_nbyf" << '\t' << "time_61_fbyn" << '\t' << "time_61_nbyf" << '\t' << "time_13_trans_f" << '\t' << "time_13_trans_p" << '\t' << "time_13_trans_n" << '\n'; // time of fancy derivatives

  #ifndef DOM_DEBUG
    dom_debug = 0;
    for(t_size=5; t_size<556; t_size++)
  #endif
  {
    if(dom_debug){M=3; N=4; P=5; K=2;} else{M=t_size; N=t_size; P=t_size; K=t_size;}
    printf("M=%d, N=%d, P=%d, K=%d\n", M, N, P, K);

    TIME_ME(11_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                              M, N, K, alpha,
                              h_A, M, 0,
                              h_B, K, K*N, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(11_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                              M, P, K, alpha,
                              h_A, M, 0,
                              h_B, K*N, K, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(11_f,
            cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                        M, N*P, K, alpha,
                        h_A, M,
                        h_B, K, beta,
                        h_C, M)
            );

    TIME_ME(12_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                              M, N, K, alpha,
                              h_A, M, 0,
                              h_B, K*P, K, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(12_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                              M, P, K, alpha,
                              h_A, M, 0,
                              h_B, K, K*P, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(21_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                              M, N, K, alpha,
                              h_A, K, 0,
                              h_B, K, K*N, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(21_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                              M, P, K, alpha,
                              h_A, K, 0,
                              h_B, K*N, K, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(21_f,
            cblas_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                        M, N*P, K, alpha,
                        h_A, K,
                        h_B, K, beta,
                        h_C, M)
            );

    TIME_ME(22_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                              M, N, K, alpha,
                              h_A, K, 0,
                              h_B, K*P, K, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(22_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                              M, P, K, alpha,
                              h_A, K, 0,
                              h_B, K, K*P, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(13_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,
                              M, N, K, alpha,
                              h_A, M, 0,
                              h_B, N, N*K, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(13_geam,
            for(int p=0; p<P; p++) mkl_domatcopy('C', 'T', N, K, alpha, h_B+(N*K*p), N, h_C+(K*N*p), K)
            );

    TIME_ME(15_b_p,
            cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,
                              M, N, K, alpha,
                              h_A, M, 0,
                              h_B, N*P, N, beta,
                              h_C, M, M*N,
                              P)
            );

    TIME_ME(15_f,
            cblas_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,
                        M, N*P, K, alpha,
                        h_A, M,
                        h_B, N*P, beta,
                        h_C, M)
            );

    TIME_ME(51_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasTrans,
                              M, P, K, alpha,
                              h_B, K, K*M,
                              h_A, P, 0, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(51_f,
            cblas_dgemm(CblasColMajor, CblasTrans, CblasTrans,
                        M*N, P, K, alpha,
                        h_B, K,
                        h_A, P, beta,
                        h_C, M*N)
            );

    TIME_ME(61_b_n,
            cblas_batch_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                              M, P, K, alpha,
                              h_B, K, K*M,
                              h_A, K, 0, beta,
                              h_C, M*N, M,
                              N)
            );

    TIME_ME(61_f,
            cblas_dgemm(CblasColMajor, CblasTrans, CblasNoTrans,
                        M*N, P, K, alpha,
                        h_B, K,
                        h_A, K, beta,
                        h_C, M*N)
            );

    fout << t_size << '\t' << time_11_b_p << '\t' << time_11_b_n << '\t' << time_11_f << '\t' << time_12_b_p << '\t' << time_12_b_n << '\t' << time_21_b_p << '\t' << time_21_b_n << '\t' << time_21_f << '\t' << time_22_b_p << '\t' << time_22_b_n << '\t' << time_13_b_p << '\t' << time_13_geam <<  '\t' << time_15_b_p << '\t' << time_15_f << '\t' << time_51_b_n << '\t' << time_51_f << '\t' << time_61_b_n << '\t' << time_61_f << '\t';

    fout << time_11_b_p/time_11_b_n << '\t' << time_11_b_n/time_11_b_p << '\t' << time_12_b_p/time_12_b_n << '\t' << time_12_b_n/time_12_b_p << '\t' << time_21_b_p/time_21_b_n << '\t' << time_21_b_n/time_21_b_p << '\t' << time_22_b_p/time_22_b_n << '\t' << time_22_b_n/time_22_b_p << '\t' << time_11_f/time_11_b_n << '\t' << time_11_b_n/time_11_f << '\t' << time_11_f/time_11_b_p << '\t' << time_11_b_p/time_11_f << '\t' << time_21_f/time_21_b_n << '\t' << time_21_b_n/time_21_f << '\t' << time_21_f/time_21_b_p << '\t' << time_21_b_p/time_21_f << '\t' << time_15_f/time_15_b_p << '\t' << time_15_b_p/time_15_f << '\t' << time_51_f/time_51_b_n << '\t' << time_51_b_n/time_51_f << '\t' << time_61_f/time_61_b_n << '\t' << time_61_b_n/time_61_f << '\t' << time_13_geam+time_11_f << '\t' << time_13_geam+time_11_b_p << '\t' << time_13_geam+time_11_b_n << '\n'; // time of fancy derivatives
  }
  fout.close();
  return(0);
}



/*
    gettimeofday(&tv1, NULL);
    //call mkl_dimatcopy(ordering, trans, rows, cols, alpha, ab, lda, ldb);
    for(int p=0; p<P; p++)
    {
      mkl_domatcopy('C', 'T', N, K, alpha, B+(N*K*p), N, tmp_B+(K*N*p), K); // A^\top
      //mkl_dimatcopy('C', 'T', N, K, alpha, B+(N*K*p), N, K); // A^\top
    }
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		M, N*P, K, alpha,
		A, M,
		tmp_B, K, beta,
		Cg, M);
    gettimeofday(&tv2, NULL);

    gettimeofday(&tv1, NULL);
    for(int p=0; p<P; p++) // trans+batch n
      mkl_domatcopy('C', 'T', N, K, alpha, B+(N*K*p), N, tmp_B+(K*N*p), K);
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, N, K, alpha,
		      A, M, 0,
		      tmp_B, K, K*N, beta,
		      Cg, M, M*N,
		      P);
    gettimeofday(&tv2, NULL);

    gettimeofday(&tv1, NULL);
    for(int p=0; p<P; p++) // trans+batch p
      mkl_domatcopy('C', 'T', N, K, alpha, B+(N*K*p), N, tmp_B+(K*N*p), K);
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, N, K, alpha,
		      A, M, 0,
		      tmp_B, K, K*P, beta,
		      Cg, M, M*P,
		      N);
    gettimeofday(&tv2, NULL);

    gettimeofday(&tv1, NULL);
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasTrans,
		      M, N, K, alpha,
		      A, M, 0,
		      B, N, K*N, beta,
		      Cb, M, M*N,
		      P);
    gettimeofday(&tv2, NULL);
 */
