
#pragma once
#include "VulkanDevice.h"

// Created on 2024-11-07 by sisco

namespace lyra
{
    inline bool IsExtensionInExtensionProperties(StringView extension, const Vector<vk::ExtensionProperties>& extensionProperties)
    {
        return (extensionProperties.end() != std::ranges::find_if(extensionProperties, [&extension](auto& val) { return val.extensionName == extension;}));
    }
    inline void EnumerateAllExtensionsAndFeatures(vk::PhysicalDevice& device, Vector<const char*>& extensions, Vector<const char*>& features, const Vector<vk::ExtensionProperties>& extensionProperties)
    {

        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        extensions.push_back(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME);
        
        auto deviceFeatures2 = device.getFeatures2<
            vk::PhysicalDeviceFeatures2,
        vk::PhysicalDevice16BitStorageFeatures,
        vk::PhysicalDevice8BitStorageFeaturesKHR,
        vk::PhysicalDeviceASTCDecodeFeaturesEXT,
        vk::PhysicalDeviceBlendOperationAdvancedFeaturesEXT,
        vk::PhysicalDeviceBufferDeviceAddressFeaturesEXT,
        vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
        vk::PhysicalDeviceComputeShaderDerivativesFeaturesNV,
        vk::PhysicalDeviceConditionalRenderingFeaturesEXT,
        vk::PhysicalDeviceCooperativeMatrixFeaturesNV,
        vk::PhysicalDeviceCornerSampledImageFeaturesNV,
        vk::PhysicalDeviceCoverageReductionModeFeaturesNV,
        vk::PhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV,
        vk::PhysicalDeviceDepthClipEnableFeaturesEXT,
        vk::PhysicalDeviceDescriptorIndexingFeaturesEXT,
        vk::PhysicalDeviceExclusiveScissorFeaturesNV,
        vk::PhysicalDeviceFragmentDensityMapFeaturesEXT,
        vk::PhysicalDeviceFragmentShaderBarycentricFeaturesNV,
        vk::PhysicalDeviceFragmentShaderInterlockFeaturesEXT,
        vk::PhysicalDeviceHostQueryResetFeaturesEXT,
        vk::PhysicalDeviceImagelessFramebufferFeaturesKHR,
        vk::PhysicalDeviceIndexTypeUint8FeaturesEXT,
        vk::PhysicalDeviceInlineUniformBlockFeaturesEXT,
        vk::PhysicalDeviceLineRasterizationFeaturesEXT,
        vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
        vk::PhysicalDeviceMeshShaderFeaturesNV,
        vk::PhysicalDeviceMultiviewFeatures,
        vk::PhysicalDevicePipelineExecutablePropertiesFeaturesKHR,
        vk::PhysicalDeviceProtectedMemoryFeatures,
        vk::PhysicalDeviceRepresentativeFragmentTestFeaturesNV,
        vk::PhysicalDeviceSamplerYcbcrConversionFeatures,
        vk::PhysicalDeviceScalarBlockLayoutFeaturesEXT,
        vk::PhysicalDeviceShaderAtomicInt64FeaturesKHR,
        vk::PhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT,
        vk::PhysicalDeviceShaderDrawParametersFeatures,
        vk::PhysicalDeviceShaderFloat16Int8FeaturesKHR,
        vk::PhysicalDeviceShaderImageFootprintFeaturesNV,
        vk::PhysicalDeviceShaderIntegerFunctions2FeaturesINTEL,
        vk::PhysicalDeviceShaderSMBuiltinsFeaturesNV,
        vk::PhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR,
        vk::PhysicalDeviceShadingRateImageFeaturesNV,
        vk::PhysicalDeviceSubgroupSizeControlFeaturesEXT,
        vk::PhysicalDeviceTexelBufferAlignmentFeaturesEXT,
        vk::PhysicalDeviceTextureCompressionASTCHDRFeaturesEXT,
        vk::PhysicalDeviceTimelineSemaphoreFeaturesKHR,
        vk::PhysicalDeviceTransformFeedbackFeaturesEXT,
        vk::PhysicalDeviceUniformBufferStandardLayoutFeaturesKHR,
        vk::PhysicalDeviceVariablePointersFeatures,
        vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT,
        vk::PhysicalDeviceVulkanMemoryModelFeaturesKHR,
        vk::PhysicalDeviceYcbcrImageArraysFeaturesEXT,
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
        vk::PhysicalDeviceRayQueryFeaturesKHR,
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
        vk::PhysicalDeviceRobustness2FeaturesEXT,
        vk::PhysicalDeviceShaderClockFeaturesKHR,
        vk::PhysicalDeviceVulkan12Features,
        vk::PhysicalDeviceFragmentShadingRateFeaturesKHR,
        vk::PhysicalDeviceRayTracingInvocationReorderFeaturesNV,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
        vk::PhysicalDeviceShaderImageAtomicInt64FeaturesEXT,
        vk::PhysicalDeviceShaderAtomicFloat2FeaturesEXT,
        vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT
        >
        
        ();

        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderResourceMinLod)
        {
            features.push_back("shader-resource-min-lod");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderFloat64)
        {
            features.push_back("double");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderInt64)
        {
            features.push_back("int64");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderInt16)
        {
            features.push_back("int16");
        }
        // If we have float16 features then enable
        if (deviceFeatures2.get<vk::PhysicalDeviceShaderFloat16Int8Features>().shaderFloat16)
        {
            // We have half support
            features.push_back("half");
        }

        const auto addFeatureExtension =
            [&](const bool feature, const char* extension = nullptr)
        {
            if (!feature)
                return false;
            if (extension)
            {
                if(!IsExtensionInExtensionProperties(extension, extensionProperties))
                    return false;
                extensions.push_back(extension);
            }
            return true;
        };

        // SIMPLE_EXTENSION_FEATURE(struct, feature member name, extension
        // name, features...) will check for the presence of the boolean
        // feature member in struct and the availability of the extensions. If
        // they are both present then the extensions are added, the struct
        // linked into the deviceCreateInfo chain and the features added to the
        // supported features list.
#define SIMPLE_EXTENSION_FEATURE(s, m, e, ...) \
    do                                         \
    {                                          \
        const static auto fs = {__VA_ARGS__};  \
        if (addFeatureExtension(deviceFeatures2.get<s>().m, e))    \
            for (const auto& p : fs)           \
                features.push_back(p);             \
    } while (0)

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDevice16BitStorageFeatures,
            storageBuffer16BitAccess,
            VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
            "16-bit-storage");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT,
            shaderBufferFloat32Atomics,
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            "atomic-float");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderAtomicFloat2FeaturesEXT,
            shaderBufferFloat16Atomics,
            VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME,
            "atomic-float-2");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderImageAtomicInt64FeaturesEXT,
            shaderImageInt64Atomics,
            VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
            "image-atomic-int64");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
            extendedDynamicState,
            VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
            "extended-dynamic-states");

        if (deviceFeatures2.get<vk::PhysicalDeviceAccelerationStructureFeaturesKHR>().accelerationStructure &&
            IsExtensionInExtensionProperties(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, extensionProperties) &&
            IsExtensionInExtensionProperties(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
            features.push_back("acceleration-structure");

            // These both depend on VK_KHR_acceleration_structure

            SIMPLE_EXTENSION_FEATURE(
                vk::PhysicalDeviceRayQueryFeaturesKHR,
                rayQuery,
                VK_KHR_RAY_QUERY_EXTENSION_NAME,
                "ray-query",
                "ray-tracing");

            SIMPLE_EXTENSION_FEATURE(
                vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
                rayTracingPipeline,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                "ray-tracing-pipeline");
        }

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceInlineUniformBlockFeaturesEXT ,
            inlineUniformBlock,
            VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
            "inline-uniform-block", );

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceRobustness2FeaturesEXT,
            nullDescriptor,
            VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
            "robustness2", );

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderClockFeaturesKHR,
            shaderDeviceClock,
            VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
            "realtime-clock");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceMeshShaderFeaturesNV,
            meshShader,
            VK_EXT_MESH_SHADER_EXTENSION_NAME,
            "mesh-shader");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceMultiviewFeatures,
            multiview,
            VK_KHR_MULTIVIEW_EXTENSION_NAME,
            "multiview");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceFragmentShadingRateFeaturesKHR,
            primitiveFragmentShadingRate,
            VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
            "fragment-shading-rate");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceRayTracingInvocationReorderFeaturesNV,
            rayTracingInvocationReorder,
            VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
            "shader-execution-reorder");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceVariablePointersFeatures,
            variablePointers,
            VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
            "variable-pointer");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceComputeShaderDerivativesFeaturesNV,
            computeDerivativeGroupLinear,
            VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,
            "computeDerivativeGroupLinear");

#undef SIMPLE_EXTENSION_FEATURE

        if (deviceFeatures2.get<vk::PhysicalDeviceShaderAtomicInt64Features>().shaderBufferInt64Atomics)
            features.push_back("atomic-int64");

        if (deviceFeatures2.get<vk::PhysicalDeviceTimelineSemaphoreFeatures>().timelineSemaphore)
            features.push_back("timeline-semaphore");

        if (deviceFeatures2.get<vk::PhysicalDeviceVulkan12Features>().shaderSubgroupExtendedTypes)
            features.push_back("shader-subgroup-extended-types");

        if (deviceFeatures2.get<vk::PhysicalDeviceVulkan12Features>().bufferDeviceAddress)
            features.push_back("buffer-device-address");

        // // Approx. DX12 waveops features
        VkPhysicalDeviceProperties2 extendedProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProps = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR
        };
        VkPhysicalDeviceSubgroupProperties subgroupProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};

        rtProps.pNext = extendedProps.pNext;
        extendedProps.pNext = &rtProps;
        subgroupProps.pNext = extendedProps.pNext;
        extendedProps.pNext = &subgroupProps;

        vkGetPhysicalDeviceProperties2(device, &extendedProps);
        // if (deviceFeatures2.get<vk::PhysicalDeviceSubgroupProperties>().supportedOperations &
        //     vk::SubgroupFeatureFlagBits::eBasic | vk::SubgroupFeatureFlagBits::eVote | vk::SubgroupFeatureFlagBits::eArithmetic |
        //     vk::SubgroupFeatureFlagBits::eBallot | vk::SubgroupFeatureFlagBits::eShuffle | vk::SubgroupFeatureFlagBits::eShuffleRelative |
        //     vk::SubgroupFeatureFlagBits::eClustered | vk::SubgroupFeatureFlagBits::eQuad |vk::SubgroupFeatureFlagBits::ePartitionedNV)
        //     features.push_back("wave-ops");
        // Approximate DX12's WaveOps boolean
        if (subgroupProps.supportedOperations &
            (VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT |
             VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT |
             VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_CLUSTERED_BIT |
             VK_SUBGROUP_FEATURE_QUAD_BIT | VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV))
        {
            features.push_back("wave-ops");
        }

        if (IsExtensionInExtensionProperties("VK_KHR_external_memory", extensionProperties))
        {
            extensions.push_back(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
#if SLANG_WINDOWS_FAMILY
            if (IsExtensionInExtensionProperties("VK_KHR_external_memory_win32", extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
            }
#else
            if (IsExtensionInExtensionProperties("VK_KHR_external_memory_fd", extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
            }
#endif
            features.push_back("external-memory");
        }
        if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
#if SLANG_WINDOWS_FAMILY
            if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
            }
#else
            if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME);
            }
#endif
            features.push_back("external-semaphore");
        }
        if (IsExtensionInExtensionProperties(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);
            features.push_back("conservative-rasterization-3");
            features.push_back("conservative-rasterization-2");
            features.push_back("conservative-rasterization-1");
        }
        if (IsExtensionInExtensionProperties(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            if (IsExtensionInExtensionProperties(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
            }
        }
        if (IsExtensionInExtensionProperties(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
        }
        if (IsExtensionInExtensionProperties(VK_NVX_BINARY_IMPORT_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NVX_BINARY_IMPORT_EXTENSION_NAME);
            features.push_back("nvx-binary-import");
        }
        if (IsExtensionInExtensionProperties(VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME);
            features.push_back("nvx-image-view-handle");
        }
        if (IsExtensionInExtensionProperties(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
            features.push_back("push-descriptor");
        }
        if (IsExtensionInExtensionProperties(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME);
            features.push_back("barycentrics");
        }
        if (IsExtensionInExtensionProperties(VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME);
            features.push_back("shader-subgroup-partitioned");
        }

        // Derive approximate DX12 shader model.
        const char* featureTable[] = {
            "sm_6_0",
            "wave-ops",
            "atomic-int64",
            nullptr,
            "sm_6_1",
            "barycentrics",
            "multiview",
            nullptr,
            "sm_6_2",
            "half",
            nullptr,
            "sm_6_3",
            "ray-tracing-pipeline",
            nullptr,
            "sm_6_4",
            "fragment-shading-rate",
            nullptr,
            "sm_6_5",
            "ray-query",
            "mesh-shader",
            nullptr,
            "sm_6_6",
            "wave-ops",
            "atomic-float",
            "atomic-int64",
            nullptr,
            nullptr,
        };

        int i = 0;
        while (i < sizeof(featureTable))
        {
            const char* sm = featureTable[i++];
            if (sm == nullptr)
            {
                break;
            }
            bool hasAll = true;
            while (i < sizeof(featureTable))
            {
                const char* feature = featureTable[i++];
                if (feature == nullptr)
                {
                    break;
                }
                hasAll &= std::ranges::find(features, feature) != features.end();
            }
            if (hasAll)
            {
                features.push_back(sm);
            }
            else
            {
                break;
            }
        }
        
        features.push_back("hardware-device");
    }
}

