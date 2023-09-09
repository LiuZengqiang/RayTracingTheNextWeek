/**
 * @file bvh.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BVH_H
#define BVH_H
#include <algorithm>

#include "hittable.h"
#include "hittable_list.h"
#include "rtweekend.h"

// BVH(树/节点) 类
// 每个节点都有一个 bbox,
// 但是只有 叶子节点里面存放物体
// bvh_node 也是 hittable 的子类
// NOTE:如果场景为空怎么办？代码中没有给出处理
class bvh_node : public hittable {
 public:
  bvh_node(const hittable_list& list)
      : bvh_node(list.objects, 0, list.objects.size()) {}

  bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start,
           size_t end) {
    auto objects =
        src_objects;  // Create a modifiable array of the source scene objects

    // 随机选择一个对比轴
    int axis = random_int(0, 2);
    auto comparator = (axis == 0)   ? box_x_compare
                      : (axis == 1) ? box_y_compare
                                    : box_z_compare;

    size_t object_span = end - start;
    if (object_span == 1) {
      // 如果只有一个物体那么 left 和 right 都指向同一个 物体
      left = right = objects[start];
    } else if (object_span == 2) {
      // 如果有两个物体，那么 left 和 right 左右各一个
      if (comparator(objects[start], objects[start + 1])) {
        left = objects[start];
        right = objects[start + 1];
      } else {
        left = objects[start + 1];
        right = objects[start];
      }
    } else {
      // 先按照 某维度 将物体排序
      std::sort(objects.begin() + start, objects.begin() + end, comparator);

      auto mid = start + object_span / 2;
      // 前一半的物体放到 left 节点
      left = make_shared<bvh_node>(objects, start, mid);
      // 后一半的物体放到 right 节点
      right = make_shared<bvh_node>(objects, mid, end);
    }
    // 根据左右节点的 bbox 更新本节点的 bbox
    bbox = aabb(left->bounding_box(), right->bounding_box());
  }
  // 判断光线是否 与本 bvh 树中的某个物体在合法范围内相交
  // 如果相交返回true， 并将交点为信息存储在 rec 中
  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    // NOTE: 这里是 bbox 不是box 吧？
    if (!bbox.hit(r, ray_t)) return false;

    // 深度搜索(递归), 不断搜索节点 bvh_node 的孩子节点
    // 直到与node没有相交或者直到叶子节点
    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right =
        right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
  }

  aabb bounding_box() const override { return bbox; }

 private:
  shared_ptr<hittable> left;
  shared_ptr<hittable> right;
  aabb bbox;

  // 按照 某维度 进行比较的函数
  static bool box_compare(const shared_ptr<hittable> a,
                          const shared_ptr<hittable> b, int axis_index) {
    return a->bounding_box().axis(axis_index).min <
           b->bounding_box().axis(axis_index).min;
  }

  static bool box_x_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
  }
};

#endif