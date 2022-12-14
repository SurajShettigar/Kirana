#include "acceleration_structure.hpp"

#include "device.hpp"
#include "command_pool.hpp"
#include "command_buffers.hpp"

#include <vk_mem_alloc.hpp>
#include "allocator.hpp"

#include "vulkan_utils.hpp"

#include <scene.hpp>


bool kirana::viewport::vulkan::AccelerationStructure::buildBLAS(
    vk::BuildAccelerationStructureFlagsKHR flags)
{
    m_BLASBuildData.resize(m_BLASInputData.size());

    vk::DeviceSize totalAccelStructSize = 0;
    vk::DeviceSize maxScratchBufferSize = 0;
    uint32_t numCompactions = 0;
    vk::DeviceSize batchAccelStructSize = 0;
    std::vector<std::vector<size_t>> batchBuildDataIndices;

    size_t batchIndex = 0;
    for (size_t i = 0; i < m_BLASInputData.size(); i++)
    {
        BuildASData bd;
        bd.buildInfo = vk::AccelerationStructureBuildGeometryInfoKHR{
            vk::AccelerationStructureTypeKHR::eBottomLevel,
            m_BLASInputData[i].flags | flags,
            vk::BuildAccelerationStructureModeKHR::eBuild,
            nullptr,
            nullptr,
            m_BLASInputData[i].geometries};
        bd.rangeInfo = &m_BLASInputData[i].offsets;

        std::vector<uint32_t> maxPrimCounts;
        for (const auto &o : m_BLASInputData[i].offsets)
            maxPrimCounts.push_back(o.primitiveCount);

        bd.sizeInfo = m_device->current.getAccelerationStructureBuildSizesKHR(
            vk::AccelerationStructureBuildTypeKHR::eDevice, bd.buildInfo,
            maxPrimCounts);

        totalAccelStructSize += bd.sizeInfo.accelerationStructureSize;
        maxScratchBufferSize =
            std::max(maxScratchBufferSize, bd.sizeInfo.buildScratchSize);
        numCompactions +=
            (bd.buildInfo.flags &
             vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction) ==
                    vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction
                ? 1
                : 0;

        m_BLASBuildData[i] = std::move(bd);

        // Batch the indices of build data based on size. This is done to ensure
        // the creation of BLAS (through command buffer) is split into chunks,
        // to avoid stalling of the pipeline.

        batchAccelStructSize +=
            m_BLASBuildData[i].sizeInfo.accelerationStructureSize;
        // Push index of buildAS input to batch
        if (batchBuildDataIndices.size() <= batchIndex)
            batchBuildDataIndices.push_back({i});
        else
            batchBuildDataIndices[batchIndex].push_back(i);

        // If the AS Size is greater than the batch limit, create a new batch
        if (batchAccelStructSize >=
            constants::VULKAN_ACCELERATION_STRUCTURE_BATCH_SIZE_LIMIT)
        {
            batchAccelStructSize = 0;
            ++batchIndex;
        }
    }
    AllocatedBuffer scratchBuffer;
    if (!m_allocator->allocateBuffer(
            maxScratchBufferSize,
            vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::eStorageBuffer,
            vma::MemoryUsage::eGpuOnly, &scratchBuffer, false))
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            "Failed to allocate scratch buffer for Acceleration Structure");
        return false;
    }
    const vk::DeviceAddress scratchBufferAddress =
        m_device->getBufferAddress(*scratchBuffer.buffer);

    // Query the compacted size of Acceleration Structure.
    vk::QueryPool compactionQueryPool = nullptr;
    if (numCompactions > 0 && numCompactions == m_BLASBuildData.size())
    {
        compactionQueryPool =
            m_device->current.createQueryPool(vk::QueryPoolCreateInfo(
                {}, vk::QueryType::eAccelerationStructureCompactedSizeKHR,
                numCompactions));
    }

    // Create command buffers to actually create the AS.
    const CommandBuffers *commandBuffers = nullptr;
    if (!m_commandPool->allocateCommandBuffers(commandBuffers,
                                               batchBuildDataIndices.size()))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to allocate command buffers to create "
                          "Acceleration Structure");
        return false;
    }
    bool commandBuffersBuilt = true;
    for (size_t bIdx = 0; bIdx < batchBuildDataIndices.size(); bIdx++)
    {
        const size_t buildDataFirstIndex = batchBuildDataIndices[bIdx][0];
        const size_t buildDataCount = batchBuildDataIndices[bIdx].size();
        if (compactionQueryPool)
            m_device->current.resetQueryPool(compactionQueryPool, 0,
                                             buildDataCount);

        size_t compactionIndex = 0;
        for (size_t i = buildDataFirstIndex;
             i < buildDataFirstIndex + buildDataCount; i++)
        {
            if (m_allocator->allocateBuffer(
                    m_BLASBuildData[i].sizeInfo.accelerationStructureSize,
                    vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
                        vk::BufferUsageFlagBits::eShaderDeviceAddress,
                    vma::MemoryUsage::eGpuOnly,
                    &m_BLASBuildData[i].accelStruct.buffer, false))
            {
                m_BLASBuildData[i].accelStruct.accelStruct =
                    m_device->current.createAccelerationStructureKHR(
                        vk::AccelerationStructureCreateInfoKHR(
                            {}, *m_BLASBuildData[i].accelStruct.buffer.buffer,
                            0,
                            m_BLASBuildData[i]
                                .sizeInfo.accelerationStructureSize,
                            vk::AccelerationStructureTypeKHR::eBottomLevel));

                m_BLASBuildData[i].buildInfo.dstAccelerationStructure =
                    m_BLASBuildData[i].accelStruct.accelStruct;
                m_BLASBuildData[i].buildInfo.scratchData.deviceAddress =
                    scratchBufferAddress;

                commandBuffers->begin(
                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit, bIdx);
                commandBuffers->buildAccelerationStructure(
                    m_BLASBuildData[i].buildInfo,
                    m_BLASBuildData[i].rangeInfo->data(), compactionQueryPool,
                    compactionIndex++, true, bIdx);
                commandBuffers->end(bIdx);
            }
            else
            {
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error,
                                  "Failed to allocate buffer for one of the "
                                  "acceleration structure");
                commandBuffersBuilt = false;
            }
        }
    }

    if (commandBuffersBuilt)
    {
        m_device->graphicsSubmit(commandBuffers->current);
        m_device->graphicsWait();

        if (compactionQueryPool)
        {
            // TODO: Implement Acceleration Structure Compaction.
            m_device->current.destroyQueryPool(compactionQueryPool);
        }
    }

    // Cleanup
    m_allocator->free(scratchBuffer);

    m_commandPool->reset();
    m_commandPool->freeCommandBuffers(commandBuffers);
    delete commandBuffers;
    commandBuffers = nullptr;

    return true;
}

kirana::viewport::vulkan::AccelerationStructure::AccelerationStructure(
    const Device *const device, const Allocator *const allocator,
    const std::unordered_map<std::string, MeshData> &meshes)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_commandPool{
          new CommandPool(m_device, m_device->queueFamilyIndices.graphics)}
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
    m_isInitialized = buildBLAS();
}

kirana::viewport::vulkan::AccelerationStructure::~AccelerationStructure()
{
    if (m_commandPool)
    {
        delete m_commandPool;
        m_commandPool = nullptr;
    }

    for (const auto &b : m_BLASBuildData)
    {
        m_allocator->free(b.accelStruct.buffer);
        m_device->current.destroyAccelerationStructureKHR(
            b.accelStruct.accelStruct);
    }
}