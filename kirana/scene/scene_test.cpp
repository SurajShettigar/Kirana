#include "material_types.hpp"


using kirana::math::Vector4;
using kirana::scene::MaterialParameter;
using kirana::scene::MaterialParameterType;
using kirana::scene::MaterialProperties;


int main(int argc, char **argv)
{

    MaterialProperties properties{};
    properties.parameters = std::unordered_map<std::string, MaterialParameter>{
        {"_BaseColor",
         MaterialParameter{"_BaseColor", MaterialParameterType::VEC_4,
                           kirana::math::Vector4(0.85f, 0.85f, 0.85f, 1.0f)}},
        {"_SubSurface",
         MaterialParameter{"_SubSurface", MaterialParameterType::FLOAT, 0.0f}},
        {"_Metallic",
         MaterialParameter{"_Metallic", MaterialParameterType::FLOAT, 0.0f}},
        {"_Specular",
         MaterialParameter{"_Specular", MaterialParameterType::FLOAT, 0.0f}},
        {"_SpecularTint",
         MaterialParameter{"_SpecularTint", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Roughness",
         MaterialParameter{"_Roughness", MaterialParameterType::FLOAT, 0.5f}},
        {"_Anisotropic",
         MaterialParameter{"_Anisotropic", MaterialParameterType::FLOAT, 0.0f}},
        {"_Sheen",
         MaterialParameter{"_Sheen", MaterialParameterType::FLOAT, 0.0f}},
        {"_SheenTint",
         MaterialParameter{"_SheenTint", MaterialParameterType::FLOAT, 0.0f}},
        {"_ClearCoat",
         MaterialParameter{"_ClearCoat", MaterialParameterType::FLOAT, 0.0f}},
        {"_ClearCoatGloss",
         MaterialParameter{"_ClearCoatGloss", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Transmission",
         MaterialParameter{"_Transmission", MaterialParameterType::FLOAT,
                           0.0f}},
        {"_Ior",
         MaterialParameter{"_Ior", MaterialParameterType::FLOAT, 1.0f}}};

    Vector4 color{};
    float roughness;

    std::vector<uint8_t> sampleData;
    properties.getParametersData(&sampleData);

    memcpy(reinterpret_cast<void *>(&color), sampleData.data(), sizeof(Vector4));
    memcpy(&roughness, reinterpret_cast<char *>(sampleData.data()) + 40, sizeof(float));

    std::cout << "Size: " << sampleData.size() << std::endl;
    std::cout << "Color: " << color << std::endl;
    std::cout << "Roughness: " << roughness << std::endl;

    return 0;
}