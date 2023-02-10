#include "shader_binding_table.hpp"

#include "device.hpp"
#include "raytrace_pipeline.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "vulkan_utils.hpp"

void kirana::viewport::vulkan::ShaderBindingTable::initializeShaderRecords()
{
    const auto& shaderStages = m_pipeline->getShaderStagesInfo();
    const auto& shaderGroups = m_pipeline->getShaderGroupsInfo();

    const uint32_t rayGenIndex = static_cast<int>(GroupType::RAY_GEN);
    const uint32_t missIndex = static_cast<int>(GroupType::MISS);
    const uint32_t hitIndex = static_cast<int>(GroupType::HIT);
    const uint32_t callableIndex = static_cast<int>(GroupType::CALLABLE);

    // TODO: Calculate record indices and count when using pipeline libraries.

    // Set record indices from the created raytracing pipeline
    for (uint32_t i = 0; i < shaderGroups.size(); i++)
    {
        if (shaderGroups[i].type ==
            vk::RayTracingShaderGroupTypeKHR::eGeneral)
        {
            switch (shaderStages[i].stage)
            {
            case vk::ShaderStageFlagBits::eRaygenKHR:
                m_groups[rayGenIndex].recordIndices.push_back(i);
                break;
            case vk::ShaderStageFlagBits::eMissKHR:
                m_groups[missIndex].recordIndices.push_back(i);
                break;
            case vk::ShaderStageFlagBits::eCallableKHR:
                m_groups[callableIndex].recordIndices.push_back(i);
                break;
            default:
                break;
            }
        }
        else
            m_groups[hitIndex].recordIndices.push_back(i);
    }

    for (int i = 0; i < static_cast<int>(GroupType::GROUP_TYPE_MAX); i++)
    {
        m_groups.at(i).type = static_cast<GroupType>(i);

        m_groups.at(i).recordCount =
            static_cast<uint32_t>(m_groups.at(i).recordIndices.size());

        // TODO: Add support for storing shader resource in SBT.
        if (m_groups.at(i).type == GroupType::RAY_GEN)
        {
            // Ray-Gen shader needs to be base-aligned
            m_groups.at(i).stride = m_device->alignSize(
                m_handleSizeAligned,
                m_device->raytracingProperties.shaderGroupBaseAlignment);
        }
        else
            m_groups.at(i).stride = m_handleSizeAligned;

        m_groups.at(i).size =
            m_groups.at(i).stride * m_groups.at(i).recordCount;
    }

    m_totalGroupCount = shaderGroups.size();
}

std::array<std::vector<uint8_t>,
           static_cast<int>(kirana::viewport::vulkan::ShaderBindingTable::
                                GroupType::GROUP_TYPE_MAX)>
kirana::viewport::vulkan::ShaderBindingTable::getBufferData() const
{
    // Get Shader Group Handles.
    const size_t dataSize = static_cast<size_t>(m_totalGroupCount) *
                            static_cast<size_t>(m_handleSize);
    const std::vector<uint8_t> shaderGroupHandles =
        m_device->current.getRayTracingShaderGroupHandlesKHR<uint8_t>(
            m_pipeline->current, 0, m_totalGroupCount, dataSize);

    std::array<std::vector<uint8_t>,
               static_cast<int>(GroupType::GROUP_TYPE_MAX)>
        bufferData;

    // TODO: Copy shader resource into SBT buffer along with handle.
    for (int g = 0; g < static_cast<int>(GroupType::GROUP_TYPE_MAX); g++)
    {
        bufferData.at(g) = std::vector<uint8_t>(m_groups.at(g).size);
        uint8_t *bufferPtr = bufferData.at(g).data();
        for (const uint32_t r : m_groups.at(g).recordIndices)
        {
            uint8_t *bufferStartPtr = bufferPtr;
            memcpy(bufferPtr,
                   shaderGroupHandles.data() +
                       (static_cast<size_t>(r) *
                        static_cast<size_t>(m_handleSize)),
                   static_cast<size_t>(m_handleSize));
            bufferPtr = bufferStartPtr + m_groups.at(g).stride;
        }
    }
    return bufferData;
}

kirana::viewport::vulkan::ShaderBindingTable::ShaderBindingTable(
    const Device *const device, const Allocator *const allocator,
    const RaytracePipeline *const pipeline)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_pipeline{pipeline}, m_name{m_pipeline->name},
      m_handleSize{m_device->raytracingProperties.shaderGroupHandleSize},
      m_handleSizeAligned{m_device->alignSize(
          m_handleSize,
          m_device->raytracingProperties.shaderGroupHandleAlignment)}
{
    // Logic inspired from Nvidia's SBT wrapper in nvpro-samples raytracing
    // tutorial.
    // Link:
    // https://github.com/nvpro-samples/nvpro_core/blob/master/nvvk/sbtwrapper_vk.cpp

    initializeShaderRecords();

    std::array<std::vector<uint8_t>,
               static_cast<int>(GroupType::GROUP_TYPE_MAX)>
        bufferData = getBufferData();

    for (int g = 0; g < static_cast<int>(GroupType::GROUP_TYPE_MAX); g++)
    {
        if(m_groups.at(g).size == 0)
            continue ;
        if (!m_allocator->allocateBuffer(
                &m_groups.at(g).buffer, m_groups.at(g).size,
                vk::BufferUsageFlagBits::eShaderBindingTableKHR |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress,
                Allocator::AllocationType::GPU_WRITEABLE,
                bufferData.at(g).data(), 0,
                m_groups.at(g).size))
        {
            Logger::get().log(
                constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                "Failed to allocate buffer for Shader Binding Table");
            return;
        }
        m_groups.at(g).bufferAddress =
            m_device->getBufferAddress(*m_groups.at(g).buffer.buffer);
        m_device->setDebugObjectName(
            *m_groups.at(g).buffer.buffer,
            "SBT_" + m_name + "_" + static_cast<std::string>(m_groups.at(g)));
    }

    m_isInitialized = true;

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Binding Table created");
}

kirana::viewport::vulkan::ShaderBindingTable::~ShaderBindingTable()
{
    for (auto &g : m_groups)
    {
        if(g.buffer.buffer)
            m_allocator->free(g.buffer);
    }

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Shader Binding Table destroyed");
}