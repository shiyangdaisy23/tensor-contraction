#ifndef __MOLDYNAMICS_H__
#define __MOLDYNAMICS_H__

class force {
  public:
  double fx;
  double fy;

  force operator-() const {
    force fnew;
    fnew.fx = -fx;
    fnew.fy = -fy;
    return fnew;
  }
  
  force operator+(force const & fother) const {
    force fnew;
    fnew.fx = fx+fother.fx;
    fnew.fy = fy+fother.fy;
    return fnew;
  }

  force(){}

  // additive identity
  force(int){
    fx = 0.0;
    fy = 0.0;
  }
};

struct particle {
  double dx;
  double dy;
  double coeff;
  int id;
};

void acc_force(force f, particle & p){
  p.dx += f.fx*p.coeff;
  p.dy += f.fy*p.coeff;
}

double get_distance(particle const & p, particle const & q){
  return sqrt((p.dx-q.dx)*(p.dx-q.dx)+(p.dy-q.dy)*(p.dy-q.dy));
}


force get_force(particle const & p, particle const & q){
  force f;
  f.fx = (p.dx-q.dx)/std::pow(get_distance(p,q)+.01,3);
  f.fy = (p.dy-q.dy)/std::pow(get_distance(p,q)+.01,3);
  return f;
}


#endif

