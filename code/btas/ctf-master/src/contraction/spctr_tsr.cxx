
/*Copyright (c) 2011, Edgar Solomonik, all rights reserved.*/

#include "../shared/util.h"
#include "spctr_tsr.h"
#include "sp_seq_ctr.h"
#include "contraction.h"
#include "../sparse_formats/coo.h"
#include "../sparse_formats/csr.h"
#include "../tensor/untyped_tensor.h"

namespace CTF_int {  
  spctr::spctr(contraction const * c)
    : ctr(c) {
    is_sparse_A = c->A->is_sparse;
    is_sparse_B = c->B->is_sparse;
    is_sparse_C = c->C->is_sparse;
  }

  spctr::spctr(spctr * other)
    : ctr(other) {
    is_sparse_A = other->is_sparse_A;
    is_sparse_B = other->is_sparse_B;
    is_sparse_C = other->is_sparse_C;
  }

  spctr::~spctr(){  }


  seq_tsr_spctr::seq_tsr_spctr(contraction const * c,
                               int                 krnl_type_,
                               iparam const *      inner_params,
                               int *               virt_blk_len_A,
                               int *               virt_blk_len_B,
                               int *               virt_blk_len_C,
                               int64_t             vrt_sz_C)
        : spctr(c) {
     
    int * new_sym_A, * new_sym_B, * new_sym_C;
    CTF_int::alloc_ptr(sizeof(int)*c->A->order, (void**)&new_sym_A);
    memcpy(new_sym_A, c->A->sym, sizeof(int)*c->A->order);
    CTF_int::alloc_ptr(sizeof(int)*c->B->order, (void**)&new_sym_B);
    memcpy(new_sym_B, c->B->sym, sizeof(int)*c->B->order);
    CTF_int::alloc_ptr(sizeof(int)*c->C->order, (void**)&new_sym_C);
    memcpy(new_sym_C, c->C->sym, sizeof(int)*c->C->order);

    this->krnl_type  = krnl_type_;
    if (krnl_type > 0){
      if (c->A->wrld->cdt.rank == 0){
        DPRINTF(2,"Folded tensor n=%d m=%d k=%d\n", inner_params->n,
          inner_params->m, inner_params->k);
      }

      this->inner_params  = *inner_params;
      this->inner_params.sz_C = vrt_sz_C;
    }

    this->is_custom  = c->is_custom;
    this->alpha      = c->alpha;
    if (is_custom){
      this->func     = c->func;
    } else {
      this->func     = NULL;
    }
    this->order_A    = c->A->order;
    this->idx_map_A  = c->idx_A;
    this->edge_len_A = virt_blk_len_A;
    this->sym_A      = new_sym_A;
    this->order_B    = c->B->order;
    this->idx_map_B  = c->idx_B;
    this->edge_len_B = virt_blk_len_B;
    this->sym_B      = new_sym_B;
    this->order_C    = c->C->order;
    this->idx_map_C  = c->idx_C;
    this->edge_len_C = virt_blk_len_C;
    this->sym_C      = new_sym_C;

  }


  void seq_tsr_spctr::print(){
    int i;
    printf("seq_tsr_spctr:\n");
    for (i=0; i<order_A; i++){
      printf("edge_len_A[%d]=%d\n",i,edge_len_A[i]);
    }
    for (i=0; i<order_B; i++){
      printf("edge_len_B[%d]=%d\n",i,edge_len_B[i]);
    }
    for (i=0; i<order_C; i++){
      printf("edge_len_C[%d]=%d\n",i,edge_len_C[i]);
    }
    printf("kernel type is %d\n", krnl_type);
    if (krnl_type>0) printf("inner n = %d m= %d k = %d\n",
                          inner_params.n, inner_params.m, inner_params.k);
  }

  seq_tsr_spctr::seq_tsr_spctr(spctr * other) : spctr(other) {
    seq_tsr_spctr * o = (seq_tsr_spctr*)other;
    alpha = o->alpha;
    
    order_A        = o->order_A;
    idx_map_A     = o->idx_map_A;
    sym_A         = (int*)CTF_int::alloc(sizeof(int)*order_A);
    memcpy(sym_A, o->sym_A, sizeof(int)*order_A);
    edge_len_A    = (int*)CTF_int::alloc(sizeof(int)*order_A);
    memcpy(edge_len_A, o->edge_len_A, sizeof(int)*order_A);

    order_B        = o->order_B;
    idx_map_B     = o->idx_map_B;
    sym_B         = (int*)CTF_int::alloc(sizeof(int)*order_B);
    memcpy(sym_B, o->sym_B, sizeof(int)*order_B);
    edge_len_B    = (int*)CTF_int::alloc(sizeof(int)*order_B);
    memcpy(edge_len_B, o->edge_len_B, sizeof(int)*order_B);

    order_C      = o->order_C;
    idx_map_C    = o->idx_map_C;
    sym_C        = (int*)CTF_int::alloc(sizeof(int)*order_C);
    memcpy(sym_C, o->sym_C, sizeof(int)*order_C);
    edge_len_C   = (int*)CTF_int::alloc(sizeof(int)*order_C);
    memcpy(edge_len_C, o->edge_len_C, sizeof(int)*order_C);

    krnl_type    = o->krnl_type;
    inner_params = o->inner_params;
    is_custom    = o->is_custom;
    func         = o->func;
  }

  spctr * seq_tsr_spctr::clone() {
    return new seq_tsr_spctr(this);
  }


  int64_t seq_tsr_spctr::mem_fp(){ return 0; }

  double seq_tsr_spctr::est_time_fp(int nlyr, double nnz_frac_A, double nnz_frac_B, double nnz_frac_C){ 
    uint64_t size_A = sy_packed_size(order_A, edge_len_A, sym_A)*sr_A->el_size;
    uint64_t size_B = sy_packed_size(order_B, edge_len_B, sym_B)*sr_B->el_size;
    uint64_t size_C = sy_packed_size(order_C, edge_len_C, sym_C)*sr_C->el_size;
    if (krnl_type>0) size_A *= inner_params.m*inner_params.k;
    if (krnl_type>0) size_B *= inner_params.n*inner_params.k;
    if (krnl_type>0) size_C *= inner_params.m*inner_params.n;
    if (is_sparse_A) size_A *= nnz_frac_A*10;
    if (is_sparse_B) size_B *= nnz_frac_B*10;
    if (is_sparse_C) size_C *= nnz_frac_C*10;
   
    int idx_max, * rev_idx_map; 
    inv_idx(order_A,       idx_map_A,
            order_B,       idx_map_B,
            order_C,       idx_map_C,
            &idx_max,     &rev_idx_map);

    double flops = 2.0;
    if (krnl_type>0) {
      flops *= inner_params.m;
      flops *= inner_params.n;
      flops *= inner_params.k;
    } else {
      for (int i=0; i<idx_max; i++){
        if (rev_idx_map[3*i+0] != -1) flops*=edge_len_A[rev_idx_map[3*i+0]];
        else if (rev_idx_map[3*i+1] != -1) flops*=edge_len_B[rev_idx_map[3*i+1]];
        else if (rev_idx_map[3*i+2] != -1) flops*=edge_len_C[rev_idx_map[3*i+2]];
      }
    }
    //FIXME only makes sense when one of A/B/C is sparse
    if (is_sparse_A) flops *= nnz_frac_A*10;
    if (is_sparse_B) flops *= nnz_frac_B*10;
    if (is_sparse_C) flops *= nnz_frac_C*10;
    ASSERT(flops >= 0.0);
    CTF_int::cdealloc(rev_idx_map);
    return COST_MEMBW*(size_A+size_B+size_C)+COST_FLOP*flops;
  }

  double seq_tsr_spctr::est_time_rec(int nlyr, double nnz_frac_A, double nnz_frac_B, double nnz_frac_C){ 
    return est_time_fp(nlyr, nnz_frac_A, nnz_frac_B, nnz_frac_C);
  }

  void seq_tsr_spctr::run(char * A, int nblk_A, int64_t const * size_blk_A,
                          char * B, int nblk_B, int64_t const * size_blk_B,
                          char * C, int nblk_C, int64_t * size_blk_C,
                          char *& new_C){
    ASSERT(idx_lyr == 0 && num_lyr == 1);
    ASSERT( is_sparse_A);
    ASSERT(!is_sparse_B);
    ASSERT(!is_sparse_C);
    ASSERT(nblk_A == 1);

    if (krnl_type==2){
      // Do mm using CSR format
      CSR_Matrix cA(A);
      if (!sr_C->isequal(beta,sr_C->mulid())){
        if (sr_C->isequal(beta,sr_C->addid())){
          sr_C->set(C, beta, inner_params.sz_C);
        } else {
          sr_C->scal(inner_params.sz_C, beta, C, 1);
        }
      }
      TAU_FSTART(CSRMM);
      cA.csrmm(sr_A, inner_params.m, inner_params.n, inner_params.k,
               alpha, B, sr_B, sr_C->mulid(), C, sr_C, func);
      TAU_FSTOP(CSRMM);

    } else if (krnl_type==1){
      // Do mm using coordinate format
      COO_Matrix cA(A);
      if (!sr_C->isequal(beta,sr_C->mulid())){
        if (sr_C->isequal(beta,sr_C->addid())){
          sr_C->set(C, beta, inner_params.sz_C);
        } else {
          sr_C->scal(inner_params.sz_C, beta, C, 1);
        }
      }
      TAU_FSTART(COOMM);
      cA.coomm(sr_A, inner_params.m, inner_params.n, inner_params.k,
               alpha, B, sr_B, sr_C->mulid(), C, sr_C, func);
      TAU_FSTOP(COOMM);
    } else {
      ASSERT(size_blk_A[0]%sr_A->pair_size() == 0);

      int64_t nnz_A = size_blk_A[0]/sr_A->pair_size();

      TAU_FSTART(spA_dnB_dnC_seq);
      spA_dnB_dnC_seq_ctr(this->alpha,
                          A,
                          nnz_A,
                          sr_A,
                          order_A,
                          edge_len_A,
                          sym_A,
                          idx_map_A,
                          B,
                          sr_B,
                          order_B,
                          edge_len_B,
                          sym_B,
                          idx_map_B,
                          this->beta,
                          C,
                          sr_C,
                          order_C,
                          edge_len_C,
                          sym_C,
                          idx_map_C,
                          func);
      TAU_FSTOP(spA_dnB_dnC_seq);
    }
  }


  spctr_virt::spctr_virt(contraction const * c,
                         int                 num_tot,
                         int *               virt_dim,
                         int64_t             vrt_sz_A,
                         int64_t             vrt_sz_B,
                         int64_t             vrt_sz_C)
      : spctr(c) {
    this->num_dim   = num_tot;
    this->virt_dim  = virt_dim;
    this->order_A   = c->A->order;
    this->blk_sz_A  = vrt_sz_A;
    this->idx_map_A = c->idx_A;
    this->order_B   = c->B->order;
    this->blk_sz_B  = vrt_sz_B;
    this->idx_map_B = c->idx_B;
    this->order_C   = c->C->order;
    this->blk_sz_C  = vrt_sz_C;
    this->idx_map_C = c->idx_C;
  }


  spctr_virt::~spctr_virt() {
    CTF_int::cdealloc(virt_dim);
    delete rec_ctr;
  }

  spctr_virt::spctr_virt(spctr * other) : spctr(other) {
    spctr_virt * o = (spctr_virt*)other;
    rec_ctr = o->rec_ctr->clone();
    num_dim   = o->num_dim;
    virt_dim  = (int*)CTF_int::alloc(sizeof(int)*num_dim);
    memcpy(virt_dim, o->virt_dim, sizeof(int)*num_dim);

    order_A   = o->order_A;
    blk_sz_A  = o->blk_sz_A;
    idx_map_A = o->idx_map_A;

    order_B   = o->order_B;
    blk_sz_B  = o->blk_sz_B;
    idx_map_B = o->idx_map_B;

    order_C   = o->order_C;
    blk_sz_C  = o->blk_sz_C;
    idx_map_C = o->idx_map_C;
  }

  spctr * spctr_virt::clone() {
    return new spctr_virt(this);
  }

  void spctr_virt::print() {
    int i;
    printf("spctr_virt:\n");
    printf("blk_sz_A = %ld, blk_sz_B = %ld, blk_sz_C = %ld\n",
            blk_sz_A, blk_sz_B, blk_sz_C);
    for (i=0; i<num_dim; i++){
      printf("virt_dim[%d] = %d\n", i, virt_dim[i]);
    }
    rec_ctr->print();
  }


  double spctr_virt::est_time_rec(int nlyr, double nnz_frac_A, double nnz_frac_B, double nnz_frac_C) {
    /* FIXME: for now treat flops like comm, later make proper cost */
    int64_t nblk = 1;
    for (int dim=0; dim<num_dim; dim++){
      nblk *= virt_dim[dim];
    }
    return nblk*rec_ctr->est_time_rec(nlyr, nnz_frac_A, nnz_frac_B, nnz_frac_C);
  }


  #define VIRT_NTD 1

  int64_t spctr_virt::mem_fp(){
    return (order_A+order_B+order_C+(3+VIRT_NTD)*num_dim)*sizeof(int);
  }

  int64_t spctr_virt::mem_rec() {
    return rec_ctr->mem_rec() + mem_fp();
  }

  void spctr_virt::run(char * A, int nblk_A, int64_t const * size_blk_A,
                       char * B, int nblk_B, int64_t const * size_blk_B,
                       char * C, int nblk_C, int64_t * size_blk_C,
                       char *& new_C){
    TAU_FSTART(spctr_virt);
    int * idx_arr, * tidx_arr, * lda_A, * lda_B, * lda_C, * beta_arr;
    int * ilda_A, * ilda_B, * ilda_C;
    int64_t i, off_A, off_B, off_C;
    int nb_A, nb_B, nb_C, alloced, ret; 

    /*if (this->buffer != NULL){    
      alloced = 0;
      idx_arr = (int*)this->buffer;
    } else {*/
      alloced = 1;
      ret = CTF_int::alloc_ptr(mem_fp(), (void**)&idx_arr);
      ASSERT(ret==0);
//    }

    
    lda_A = idx_arr + VIRT_NTD*num_dim;
    lda_B = lda_A + order_A;
    lda_C = lda_B + order_B;
    ilda_A = lda_C + order_C;
    ilda_B = ilda_A + num_dim;
    ilda_C = ilda_B + num_dim;

  #define SET_LDA_X(__X)                                                  \
  do {                                                                    \
    nb_##__X = 1;                                                         \
    for (i=0; i<order_##__X; i++){                                         \
      lda_##__X[i] = nb_##__X;                                            \
      nb_##__X = nb_##__X*virt_dim[idx_map_##__X[i]];                     \
    }                                                                     \
    memset(ilda_##__X, 0, num_dim*sizeof(int));                           \
    for (i=0; i<order_##__X; i++){                                         \
      ilda_##__X[idx_map_##__X[i]] += lda_##__X[i];                       \
    }                                                                     \
  } while (0)
    SET_LDA_X(A);
    SET_LDA_X(B);
    SET_LDA_X(C);
  #undef SET_LDA_X
   
    /* dynammically determined size */ 
    beta_arr = (int*)CTF_int::alloc(sizeof(int)*nb_C);
    memset(beta_arr, 0, nb_C*sizeof(int));

    int64_t * sp_offsets_A = NULL;
    if (is_sparse_A){
      sp_offsets_A = (int64_t*)alloc(sizeof(int64_t)*nb_A);
      sp_offsets_A[0] = 0;
      for (int i=1; i<nb_A; i++){
        sp_offsets_A[i] = sp_offsets_A[i-1]+size_blk_A[i-1];
      }
    }
    int64_t * sp_offsets_B = NULL;
    if (is_sparse_B){
      sp_offsets_B = (int64_t*)alloc(sizeof(int64_t)*nb_B);
      sp_offsets_B[0] = 0;
      for (int i=1; i<nb_B; i++){
        sp_offsets_B[i] = sp_offsets_B[i-1]+size_blk_B[i-1];
      }
    }

    int64_t * sp_offsets_C = NULL;
    int64_t * new_sp_szs_C = NULL;
    char ** buckets_C = NULL;
    if (is_sparse_C){
      sp_offsets_C = (int64_t*)alloc(sizeof(int64_t)*nb_C);
      new_sp_szs_C = size_blk_C; //(int64_t*)alloc(sizeof(int64_t)*nb_C);
//      memcpy(new_sp_szs_C, blk_sz_C, sizeof(int64_t)*nb_C);
      buckets_C = (char**)alloc(sizeof(char*)*nb_C);
      for (int i=0; i<nb_C; i++){
        if (i==0)
          sp_offsets_C[0] = 0;
        else
          sp_offsets_C[i] = sp_offsets_C[i-1]+size_blk_C[i-1];
        buckets_C[i] = C + sp_offsets_C[i];
      }      
    }

  #if (VIRT_NTD>1)
//  #pragma omp parallel private(off_A,off_B,off_C,tidx_arr,i) 
  #endif
    {
      int tid, ntd, start_off, end_off;
  #if (VIRT_NTD>1)
      tid = omp_get_thread_num();
      ntd = MIN(VIRT_NTD, omp_get_num_threads());
  #else
      tid = 0;
      ntd = 1;
  #endif
  #if (VIRT_NTD>1)
      DPRINTF(2,"%d/%d %d %d\n",tid,ntd,VIRT_NTD,omp_get_num_threads());
  #endif
      if (tid < ntd){
        tidx_arr = idx_arr + tid*num_dim;
        memset(tidx_arr, 0, num_dim*sizeof(int));

        start_off = (nb_C/ntd)*tid;
        if (tid < nb_C%ntd){
          start_off += tid;
          end_off = start_off + nb_C/ntd + 1;
        } else {
          start_off += nb_C%ntd;
          end_off = start_off + nb_C/ntd;
        }

        spctr * tid_rec_ctr;
        if (tid > 0)
          tid_rec_ctr = rec_ctr->clone();
        else
          tid_rec_ctr = rec_ctr;
        
        tid_rec_ctr->num_lyr = this->num_lyr;
        tid_rec_ctr->idx_lyr = this->idx_lyr;

        off_A = 0, off_B = 0, off_C = 0;
        for (;;){
          if (off_C >= start_off && off_C < end_off) {
            char * rec_A, * rec_B, * rec_C;
            if (is_sparse_A){
              rec_A     = A + sp_offsets_A[off_A];
            } else
              rec_A     = A + off_A*blk_sz_A*sr_A->el_size;
            if (is_sparse_B){
              rec_B     = B + sp_offsets_B[off_B];
            } else
              rec_B     = B + off_B*blk_sz_B*sr_A->el_size;
            if (is_sparse_C){
              rec_C     = C + sp_offsets_C[off_C];
            } else
              rec_C     = C + off_C*blk_sz_C*sr_A->el_size;
            if (beta_arr[off_C]>0)
              rec_ctr->beta = sr_C->mulid();
            else
              rec_ctr->beta = this->beta; 
            beta_arr[off_C]       = 1;
            tid_rec_ctr->run(rec_A, 1, size_blk_A+off_A,
                             rec_B, 1, size_blk_B+off_B,
                             rec_C, 1, new_sp_szs_C+off_C,
                             buckets_C[off_C]);
          }

          if (is_sparse_C){
            if (beta_arr[off_C] > 0) cdealloc(buckets_C[off_C]);
          }

          for (i=0; i<num_dim; i++){
            off_A -= ilda_A[i]*tidx_arr[i];
            off_B -= ilda_B[i]*tidx_arr[i];
            off_C -= ilda_C[i]*tidx_arr[i];
            tidx_arr[i]++;
            if (tidx_arr[i] >= virt_dim[i])
              tidx_arr[i] = 0;
            off_A += ilda_A[i]*tidx_arr[i];
            off_B += ilda_B[i]*tidx_arr[i];
            off_C += ilda_C[i]*tidx_arr[i];
            if (tidx_arr[i] != 0) break;
          }
          if (i==num_dim) break;
        }
        if (tid > 0){
          delete tid_rec_ctr;
        }
      }
    }
    if (is_sparse_C){
      int64_t new_size_C = 0;
      for (int i=0; i<nb_C; i++){
        new_size_C += new_sp_szs_C[i];
      }
      new_C = (char*)alloc(new_size_C);
      int64_t pfx = 0;
      for (int i=0; i<nb_C; i++){
        memcpy(new_C+pfx, buckets_C[i], new_sp_szs_C[i]*this->sr_C->pair_size());
        pfx += new_sp_szs_C[i]*this->sr_C->pair_size();
        if (beta_arr[i] > 0) cdealloc(buckets_C[i]);
      }
      //FIXME: how to pass C back generally
      //cdealloc(C);
      cdealloc(buckets_C);
    }
    if (is_sparse_A) cdealloc(sp_offsets_A);
    if (is_sparse_B) cdealloc(sp_offsets_B);
    if (is_sparse_B) cdealloc(sp_offsets_C);
    if (alloced){
      CTF_int::cdealloc(idx_arr);
    }
    CTF_int::cdealloc(beta_arr);
    TAU_FSTOP(spctr_virt);
  }

  spctr_pin_keys::~spctr_pin_keys(){
    delete rec_ctr;
    cdealloc(divisor);
    cdealloc(virt_dim);
    cdealloc(phys_rank);
  }
  
  spctr_pin_keys::spctr_pin_keys(spctr * other) : spctr(other) {
    spctr_pin_keys * o = (spctr_pin_keys*)other;

    rec_ctr   = o->rec_ctr->clone();
    AxBxC     = o->AxBxC;
    order     = o->order;
    lens      = o->lens;
    divisor   = (int*)alloc(sizeof(int)*order);
    phys_rank = (int*)alloc(sizeof(int)*order);
    virt_dim  = (int*)alloc(sizeof(int)*order);
    memcpy(divisor, o->divisor, sizeof(int)*order);
    memcpy(phys_rank, o->phys_rank, sizeof(int)*order);
    memcpy(virt_dim, o->virt_dim, sizeof(int)*order);
  }

  spctr * spctr_pin_keys::clone() {
    return new spctr_pin_keys(this);
  }

  void spctr_pin_keys::print(){
    printf("spctr_pin_keys:\n");
    switch (AxBxC){ 
      case 0:
        printf("transforming global keys of A to local keys\n");
        break;
      case 1:
        printf("transforming global keys of B to local keys\n");
        break;
      case 2:
        printf("transforming global keys of C to local keys\n");
        break;
    }
    rec_ctr->print();
  }

  int64_t spctr_pin_keys::mem_fp(){
    return 3*order*sizeof(int);
  }
  
  int64_t spctr_pin_keys::mem_rec(){
    return mem_fp() + rec_ctr->mem_rec();
  }

  spctr_pin_keys::spctr_pin_keys(contraction const * s, int AxBxC_) : spctr(s) {
    tensor * X = NULL;
    AxBxC = AxBxC_;
    switch (AxBxC){ 
      case 0:
        X = s->A;
        dns_blk_sz = s->A->size;
        break;
      case 1:
        X = s->B;
        dns_blk_sz = s->B->size;
        break;
      case 2:
        X = s->C;
        dns_blk_sz = s->C->size;
        break;
    }

    order = X->order;
    lens = X->lens;

    divisor = (int*)alloc(sizeof(int)*order);
    phys_rank = (int*)alloc(sizeof(int)*order);
    virt_dim = (int*)alloc(sizeof(int*)*order);

    for (int i=0; i<order; i++){
      divisor[i] = X->edge_map[i].calc_phase();
      phys_rank[i] = X->edge_map[i].calc_phys_rank(X->topo);
      virt_dim[i] = divisor[i]/X->edge_map[i].calc_phys_phase();
    }
  }

  double spctr_pin_keys::est_time_fp(int nlyr, double nnz_frac_A, double nnz_frac_B, double nnz_frac_C) {
    switch (AxBxC){
      case 0:
        return 2.*dns_blk_sz*nnz_frac_A*COST_MEMBW;
      case 1:
        return 2.*dns_blk_sz*nnz_frac_B*COST_MEMBW;
      case 2:
        return 4.*dns_blk_sz*nnz_frac_C*COST_MEMBW;
    }
    return 0;
  }

  double spctr_pin_keys::est_time_rec(int nlyr, double nnz_frac_A, double nnz_frac_B, double nnz_frac_C) {
    return est_time_fp(nlyr, nnz_frac_A, nnz_frac_B, nnz_frac_C)+rec_ctr->est_time_rec(nlyr, nnz_frac_A, nnz_frac_B, nnz_frac_C);
  }

  void spctr_pin_keys::run(char * A, int nblk_A, int64_t const * size_blk_A,
                           char * B, int nblk_B, int64_t const * size_blk_B,
                           char * C, int nblk_C, int64_t * size_blk_C,
                           char *& new_C){
    TAU_FSTART(spctr_pin_keys);
    char * X = NULL;
    algstrct const * sr = NULL;
    int64_t nnz = 0;
    switch (AxBxC){ 
      case 0:
        X = A;
        sr = this->sr_A;
        for (int i=0; i<nblk_A; i++){
          ASSERT(size_blk_A[i]%sr_A->pair_size() == 0);
          nnz += size_blk_A[i]/sr_A->pair_size();
        }
        break;
      case 1:
        X = B;
        sr = this->sr_B;
        for (int i=0; i<nblk_B; i++){
          ASSERT(size_blk_B[i]%sr_B->pair_size() == 0);
          nnz += size_blk_B[i]/sr_B->pair_size();
        }
        break;
      case 2:
        X = C;
        sr = this->sr_C;
        for (int i=0; i<nblk_C; i++){
          ASSERT(size_blk_C[i]%sr_C->pair_size() == 0);
          nnz += size_blk_C[i]/sr_C->pair_size();
        }
        break;
    }

/*    int * div_lens;
    alloc_ptr(order*sizeof(int), (void**)&div_lens);
    for (int j=0; j<order; j++){
      div_lens[j] = (lens[j]/divisor[j] + (lens[j]%divisor[j] > 0));
//      printf("lens[%d] = %d divisor[%d] = %d div_lens[%d] = %d\n",j,lens[j],j,divisor[j],j,div_lens[j]);
    }*/
    char * nA, * nB, * nC;
    nA = A; nB = B; nC = C;
    char * nX = NULL;
    switch (AxBxC){
      case 0:
        mst_alloc_ptr(sr->pair_size()*nnz, (void**)&nX);
        memcpy(nX, X, sr->pair_size()*nnz);
        nA = nX;
        break;
      case 1:
        mst_alloc_ptr(sr->pair_size()*nnz, (void**)&nX);
        memcpy(nX, X, sr->pair_size()*nnz);
        nB = nX;
        break;
    }
    ConstPairIterator pi(sr, X);
    PairIterator pi_new(sr, nX);

    pi.pin(nnz, order, lens, divisor, pi_new);

    TAU_FSTOP(spctr_pin_keys);
    rec_ctr->run(nA, nblk_A, size_blk_A,
                 nB, nblk_B, size_blk_B,
                 nC, nblk_C, size_blk_C,
                 new_C);
    TAU_FSTART(spctr_pin_keys);


    switch (AxBxC){
      case 0:
      case 1:
        cdealloc(nX);
        break;
      case 2:
        int64_t new_nnz_C=0;
        for (int i=0; i<nblk_C; i++){
          ASSERT(size_blk_C[i] % sr_C->pair_size() == 0);
          new_nnz_C += size_blk_C[i] / sr_C->pair_size();
        }
        depin(sr_C, order, lens, divisor, nblk_C, virt_dim, phys_rank, new_C, new_nnz_C, size_blk_C, new_C, true);
        break;
    }
    TAU_FSTOP(spctr_pin_keys);
  }
}
