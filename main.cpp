#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
// For EXIT_SUCCESS and EXIT_FAILURE macro
#include <cstdlib>
#include <vector>
#include <sstream>
#include <cstring>

class Application {
public:
    const int WIDTH = 800;
    const int HEIGHT = 600;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window = nullptr;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    void initWindow() {
        glfwInit();
        // Tell GLFW to not create an OpenGL context (hint, value)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disable window resize
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // Initialize GLFWwindow (GLFWmonitor param is used for optionally opening window on a specific monitor, and GLFWwindow share param is only relevant to OpenGL)
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        if(window == NULL) {
            throw std::runtime_error("failed to create GLFWwindow!\n");
        } else { /* Segmentation fault when calling std::cout ?? */ }
    }

    void initVulkan() {
        createInstance();
        queryAvailableExtensions();
        pickPhysicalDevice();
    }

    void createInstance() {
        if(enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but were not available!\n");
        } else { std::cout << "validation layers enabled\n"; }

        // VkApplicationInfo struct
        VkApplicationInfo appInfo;
        // Set the VkStructureType
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // VKInstanceCreateInfo struct
        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        // Reference our appInfo struct
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        // If validationLayers is true, set layer count and pass vector of layer names (Vulkan wants a pointer to the internal array, which is why .data() is called)
        if(enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else { createInfo.enabledLayerCount = 0; }

        // Create VkInstance passing createInfo struct and assign handle to class member 'instance'
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vkInstance\n!");
        } else { std::cout << "vkInstance created!\n"; }
    }

    void queryAvailableExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::stringstream ss;

        ss << "Available extensions:\n";
        for(const auto& extension : extensions) {
            ss << "\t" << extension.extensionName << "\n";
        }

        std::cout << ss.str() << std::endl;
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        bool layerFound = false;
        for(const char* layerName : validationLayers) {
            for(const auto& layerProperties : availableLayers) {
                if(strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
        }

        return layerFound;
    }
    
    // The selected graphics card is stored in a VkPhysicalDevice handle
    // This process is similar to querying available extensions and checking validation layer support
    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if(deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for(const auto& device : devices) {
            if(isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if(physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        std::stringstream ss;
        ss << "DEVICE NAME: " << deviceProperties.deviceName << std::endl;
        ss << "\t" << "DEVICE ID: " << deviceProperties.deviceID << std::endl;
        ss << "\t" << "VENDOR ID: " << deviceProperties.vendorID << std::endl;
        ss << "\t" << "DEVICE TYPE: " << deviceProperties.deviceType << std::endl;
        ss << "\t" << "API VERSION: " << deviceProperties.apiVersion << std::endl;
        ss << "\t" << "DRIVER VERSION: " << deviceProperties.driverVersion << std::endl;
        std::cout << ss.str();

        // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        // Doesn't matter at the moment anyway
        return true;
    }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        // Cleanup Vulkan
        vkDestroyInstance(instance, nullptr);

        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    Application app;

    try {
        app.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}