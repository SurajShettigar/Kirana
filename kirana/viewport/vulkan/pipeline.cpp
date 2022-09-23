#include "pipeline.hpp"

#include "device.hpp"
#include "renderpass.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(const Device *const device,
                                             const RenderPass *const renderPass)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass}
{
}

kirana::viewport::vulkan::Pipeline::~Pipeline()
{
}