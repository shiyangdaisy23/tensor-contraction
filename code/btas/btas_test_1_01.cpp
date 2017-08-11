#include "btas/tensor.h"
#include "btas/tensor_func.h"
#include "btas/generic/contract.h"
#include <random>
#include <chrono>
#include <fstream>
//#include <sys/time.h>
//#include <mkl.h>
//#include <mkl_service.h>

using namespace btas;
using namespace std;
//typedef std::chrono::high_resolution_clock Clock;

template <typename T>
std::ostream& 
operator<<(std::ostream& s, const Tensor<T>& X)
{
  for(auto i : X.range()) s << i << " " << X(i) << "\n";
  return s;
}

template <typename T>
T
randomReal()
{
  static std::mt19937 rng(time(NULL)); // std::time(NULL) replaced
  static auto dist = std::uniform_real_distribution<T>{0., 1.};
  return dist(rng);
}

int main()
{
  /*//cblas_dgemm_batch();
  //mkl_verbose(1);
  MKLVersion Version;
  mkl_get_version(&Version);
  printf("Major version: %d\n",Version.MajorVersion);
  printf("Minor version: %d\n",Version.MinorVersion);
  printf("Update version: %d\n",Version.UpdateVersion);
  printf("Product status: %s\n",Version.ProductStatus);
  printf("Build: %s\n",Version.Build);
  printf("Platform: %s\n",Version.Platform);
  printf("Processor optimization: %s\n",Version.Processor);
  printf("================================================================\n");
  printf("\n");
  //cblas_dgemm();*/
  ofstream fout_1;
  fout_1.open("timing_1_01.txt",ios::out|ios::app);
  std::chrono::duration<double>* tic_toc = new std::chrono::duration<double>[4];
  //struct timeval tv1, tv2;
  int t_size;
  for(t_size=5; t_size<6; t_size+=50)
    //for(t_size=5; t_size<556; t_size++)
  {
    cout << "size = " << t_size << endl;
    Tensor<double> A(t_size,t_size);
    Tensor<double> B(t_size,t_size,t_size);
    Tensor<double> C(t_size,t_size,t_size);
    A.generate([](){ return randomReal<double>(); });
    B.generate([](){ return randomReal<double>(); });
    C.generate([](){ return randomReal<double>(); });
    double alpha = 1;
    double beta = 0; 
    auto tic_1 = Clock::now();
    //gettimeofday(&tv1, NULL);
    //contract(alpha,A,{'i','k','j'},B,{'k','l'},beta,C,{'i','j','l'});
    //contract(alpha,A,{'k','m'},B,{'k','p','n'},beta,C,{'m','n','p'},tic_toc);
    //contract(alpha,A,{'k','m'},B,{'k','n','p'},beta,C,{'m','n','p'},tic_toc);
    //contract(alpha,A,{'a','i','j'},B,{'i','j','b'},beta,C,{'a','b'},tic_toc);
    //contract(alpha,A,{'a','i','j'},B,{'i','j','b'},beta,C,{'b','a'},tic_toc);
    //contract(alpha,A,{'a','i','j'},B,{'q','i','j'},beta,C,{'a','q'},tic_toc);
    //contract(alpha,A,{'k','n'},B,{'p','k','m'},beta,C,{'m','n','p'},tic_toc); // 4.4
    //contract(alpha,A,{'k','m'},B,{'p','k','n'},beta,C,{'m','n','p'},tic_toc); // 2.4
    //contract(alpha,A,{'m','k'},B,{'n','k','p'},beta,C,{'m','n','p'},tic_toc); // 1.3
    //contract(alpha,A,{'m','k'},B,{'k','n','p'},beta,C,{'m','n','p'},tic_toc);//1.1
    // contract(alpha,A,{'m','k'},B,{'n','p','k'},beta,C,{'m','n','p'},tic_toc);//1.5
    //contract(alpha,A,{'n','k'},B,{'p','k','m'},beta,C,{'m','n','p'},tic_toc); // 3.4
    //contract(alpha,A,{'k','m'},B,{'k','n','p'},beta,C,{'m','n','p'},tic_toc);//2.1 
    //contract(alpha,A,{'p','k'},B,{'k','m','n'},beta,C,{'m','n','p'},tic_toc);//5.1
    //contract(alpha,A,{'k','p'},B,{'k','m','n'},beta,C,{'m','n','p'},tic_toc);//6.1
    //contract(alpha,A,{'m','k'},B,{'k','p','n'},beta,C,{'m','n','p'},tic_toc);//1.2
    contract(alpha,A,{'k','m'},B,{'k','n','p'},beta,C,{'m','n','p'},tic_toc);//2.2 
    auto toc_1 = Clock::now();
    //auto tic_toc_1 = chrono::duration_cast<std::chrono::nanoseconds>(toc_1-tic_1).count();
    std::chrono::duration<double> tic_toc_1 = toc_1-tic_1;
    //cout << "Overall time = " << (tic_toc_1).count() << endl;
    //cout << "Overall time = " << (tic_toc_1).count()*1000 << endl;
    //cout << "Overall time = " << chrono::duration_cast<std::chrono::microseconds>(tic_toc_1).count() << endl;
    //cout << "Sum of splits = " << (tic_toc[0]+tic_toc[1]+tic_toc[2]+tic_toc[3]).count() << endl;
    //cout << "Sum of splits = " << chrono::duration_cast<std::chrono::microseconds>(tic_toc[0]+tic_toc[1]+tic_toc[2]+tic_toc[3]).count() << endl;
    fout_1 << t_size << '\t' << tic_toc_1.count()*1000 << '\t' << (tic_toc[0]+tic_toc[1]+tic_toc[2]+tic_toc[3]).count()*1000 << '\t' << tic_toc[0].count()*1000 << '\t' << tic_toc[1].count()*1000 << '\t' << tic_toc[2].count()*1000 << '\t' << tic_toc[3].count()*1000 << endl;
  }
  fout_1.close();
  return 0;
}



