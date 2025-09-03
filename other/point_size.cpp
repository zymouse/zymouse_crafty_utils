#include <iostream>
#include <cstddef>  // for offsetof

struct PointXYZIN
{
  float x{0.0F};
  float y{0.0F};
  float z{0.0F};
  uint8_t id{0U};
  float intensity{0.0F};

  friend bool operator==(const PointXYZIN & p1, const PointXYZIN & p2) noexcept
  {
    return (p1.x == p2.x) && (p1.y == p2.y) &&
           (p1.z == p2.z) && (p1.intensity == p2.intensity) &&
           (p1.id == p2.id);
  }
};

int main()
{
  std::cout << "sizeof(PointXYZIN) = " << sizeof(PointXYZIN) << " bytes\n";
  std::cout << "sizeof(float)      = " << sizeof(float) << " bytes\n";
  std::cout << "sizeof(uint8_t)    = " << sizeof(uint8_t) << " bytes\n";

  std::cout << "Offset of x         = " << offsetof(PointXYZIN, x) << "\n";
  std::cout << "Offset of y         = " << offsetof(PointXYZIN, y) << "\n";
  std::cout << "Offset of z         = " << offsetof(PointXYZIN, z) << "\n";
  std::cout << "Offset of id        = " << offsetof(PointXYZIN, id) << "\n";
  std::cout << "Offset of intensity = " << offsetof(PointXYZIN, intensity) << "\n";

  return 0;
}

// 测试结构体大小 -- 内存对齐
// g++ -std=c++17 point_size.cpp -o point_size
