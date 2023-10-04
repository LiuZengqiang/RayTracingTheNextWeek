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
#include "hittable_list.h"
#include "rtweekend.h"

/**
 * @brief 平行四边形类
 *
 */
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
    // 尝试使用 Moller-Trumbore 方法求交点
    vec3 e1 = u;
    vec3 e2 = v;
    vec3 P = cross(r.direction(), e2);
    vec3 T = r.origin() - Q;
    vec3 QQ = cross(T, e1);

    float det = dot(P, e1);

    float uu = dot(T, P);
    float vv = dot(r.direction(), QQ);
    float t = dot(e2, QQ);
    float f_inv_det = 1.0f / det;
    t *= f_inv_det;
    uu *= f_inv_det;
    vv *= f_inv_det;
    if (ray_t.contains(t) && uu >= 0 && vv >= 0 && uu <= 1.0 && vv <= 1.0) {
      auto intersection = r.at(t);

      rec.u = uu;
      rec.v = vv;
      rec.t = t;
      rec.p = intersection;
      rec.mat = mat;
      rec.set_face_normal(r, normal);
      return true;  // To be implemented
    }
    return false;
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

// 根据两个点构造一个六面体
inline shared_ptr<hittable_list> box(const point3& a, const point3& b,
                                     shared_ptr<material> mat) {
  // Returns the 3D box (six sides) that contains the two opposite vertices a
  // & b.

  auto sides = make_shared<hittable_list>();

  // Construct the two opposite vertices with the minimum and maximum
  // coordinates.
  auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
  auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

  auto dx = vec3(max.x() - min.x(), 0, 0);
  auto dy = vec3(0, max.y() - min.y(), 0);
  auto dz = vec3(0, 0, max.z() - min.z());

  sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy,
                               mat));  // front
  sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy,
                               mat));  // right
  sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy,
                               mat));  // back
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy,
                               mat));  // left
  sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz,
                               mat));  // top
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz,
                               mat));  // bottom

  return sides;
}
#endif