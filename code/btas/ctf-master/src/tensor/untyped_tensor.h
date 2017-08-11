/*Copyright (c) 2011, Edgar Solomonik, all rights reserved.*/

#ifndef __UNTYPED_TENSOR_H__
#define __UNTYPED_TENSOR_H__

#include "../mapping/mapping.h"
#include "../mapping/distribution.h"
#include "../interface/world.h"
#include "../interface/partition.h"
#include "algstrct.h"
#include <functional>

namespace CTF {
  class Idx_Tensor;
}

namespace CTF_int {

  /** \brief internal distributed tensor class */
  class tensor {
    protected:
      /**
       * \brief initializes tensor data
       * \param[in] sr defines the tensor arithmetic for this tensor
       * \param[in] order number of dimensions of tensor
       * \param[in] edge_len edge lengths of tensor
       * \param[in] sym symmetries of tensor (e.g. symmetric matrix -> sym={SY, NS})
       * \param[in] wrld a distributed context for the tensor to live in
       * \param[in] name_an optionary name for the tensor
       * \param[in] profile set to 1 to profile contractions involving this tensor
       * \param[in] is_sparse set to 1 to store only nontrivial tensor elements
       */
      void init(algstrct const * sr,
                int              order,
                int const *      edge_len,
                int const *      sym,
                CTF::World *     wrld,
                bool             alloc_data,
                char const *     name,
                bool             profile,
                bool             is_sparse);
      /**
       * \brief read all pairs with each processor (packed)
       * \param[out] num_pair number of values read
       * return pair iterator with allocated all pairs read 
       */
      PairIterator read_all_pairs(int64_t * num_pair, bool unpack);

      /** 
       * \brief copies all tensor data from other
       * \param[in] other tensor to copy from
       */
      void copy_tensor_data(tensor const * other);

      /** 
       * \brief set edge mappings as specified
       * \param[in] idx assignment of characters to each dim
       * \param[in] prl mesh processor topology with character labels
       * \param[in] blk local blocking with processor labels
       */
      void set_distribution(char const *               idx,
                            CTF::Idx_Partition const & prl,
                            CTF::Idx_Partition const & blk);

    public:

      /** \brief distributed processor context on which tensor is defined */
      CTF::World * wrld;
      /** \brief algstrct on which tensor elements and operations are defined */
      algstrct * sr;
      /** \brief symmetries among tensor dimensions */
      int * sym;
      /** \brief number of tensor dimensions */
      int order;
      /** \brief unpadded tensor edge lengths */
      int * lens;
      /** \brief padded tensor edge lengths */
      int * pad_edge_len;
      /** \brief padding along each edge length (less than distribution phase) */
      int * padding;
      /** \brief name given to tensor */
      char * name;
      /** \brief whether tensor data has additional padding */
      int is_scp_padded;
      /** \brief additional padding, may be greater than ScaLAPACK phase */
      int * scp_padding; 
      /** \brief order-by-order table of dimensional symmetry relations */
      int * sym_table; 
      /** \brief whether a mapping has been selected */
      bool is_mapped;
      /** \brief topology to which the tensor is mapped */
      topology * topo;
      /** \brief mappings of each tensor dimension onto topology dimensions */
      mapping * edge_map;
      /** \brief current size of local tensor data chunk (mapping-dependent) */
      int64_t size;
      /** \brief whether the data is folded/transposed into a (lower-order) tensor */
      bool is_folded;
      /** \brief ordering of the dimensions according to which the tensori s folded */
      int * inner_ordering;
      /** \brief representation of folded tensor (shares data pointer) */
      tensor * rec_tsr;
      /** \brief whether the tensor data is cyclically distributed (blocked if false) */
      bool is_cyclic;
      /** \brief whether the tensor data is an alias of another tensor object's data */
      bool is_data_aliased;
      /** \brief tensor object associated with tensor object whose data pointer needs to be preserved, 
          needed for ScaLAPACK wrapper FIXME: home buffer should presumably take care of this... */
      tensor * slay;
      /** \brief if true tensor has a zero edge length, so is zero, which short-cuts stuff */
      bool has_zero_edge_len;
      /** \brief tensor data, either the data or the key-value pairs should exist at any given time */
      char * data;
      /** \brief whether the tensor has a home mapping/buffer */
      bool has_home;
      /** \brief buffer associated with home mapping of tensor, to which it is returned */
      char * home_buffer;
      /** \brief size of home buffer */
      int64_t home_size;
      /** \brief whether the latest tensor data is in the home buffer */
      bool is_home;
      /** \brief whether profiling should be done for contractions/sums involving this tensor */
      bool profile;
      /** \brief whether only the non-zero elements of the tensor are stored */
      bool is_sparse;
      /** \brief number of local nonzero elements */
      int64_t nnz_loc;
      /** \brief maximum number of local nonzero elements over all procs*/
      int64_t nnz_tot;
      /** \brief nonzero elements in each block owned locally */
      int64_t * nnz_blk;
      
      /**
       * \brief associated an index map with the tensor for future operation
       * \param[in] idx_map_ index assignment for this tensor
       */
      CTF::Idx_Tensor operator[](char const * idx_map);
 
      tensor();

      /** \brief class free self */
      ~tensor();

      /** \brief destructor */
      void free_self();

      /**
       * \brief defines a tensor object with some mapping (if alloc_data)
       * \param[in] sr defines the tensor arithmetic for this tensor
       * \param[in] order number of dimensions of tensor
       * \param[in] edge_len edge lengths of tensor
       * \param[in] sym symmetries of tensor (e.g. symmetric matrix -> sym={SY, NS})
       * \param[in] wrld a distributed context for the tensor to live in
       * \param[in] alloc_data whether to allocate and set data to zero immediately
       * \param[in] name_ an optionary name for the tensor
       * \param[in] profile set to 1 to profile contractions involving this tensor
       * \param[in] is_sparse set to 1 to store only nontrivial tensor elements
       */
      tensor(algstrct const * sr,
             int              order,
             int const *      edge_len,
             int const *      sym,
             CTF::World *     wrld,
             bool             alloc_data=true,
             char const *     name=NULL,
             bool             profile=1,
             bool             is_sparse=0);

      /**
       * \brief defines a tensor object with some mapping (if alloc_data)
       * \param[in] sr defines the tensor arithmetic for this tensor
       * \param[in] order number of dimensions of tensor
       * \param[in] edge_len edge lengths of tensor
       * \param[in] sym symmetries of tensor (e.g. symmetric matrix -> sym={SY, NS})
       * \param[in] wrld a distributed context for the tensor to live in
       * \param[in] idx assignment of characters to each dim
       * \param[in] prl mesh processor topology with character labels
       * \param[in] blk local blocking with processor labels
       * \param[in] name_ an optionary name for the tensor
       * \param[in] profile set to 1 to profile contractions involving this tensor
       */
      tensor(algstrct const *           sr,
             int                        order,
             int const *                edge_len,
             int const *                sym,
             CTF::World *               wrld,
             char const *               idx,
             CTF::Idx_Partition const & prl,
             CTF::Idx_Partition const & blk,
             char const *               name=NULL,
             bool                       profile=1);

      /**
       * \brief creates tensor copy, unfolds other if other is folded
       * \param[in] other tensor to copy
       * \param[in] copy whether to copy mapping and data
       * \param[in] alloc_data whether th allocate data
       */
      tensor(tensor const * other, bool copy = 1, bool alloc_data = 1);

      /**
       * \brief repacks the tensor other to a different symmetry 
       *        (assumes existing data contains the symmetry and keeps only values with indices in increasing order)
       * WARN: LIMITATION: new_sym must cause unidirectional structural changes, i.e. {NS,NS}->{SY,NS} OK, {SY,NS}->{NS,NS} OK, {NS,NS,SY,NS}->{SY,NS,NS,NS} NOT OK!
       * \param[in] other tensor to copy
       * \param[in] new_sym new symmetry array (replaces this->sym)
       */
      tensor(tensor * other, int const * new_sym);

      /**
       * \brief compute the cyclic phase of each tensor dimension
       * \return int * of cyclic phases
       */
      int * calc_phase() const;

      /**
       * \brief calculate the total number of blocks of the tensor
       * \return int total phase factor
       */
      int calc_tot_phase() const;

      /**
       * \brief calculate virtualization factor of tensor
       * return virtualization factor
       */
      int64_t calc_nvirt() const;

      /**
       * \brief calculate the number of processes this tensor is distributed over
       * return number of processes owning a block of the tensor
       */
      int64_t calc_npe() const;


      /**
       * \brief sets padding and local size of a tensor given a mapping
       */
      void set_padding();

      /**
       * \brief elects a mapping and sets tensor data to zero
       */
      int set_zero();

      /**
       * \brief sets tensor data to val
       */
      int set(char const * val);

      /**
       * \brief sets padded portion of tensor to zero (this should be maintained internally)
       */
      int zero_out_padding();

      /**
       * \brief scales each element by 1/(number of entries equivalent to it after permutation of indices for which sym_mask is 1)
       * \param[in] identifies which tensor indices are part of the symmetric group which diagonals we want to scale (i.e. sym_mask [1,1] does A["ii"]= (1./2.)*A["ii"])
       */
      void scale_diagonals(int const * sym_mask);

      // apply an additive inverse to all elements of the tensor
      void addinv();

      /**
       * \brief displays mapping information
       * \param[in] stream output log (e.g. stdout)
       * \param[in] allcall (if 1 print only with proc 0)
       */
      void print_map(FILE * stream=stdout, bool allcall=1) const;

      /**
       * \brief set the tensor name 
       * \param[in] name to set
       */
      void set_name(char const * name);

      /**
       * \brief get the tensor name 
       * \return tensor name 
       */
      char const * get_name() const;

      /** \brief turn on profiling */
      void profile_on();

      /** \brief turn off profiling */
      void profile_off();


      /**
       * \brief get raw data pointer without copy WARNING: includes padding 
       * \param[out] data raw local data in char * format
       * \param[out] size number of elements in data
       */
      void get_raw_data(char ** data, int64_t * size) const;

      /** 
       * \brief  Add tensor data new=alpha*new+beta*old
       *         with <key, value> pairs where key is the 
       *         global index for the value. 
       * \param[in] num_pair number of pairs to write
       * \param[in] alpha scaling factor of written (read) value
       * \param[in] beta scaling factor of old (existing) value
       * \param[in] mapped_data pairs to write
       * \param[in] rw weather to read (r) or write (w)
       */
       int write(int64_t      num_pair,
                 char const * alpha,
                 char const * beta,
                 char *       mapped_data,
                 char const   rw='w');

      /**
       * \brief read tensor data with <key, value> pairs where key is the
       *         global index for the value, which gets filled in with
       *         beta times the old values plus alpha times the values read from the tensor. 
       * \param[in] num_pair number of pairs to read
       * \param[in] alpha scaling factor of read value
       * \param[in] beta scaling factor of old value
       * \param[in] mapped_data pairs to write
       */
      int read(int64_t      num_pair,
               char const * alpha,
               char const * beta,
               char * const mapped_data);

      /**
       * \brief returns local data of tensor with parallel distribution prl and local blocking blk
       * \param[in] idx assignment of characters to each dim
       * \param[in] prl mesh processor topology with character labels
       * \param[in] blk local blocking with processor labels
       * \param[in] unpack whether to unpack from symmetric layout
       * \return local piece of data of tensor in this distribution
       */
      char * read(char const *               idx,
                  CTF::Idx_Partition const & prl,
                  CTF::Idx_Partition const & blk,
                  bool                       unpack);

      /**
       * \brief read tensor data with <key, value> pairs where key is the
       *              global index for the value, which gets filled in. 
       * \param[in] num_pair number of pairs to read
       * \param[in,out] mapped_data pairs to read
       */
      int read(int64_t      num_pair,
               char * const mapped_data);

      /**
       * \brief read entire tensor with each processor (in packed layout).
       *         WARNING: will use an 'unscalable' amount of memory. 
       * \param[out] num_pair number of values read
       * \param[in,out] mapped_data values read (allocated by library)
       * \param[in] unpack if true any symmetric tensor is unpacked, otherwise only unique elements are read
       */
      int allread(int64_t * num_pair,
                  char **   all_data,
                  bool      unpack);

      /**
       * \brief read entire tensor with each processor (in packed layout).
       *         WARNING: will use an 'unscalable' amount of memory. 
       * \param[out] num_pair number of values read
       * \param[in,out] preallocated mapped_data values read
       * \param[in] unpack if true any symmetric tensor is unpacked, otherwise only unique elements are read
       */
      int allread(int64_t * num_pair,
                  char *    all_data,
                  bool      unpack);

       /**
       * \brief cuts out a slice (block) of this tensor = B
       *   B[offsets,ends)=beta*B[offsets,ends) + alpha*A[offsets_A,ends_A)
       * \param[in] offsets_B bottom left corner of block
       * \param[in] ends_B top right corner of block
       * \param[in] alpha scaling factor of this tensor
       * \param[in] A tensor who owns pure-operand slice
       * \param[in] offsets bottom left corner of block of A
       * \param[in] ends top right corner of block of A
       * \param[in] alpha scaling factor of tensor A
       */
      void slice(int const *  offsets_B,
                 int const *  ends_B,
                 char const * beta,
                 tensor  *    A,
                 int const *  offsets_A,
                 int const *  ends_A,
                 char const * alpha);
     
      /* Same as above, except tid_B lives on dt_other_B */
/*      int slice_tensor(int            tid_A,
                       int const *    offsets_A,
                       int const *    ends_A,
                       char const *   alpha,
                       int            tid_B,
                       int const *    offsets_B,
                       int const *    ends_B,
                       char const *   beta,
                       world *        dt_other_B);
*/
      /**
       * Permutes a tensor along each dimension skips if perm set to -1, generalizes slice.
       *        one of permutation_A or permutation_B has to be set to NULL, if multiworld read, then
       *        the parent world tensor should not be being permuted
       * \param[in] A pure-operand tensor
       * \param[in] permutation_A mappings for each dimension of A indices
       * \param[in] alpha scaling factor for A
       * \param[in] permutation_B mappings for each dimension of B (this) indices
       * \param[in] alpha scaling factor for current values of B
       */
      int permute(tensor *      A,
                  int * const * permutation_A,
                  char const *  alpha,
                  int * const * permutation_B,
                  char const *  beta);

      /**
       * \brief reduce tensor to sparse format, storing only nonzero data, or data above a specified threshold.
       *        makes dense tensors sparse.
       *        cleans sparse tensors of any 'computed' zeros.
       * \param[in] threshold all values smaller or equal to than this one will be removed/not stored (by default is NULL, meaning only zeros are removed, so same as threshold=additive identity)
       * \param[in] take_abs whether to take absolute value when comparing to threshold
       */
      int sparsify(char const * threshold=NULL,
                   bool         take_abs=true);
  
      /**
       * \brief sparsifies tensor keeping only values v such that filter(v) = true
       * \param[in] f boolean function to apply to values to determine whether to keep them
       */ 
      int sparsify(std::function<bool(char const*)> f);

      /**
       * \brief read tensor data pairs local to processor including those with zero values
       *          WARNING: for sparse tensors this includes the zeros to maintain consistency with 
       *                   the behavior for dense tensors, use read_local_nnz to get only nonzeros
       * \param[out] num_pair number of values read
       * \param[out] mapped_data values read
       */
      int read_local(int64_t * num_pair,
                     char **   mapped_data) const;

      /**
       * \brief read tensor data pairs local to processor that have nonzero values
       * \param[out] num_pair number of values read
       * \param[out] mapped_data values read
       */
      int read_local_nnz(int64_t * num_pair,
                         char **   mapped_data) const;

      /** 
       * \brief copy A into this (B). Realloc if necessary 
       * \param[in] A tensor to copy
       */
      //int copy(tensor * A);

      /**
       * \brief align mapping of thisa tensor to that of B
       * \param[in] B tensor handle of B
       */
      int align(tensor const * B);

      /* product will contain the dot prodiuct if tsr_A and tsr_B */
      //int dot_tensor(int tid_A, int tid_B, char *product);

      /**
       * \brief Performs an elementwise summation reduction on a tensor 
       * \param[out] result result of reduction operation
       */
      int reduce_sum(char * result);
 
      /**
       * \brief Performs an elementwise summation reduction on a tensor with summation defined by sr_other
       * \param[out] result result of reduction operation
       * \param[in] sr_other an algebraic structure (at least a monoid) defining the summation operation
       */
      int reduce_sum(char * result, algstrct const * sr_other);

      /**
       * \brief Performs an elementwise absolute value summation reduction on a tensor 
       * \param[out] result result of reduction operation
       */
      int reduce_sumabs(char * result);

      /**
       * \brief Performs an elementwise absolute value summation reduction on a tensor 
       * \param[out] result result of reduction operation
       * \param[in] sr_other an algebraic structure (at least a monoid) defining the summation operation
       */
      int reduce_sumabs(char * result, algstrct const * sr_other) ;


      /**
       * \brief computes the sum of squares of the elements
       * \param[out] result result of reduction operation
       */
      int reduce_sumsq(char * result);

      /* map data of tid_A with the given function */
/*      int map_tensor(int tid,
                     dtype (*map_func)(int order, 
                                       int const * indices,
                                       dtype elem));*/
      /** 
       * \brief obtains the largest n elements (in absolute value) of the tensor 
       * \param[in] n number of elements to fill
       * \param[in,out] data preallocated array of size at least n, in which to put the elements
       */
      int get_max_abs(int n, char * data) const;

      /**
       * \brief prints tensor data to file using process 0
       * \param[in] fp file to print to e.g. stdout
       * \param[in] cutoff do not print values of absolute value smaller than this
       */
      void print(FILE * fp = stdout, char const * cutoff = NULL) const;

      /**
       * \brief prints two sets of tensor data side-by-side to file using process 0
       * \param[in] A tensor to compare against
       * \param[in] fp file to print to e.g. stdout
       * \param[in] cutoff do not print values of absolute value smaller than this
       */
      void compare(const tensor * A, FILE * fp, char const * cutoff);

      /**
       * \brief maps data from this world (subcomm) to the correct order of processors with
       *        respect to a parent (greater_world) comm
       * \param[in] greater_world comm with respect to which the data needs to be ordered
       * \param[out] bw_mirror_rank processor rank in greater_world from which data is received
       * \param[out] fw_mirror_rank processor rank in greater_world to   which data is sent
       * \param[out] distribution mapping of data on output defined on oriented subworld
       * \param[out] sub_buffer_ allocated buffer of received data on oriented subworld
      */
      void orient_subworld(CTF::World *    greater_world,
                           int &           bw_mirror_rank,
                           int &           fw_mirror_rank,
                           distribution *& odst,
                           char **         sub_buffer_);

      /**
        * \brief accumulates this tensor to a tensor object defined on a different world
        * \param[in] tsr_sub tensor on a subcomm of this world
        * \param[in] alpha scaling factor for this tensor
        * \param[in] beta scaling factor for tensor tsr
        */
      void add_to_subworld(tensor *     tsr_sub,
                           char const * alpha,
                           char const * beta);

      /**
        * \brief accumulates this tensor from a tensor object defined on a different world
        * \param[in] tsr_sub id of tensor on a subcomm of this CTF inst
        * \param[in] tC_sub CTF instance on a mpi subcomm
        * \param[in] alpha scaling factor for this tensor
        * \param[in] beta scaling factor for tensor tsr
        */
      void add_from_subworld(tensor *     tsr_sub,
                             char const * alpha,
                             char const * beta);

      /**
       * \brief undo the folding of a local tensor block
       *        unsets is_folded and deletes rec_tsr
       */
      void unfold();
      
      /**
       * \brief removes folding without doing transpose
       *        unsets is_folded and deletes rec_tsr
       */
      void remove_fold();


      /**
       * \brief estimate cost of potential transpose involved in undoing the folding of a local tensor block
       * \return estimated time for transpose
       */
      double est_time_unfold();



      /**
       * \brief fold a tensor by putting the symmetry-preserved 
       *        portion in the leading dimensions of the tensor
       *        sets is_folded and creates rec_tsr with aliased data
       *
       * \param[in] nfold number of global indices we are folding
       * \param[in] fold_idx which global indices we are folding
       * \param[in] idx_map how this tensor indices map to the global indices
       * \param[out] all_fdim number of dimensions including unfolded dimensions
       * \param[out] all_flen edge lengths including unfolded dimensions
       */
      void fold(int         nfold,
                int const * fold_idx,
                int const * idx_map,
                int *       all_fdim,
                int **      all_flen);

      /**
        * \brief pulls data from an tensor with an aliased buffer
        * \param[in] other tensor with aliased data to pull from
        */ 
      void pull_alias(tensor const * other);

    
      /** \brief zeros out mapping */
      void clear_mapping();


      /**
       * \brief permutes the data of a tensor to its new layout
       * \param[in] old_dist previous distribution to remap data from
       */
      int redistribute(distribution const & old_dist,
                       int const *  old_offsets = NULL,
                       int * const * old_permutation = NULL,
                       int const *  new_offsets = NULL,
                       int * const * new_permutation = NULL);

      double est_redist_time(distribution const & old_dist, double nnz_frac);
  
      int64_t get_redist_mem(distribution const & old_dist, double nnz_frac);

      /**
        * \brief map the remainder of a tensor 
        * \param[in] num_phys_dims number of physical processor grid dimensions
        * \param[in] phys_comm dimensional communicators
        */
      int map_tensor_rem(int        num_phys_dims,
                         CommData * phys_comm,
                         int        fill=0);

      /**
       * \brief extracts the diagonal of a tensor if the index map specifies to do so
       * \param[in] idx_map index map of tensor for this operation
       * \param[in] rw if 1 this writes to the diagonal, if 0 it reads the diagonal
       * \param[in,out] new_tsrw if rw=1 this will be output as new tensor
                                if rw=0 this should be input as the tensor of the extracted diagonal 
       * \param[out] idx_map_new if rw=1 this will be the new index map
       */
      int extract_diag(int const * idx_map,
                       int         rw,
                       tensor *&   new_tsr,
                       int **      idx_map_new);

      /** \brief sets symmetry, WARNING: for internal use only !!!!
        * \param[in] sym
        */
      void set_sym(int const * sym); 

      /**
       * \brief sets the number of nonzeros both locally (nnz_loc) and overall globally (nnz_tot)
       * \param[in] nnz_blk number of nonzeros in each block  
       */
      void set_new_nnz_glb(int64_t const * nnz_blk);
  };
}

#endif// __UNTYPED_TENSOR_H__

