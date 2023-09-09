/**
 * @file texture.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
// 纹理类
class texture {
 public:
  virtual ~texture() = default;
  // 得到空间坐标位置为p, 纹理坐标为(u,v)处的颜色值
  virtual color value(double u, double v, const point3& p) const = 0;
};

// 一个简单的纹理类
// 该类只是单纯的颜色纹理
class solid_color : public texture {
 public:
  solid_color(color c) : color_value(c) {}

  solid_color(double red, double green, double blue)
      : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const point3& p) const override {
    return color_value;
  }

 private:
  color color_value;
};

// 这是一个网格纹理(checkered pattern)
class checker_texture : public texture {
 public:
  checker_texture(double _scale, shared_ptr<texture> _even,
                  shared_ptr<texture> _odd)
      : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

  checker_texture(double _scale, color c1, color c2)
      : inv_scale(1.0 / _scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3& p) const override {
    auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
    auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
    auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));
    // 得到一个网格类型的纹理
    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
  }

 private:
  double inv_scale;
  shared_ptr<texture> even;
  shared_ptr<texture> odd;
};
#endif