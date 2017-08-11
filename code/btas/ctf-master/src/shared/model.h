#ifndef __MODEL_H__
#define __MODEL_H__

#include "mpi.h"
#include "init_models.h"

namespace CTF_int { 
  class Model {
    public:
      virtual void update(MPI_Comm cm){};
      virtual void print(){};
      virtual void print_uo(){};
  };

  void update_all_models(MPI_Comm cm);
  void print_all_models();

  /**
   * \brief Linear performance models, which given measurements, provides new model guess
   */
  template <int nparam>
  class LinModel : Model {
    private:
      int nobs;
      int mat_lda;
      bool is_tuned;
      double tot_time;
      double over_time;
      double under_time;
    public:
      int hist_size;
      double * time_param_mat;
      double param_guess[nparam];
      char * name;

      /** 
       * \brief constructor
       * \param[in] init guess array of size nparam consisting of initial model parameter guesses
       * \param[in] hist_size number of times to keep in history
       * \param[in] tune_interval
       */
      LinModel(double const * init_guess, char const * name, int hist_size=16384);

      LinModel();
      ~LinModel();

      /**
       * \brief updates model based on observarions
       * \param[in] cm communicator across which we should synchronize model (collect observations)
       */
      void update(MPI_Comm cm);

      /**
       * \brief records observation consisting of execution time and nparam paramter values
       * \param[in] time_param array of size nparam+1 of form [exe_sec,val_1,val_2,...,val_nparam]
       */
      void observe(double const * time_param);
      
      /**
       * \brief estimates model time based on observarions
       * \param[in] param array of size nparam of form [val_1,val_2,...,val_nparam]
       * \return estimated time
       */
      double est_time(double const * param);

      /**
       * \brief prints current parameter estimates
       */
      void print();

      /**
       * \brief prints time estimate errors
       */
      void print_uo();
  };

  /**
   * \brief Cubic performance models, which given measurements, provides new model guess
   */
  template <int nparam>
  class CubicModel : Model {
    private:
      LinModel<nparam*(nparam+1)*(nparam+2)/6+nparam*(nparam+1)/2+nparam> lmdl;

    public:
      /** 
       * \brief constructor
       * \param[in] init guess array of size nparam consisting of initial model parameter guesses
       * \param[in] hist_size number of times to keep in history
       * \param[in] tune_interval
       */
      CubicModel(double const * init_guess, char const * name, int hist_size=8192);

      ~CubicModel();

      /**
       * \brief updates model based on observarions
       * \param[in] cm communicator across which we should synchronize model (collect observations)
       */
      void update(MPI_Comm cm);

      /**
       * \brief records observation consisting of execution time and nparam paramter values
       * \param[in] time_param array of size nparam+1 of form [exe_sec,val_1,val_2,...,val_nparam]
       */
      void observe(double const * time_param);
      
      /**
       * \brief estimates model time based on observarions
       * \param[in] param array of size nparam of form [val_1,val_2,...,val_nparam]
       * \return estimated time
       */
      double est_time(double const * param);

      /**
       * \brief prints current parameter estimates
       */
      void print();

      /**
       * \brief prints time estimate errors
       */
      void print_uo();

  };

}

#endif
