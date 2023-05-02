#include "frame_graph.hpp"

#include "vulkan_utils.hpp"

const kirana::viewport::vulkan::FrameGraph::ResourceHandle
    kirana::viewport::vulkan::FrameGraph::DEPENDENCY_RESOURCE_HANDLE{
        std::numeric_limits<uint32_t>::max() - 1};

const std::string
    kirana::viewport::vulkan::FrameGraph::DEPENDENCY_RESOURCE_NAME =
        "DEPENDENCY_RESOURCE";

kirana::viewport::vulkan::FrameGraph::ResourceHandle kirana::viewport::vulkan::
    FrameGraph::addOutputResource(const PassOutputResourceData &resourceData)
{
    if (m_resourceIndexTable.find(resourceData.name) !=
        m_resourceIndexTable.end())
        return ResourceHandle{};

    const ResourceHandle rHandle =
        static_cast<ResourceHandle>(m_resources.size());

    m_resources.emplace_back(Resource{rHandle, resourceData.name,
                                      resourceData.type, resourceData.info});
    m_resourceIndexTable[resourceData.name] = static_cast<uint32_t>(rHandle);
    return rHandle;
}

kirana::viewport::vulkan::FrameGraph::NodeHandle kirana::viewport::vulkan::
    FrameGraph::addNode(const PassData &passData)
{
    if (m_nodeIndexTable.find(passData.name) != m_nodeIndexTable.end())
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Pass: " + passData.name + " already exists");
        return NodeHandle{};
    }

    const NodeHandle nHandle = static_cast<NodeHandle>(m_nodes.size());

    Node node{nHandle, passData.name};
    // Set node-outputs
    for (const auto &o : passData.outputs)
    {
        const ResourceHandle rHandle = addOutputResource(o);
        if (!rHandle.isValid())
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Output: " + o.name +
                                  " for pass: " + passData.name +
                                  " is already being produced by some other "
                                  "pass");
            return NodeHandle{};
        }
        node.outputs.push_back(rHandle);
    }

    m_nodes.emplace_back(std::move(node));
    m_nodeIndexTable[passData.name] = static_cast<uint32_t>(nHandle);
    return nHandle;
}

bool kirana::viewport::vulkan::FrameGraph::addPasses(
    const std::vector<PassData> &passes)
{
    for (const auto &p : passes)
    {
        const NodeHandle nHandle = addNode(p);
        if (!nHandle.isValid())
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to add pass: " + p.name);
            return false;
        }
    }
    // Resolve node dependencies and resource inputs
    for (uint32_t passIdx = 0; passIdx < passes.size(); passIdx++)
    {
        Node &node = m_nodes[passIdx];
        // Add dependency inputs
        for (const auto &dPass : passes[passIdx].dependentPasses)
        {
            if (m_nodeIndexTable.find(dPass) == m_nodeIndexTable.end())
            {
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error,
                                  "Failed to find dependency pass: " + dPass +
                                      " for pass: " + node.name);
                return false;
            }
            node.inputs.emplace_back(
                NodeInput{DEPENDENCY_RESOURCE_HANDLE,
                          m_nodes.at(m_nodeIndexTable.at(dPass)).id});
        }
        // Add resource inputs
        for (const auto &i : passes[passIdx].inputs)
        {
            if (m_resourceIndexTable.find(i.name) == m_resourceIndexTable.end())
            {
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error,
                                  "Failed to find input resource: " + i.name +
                                      " for pass: " + node.name);
                return false;
            }
            if (m_nodeIndexTable.find(i.producer) == m_nodeIndexTable.end())
            {
                Logger::get().log(
                    constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                    "Failed to find input resource producer: " + i.producer +
                        " for pass: " + node.name);
                return false;
            }
            node.inputs.emplace_back(
                NodeInput{m_resources.at(m_resourceIndexTable.at(i.name)).id,
                          m_nodes.at(m_nodeIndexTable.at(i.producer)).id});
        }
    }
    return true;
}

bool kirana::viewport::vulkan::FrameGraph::compile()
{

}

void kirana::viewport::vulkan::FrameGraph::render()
{
}