#include "/home/yang/btas_test_2/ctf-master/include/ctf.hpp"

using namespace CTF;

int main(int argc, char ** argv)
{
  int shape[] = {2,3,4};
  int sym[] = {NS,NS,NS};
  World dw(argc,argv);
  Tensor<> A(3,shape,sym,dw);
  return 0;
}
