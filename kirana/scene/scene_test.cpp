#include "material_properties.hpp"


using kirana::math::Vector4;
using kirana::scene::MaterialParameter;
using kirana::scene::MaterialParameterType;
using kirana::scene::MaterialProperties;


int main(int argc, char **argv)
{

    MaterialProperties properties{
        kirana::scene::RasterPipelineData{},
        kirana::scene::RaytracePipelineData{},
        std::vector<MaterialParameter>{
            {MaterialParameter{
                "_BaseColor", MaterialParameterType::VEC_4,
                kirana::math::Vector4(0.85f, 0.85f, 0.85f, 1.0f)}},
            {MaterialParameter{"_SubSurface", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Metallic", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Specular", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_SpecularTint", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Roughness", MaterialParameterType::FLOAT,
                               0.5f}},
            {MaterialParameter{"_Anisotropic", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Sheen", MaterialParameterType::FLOAT, 0.0f}},
            {MaterialParameter{"_SheenTint", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_ClearCoat", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_ClearCoatGloss", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Transmission", MaterialParameterType::FLOAT,
                               0.0f}},
            {MaterialParameter{"_Ior", MaterialParameterType::FLOAT, 1.0f}}}};

    Vector4 color{};
    float roughness = 0.0f;
    std::vector<uint8_t> sampleData;
    properties.getParametersData(&sampleData);

    memcpy(reinterpret_cast<void *>(&color), sampleData.data(),
           sizeof(Vector4));
    memcpy(&roughness, reinterpret_cast<char *>(sampleData.data()) + 40,
           sizeof(float));

    std::cout << "Size: " << sampleData.size() << std::endl;
    std::cout << "Color: " << color << std::endl;
    std::cout << "Roughness: " << roughness << std::endl;

    return 0;
}