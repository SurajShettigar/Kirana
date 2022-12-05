#include "math_utils.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix4x4.hpp"
#include "transform.hpp"
#include "bounds3.hpp"
#include "bounds2.hpp"
#include "ray.hpp"

#include <iostream>

using namespace kirana::math;
int main(int argc, char **argv)
{
    std::cout << "Size of Vector2: " << sizeof(Vector2) << std::endl;
    std::cout << "Size of Vector3: " << sizeof(Vector3) << std::endl;
    std::cout << "Size of Vector4: " << sizeof(Vector4) << std::endl;
    std::cout << "Size of Matrix4x4: " << sizeof(Matrix4x4) << std::endl;
    std::cout << "Size of Transform: " << sizeof(Transform) << std::endl;

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
    vec1.normalize();
    Vector3 vec2, vec3;

    Vector3::coordinateFrame(vec1, &vec2, &vec3);
    std::cout << "Co-ordinate frame: " << vec1 << " " << vec2 << " " << vec3
              << std::endl;

    Matrix4x4 mat1;
    Matrix4x4 mat2(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -2.0f, 0.0f, 0.0f,
                   1.0f, 3.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    std::cout << "Matrix Mul: " << (mat1 * mat2) << std::endl;
    std::cout << "Matrix Inverse: " << Matrix4x4::inverse((mat1 * mat2))
              << std::endl;

    // Bounding Box related functions
    Bounds3 bounds(Vector3{-0.5f, -0.5f, -0.6f}, Vector3{1.5f, 0.5f, 0.6f});
    std::cout << "Bounds: " << bounds << std::endl;
    std::cout << "Bounds Center: " << bounds.getCenter()
              << " Size: " << bounds.getSize() << std::endl;
    Transform transform;
//    transform.setPosition(Vector3{0.5f, 2.0f, 0.0f});
    transform.rotateY(90.0f);
//    transform.setLocalScale(Vector3{1.0f, 2.0f, 1.5f});
    bounds = transform.transformBounds(bounds);
    std::cout << "Transformed Bounds: " << bounds << std::endl;
    std::cout << "Transformed Bounds Center: " << bounds.getCenter()
              << " Size: " << bounds.getSize() << std::endl;


    // Ray-related functions
    Ray ray(Vector3{0.0f, 0.1f, 2.0f}, Vector3{0.0f, 0.1f, -1.0f});

    std::cout << "Ray: " << ray << std::endl;

    Vector3 enterPoint, exitPoint;
    std::cout << "Intersection: "
              << bounds.intersectWithRay(ray, &enterPoint, &exitPoint)
              << std::endl;
    std::cout << "Intersection Points: " << enterPoint << ", " << exitPoint
              << std::endl;

    return 0;
}