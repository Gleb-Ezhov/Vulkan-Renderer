#pragma once

#include "../Pipeline.hpp"
#include "../Device.hpp"
#include "../Renderer.hpp"
#include "../SceneObject.hpp"
#include "../Camera.hpp"
#include "../FrameInfo.hpp"
#include "../SwapChain.hpp"
#include "../Descriptors.hpp"

// std
#include <memory>
#include <vector>

class TextureRenderSystem
{
public:
    TextureRenderSystem(WrpDevice& device, WrpRenderer& renderer, VkDescriptorSetLayout globalSetLayout, FrameInfo frameInfo);
    ~TextureRenderSystem();

    TextureRenderSystem(const TextureRenderSystem&) = delete;
    TextureRenderSystem& operator=(const TextureRenderSystem&) = delete;

    void renderSceneObjects(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);
    void createUboBuffers();

    int fillModelsIds(SceneObject::Map& sceneObjects);
    void createDescriptorSets(FrameInfo& frameInfo);
    void rewriteAndRecompileFragShader(int texturesCount);

    WrpDevice& wrpDevice;
    WrpRenderer& wrpRenderer;
    VkDescriptorSetLayout globalSetLayout;

    VkShaderModule fragShaderModule = nullptr;
    std::unique_ptr<WrpPipeline> wrpPipeline;
    VkPipelineLayout pipelineLayout = nullptr;

    std::vector<SceneObject::id_t> modelObjectsIds{};
    size_t prevModelCount = 0;

    std::unique_ptr<WrpDescriptorPool> systemDescriptorPool;
    std::unique_ptr<WrpDescriptorSetLayout> systemDescriptorSetLayout;
    std::vector<VkDescriptorSet> systemDescriptorSets{WrpSwapChain::MAX_FRAMES_IN_FLIGHT};
};
