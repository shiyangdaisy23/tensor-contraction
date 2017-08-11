#include "csr.h"
#include "../shared/util.h"

namespace CTF_int {
  int64_t get_csr_size(int64_t nnz, int nrow, int val_size){
    return nnz*(val_size+sizeof(int))+(nrow+1)*sizeof(int)+3*sizeof(int64_t);
  }

  CSR_Matrix::CSR_Matrix(int64_t nnz, int nrow, algstrct const * sr){
    int64_t size = get_csr_size(nnz, nrow, sr->el_size);
    all_data = (char*)alloc(size);
    ((int64_t*)all_data)[0] = nnz;
    ((int64_t*)all_data)[1] = sr->el_size;
    ((int64_t*)all_data)[2] = nrow;
  }

  CSR_Matrix::CSR_Matrix(char * all_data_){
    all_data = all_data_;
  }

  CSR_Matrix::CSR_Matrix(COO_Matrix const & coom, int nrow, algstrct const * sr, char * data){
    int64_t nz = coom.nnz(); 
    int64_t v_sz = coom.val_size(); 
    int const * coo_rs = coom.rows();
    int const * coo_cs = coom.cols();
    char const * vs = coom.vals();

    int64_t size = get_csr_size(nz, nrow, v_sz);
    if (data == NULL)
      all_data = (char*)alloc(size);
    else
      all_data = data;
    ((int64_t*)all_data)[0] = nz;
    ((int64_t*)all_data)[1] = v_sz;
    ((int64_t*)all_data)[2] = nrow;

    char * csr_vs = vals();
    int * csr_rs = rows();
    int * csr_cs = cols();

    //memcpy(csr_vs, vs, nz*v_sz);
    //memset(csr_rs

    sr->coo_to_csr(nz, nrow, csr_vs, csr_cs, csr_rs, vs, coo_rs, coo_cs);
    
  }

  int64_t CSR_Matrix::nnz() const {
    return ((int64_t*)all_data)[0];
  }

  int CSR_Matrix::val_size() const {
    return ((int64_t*)all_data)[1];
  }


  int64_t CSR_Matrix::size() const {
    return get_csr_size(nnz(),nrow(),val_size());
  }
  
  int CSR_Matrix::nrow() const {
    return ((int64_t*)all_data)[2];
  }
  
  char * CSR_Matrix::vals() const {
    return all_data + 3*sizeof(int64_t);
  }

  int * CSR_Matrix::rows() const {
    int64_t n = this->nnz();
    int v_sz = this->val_size();

    return (int*)(all_data + n*v_sz+3*sizeof(int64_t));
  } 

  int * CSR_Matrix::cols() const {
    int64_t n = this->nnz();
    int64_t nr = this->nrow();
    int v_sz = this->val_size();

    return (int*)(all_data + n*v_sz+(nr+1)*sizeof(int)+3*sizeof(int64_t));
  } 

  void CSR_Matrix::csrmm(algstrct const * sr_A, int m, int n, int k, char const * alpha, char const * B, algstrct const * sr_B, char const * beta, char * C, algstrct const * sr_C, bivar_function const * func){
    int64_t nz = nnz(); 
    int const * rs = rows();
    int const * cs = cols();
    char const * vs = vals();
    ASSERT(sr_B->el_size == sr_A->el_size);
    ASSERT(sr_C->el_size == sr_A->el_size);
    sr_A->csrmm(m,n,k,alpha,vs,rs,cs,nz,B,beta,C,func);
  }

}
