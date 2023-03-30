#ifndef KIRANA_SCENE_MATERIAL_PROPERTIES_HPP
#define KIRANA_SCENE_MATERIAL_PROPERTIES_HPP

#include "image.hpp"
#include "material_types.hpp"

namespace kirana::scene
{

class MaterialProperties
{
    friend class external::AssimpSceneConverter;

  public:
    MaterialProperties() = default;

    MaterialProperties(RasterPipelineData rasterData,
                       RaytracePipelineData raytraceData,
                       const std::vector<MaterialParameter> &parameters)
        : m_rasterData{std::move(rasterData)},
          m_raytraceData{std::move(raytraceData)}, m_parameters{parameters}
    {
        for (uint32_t i = 0; i < m_parameters.size(); i++)
            m_parameterIndices[m_parameters[i].id] = i;
    }

    ~MaterialProperties() = default;
    MaterialProperties(const MaterialProperties &properties) = default;
    MaterialProperties &operator=(const MaterialProperties &properties) =
        default;
    MaterialProperties(MaterialProperties &&properties) = default;
    MaterialProperties &operator=(MaterialProperties &&properties) = default;

    // Event-listeners

    inline uint32_t addOnParameterChangeEventListener(
        const std::function<void(const MaterialProperties &properties,
                                 const std::string &, const std::any &)>
            &callback) const
    {
        return m_onParameterChange.addListener(callback);
    }
    inline void removeOnParameterChangeEventListener(uint32_t callbackID) const
    {
        m_onParameterChange.removeListener(callbackID);
    }

    inline const RasterPipelineData &getRasterPipelineData() const
    {
        return m_rasterData;
    }

    inline const RaytracePipelineData &getRaytracePipelineData() const
    {
        return m_raytraceData;
    }

    bool setTextureParameter(const std::string &paramName, const Image &image)
    {
        if (m_parameterIndices.find(paramName) == m_parameterIndices.end())
            return false;
        m_textureInfos[paramName] = image;
        return true;
    }

    bool setParameter(const std::string &paramName, const std::any &value)
    {
        if (m_parameterIndices.find(paramName) == m_parameterIndices.end())
            return false;
        const uint32_t index = m_parameterIndices.at(paramName);
        if (value.type() != m_parameters[index].value.type())
            return false;
        m_parameters[index].value = value;
        m_onParameterChange(*this, paramName, m_parameters[index].value);
        return true;
    }

    template <typename T>
    inline T getParameter(const std::string &paramName) const
    {
        return m_parameterIndices.find(paramName) != m_parameterIndices.end()
                   ? std::any_cast<T>(
                         m_parameters[m_parameterIndices.at(paramName)].value)
                   : T{};
    }


    void getParametersData(std::vector<uint8_t> *dataBuffer) const
    {
        const auto &align = [](size_t size, size_t alignment) {
            return alignment > 0 ? (size + alignment - 1) & ~(alignment - 1)
                                 : size;
        };

        dataBuffer->resize(sizeof(uint8_t));

        size_t bufferSize = 0;
        for (const auto &p : m_parameters)
        {
            const int type = static_cast<int>(p.type);

            size_t alignment = 4;
            if (type > 6 && type < 10)
                alignment = 8;
            else if (type == 10)
                alignment = sizeof(math::Vector2);
            else if (type > 11)
                alignment = sizeof(math::Vector4);


            size_t bufferOffset = align(bufferSize, alignment);
            bufferSize = bufferOffset + alignment;
            dataBuffer->resize(bufferSize);

            void *bufferPtr =
                reinterpret_cast<void *>(dataBuffer->data() + bufferOffset);
            switch (p.type)
            {
            case MaterialParameterType::BOOL:
                memcpy(bufferPtr, std::any_cast<bool>(&p.value), sizeof(bool));
                break;
            case MaterialParameterType::INT:
                memcpy(bufferPtr, std::any_cast<int>(&p.value), sizeof(int));
                break;
            case MaterialParameterType::UINT:
                memcpy(bufferPtr, std::any_cast<uint32_t>(&p.value),
                       sizeof(uint32_t));
                break;
            case MaterialParameterType::FLOAT:
                memcpy(bufferPtr, std::any_cast<float>(&p.value),
                       sizeof(float));
                break;
            case MaterialParameterType::TEX_1D:
            case MaterialParameterType::TEX_2D:
            case MaterialParameterType::TEX_3D:
                memcpy(bufferPtr, std::any_cast<int>(&p.value), sizeof(int));
                break;
            case MaterialParameterType::INT64:
                memcpy(bufferPtr, std::any_cast<int64_t>(&p.value),
                       sizeof(int64_t));
                break;
            case MaterialParameterType::UINT64:
                memcpy(bufferPtr, std::any_cast<uint64_t>(&p.value),
                       sizeof(uint64_t));
                break;
            case MaterialParameterType::DOUBLE:
                memcpy(bufferPtr, std::any_cast<double>(&p.value),
                       sizeof(double));
                break;
            case MaterialParameterType::VEC_2:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector2>(&p.value)->data(),
                       sizeof(math::Vector2));
                break;
            case MaterialParameterType::VEC_3:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector3>(&p.value)->data(),
                       sizeof(math::Vector3));
                break;
            case MaterialParameterType::VEC_4:
                memcpy(bufferPtr,
                       std::any_cast<math::Vector4>(&p.value)->data(),
                       sizeof(math::Vector4));
                break;
            case MaterialParameterType::MAT_2x2:
                // TODO: Matrix 2x2 not yet implemented
                memcpy(
                    bufferPtr,
                    std::any_cast<std::array<std::array<float, 2>, 2>>(&p.value)
                        ->data(),
                    sizeof(float) * 4);
                break;
            case MaterialParameterType::MAT_3x3:
                // TODO: Matrix 3x3 not yet implemented
                memcpy(
                    bufferPtr,
                    std::any_cast<std::array<std::array<float, 3>, 3>>(&p.value)
                        ->data(),
                    sizeof(float) * 9);
                break;
            case MaterialParameterType::MAT_3x4:
                // TODO: Matrix 3x4 not yet implemented
                memcpy(
                    bufferPtr,
                    std::any_cast<std::array<std::array<float, 3>, 4>>(&p.value)
                        ->data(),
                    sizeof(float) * 12);
                break;
            case MaterialParameterType::MAT_4x4:
                memcpy(bufferPtr,
                       std::any_cast<math::Matrix4x4>(&p.value)->data(),
                       sizeof(math::Matrix4x4));
                break;
            }
        }
        bufferSize = align(bufferSize, 16);
        dataBuffer->resize(bufferSize);
    }

  protected:
    RasterPipelineData m_rasterData{};
    RaytracePipelineData m_raytraceData{};
    std::vector<MaterialParameter> m_parameters;
    std::unordered_map<std::string, uint32_t> m_parameterIndices;
    std::unordered_map<std::string, Image> m_textureInfos;

    mutable utils::Event<MaterialProperties, std::string, std::any>
        m_onParameterChange;
};
} // namespace kirana::scene
#endif