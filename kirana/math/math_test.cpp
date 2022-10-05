#include "math_utils.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix4x4.hpp"

#include <iostream>

using namespace kirana::math;
int main(int argc, char **argv)
{
    float angle1 = 45.0f;
    float angle2 = 60.0f;
    float angle3 = 275.0f;

    std::cout << "Angle: " << angle1 << " in radians: " << radians(angle1)
              << std::endl;
    std::cout << "Angle: " << angle2 << " in radians: " << radians(angle2)
              << std::endl;
    std::cout << "Angle: " << angle3 << " in radians: " << radians(angle3)
              << std::endl;

    float angle4 = PI;
    float angle5 = PI / 4.0f;
    float angle6 = 2 * PI;

    std::cout << "Angle: " << angle4 << " in degrees: " << degrees(angle4)
              << std::endl;
    std::cout << "Angle: " << angle5 << " in degrees: " << degrees(angle5)
              << std::endl;
    std::cout << "Angle: " << angle6 << " in degrees: " << degrees(angle6)
              << std::endl;

    Vector3 vec1(1.0f, 2.0f, 3.0f);
    Vector3 vec2, vec3;

    Vector3::coordinateFrame(vec1.normalize(), &vec2, &vec3);
    std::cout << "Co-ordinate frame: " << vec1.normalize() << " " << vec2 << " " << vec3
              << std::endl;
    return 0;
}