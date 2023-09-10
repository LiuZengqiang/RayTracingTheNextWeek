/**
 * @file constant_medium.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief 实体物体类
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H
#include "hittable.h"
#include "material.h"
#include "rtweekend.h"
#include "texture.h"

class constant_medium : public hittable {
 public:
  // 构造函数:
  // b: 物体的边界(虽然是hittable, 但实际上是其子类hittable_list)
  // d: 物体的透明度, d越大物体越不透明
  // a: 物体的材质(用于确定光在其中的传播方向和物体颜色)
  constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
      : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(a)) {}

  constant_medium(shared_ptr<hittable> b, double d, color c)
      : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(make_shared<isotropic>(c)) {}

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;
    // 如果光线 与物体无法相交 直接返回 false
    if (!boundary->hit(r, interval::universe, rec1)) return false;

    // 如果光线 无法在物体内部(说明rec1是光线射出物体的边界交点处)
    // 直接返回 false
    if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    if (debugging)
      std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';

    if (rec1.t < ray_t.min) rec1.t = ray_t.min;
    if (rec2.t > ray_t.max) rec2.t = ray_t.max;

    // rec1 应该是入射光与边界的交点
    // 如果 rec1>=rec2 说明现在已经在物体内部,
    // 因此此时直接传出物体(把该物体当做透明)
    if (rec1.t >= rec2.t) return false;

    // rec1<0 说明光线起点在物体内部
    if (rec1.t < 0) rec1.t = 0;

    // ray_length 为光在单位时间内传播的距离
    auto ray_length = r.direction().length();

    // distance_inside_boundary 等于光线最终射出需要的耗时(表示物体的厚度)
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

    // 随机取一个 ray 的传播长度
    auto hit_distance = neg_inv_density * log(random_double());

    // 如果随机的传播长度 大于
    // 光线的最大能在物体内部的长度，就认为直接穿出物体(视物体为透明, 因此return
    // false)
    if (hit_distance > distance_inside_boundary) return false;

    // 更新光线向前传播的时间
    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
      std::clog << "hit_distance = " << hit_distance << '\n'
                << "rec.t = " << rec.t << '\n'
                << "rec.p = " << rec.p << '\n';
    }

    rec.normal = vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;       // also arbitrary
    rec.mat = phase_function;

    return true;
  }

  aabb bounding_box() const override { return boundary->bounding_box(); }

 private:
  shared_ptr<hittable> boundary;
  double neg_inv_density;
  shared_ptr<material> phase_function;
};

#endif