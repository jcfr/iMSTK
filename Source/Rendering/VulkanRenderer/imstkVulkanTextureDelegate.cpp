/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#include "imstkVulkanTextureDelegate.h"

namespace imstk
{
VulkanTextureDelegate::VulkanTextureDelegate(
    VulkanMemoryManager& memoryManager,
    std::shared_ptr<Texture> texture)
{
    m_path = texture->getPath();
    m_type = texture->getType();

    // Load textures and get texture information
    if ((texture->getType() != Texture::Type::IRRADIANCE_CUBEMAP)
        && (texture->getType() != Texture::Type::RADIANCE_CUBEMAP))
    {
        m_arrayLayers = 1;
        this->loadTexture(memoryManager);
        m_imageInfo.flags = 0;
    }
    else
    {
        m_arrayLayers = 6;
        this->loadCubemapTexture(memoryManager);
        m_imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        m_isCubemap = true;
    }

    // Determine number of mipmaps
    if (m_mipLevels < 1)
    {
        if (!texture->getMipmapsEnabled())
        {
            m_mipLevels = 1;
        }
        else
        {
            m_mipLevels = std::log2(std::max(m_width, m_height)) + 1;
        }
    }

    m_layout = VK_IMAGE_LAYOUT_GENERAL;

    m_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    m_imageInfo.pNext = nullptr;

    if (m_type == Texture::Type::DIFFUSE)
    {
        m_imageInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
    }
    else
    {
        m_imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    }

    m_imageInfo.imageType = VK_IMAGE_TYPE_2D;
    m_imageInfo.extent = { m_width, m_height, 1 };
    m_imageInfo.mipLevels = m_mipLevels;
    m_imageInfo.arrayLayers = m_arrayLayers;
    m_imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    m_imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    m_imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    m_imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    m_imageInfo.queueFamilyIndexCount = 1;
    m_imageInfo.pQueueFamilyIndices = &memoryManager.m_queueFamilyIndex;
    m_imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

    vkCreateImage(memoryManager.m_device, &m_imageInfo, nullptr, &m_image);

    m_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    m_range.baseMipLevel = 0;
    m_range.levelCount = m_mipLevels;
    m_range.baseArrayLayer = 0;
    m_range.layerCount = m_arrayLayers;

    if (m_isCubemap)
    {
        this->uploadCubemapTexture(memoryManager);
    }
    else
    {
        this->uploadTexture(memoryManager);
    }

    VkComponentMapping mapping;
    mapping.r = VK_COMPONENT_SWIZZLE_R;
    mapping.g = VK_COMPONENT_SWIZZLE_G;
    mapping.b = VK_COMPONENT_SWIZZLE_B;
    mapping.a = VK_COMPONENT_SWIZZLE_A;

    VkImageViewCreateInfo imageViewInfo;
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.pNext = nullptr;
    imageViewInfo.flags = 0;
    imageViewInfo.image = m_image;

    if (!m_isCubemap)
    {
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    }
    else
    {
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    }

    imageViewInfo.format = m_imageInfo.format;
    imageViewInfo.components = mapping;
    imageViewInfo.subresourceRange = m_range;

    vkCreateImageView(memoryManager.m_device, &imageViewInfo, nullptr, &m_imageView);

    VkSamplerCreateInfo samplerInfo;
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.flags = 0;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // Trilinear interpolation
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0;
    samplerInfo.anisotropyEnable = VK_FALSE; // TODO:: add option to enable
    samplerInfo.maxAnisotropy = 1.0;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = 0;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    vkCreateSampler(memoryManager.m_device, &samplerInfo, nullptr, &m_sampler);
}

void
VulkanTextureDelegate::loadTexture(VulkanMemoryManager& memoryManager)
{
    if (m_path != "")
    {
        auto readerGenerator = vtkSmartPointer<vtkImageReader2Factory>::New();
        auto reader = readerGenerator->CreateImageReader2(m_path.c_str());

        reader->SetFileName(m_path.c_str());
        reader->Update();

        auto data = reader->GetOutput();
        m_width = data->GetDimensions()[0];
        m_height = data->GetDimensions()[1];
        m_channels = reader->GetNumberOfScalarComponents();
        m_data = (unsigned char *)data->GetScalarPointer();
    }
    else
    {
        std::vector<unsigned char> data(4);
        data[0] = '\255';
        data[1] = '\255';
        data[2] = '\255';
        data[3] = '\255';
        m_width = 1;
        m_height = 1;
        m_data = &data[0];
    }
}

void
VulkanTextureDelegate::loadCubemapTexture(VulkanMemoryManager& memoryManager)
{
    if (m_path != "")
    {
        m_cubemap = gli::texture_cube(gli::load(m_path));

        m_width = m_cubemap.extent().x;
        m_height = m_cubemap.extent().y;
        m_mipLevels = (uint32_t)m_cubemap.levels();
    }
    else
    {
        m_cubemap = gli::texture_cube(gli::format::FORMAT_BGRA8_UNORM_PACK8, gli::extent2d(1,1), 1);
        m_width = 1;
        m_height = 1;
        m_mipLevels = 1;
    }
}

void
VulkanTextureDelegate::uploadTexture(VulkanMemoryManager& memoryManager)
{
    uint32_t imageSize = m_width * m_height * 4;

    // Staging image
    VkBufferCreateInfo stagingBufferInfo;
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.pNext = nullptr;
    stagingBufferInfo.flags = 0;
    stagingBufferInfo.size = imageSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    stagingBufferInfo.queueFamilyIndexCount = 0;
    stagingBufferInfo.pQueueFamilyIndices = nullptr;

    vkCreateBuffer(memoryManager.m_device, &stagingBufferInfo, nullptr, &m_stagingBuffer);

    VkMemoryRequirements memoryRequirementsStagingBuffer;
    vkGetBufferMemoryRequirements(memoryManager.m_device, m_stagingBuffer, &memoryRequirementsStagingBuffer);
    m_stagingBufferMemory = *memoryManager.allocateMemory(memoryRequirementsStagingBuffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryRequirements memoryRequirementsImage;
    vkGetImageMemoryRequirements(memoryManager.m_device, m_image, &memoryRequirementsImage);
    m_imageMemory = *memoryManager.allocateMemory(memoryRequirementsImage,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    void * imageData;
    vkMapMemory(memoryManager.m_device, m_stagingBufferMemory, 0, imageSize, 0, &imageData);

    auto imageEditData = (unsigned char*)imageData;

    unsigned int y_offset = 0;
    unsigned int colorChannels = std::min(m_channels, 3u);

    for (unsigned int y = 0; y < m_height; y++)
    {
        y_offset = y * m_width;
        for (unsigned int x = 0; x < m_width; x++)
        {
            // Fill in image data
            for (unsigned int z = 0; z < colorChannels; z++)
            {
                imageEditData[4 * (y_offset + x) + z] =
                    m_data[m_channels * (y_offset + x) + (colorChannels - z - 1)];
            }

            // Fill in the rest of the memory
            memset(&imageEditData[4 * (y_offset + x) + colorChannels],
                (unsigned char)255,
                (4 - colorChannels) * sizeof(unsigned char));
        }
    }

    vkUnmapMemory(memoryManager.m_device, m_stagingBufferMemory);

    // Start transfer commands
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(*memoryManager.m_transferCommandBuffer, &commandBufferBeginInfo);
    vkBindBufferMemory(memoryManager.m_device, m_stagingBuffer, m_stagingBufferMemory, 0);
    vkBindImageMemory(memoryManager.m_device, m_image, m_imageMemory, 0);

    VkImageSubresourceLayers layersDestination;
    layersDestination.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    layersDestination.mipLevel = 0;
    layersDestination.baseArrayLayer = 0;
    layersDestination.layerCount = 1;

    VkBufferImageCopy copyInfo;
    copyInfo.bufferOffset = 0;
    copyInfo.bufferRowLength = m_width;
    copyInfo.bufferImageHeight = m_height;
    copyInfo.imageSubresource = layersDestination;
    copyInfo.imageOffset = { 0, 0, 0 };
    copyInfo.imageExtent = { m_width, m_height, 1 };

    VkImageSubresourceRange baseRange = m_range;
    baseRange.levelCount = 1;

    this->changeImageLayout(*memoryManager.m_transferCommandBuffer, m_image,
        VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, m_range);
    vkCmdCopyBufferToImage(*memoryManager.m_transferCommandBuffer, m_stagingBuffer,
        m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

    if (m_mipLevels != 1)
    {
        this->generateMipmaps(*memoryManager.m_transferCommandBuffer);
    }

    this->changeImageLayout(*memoryManager.m_transferCommandBuffer, m_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, m_range);

    vkEndCommandBuffer(*memoryManager.m_transferCommandBuffer);

    VkCommandBuffer commandBuffers[] = { *memoryManager.m_transferCommandBuffer };

    VkPipelineStageFlags stageWaitFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo[1];
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].pNext = nullptr;
    submitInfo[0].waitSemaphoreCount = 0;
    submitInfo[0].pWaitSemaphores = nullptr;
    submitInfo[0].pWaitDstStageMask = &stageWaitFlags;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = commandBuffers;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    vkQueueSubmit(*memoryManager.m_transferQueue, 1, submitInfo, nullptr);
    vkDeviceWaitIdle(memoryManager.m_device);
}

void
VulkanTextureDelegate::uploadCubemapTexture(VulkanMemoryManager& memoryManager)
{
    uint32_t imageSize = (uint32_t)m_cubemap.size();

    // Staging image
    VkBufferCreateInfo stagingBufferInfo;
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.pNext = nullptr;
    stagingBufferInfo.flags = 0;
    stagingBufferInfo.size = imageSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    stagingBufferInfo.queueFamilyIndexCount = 0;
    stagingBufferInfo.pQueueFamilyIndices = nullptr;

    vkCreateBuffer(memoryManager.m_device, &stagingBufferInfo, nullptr, &m_stagingBuffer);

    VkMemoryRequirements memoryRequirementsStagingBuffer;
    vkGetBufferMemoryRequirements(memoryManager.m_device, m_stagingBuffer, &memoryRequirementsStagingBuffer);
    m_stagingBufferMemory = *memoryManager.allocateMemory(memoryRequirementsStagingBuffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryRequirements memoryRequirementsImage;
    vkGetImageMemoryRequirements(memoryManager.m_device, m_image, &memoryRequirementsImage);
    m_imageMemory = *memoryManager.allocateMemory(memoryRequirementsImage,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    void * imageData;
    vkMapMemory(memoryManager.m_device, m_stagingBufferMemory, 0, imageSize, 0, &imageData);

    memcpy(imageData, m_cubemap.data(), m_cubemap.size());

    vkUnmapMemory(memoryManager.m_device, m_stagingBufferMemory);

    // Start transfer commands
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(*memoryManager.m_transferCommandBuffer, &commandBufferBeginInfo);
    vkBindBufferMemory(memoryManager.m_device, m_stagingBuffer, m_stagingBufferMemory, 0);
    vkBindImageMemory(memoryManager.m_device, m_image, m_imageMemory, 0);

    std::vector<VkBufferImageCopy> copyInfos(m_mipLevels * m_arrayLayers);

    unsigned int currentOffset = 0;
    for (unsigned int layer = 0; layer < m_arrayLayers; layer++)
    {
        for (unsigned int level = 0; level < m_mipLevels; level++)
        {
            VkImageSubresourceLayers layersDestination;
            layersDestination.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            layersDestination.mipLevel = level;
            layersDestination.baseArrayLayer = layer;
            layersDestination.layerCount = 1;

            unsigned int currentRegion = layer * m_mipLevels + level;

            copyInfos[currentRegion].bufferOffset = currentOffset;
            copyInfos[currentRegion].bufferRowLength = m_cubemap[layer][level].extent().x;
            copyInfos[currentRegion].bufferImageHeight = m_cubemap[layer][level].extent().y;
            copyInfos[currentRegion].imageSubresource = layersDestination;
            copyInfos[currentRegion].imageOffset = { 0, 0, 0 };
            copyInfos[currentRegion].imageExtent = {
                m_cubemap[layer][level].extent().x,
                m_cubemap[layer][level].extent().y,
                1
            };
            currentOffset += (unsigned int)m_cubemap[layer][level].size();
        }
    }

    this->changeImageLayout(*memoryManager.m_transferCommandBuffer, m_image,
        VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, m_range);

    vkCmdCopyBufferToImage(*memoryManager.m_transferCommandBuffer, m_stagingBuffer,
        m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)copyInfos.size(), &copyInfos[0]);

    this->changeImageLayout(*memoryManager.m_transferCommandBuffer, m_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, m_range);

    vkEndCommandBuffer(*memoryManager.m_transferCommandBuffer);

    VkCommandBuffer commandBuffers[] = { *memoryManager.m_transferCommandBuffer };

    VkPipelineStageFlags stageWaitFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo[1];
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].pNext = nullptr;
    submitInfo[0].waitSemaphoreCount = 0;
    submitInfo[0].pWaitSemaphores = nullptr;
    submitInfo[0].pWaitDstStageMask = &stageWaitFlags;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = commandBuffers;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    vkQueueSubmit(*memoryManager.m_transferQueue, 1, submitInfo, nullptr);
    vkDeviceWaitIdle(memoryManager.m_device);
}

void
VulkanTextureDelegate::changeImageLayout(VkCommandBuffer& commandBuffer,
                                         VkImage& image,
                                         VkImageLayout layout1,
                                         VkImageLayout layout2,
                                         VkAccessFlags sourceFlags,
                                         VkAccessFlags destinationFlags,
                                         VkImageSubresourceRange range)
{
    VkImageMemoryBarrier layoutChange;
    layoutChange.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    layoutChange.pNext = nullptr;
    layoutChange.srcAccessMask = sourceFlags;
    layoutChange.dstAccessMask = destinationFlags;
    layoutChange.oldLayout = layout1;
    layoutChange.newLayout = layout2;
    layoutChange.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutChange.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutChange.image = image;
    layoutChange.subresourceRange = range;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &layoutChange);
}

void
VulkanTextureDelegate::generateMipmaps(VkCommandBuffer& commandBuffer)
{
    for (uint32_t i = 0; i < m_mipLevels - 1; i++)
    {
        VkImageSubresourceLayers sourceLayers;
        sourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        sourceLayers.mipLevel = i;
        sourceLayers.baseArrayLayer = 0;
        sourceLayers.layerCount = 1;

        VkImageSubresourceLayers destinationLayers;
        destinationLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        destinationLayers.mipLevel = i + 1;
        destinationLayers.baseArrayLayer = 0;
        destinationLayers.layerCount = 1;

        VkOffset3D sourceOffsets[2];
        sourceOffsets[0].x = 0;
        sourceOffsets[0].y = 0;
        sourceOffsets[0].z = 0;

        sourceOffsets[1].x = m_width / (1 << i);
        sourceOffsets[1].y = m_height / (1 << i);
        sourceOffsets[1].z = 1;

        VkOffset3D destinationOffsets[2];
        destinationOffsets[0].x = 0;
        destinationOffsets[0].y = 0;
        destinationOffsets[0].z = 0;

        destinationOffsets[1].x = m_width / (1 << (i + 1));
        destinationOffsets[1].y = m_height / (1 << (i + 1));
        destinationOffsets[1].z = 1;

        VkImageBlit mipFormat;
        mipFormat.srcSubresource = sourceLayers;
        mipFormat.srcOffsets[0] = sourceOffsets[0];
        mipFormat.srcOffsets[1] = sourceOffsets[1];
        mipFormat.dstSubresource = destinationLayers;
        mipFormat.dstOffsets[0] = destinationOffsets[0];
        mipFormat.dstOffsets[1] = destinationOffsets[1];

        VkImageSubresourceRange mipHighRange = m_range;
        mipHighRange.baseMipLevel = i;
        mipHighRange.levelCount = 1;

        VkImageSubresourceRange mipLowRange = m_range;
        mipLowRange.baseMipLevel = i + 1;
        mipLowRange.levelCount = 1;

        this->changeImageLayout(commandBuffer, m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, m_range);

        vkCmdBlitImage(commandBuffer,
            m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &mipFormat, VK_FILTER_LINEAR);

        this->changeImageLayout(commandBuffer, m_image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, m_range);
    }
}
}