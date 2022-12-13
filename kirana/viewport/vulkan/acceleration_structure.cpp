#include "acceleration_structure.hpp"

#include "device.hpp"

#include <scene.hpp>


void kirana::viewport::vulkan::AccelerationStructure::buildBLAS()
{
    // TODO: Build Bottom-Level Acceleration Structure
}

kirana::viewport::vulkan::AccelerationStructure::AccelerationStructure(
    const Device *const device,
    const std::unordered_map<std::string, MeshData> &meshes)
    : m_device{device}
{
    for (const auto &m : meshes)
    {
        const vk::DeviceAddress &vertexBufferAdd =
            m_device->getBufferAddress(*m.second.vertexBuffer.buffer);
        const vk::DeviceAddress &indexBufferAdd =
            m_device->getBufferAddress(*m.second.indexBuffer.buffer);

        const vk::AccelerationStructureGeometryTrianglesDataKHR triangles{
            vk::Format::eR32G32B32Sfloat,
            vertexBufferAdd,
            sizeof(scene::Vertex),
            static_cast<uint32_t>(m.second.vertexCount),
            vk::IndexType::eUint32,
            indexBufferAdd,
            nullptr,
        };

        const vk::AccelerationStructureGeometryKHR geo{
            vk::GeometryTypeKHR::eTriangles, triangles,
            vk::GeometryFlagsKHR(vk::GeometryFlagBitsKHR::eOpaque)};

        const vk::AccelerationStructureBuildRangeInfoKHR offset{
            static_cast<uint32_t>(m.second.indexCount / 3), 0, 0, 0};

        // TODO: Add multiple geometries in a single BLAS.
        BLASData blasData;
        blasData.geometries.emplace_back(geo);
        blasData.offsets.emplace_back(offset);

        m_BLASInputData.emplace_back(std::move(blasData));
    }
    buildBLAS();
}