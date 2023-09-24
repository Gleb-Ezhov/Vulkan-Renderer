#pragma once

#include "../Pipeline.hpp"
#include "../Device.hpp"
#include "../GameObject.hpp"
#include "../Camera.hpp"
#include "../FrameInfo.hpp"

// std
#include <memory>
#include <vector>

ENGINE_BEGIN

class SimpleRenderSystem
{
public:
    SimpleRenderSystem(WrpDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    WrpDevice& wrpDevice;

    std::unique_ptr<WrpPipeline> wrpPipeline;
    VkPipelineLayout pipelineLayout;
};

ENGINE_END