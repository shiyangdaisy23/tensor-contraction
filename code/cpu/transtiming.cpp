#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>
#include "mkl_trans.h"
#include "cblas_batch_gemm.hpp"
//#define DOM_DEBUG

using namespace std;
using namespace std::chrono;
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

int main()
{
 
 high_resolution_clock::time_point t1;  
high_resolution_clock::time_point t2;
duration<double> time_span;  
 high_resolution_clock::time_point tt1;  
high_resolution_clock::time_point tt2;
duration<double> ttime_span;  

int M, N, P, K, t_size=2, i, dom_debug=1;
  size_t sd = sizeof(double); // size of datatype used in gemm calls - single or double precision
  double alpha = 1.0f, beta = 0.0f, *h_A, *h_B, *h_C, *h_temp_A;
  double time_21_b_p=0, time_21_b_n=0, time_21_f=0, time_22_b_p=0, time_22_b_n=0,time_trans_21_b_p=0,time_trans_21_b_n=0,time_trans_21_f=0,time_trans_22_b_p=0,time_trans_22_b_n=0;

ofstream fout("transtiming_cpu_serial_chrono.txt",ios::out|ios::app);
fout << "t_size" << '\t' << "time_21_b_p" << '\t' << "time_21_b_n" << '\t' << "time_21_f" << '\t' << "time_22_b_p" << '\t' << "time_22_b_n" << '\t' << "time_trans_21_b_p" <<'\t'<< "time_trans_21_b_n" <<'\t'<< "time_trans_21_f" << '\t' << "time_trans_22_b_p" << '\t' << "time_trans_22_b_n" << '\n'; // time of fancy derivatives

  #ifndef DOM_DEBUG
    dom_debug = 0;
    for(t_size=5; t_size<556; t_size++)
  #endif
  {
    if(dom_debug){M=3; N=4; P=5; K=2;} else{M=t_size; N=t_size; P=t_size; K=t_size;}
    printf("M=%d, N=%d, P=%d, K=%d\n", M, N, P, K);

   
    h_A = (double*)malloc(K*M*sd); h_B = (double*)malloc(K*N*P*sd); h_C = (double*)malloc(M*N*P*sd);h_temp_A = (double*)malloc(K*M*sd); // host variables
    for(i=0; i<K*M; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;
    for(i=0; i<K*N*P; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;
    if(dom_debug) printf("case 2.1 batch in p\n");
    t1 =  high_resolution_clock::now();   
    /////////////////////////////////////////////////////
      tt1 =  high_resolution_clock::now(); 
     int qq=0,tt=1; 
      for(int jj=0;jj<M*K;jj++){
	h_temp_A[jj] = h_A[qq+M*(tt-1)];
        tt=tt+1;
        if(tt % K == 1){
	  qq = qq+1;
          tt = 1;
	}
      }
     tt2 =  high_resolution_clock::now(); 
   ttime_span = duration_cast<duration<double>>(tt2-tt1);         
    time_trans_21_b_p = (double)ttime_span.count();
    ////////////////////////////////////////////////////////////////
    for(i=0;i<10;i++) {
        
   cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, N, K, alpha,
		      h_temp_A, K, 0,
		      h_B, K, K*N, beta,
		      h_C, M, M*N,
		      P);
    }
     t2 =  high_resolution_clock::now(); 
    time_span = duration_cast<duration<double>>(t2-t1);         
    time_21_b_p = (double)time_span.count()/10*1000;
    //  time_trans_21_b_p = (double)time_trans_21_b_p/10*1000;
    if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}
    free(h_A); free(h_B); free(h_C);free(h_temp_A);

    h_A = (double*)malloc(K*M*sd); h_B = (double*)malloc(K*N*P*sd); h_C = (double*)malloc(M*N*P*sd); h_temp_A = (double*)malloc(K*M*sd);// host variables
    for(i=0; i<K*M; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;
    for(i=0; i<K*N*P; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;
    if(dom_debug) printf("case 2.1 batch in n\n");
    t1 =  high_resolution_clock::now();    
    

///////////////////////////////////////////////////////
     tt1 =  high_resolution_clock::now(); 
     qq=0,tt=1; 
      for(int jj=0;jj<M*K;jj++){
	h_temp_A[jj] = h_A[qq+M*(tt-1)];
        tt=tt+1;
        if(tt % K == 1){
	  qq = qq+1;
          tt = 1;
	}
      }
     tt2 =  high_resolution_clock::now(); 
   ttime_span = duration_cast<duration<double>>(tt2-tt1);         
    time_trans_21_b_n = (double)ttime_span.count();
////////////////////////////////////////////////////////////

    for(i=0;i<10;i++) {
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, P, K, alpha,
		      h_temp_A, K, 0,
		      h_B, K*N, K, beta,
		      h_C, M*N, M,
		      N);
    }
     t2 =  high_resolution_clock::now(); 
    time_span = duration_cast<duration<double>>(t2-t1);         
    time_21_b_n = (double)time_span.count()/10*1000;
    // time_trans_21_b_n = (double)time_trans_21_b_n/10*1000;
    if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}
    free(h_A); free(h_B); free(h_C);free(h_temp_A);

    h_A = (double*)malloc(K*M*sd); h_B = (double*)malloc(K*N*P*sd); h_C = (double*)malloc(M*N*P*sd); h_temp_A = (double*)malloc(K*M*sd);// host variables
    for(i=0; i<K*M; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;
    for(i=0; i<K*N*P; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;
    if(dom_debug) printf("case 2.1 flat\n");
    t1 =  high_resolution_clock::now();    
   


    ///////////////////////////////////////////////////////
     tt1 =  high_resolution_clock::now(); 
     qq=0,tt=1; 
      for(int jj=0;jj<M*K;jj++){
	h_temp_A[jj] = h_A[qq+M*(tt-1)];
        tt=tt+1;
        if(tt % K == 1){
	  qq = qq+1;
          tt = 1;
	}
      }
     tt2 =  high_resolution_clock::now(); 
   ttime_span = duration_cast<duration<double>>(tt2-tt1);         
    time_trans_21_f = (double)ttime_span.count();
//////////////////////////////////////////////////////////// 


    for(i=0;i<10;i++) {
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		M, N*P, K, alpha,
		h_temp_A, K,
		h_B, K, beta,
		h_C, M);
    }
     t2 =  high_resolution_clock::now(); 
     time_span = duration_cast<duration<double>>(t2-t1);         
    time_21_f = (double)time_span.count()/10*1000;
    // time_trans_21_f = (double)time_trans_21_f/10*1000;
   if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}
    free(h_A); free(h_B); free(h_C);free(h_temp_A);

    h_A = (double*)malloc(K*M*sd); h_B = (double*)malloc(K*P*N*sd); h_C = (double*)malloc(M*N*P*sd); h_temp_A = (double*)malloc(K*M*sd);// host variables
    for(i=0; i<K*M; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;
    for(i=0; i<K*P*N; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;
    if(dom_debug) printf("case 2.2 batch in p\n");
    t1 =  high_resolution_clock::now();    
   ///////////////////////////////////////////////////////
     tt1 =  high_resolution_clock::now(); 
     qq=0,tt=1; 
      for(int jj=0;jj<M*K;jj++){
	h_temp_A[jj] = h_A[qq+M*(tt-1)];
        tt=tt+1;
        if(tt % K == 1){
	  qq = qq+1;
          tt = 1;
	}
      }
     tt2 =  high_resolution_clock::now(); 
   ttime_span = duration_cast<duration<double>>(tt2-tt1);         
    time_trans_22_b_p = (double)ttime_span.count();
////////////////////////////////////////////////////////////    

     

   for(i=0;i<10;i++) {
    
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, N, K, alpha,
		      h_temp_A, K, 0,
		      h_B, K*P, K, beta,
		      h_C, M, M*N,
		      P);
    }
     t2 =  high_resolution_clock::now(); 
     time_span = duration_cast<duration<double>>(t2-t1);         
    time_22_b_p = (double)time_span.count()/10*1000;
    //  time_trans_22_b_p = (double)time_trans_22_b_p/10*1000;
   if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}
    free(h_A); free(h_B); free(h_C);free(h_temp_A);

    h_A = (double*)malloc(K*M*sd); h_B = (double*)malloc(K*P*N*sd); h_C = (double*)malloc(M*N*P*sd);h_temp_A = (double*)malloc(K*M*sd); // host variables
    for(i=0; i<K*M; ++i) if(dom_debug) h_A[i] = i+1; else h_A[i] = rand()/(double)RAND_MAX;
    for(i=0; i<K*P*N; ++i) if(dom_debug) h_B[i] = i+1; else h_B[i] = rand()/(double)RAND_MAX;
    for(i=0; i<M*N*P; ++i) h_C[i] = 0;
    if(dom_debug) printf("case 2.2 batch in n\n");
     t1 =  high_resolution_clock::now(); 
    
     ///////////////////////////////////////////////////////
     tt1 =  high_resolution_clock::now(); 
     qq=0,tt=1; 
      for(int jj=0;jj<M*K;jj++){
	h_temp_A[jj] = h_A[qq+M*(tt-1)];
        tt=tt+1;
        if(tt % K == 1){
	  qq = qq+1;
          tt = 1;
	}
      }
     tt2 =  high_resolution_clock::now(); 
   ttime_span = duration_cast<duration<double>>(tt2-tt1);         
    time_trans_22_b_n = (double)ttime_span.count();
////////////////////////////////////////////////////////////    
     for(i=0;i<10;i++) {
    cblas_batch_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		      M, P, K, alpha,
		      h_temp_A, K, 0,
		      h_B, K, K*P, beta,
		      h_C, M*N, M,
		      N);
    }
     t2 =  high_resolution_clock::now(); 
 time_span = duration_cast<duration<double>>(t2-t1);         
    time_22_b_n = (double)time_span.count()/10*1000;
    //   time_trans_22_b_n = (double)time_trans_22_b_n/10*1000;
   if(dom_debug) {printf("C:\n"); print_ten(M,N,P,h_C);}
    free(h_A); free(h_B); free(h_C);free(h_temp_A);

    fout << t_size << '\t' <<  time_21_b_p << '\t' << time_21_b_n << '\t' << time_21_f << '\t' << time_22_b_p << '\t' << time_22_b_n << '\t' <<  time_trans_21_b_p <<'\t'<< time_trans_21_b_n <<'\t'<< time_trans_21_f << '\t' << time_trans_22_b_p << '\t' << time_trans_22_b_n << '\n';
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
