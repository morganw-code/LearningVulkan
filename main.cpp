#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
// For EXIT_SUCCESS and EXIT_FAILURE macro
#include <cstdlib>
#include <vector>
#include <sstream>

class Application {
public:
    const int WIDTH = 800;
    const int HEIGHT = 600;

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window = nullptr;
    VkInstance instance;

    void initWindow() {
        glfwInit();
        // Tell GLFW to not create an OpenGL context (hint, value)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disable window resize
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // Initialize GLFWwindow (GLFWmonitor param is used for optionally opening window on a specific monitor, and GLFWwindow share param is only relevant to OpenGL)
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        queryAvailableExtensions();
    }

    void createInstance() {
        // VkApplicationInfo struct
        VkApplicationInfo appInfo;
        // Set the VkStructureType
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
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
        createInfo.enabledLayerCount = 0;

        // Create VkInstance passing createInfo struct and assign handle to class member 'instance'
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vkInstance!");
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