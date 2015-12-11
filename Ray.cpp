#include "Ray.h"
Ray::Ray(Vec3d o, Vec3d d){
  origin = o;
  direction = normalize(d);
}

bool Ray::is_intersect_with(Vec3d p0, Vec3d p1, Vec3d p2){
  Vec3d e0(p1-p0);
  Vec3d e1(p2-p0);
  Vec3d n(cross(e0,e1)/cross(e0,e1).length());
  Vec3d q(cross(direction,e1));
  double a = dot(e0,q);
  if(dot(n,direction) >= 0 || fabs(a) < 0.00001)
    return false;
  Vec3d s((origin-p0)/a);
  Vec3d r(cross(s,e0));
  double b0(dot(s,q));
  double b1(dot(r,direction));
  double b2(1-b0-b1);
  if(b0 < 0 || b1 < 0 || b2 < 0)
    return false;
  double t(dot(e1,r));
  if(t<0)
    return false;
  return true;
}

Vec3d Ray::intersect(Vec3d p0, Vec3d p1, Vec3d p2){
  Vec3d e0(p1-p0);
  Vec3d e1(p2-p0);
  Vec3d n(cross(e0,e1)/cross(e0,e1).length());
  Vec3d q(cross(direction,e1));
  double a = dot(e0,q);
  Vec3d s((origin-p0)/a);
  Vec3d r(cross(s,e0));
  double b0(dot(s,q));
  double b1(dot(r,direction));
  double b2(1-b0-b1);
  Vec3d ans(b0,b1,b2);
  return ans;
}
