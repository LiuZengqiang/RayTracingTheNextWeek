/**
 * @file quad.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief 四边形类
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "rtweekend.h"

// 与 sphere 类似，都需要继承 hittable 类
// 因此需要 重载实现 bounding_box() 和 hit() 两个函数
// quad 是平行四边形 (是一个平面), 因此其 bbox 需要通过 pad() 扩大其包围盒
class quad : public hittable {
 public:
  quad(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m)
      : Q(_Q), u(_u), v(_v), mat(m) {
    auto n = cross(u, v);
    // 平行四边形的法向
    normal = unit_vector(n);
    // 计算光线与平行四边形相交时的辅助量
    D = dot(normal, Q);
    w = n / dot(n, n);
    set_bounding_box();
  }

  virtual void set_bounding_box() { bbox = aabb(Q, Q + u + v).pad(); }

  aabb bounding_box() const override { return bbox; }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    auto denom = dot(normal, r.direction());
    // 如果 r 与该平面的法向平行, 说明没有交点
    if (fabs(denom) < 1e-8) {
      return false;
    }
    // 计算相交点的 t
    auto t = (D - dot(normal, r.origin())) / denom;
    // 如果 t 不在合法区间内, 说明没有交点
    if (!ray_t.contains(t)) {
      return false;
    }
    auto intersection = r.at(t);

    vec3 planar_hitpt_vector = intersection - Q;
    // 如果参数 alpha, beta 都在 [0,1] 范围内部, 说明交点在四边形内
    auto alpha = dot(w, cross(planar_hitpt_vector, v));
    auto beta = dot(w, cross(u, planar_hitpt_vector));
    if (!is_interior(alpha, beta, rec)) {
      return false;
    }

    rec.t = t;
    rec.p = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);
    return true;  // To be implemented
  }

 private:
  point3 Q;                  // 平行四边形的 左下角
  vec3 u, v;                 // 平行四边形的 左/上 两条边(向量)
  shared_ptr<material> mat;  // 平行四边形的纹理
  aabb bbox;                 // 平行四边形的包围盒
  vec3 normal;               // 平行四边形的法向, 等于 cross(u,v)
  double D;  // 计算光线与平行四边形相交时的辅助变量
  vec3 w;  // 计算光线与平行四边形相交点是否在四边形内部的辅助变量

  // 判断局部参数 a,b 是否合法(是否在[0,1]内)
  // 假如在, 则将参数a,b 赋值给 rec
  virtual bool is_interior(double a, double b, hit_record& rec) const {
    // Given the hit point in plane coordinates, return false if it is outside
    // the primitive, otherwise set the hit record UV coordinates and return
    // true.

    if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) return false;

    rec.u = a;
    rec.v = b;
    return true;
  }
};

#endif