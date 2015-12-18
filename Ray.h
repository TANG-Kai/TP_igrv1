#ifndef RAY_H
#define RAY_H
#include "Vec3.h"
class Ray {
public:
  Vec3d origin;
  Vec3d direction;
  Ray(Vec3d o,Vec3d direct){
    origin = o;
    direction =normalize(direct);

  }

  bool is_intersect_with(Vec3d p0, Vec3d p1, Vec3d p2)
  {

    // std::cout << "is_intersect_with" << std::endl;

    //	std::cout << "ray.origin = " << ray.origin[0] << " ; " << ray.origin[1] << " ; " << ray.origin[2] << std::endl;
    //	std::cout << "ray.direction = " << ray.direction[0] << " ; " << ray.direction[1] << " ; " << ray.direction[2] << std::endl;
    //std::cout << "p0" << p0[0] << " ; " << p0[1] << " ; " << p0[2] << std::endl;
  //  std::cout << "p1" << p1[0] << " ; " << p1[1] << " ; " << p1[2] << std::endl;
//    std::cout << "p2" << p2[0] << " ; " << p2[1] << " ; " << p2[2] << std::endl;
    Vec3d o = origin;
    Vec3d w = normalize(direction);
    Vec3d e0, e1;
    e0 = p1 - p0;
    e1 = p2 - p0;
    Vec3d n = normalize(cross(e0 ,e1)) ;
    Vec3d q = cross(direction ,e1);
    double a  = dot(e0, q);
    if(dot(n, direction) >= 0 )
    {
      return false;
    }
    else if(fabs(a) < 0.001)
    {
      return false;
    }
    Vec3d s;
    s = (origin - p0) /a;
    Vec3d r = cross(s ,e0);
    double b0, b1, b2;
    b0 = dot(s,q);
    b1 = dot(r,w);
    b2 = 1.0 - b0 - b1;
    if( b1<0 || b1<0 || b2<0 )
    {
      return false;
    }
    double t;
    t = dot(e1, r);
    if(t >= 0)
    return true;
    else
    {
      return false;
    }
  }
};
#endif
