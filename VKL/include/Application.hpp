#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/GLfw3.h"

#include <vector>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

namespace VL
{
    constexpr uint32_t WIN_WIDTH = 800;
    constexpr uint32_t WIN_HEIGHT = 600;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    class Application
    {
    public:
        Application();
        ~Application() = default;
        void Run();
    private:
        void InitWindow();
        void InitVulkan();
        void MainLoop();
        void CleanUp();
        void pickPhysicalDevice();

        void createInstance();
        void setupDebugMessenger();

        bool checkValidationLayersSupport();
        std::vector<const char*> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance
            , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
            , const VkAllocationCallbacks* pAllocator
            , VkDebugUtilsMessengerEXT* pDebugMessenger);

        static void DestroyDebugUtilsMessengerEXT(
            VkInstance instance
            , VkDebugUtilsMessengerEXT debugMessenger
            , const VkAllocationCallbacks* pAllocator);

        bool isDeviceSuitable(VkPhysicalDevice device);
    private:
        GLFWwindow*                 m_ApplicationWindow;
        VkInstance                  m_vkInstance;
        VkDebugUtilsMessengerEXT    m_debugMessenger{ VK_NULL_HANDLE };
        VkPhysicalDevice            m_physicalDevice{ VK_NULL_HANDLE };
    };
}

#endif //APPLICATION_H_

