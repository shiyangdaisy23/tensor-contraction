#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <cublas_v2.h>
#include "cublas_batch_gemm.cuh"
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

#define TIME_ME(NAME,CODE)               \
  h_A = (double*)malloc(M*K*sd); h_B = (double*)malloc(K*N*P*sd); h_C = (double*)malloc(M*N*P*sd);   \
    cudaMalloc((void **)&d_A, M*K*sd); cudaMalloc((void **)&d_B, K*N*P*sd); cudaMalloc((void **)&d_C, M*N*P*sd);     \
    for(i=0; i<M*K; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;    \
    for(i=0; i<K*N*P; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;     \
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;      \
    cublasSetVector(M*K,sd,h_A,1,d_A,1); cublasSetVector(K*N*P,sd,h_B,1,d_B,1); cublasSetVector(M*N*P,sd,h_C,1,d_C,1);       \
 cudaEventCreate(&start);           \
    cudaEventCreate(&stop);                   \
    cudaEventRecord(start);                  \
for(i=0;i<10;i++)                            \
  CODE;                                    \
    cudaEventRecord(stop);                         \
    cudaEventSynchronize(stop);                            \
    cudaEventElapsedTime(&time_##NAME, start, stop);time_##NAME  /= 10;                             \
    cudaEventDestroy(start);                         \
    cudaEventDestroy(stop);                            \
    if(dom_debug) {cublasGetVector(M*N*P, sd, d_C, 1, h_C, 1); printf("C:\n"); print_ten(M,N,P,h_C);}                      \
    cudaFree(d_A);         \
    cudaFree(d_B); \
    cudaFree(d_C);     \           
    free(h_A);  \
    free(h_B); \
    free(h_C);


int main(int argc, char **argv)
{
  int M, N, P, K, t_size=2, i, dom_debug=1;
  size_t sd = sizeof(double); // size of datatype used in gemm calls - single or double precision
  double alpha = 1.0f, beta = 0.0f, *h_A, *h_B, *h_C, *d_A, *d_B, *d_C, *d_tmp_B;
 float time_11_b_p=0, time_11_b_n=0, time_11_f=0, time_12_b_p=0, time_12_b_n=0, time_21_b_p=0, time_21_b_n=0, time_21_f=0, time_22_b_p=0, time_22_b_n=0, time_13_b_p=0, time_13_geam=0, time_15_b_p=0, time_15_f=0, time_51_b_n=0, time_51_f=0, time_61_b_n=0, time_61_f=0;
//  struct timeval tv1, tv2;
  ofstream fout("gpu_timing_cudaevent_test2.txt",ios::out|ios::app);
  fout << "t_size" << '\t' << "time_11_b_p" << '\t' << "time_11_b_n" << '\t' << "time_11_f" << '\t' << "time_12_b_p" << '\t' << "time_12_b_n" << '\t' << "time_21_b_p" << '\t' << "time_21_b_n" << '\t' << "time_21_f" << '\t' << "time_22_b_p" << '\t' << "time_22_b_n" << '\t' << "time_13_b_p" << '\t' << "time_13_geam" << '\t' << "time_15_b_p" << '\t' << "time_15_f" << '\t' << "time_51_b_n" << '\t' << "time_51_f" << '\t' << "time_61_b_n" << '\t' << "time_61_f" << '\t';

  fout << "time_11_pbyn" << '\t' << "time_11_nbyp" << '\t' << "time_12_pbyn" << '\t' << "time_12_nbyp" << '\t' << "time_21_pbyn" << '\t' << "time_21_nbyp" << '\t' << "time_22_pbyn" << '\t' << "time_22_nbyp" << '\t' << "time_11_fbyn" << '\t' << "time_11_nbyf" << '\t' << "time_11_fbyp" << '\t' << "time_11_pbyf" << '\t' << "time_21_fbyn" << '\t' << "time_21_nbyf" << '\t' << "time_21_fbyp" << '\t' << "time_21_pbyf" << '\t' << "time_15_fbyp" << '\t' << "time_15_pbyf" << '\t' << "time_51_fbyn" << '\t' << "time_51_nbyf" << '\t' << "time_61_fbyn" << '\t' << "time_61_nbyf" << '\t' << "time_13_trans_f" << '\t' << "time_13_trans_p" << '\t' << "time_13_trans_n" << '\n'; // time of fancy derivatives

  cublasHandle_t handle; cublasCreate(&handle);
  #ifndef DOM_DEBUG
    dom_debug = 0;
    for(t_size=5; t_size<56; t_size++)
  #endif
  {
    if(dom_debug){M=3; N=4; P=5; K=2;} else{M=t_size; N=t_size; P=t_size; K=t_size;}
    printf("M=%d, N=%d, P=%d, K=%d\n", M, N, P, K);

       if(dom_debug) printf("case 1.1 batch in p\n");
    cudaEvent_t start, stop;
     TIME_ME(11_b_p,
             cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_N,
     		M, N, K, &alpha,
				d_A, M, 0,
				d_B, K, K*N, &beta,
				d_C, M, M*N,
				P)
				);
    if(dom_debug) printf("case 1.1 batch in n\n");
    TIME_ME(11_b_n,
    cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_N,
				M, P, K, &alpha,
				d_A, M, 0,
				d_B, K*N, K, &beta,
				d_C, M*N, M,
				N)
      );
    if(dom_debug) printf("case 1.1 flat\n");
    TIME_ME(11_f,
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
		                M, N*P, K, &alpha,
				d_A, M,
				d_B, K, &beta,
				d_C, M)
      );


  if(dom_debug) printf("case 1.2 batch in p\n");
  TIME_ME(12_b_p,
          cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_N,
				M, N, K, &alpha,
				d_A, M, 0,
				d_B, K*P, K, &beta,
				d_C, M, M*N,
				P)
    );
    if(dom_debug) printf("case 1.2 batch in n\n");
    TIME_ME(12_b_n,
            cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_N,
				M, P, K, &alpha,
				d_A, M, 0,
				d_B, K, K*P, &beta,
		                d_C, M*N, M,
				N)
      );
    if(dom_debug) printf("case 2.1 batch in p\n");
    TIME_ME(21_b_p,
            cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M, N, K, &alpha,
				d_A, K, 0,
				d_B, K, K*N, &beta,
				d_C, M, M*N,
				P)
      );

    if(dom_debug) printf("case 2.1 batch in n\n");
    TIME_ME(21_b_n,
            cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M, P, K, &alpha,
				d_A, K, 0,
				d_B, K*N, K, &beta,
				d_C, M*N, M,
				N)
      );
     if(dom_debug) printf("case 2.1 flat\n");
     TIME_ME(21_f,
             cublasDgemm(handle, CUBLAS_OP_T, CUBLAS_OP_N,
		                M, N*P, K, &alpha,
				d_A, K,
				d_B, K, &beta,
				d_C, M)
       );
  
    if(dom_debug) printf("case 2.2 batch in p\n");
    TIME_ME(22_b_p,
     cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M, N, K, &alpha,
				d_A, K, 0,
				d_B, K*P, K, &beta,
				d_C, M, M*N,
				P)
      );
     if(dom_debug) printf("case 2.2 batch in n\n");
     TIME_ME(22_b_n,
              cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M, P, K, &alpha,
				d_A, K, 0,
				d_B, K, K*P, &beta,
		                d_C, M*N, M,
				N)
       );
     if(dom_debug) printf("case 1.3 batch in p\n");
     TIME_ME(13_b_p,
cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_T,
				M, N, K, &alpha,
				d_A, M, 0,
				d_B, N, N*K, &beta,
		                d_C, M, M*N,
		                P)
       );
    if(dom_debug) {cublasGetVector(M*N*P, sd, d_C, 1, h_C, 1); printf("C:\n"); print_ten(M,N,P,h_C);}
    TIME_ME(13_geam,  
 for(int p=0; p<P; p++) // timing explicit transpose: btas-like
      cublasDgeam(handle, CUBLAS_OP_T, CUBLAS_OP_N, K, N, &alpha, d_B+(N*K*p), N, &beta, d_tmp_B, K, d_B+(N*K*p), K)
      );
    if(dom_debug) printf("case 1.5 batch in p\n");
    TIME_ME(15_b_p,
       cublasDgemmBatched(handle, CUBLAS_OP_N, CUBLAS_OP_T,
				M, N, K, &alpha,
				d_A, M, 0,
				d_B, N*P, N, &beta,
		                d_C, M, M*N,
		                P)
      );
    if(dom_debug) printf("case 1.5 flat\n");
    TIME_ME(15_f,
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_T,
				M, N*P, K, &alpha,
				d_A, M,
				d_B, N*P, &beta,
		                d_C, M)
      );
     if(dom_debug) printf("case 5.1 batch in n\n");
     TIME_ME(51_b_n,
      cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_T,
				M, P, K, &alpha,
		                d_B, K, K*M,
		                d_A, P, 0, &beta,
		                d_C, M*N, M,
		                N)
       );
     if(dom_debug) printf("case 5.1 flat\n");
     TIME_ME(51_f,
       cublasDgemm(handle, CUBLAS_OP_T, CUBLAS_OP_T,
				M*N, P, K, &alpha,
		                d_B, K,
		                d_A, P, &beta,
		                d_C, M*N)
       );
    if(dom_debug) printf("case 6.1 batch in n\n");
    TIME_ME(61_b_n,
    cublasDgemmBatched(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M, P, K, &alpha,
		                d_B, K, K*M,
		                d_A, K, 0, &beta,
		                d_C, M*N, M,
		                N)
      );
    if(dom_debug) printf("case 6.1 flat\n");
    TIME_ME(61_f,
    cublasDgemm(handle, CUBLAS_OP_T, CUBLAS_OP_N,
				M*N, P, K, &alpha,
		                d_B, K,
		                d_A, K, &beta,
		                d_C, M*N)
      );
  
    fout << t_size << '\t' << time_11_b_p << '\t' << time_11_b_n << '\t' << time_11_f << '\t' << time_12_b_p << '\t' << time_12_b_n << '\t' << time_21_b_p << '\t' << time_21_b_n << '\t' << time_21_f << '\t' << time_22_b_p << '\t' << time_22_b_n << '\t' << time_13_b_p << '\t' << time_13_geam <<  '\t' << time_15_b_p << '\t' << time_15_f << '\t' << time_51_b_n << '\t' << time_51_f << '\t' << time_61_b_n << '\t' << time_61_f << '\t';

    fout << time_11_b_p/time_11_b_n << '\t' << time_11_b_n/time_11_b_p << '\t' << time_12_b_p/time_12_b_n << '\t' << time_12_b_n/time_12_b_p << '\t' << time_21_b_p/time_21_b_n << '\t' << time_21_b_n/time_21_b_p << '\t' << time_22_b_p/time_22_b_n << '\t' << time_22_b_n/time_22_b_p << '\t' << time_11_f/time_11_b_n << '\t' << time_11_b_n/time_11_f << '\t' << time_11_f/time_11_b_p << '\t' << time_11_b_p/time_11_f << '\t' << time_21_f/time_21_b_n << '\t' << time_21_b_n/time_21_f << '\t' << time_21_f/time_21_b_p << '\t' << time_21_b_p/time_21_f << '\t' << time_15_f/time_15_b_p << '\t' << time_15_b_p/time_15_f << '\t' << time_51_f/time_51_b_n << '\t' << time_51_b_n/time_51_f << '\t' << time_61_f/time_61_b_n << '\t' << time_61_b_n/time_61_f << '\t' << time_13_geam+time_11_f << '\t' << time_13_geam+time_11_b_p << '\t' << time_13_geam+time_11_b_n << '\n'; // time of fancy derivatives
  }
  cublasDestroy(handle);
  fout.close();
  return(0);
}





    /* double error_norm = 0, ref_norm = 0; // check result against reference
    for(i=0; i < M*N*P; ++i)
    {
      double diff = h_C[i]-h_Ctrue[i];
      error_norm += diff*diff;
      ref_norm += h_Cg[i]*h_Cb[i];
      //printf("%d: %f \t %f\n", i, h_Cg[i], h_Cb[i]);
    }
    error_norm = sqrt(error_norm);
    ref_norm   = sqrt(ref_norm);
    if(fabs(ref_norm) < 1e-7) printf("\n*** error ***\n");
    if(error_norm / ref_norm < 1e-6f) printf("\npassed comparison\n");
    else printf("\nfailed comparison\n");*/
    
