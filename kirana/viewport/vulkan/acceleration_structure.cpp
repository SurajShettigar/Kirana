#include "acceleration_structure.hpp"

#include "device.hpp"
#include "command_pool.hpp"
#include "command_buffers.hpp"
#include "scene_data.hpp"

#include <vk_mem_alloc.hpp>
#include "allocator.hpp"

#include "vulkan_utils.hpp"

#include <scene.hpp>

vk::TransformMatrixKHR kirana::viewport::vulkan::AccelerationStructure::
    getVulkanTransformMatrix(const math::Matrix4x4 &matrix)
{
    return vk::TransformMatrixKHR(
        {std::array<float, 4>{matrix[0][0], matrix[0][1], matrix[0][2],
                              matrix[0][3]},
         std::array<float, 4>{matrix[1][0], matrix[1][1], matrix[1][2],
                              matrix[1][3]},
         std::array<float, 4>{matrix[2][0], matrix[2][1], matrix[2][2],
                              matrix[2][3]}});
}

bool kirana::viewport::vulkan::AccelerationStructure::
    createAccelerationStructure(
        const vk::AccelerationStructureBuildSizesInfoKHR &sizeInfo,
        vk::AccelerationStructureTypeKHR type,
        ASData *accelerationStructure) const
{
    if (m_allocator->allocateBuffer(
            &(accelerationStructure->buffer),
            sizeInfo.accelerationStructureSize,
            vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
                vk::BufferUsageFlagBits::eShaderDeviceAddress,
            Allocator::AllocationType::GPU_READ_ONLY))
    {
        accelerationStructure->as =
            m_device->current.createAccelerationStructureKHR(
                vk::AccelerationStructureCreateInfoKHR(
                    {}, *(accelerationStructure->buffer.buffer), 0,
                    sizeInfo.accelerationStructureSize, type));
        return true;
    }
    return false;
}

vk::DeviceAddress kirana::viewport::vulkan::AccelerationStructure::
    getBLASAddress(uint32_t meshIndex) const
{
    return m_device->current.getAccelerationStructureAddressKHR(
        vk::AccelerationStructureDeviceAddressInfoKHR(
            m_BLASData[meshIndex].accelStruct.as));
}

void kirana::viewport::vulkan::AccelerationStructure::createBLAS(
    const SceneData &sceneData)
{
    for (const auto &m : sceneData.getSceneMeshes())
    {
        const vk::AccelerationStructureGeometryTrianglesDataKHR triangles{
            vk::Format::eR32G32B32A32Sfloat,
            sceneData.getVertexBufferAddress(m.vertexBufferIndex),
            sizeof(scene::Vertex),
            static_cast<uint32_t>(m.vertexCount),
            vk::IndexType::eUint32,
            sceneData.getIndexBufferAddress(m.indexBufferIndex),
            {},
        };

        const vk::AccelerationStructureGeometryKHR geo{
            vk::GeometryTypeKHR::eTriangles, triangles,
            vk::GeometryFlagsKHR(vk::GeometryFlagBitsKHR::eOpaque)};

        const vk::AccelerationStructureBuildRangeInfoKHR offset{
            static_cast<uint32_t>(m.indexCount / 3),
            static_cast<uint32_t>(m.firstIndex * sizeof(uint32_t)),
            static_cast<uint32_t>(m.vertexOffset), 0};

        // TODO: Add multiple geometries in a single BLAS.
        BLASData blasData{};
        blasData.geometries.emplace_back(geo);
        blasData.offsets.emplace_back(offset);

        m_BLASData.emplace_back(std::move(blasData));
    }
}

bool kirana::viewport::vulkan::AccelerationStructure::buildBLAS(
    vk::BuildAccelerationStructureFlagsKHR flags)
{
    vk::DeviceSize totalAccelStructSize = 0;
    vk::DeviceSize maxScratchBufferSize = 0;
    uint32_t numCompactions = 0;
    vk::DeviceSize batchAccelStructSize = 0;
    std::vector<std::vector<uint32_t>> batchBuildDataIndices;

    uint32_t batchIndex = 0;
    uint32_t blasIndex = 0;
    for (auto &b : m_BLASData)
    {
        b.buildInfo = vk::AccelerationStructureBuildGeometryInfoKHR{
            vk::AccelerationStructureTypeKHR::eBottomLevel,
            b.flags | flags,
            vk::BuildAccelerationStructureModeKHR::eBuild,
            nullptr,
            nullptr,
            b.geometries};

        std::vector<uint32_t> maxPrimCounts;
        for (const auto &o : b.offsets)
            maxPrimCounts.push_back(o.primitiveCount);

        b.sizeInfo = m_device->current.getAccelerationStructureBuildSizesKHR(
            vk::AccelerationStructureBuildTypeKHR::eDevice, b.buildInfo,
            maxPrimCounts);

        totalAccelStructSize += b.sizeInfo.accelerationStructureSize;
        maxScratchBufferSize =
            std::max(maxScratchBufferSize, b.sizeInfo.buildScratchSize);
        numCompactions +=
            (b.buildInfo.flags &
             vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction) ==
                    vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction
                ? 1
                : 0;

        // Batch the indices of build data based on size. This is done to
        // ensure the creation of BLAS (through command buffer) is split
        // into chunks, to avoid stalling of the pipeline.

        batchAccelStructSize += b.sizeInfo.accelerationStructureSize;
        // Push index of buildAS input to batch
        if (batchBuildDataIndices.size() <= batchIndex)
            batchBuildDataIndices.push_back({blasIndex});
        else
            batchBuildDataIndices[batchIndex].push_back(blasIndex);

        // If the AS Size is greater than the batch limit, create a new
        // batch
        if (batchAccelStructSize >=
            constants::VULKAN_ACCELERATION_STRUCTURE_BATCH_SIZE_LIMIT)
        {
            batchAccelStructSize = 0;
            ++batchIndex;
        }
        ++blasIndex;
    }

    AllocatedBuffer scratchBuffer;
    if (!m_allocator->allocateBuffer(
            &scratchBuffer, maxScratchBufferSize,
            vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::eStorageBuffer,
            Allocator::AllocationType::GPU_READ_ONLY))
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            "Failed to allocate scratch buffer for Acceleration Structure");
        return false;
    }

    m_device->setDebugObjectName(*scratchBuffer.buffer, "BLAS_ScratchBuffer");

    // Query the compacted size of Acceleration Structure.
    vk::QueryPool compactionQueryPool = nullptr;
    if (numCompactions > 0 && numCompactions == m_BLASData.size())
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
    for (uint32_t bIdx = 0; bIdx < batchBuildDataIndices.size(); bIdx++)
    {
        const uint32_t buildDataFirstIndex = batchBuildDataIndices[bIdx][0];
        const uint32_t buildDataCount =
            static_cast<uint32_t>(batchBuildDataIndices[bIdx].size());
        if (compactionQueryPool)
            m_device->current.resetQueryPool(compactionQueryPool, 0,
                                             buildDataCount);

        uint32_t compactionIndex = 0;
        commandBuffers->begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                              bIdx);
        for (uint32_t i = buildDataFirstIndex;
             i < buildDataFirstIndex + buildDataCount; i++)
        {
            if (createAccelerationStructure(
                    m_BLASData[i].sizeInfo,
                    vk::AccelerationStructureTypeKHR::eBottomLevel,
                    &m_BLASData[i].accelStruct))
            {
                m_BLASData[i].buildInfo.dstAccelerationStructure =
                    m_BLASData[i].accelStruct.as;
                m_BLASData[i].buildInfo.scratchData = m_device->alignSize(
                    static_cast<vk::DeviceSize>(scratchBuffer.address),
                    static_cast<vk::DeviceSize>(
                        m_device->accelStructProperties
                            .minAccelerationStructureScratchOffsetAlignment));
                commandBuffers->buildAccelerationStructure(
                    m_BLASData[i].buildInfo, m_BLASData[i].offsets.data(),
                    compactionQueryPool, compactionIndex++, true, bIdx);

                m_device->setDebugObjectName(m_BLASData[i].accelStruct.as,
                                             "BLAS_" + std::to_string(i));
                m_device->setDebugObjectName(
                    *m_BLASData[i].accelStruct.buffer.buffer,
                    "BLAS_" + std::to_string(i));
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
        commandBuffers->end(bIdx);
    }

    if (commandBuffersBuilt)
    {
        m_device->transferSubmit(commandBuffers->current);
        m_device->transferWait();

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

void kirana::viewport::vulkan::AccelerationStructure::createTLAS(
    const std::vector<MeshData> &meshes)
{
    for (const auto &m : meshes)
    {
        for (const auto &i : m.instances)
        {
            m_TLASInstanceData.emplace_back(
                vk::AccelerationStructureInstanceKHR{
                    getVulkanTransformMatrix(i.transform->getMatrix()),
                    m.getGlobalInstanceIndex(i.index),
                    static_cast<uint32_t>(m.render ? 0xFF : 0x00), 0,
                    vk::GeometryInstanceFlagBitsKHR::eTriangleCullDisable,
                    getBLASAddress(m.index)});
        }
    }
}

bool kirana::viewport::vulkan::AccelerationStructure::buildTLAS(
    vk::BuildAccelerationStructureFlagsKHR flags, bool update)
{
    // TODO: Add update mode support to update instance transforms
    if (!update && m_TLASData.buffer.buffer != nullptr)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                          "Top-Level Acceleration Structure already built");
        return false;
    }

    AllocatedBuffer TLASInstanceBuffer;
    size_t bufferSize = sizeof(vk::AccelerationStructureInstanceKHR) *
                        m_TLASInstanceData.size();
    if (!m_allocator->allocateBuffer(
            &TLASInstanceBuffer, bufferSize,
            vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::
                    eAccelerationStructureBuildInputReadOnlyKHR,
            Allocator::AllocationType::GPU_WRITEABLE, m_TLASInstanceData.data(),
            0, bufferSize))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                          "Failed to create buffer for Top-Level Acceleration "
                          "Structure instances");
        return false;
    }
    m_device->setDebugObjectName(*TLASInstanceBuffer.buffer,
                                 "TLAS_InstanceBuffer");

    const vk::DeviceAddress TLASInstanceBufferAddress =
        m_device->getBufferAddress(*TLASInstanceBuffer.buffer);

    const CommandBuffers *commandBuffers = nullptr;
    if (!m_commandPool->allocateCommandBuffers(commandBuffers))
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                          "Failed to allocate command buffers to create "
                          "Top-Level Acceleration Structures");
        return false;
    }

    vk::AccelerationStructureGeometryKHR geoInstances{
        vk::GeometryTypeKHR::eInstances,
        vk::AccelerationStructureGeometryInstancesDataKHR{
            {},
            TLASInstanceBufferAddress}};

    vk::AccelerationStructureBuildGeometryInfoKHR buildInfo{
        vk::AccelerationStructureTypeKHR::eTopLevel,
        flags,
        update ? vk::BuildAccelerationStructureModeKHR::eUpdate
               : vk::BuildAccelerationStructureModeKHR::eBuild,
        nullptr,
        nullptr,
        geoInstances};

    vk::AccelerationStructureBuildSizesInfoKHR sizeInfo{};

    sizeInfo = m_device->current.getAccelerationStructureBuildSizesKHR(
        vk::AccelerationStructureBuildTypeKHR::eDevice, buildInfo,
        static_cast<uint32_t>(m_TLASInstanceData.size()));

    if (createAccelerationStructure(
            sizeInfo, vk::AccelerationStructureTypeKHR::eTopLevel, &m_TLASData))
    {
        m_device->setDebugObjectName(m_TLASData.as, "TLAS");
        m_device->setDebugObjectName(*m_TLASData.buffer.buffer, "TLAS");

        AllocatedBuffer scratchBuffer;
        if (!m_allocator->allocateBuffer(
                &scratchBuffer,
                update ? sizeInfo.updateScratchSize : sizeInfo.buildScratchSize,
                vk::BufferUsageFlagBits::eStorageBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress,
                Allocator::AllocationType::WRITEABLE))
        {

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::info,
                              "Failed to allocate scratch buffer to create "
                              "Top-Level Acceleration Structures");
            return false;
        }
        m_device->setDebugObjectName(*scratchBuffer.buffer,
                                     "TLAS_ScratchBuffer");
        const vk::DeviceAddress scratchBufferAddress =
            m_device->getBufferAddress(*scratchBuffer.buffer);

        buildInfo.dstAccelerationStructure = m_TLASData.as;
        buildInfo.scratchData = m_device->alignSize(
            static_cast<vk::DeviceSize>(scratchBufferAddress),
            static_cast<vk::DeviceSize>(
                m_device->accelStructProperties
                    .minAccelerationStructureScratchOffsetAlignment));

        const vk::AccelerationStructureBuildRangeInfoKHR rangeInfo{
            static_cast<uint32_t>(m_TLASInstanceData.size()), 0, 0, 0};
        vk::QueryPool compactionPool = nullptr;

        commandBuffers->begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffers->buildAccelerationStructure(buildInfo, &rangeInfo,
                                                   compactionPool, 0, false);
        commandBuffers->end();


        m_device->transferSubmit(commandBuffers->current);
        m_device->transferWait();
        m_allocator->free(scratchBuffer);
    }

    m_allocator->free(TLASInstanceBuffer);
    m_commandPool->reset();
    m_commandPool->freeCommandBuffers(commandBuffers);

    return true;
}

kirana::viewport::vulkan::AccelerationStructure::AccelerationStructure(
    const Device *const device, const Allocator *const allocator,
    const SceneData &sceneData)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_commandPool{
          new CommandPool(m_device, m_device->queueFamilyIndices.transfer)}
{
    m_commandFence = m_device->current.createFence(vk::FenceCreateInfo{});
    createBLAS(sceneData);
    m_isInitialized = buildBLAS();
    if (m_isInitialized)
    {
        createTLAS(sceneData.getSceneMeshes());
        m_isInitialized = buildTLAS();
    }

    if (m_isInitialized)
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Raytrace Acceleration Structure created");
    else
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Acceleration Structure");
}

kirana::viewport::vulkan::AccelerationStructure::~AccelerationStructure()
{
    if (m_TLASData.buffer.buffer)
    {
        m_allocator->free(m_TLASData.buffer);
        m_device->current.destroyAccelerationStructureKHR(m_TLASData.as);
    }

    for (const auto &b : m_BLASData)
    {
        m_allocator->free(b.accelStruct.buffer);
        m_device->current.destroyAccelerationStructureKHR(b.accelStruct.as);
    }

    if (m_commandFence)
    {
        m_device->current.destroyFence(m_commandFence);
    }

    if (m_commandPool)
    {
        delete m_commandPool;
        m_commandPool = nullptr;
    }
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Acceleration Structure destroyed");
}