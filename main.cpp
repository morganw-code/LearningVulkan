#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
// For EXIT_SUCCESS and EXIT_FAILURE macro
#include <cstdlib>

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
    GLFWwindow* window;
    
    void initWindow() {
        glfwInit();
        // Tell GLFW to not create an OpenGL context (hint, value)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disable window resize
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // Initialize GLFWwindow (GLFWmonitor param is used for optionally opening window on a specific monitor, and GLFWwindow share param is only relevant to OpenGL)
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() { }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        // cleanup glfw
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