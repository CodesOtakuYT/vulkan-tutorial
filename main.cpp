#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <string>

void glfw_error_callback(int error_code, const char* description) {
    std::cerr << "GLFW Error(" << error_code << "): " << description << std::endl;
    std::exit(EXIT_FAILURE);
}

int main() {
    int width = 720;
    int height = 720;
    std::string title("CODOTAKU");

    GLFWwindow* window;
    VkResult result;

    // Init GLFW
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    // Check for vulkan support
    if(glfwVulkanSupported() == GLFW_FALSE) {
        std::cerr << "Vulkan is not supported in this system" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    // Get required extensions
    uint32_t required_extensions_count;
    const char** required_extensions = glfwGetRequiredInstanceExtensions(&required_extensions_count);
    std::vector<const char*> extensions;
    extensions.reserve(required_extensions_count);

    for(uint32_t i = 0; i < required_extensions_count; i++) {
        extensions.emplace_back(required_extensions[i]);
    }

#ifdef __APPLE__
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    // Check extensions support
    uint32_t available_extensions_count = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, nullptr);
    if(result != VK_SUCCESS) {
        std::cerr << "Unable to get vulkan available extensions count";
        std::exit(result);
    }

    std::vector<VkExtensionProperties> available_extensions_properties(available_extensions_count);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, available_extensions_properties.data());
    if(result != VK_SUCCESS) {
        std::cerr << "Unable to get vulkan available extensions";
        std::exit(result);
    }

    std::vector<size_t> extensions_not_found;

    for(size_t i = 0; i < extensions.size(); i++) {
        const char* extension = extensions[i];
        bool found = false;
        for(auto available_extension_properties: available_extensions_properties) {
            if(strcmp(extension, available_extension_properties.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if(!found) {
            extensions_not_found.emplace_back(i);
        }
    }

    for(auto extension_index: extensions_not_found) {
        std::cerr << "Required vulkan extension '" << extensions[extension_index] << "' not found!" << std::endl;
    }

    if(!extensions_not_found.empty()) {
        exit(EXIT_FAILURE);
    }

    // Create instance
    VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = title.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
    };

    VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
    };

    VkInstance instance;
    result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if(result != VK_SUCCESS) std::exit(result);

    // Application loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}