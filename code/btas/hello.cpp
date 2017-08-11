#include "btas/tensor.h"
#include "btas/tensor_func.h"
#include "btas/generic/contract.h"
#include <random>
#include <chrono>

using namespace btas;
using namespace std;
typedef std::chrono::high_resolution_clock Clock;

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
  Tensor<double> A(200,300,500);
  Tensor<double> B(500,600);
  Tensor<double> C(200,300,600);
  // Tensor<double> Ctest(200,300,600);
  A.generate([](){ return randomReal<double>(); });
  B.generate([](){ return randomReal<double>(); });
  C.generate([](){ return randomReal<double>(); });
  double alpha = 1;
  double beta = 0; 
  /*  int start_s=clock();
  contract(alpha,A,{'i','j','k'},B,{'k','l'},beta,C,{'i','j','l'});
  int stop_s=clock();
  cout << "time:"<<(stop_s-start_s)/double(CLOCKS_PER_SEC)*1000
       <<endl;
   cout<<"contract done"<<endl;  */
  auto tic_1 = Clock::now();
  contract(alpha,A,{'i','j','k'},B,{'k','l'},beta,C,{'i','j','l'});
  auto toc_1 = Clock::now();
  cout << std::chrono::duration_cast<std::chrono::nanoseconds>(toc_1 - tic_1).count() << endl;
   /*   for (long i=0;i<A.extent(0);i++)
    for (long j=0;j<A.extent(1);j++)
      for (long k=0;k<A.extent(2);k++)
	for (long l=0;l<B.extent(1);l++){
	  Ctest(i,j,l)+=alpha*A(i,j,k)*B(k,l);
}
   double res = 1;
   for(auto i :C.range()){ res+=abs(C(i)-Ctest(i));}
   res = 100000*res;   
cout<<"res:"<<endl;
   cout<<res<<endl;   
   cout<<"C:"<<endl;
    cout<<C<<endl;
    cout<<"Ctest:"<<endl;
    cout<<Ctest<<endl;*/
  return 0;
}
