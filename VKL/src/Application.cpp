#include "Application.hpp"

namespace VL
{
    VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        std::cerr << "validation layer : " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult Application::CreateDebugUtilsMessengerEXT(
        VkInstance instance
        , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
        , const VkAllocationCallbacks* pAllocator
        , VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Application::DestroyDebugUtilsMessengerEXT(
        VkInstance instance
        , VkDebugUtilsMessengerEXT debugMessenger
        , const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    Application::Application()
        : m_ApplicationWindow(nullptr)
        , m_vkInstance(0)
    {
    }

    void Application::Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();
    }

    void Application::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_ApplicationWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Vulkan Window", nullptr, nullptr);
    }
    void Application::createInstance()
    {
        if (enableValidationLayers && !checkValidationLayersSupport())
        {
            throw std::runtime_error("validation layers requested, but not avaliable!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Vulkan Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        // create instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("fail to create VK instance!");
        }

        // get count of moreExtensionCnt
        uint32_t moreExtensionCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &moreExtensionCnt, nullptr);

        // get all VkExtensionProperties, and store in the extensions
        std::vector<VkExtensionProperties> exts(moreExtensionCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &moreExtensionCnt, exts.data());

        std::cout << "Avaliable extensions:" << std::endl;

        for (const auto& ext : exts)
        {
            std::cout << "\t" << ext.extensionName << "\n";
        }

        // check whether glfwExtensions all in the support VkExtensionProperties
        auto fun = [=]() ->bool {
            for (const auto& ext : extensions)
            {
                auto it = std::find_if(exts.begin(), exts.end(), [=](const VkExtensionProperties& proerty) {
                    std::string str(proerty.extensionName);
                    return str == ext;
                    });

                if (it == exts.end())
                {
                    return false;
                }
            }

            return true;
        };

        if (fun())
        {
            std::cout << "glfwExtensions all in the support VkExtensionProperties!" << std::endl;
        }
        else
        {
            std::cout << "glfwExtensions:" << std::endl;
            for (const auto& ext : extensions)
            {
                std::cout << ext << std::endl;
            }
        }
    }
    void Application::setupDebugMessenger()
    {
        if (enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (Application::CreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void Application::InitVulkan()
    {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
    }
    void Application::MainLoop()
    {
        while (!glfwWindowShouldClose(m_ApplicationWindow))
        {
            glfwPollEvents();
        }
    }
    void Application::CleanUp()
    {
        if (enableValidationLayers) {
            Application::DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
        }

        vkDestroyInstance(m_vkInstance, nullptr);

        glfwDestroyWindow(m_ApplicationWindow);

        glfwTerminate();
    }

    bool Application::checkValidationLayersSupport()
    {
        uint32_t layer_cnt = 0;
        vkEnumerateInstanceLayerProperties(&layer_cnt, nullptr);

        std::vector<VkLayerProperties> avaliableLayers(layer_cnt);

        vkEnumerateInstanceLayerProperties(&layer_cnt, avaliableLayers.data());

        for (const char* layername : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperty : avaliableLayers)
            {
                if ( strcmp(layername, layerProperty.layerName) == 0 )
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<const char*> Application::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;

        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) 
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ;

        createInfo.pfnUserCallback = debugCallback;
    }

    void Application::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
    
    bool Application::isDeviceSuitable(VkPhysicalDevice device){

        return true;
    }
}