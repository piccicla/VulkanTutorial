#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include<vector>
#include <algorithm> //std::find
#include<cstring>    //strcmp

const int WIDTH = 800;
const int HEIGHT = 600;

class HelloTriangleApplication
{
public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow * window;
	VkInstance instance;

	/// validation layers
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"  //implicitly enables a whole range of useful diagnostics layers
	};
	#ifdef NDEBUG  //enable validation layer only for debug
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif
	///////////////////

	void initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // do not create opengl
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // disable windows resize

		window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanTutorial", nullptr, nullptr);
	}
	void initVulkan()
	{
		createInstance();
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void createInstance()
	{
		//first check validation layers
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
		if (enableValidationLayers)
		{
			std::cout << "--requested validation layers requested are running--" << std::endl;
		}


		////get info about extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr); // get number of available extensions
		std::vector<VkExtensionProperties> extensions(extensionCount);
		//fill in with extension properties and print them
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()); 
		std::cout << "available extensions:" << std::endl;
		for (const auto& extension : extensions) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}
		

		///////// create instance data
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//uint32_t glfwExtensionCount = 0;
		//const char** glfwExtensions;
		//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		//createInfo.enabledExtensionCount = glfwExtensionCount;
		//createInfo.ppEnabledExtensionNames = glfwExtensions;

		auto extnsions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extnsions.size());
		createInfo.ppEnabledExtensionNames = extnsions.data();


		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		/////////////////

		/////////// are required extension available on this system?
		/*std::cout << "glfw required extensions:" << std::endl;
		for (int i = 0; i < glfwExtensionCount; i++) {
			std::cout << "\t" << glfwExtensions[i] << std::endl;
		}
		if (!checkRequiredExtensionsPresent(extensions, glfwExtensions, glfwExtensionCount))
		{
			throw std::runtime_error("missing vulkan extensions");
		}*/
		////////////


		///////// finally we can create the instance.....
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

	}

	/*check if required extension available on this system*/
	bool checkRequiredExtensionsPresent(std::vector<VkExtensionProperties> availableExt, const char** requiredExt, int requiredExtCount)
	{
		for (auto i = 0; i < requiredExtCount; ++i) {
			bool found = false;
			for (const auto& extension : availableExt) {
				if (strcmp(requiredExt[i], extension.extensionName)==0) {
					found = true;
					break;
				}
			}
			if (!found) {
				return false;
			}
		}
		std::cout << "extension requirement fulfilled" << std::endl;
		return true;
	}

	/*
	check all available validation layers are there
	*/
	bool checkValidationLayerSupport()
	{
		// first create a vector with available layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;

	}

	/*
	return the required list of extensions based on whether validation layers are enabled or not
	*/
	std::vector<const char*> getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}



};

int main()
{
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}