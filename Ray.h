#include "Vec3.h"

class Ray{
public:
  Ray(Vec3d o, Vec3d d);
  Vec3d origin;
  Vec3d direction;
  Vec3d intersect(Vec3d p0, Vec3d p1, Vec3d p2);
  bool is_intersect_with(Vec3d p0, Vec3d p1, Vec3d p2);
};
