/*======================================================================
Vulkan Presentation : VulkanFramework.cpp
Author:			Sim Luigi
Last Modified:	2020.12.13

<< �C�Â������ƁE���� >>�@���Ԉ���Ă���ꍇ�A�����Ă���������΍K���ł��B

���@VulkanAPI�̃l�[�~���O�R���x���V�����ɂ��āF
	
	�������u k �v�F �֐�          vkCreateInstance()   
	�啶���u K �v�F �f�[�^�^      VkInstance
	�S�đ啶���@ �F �}�N��        VK_KHR_SWAPCHAIN_EXTENSION_NAME

	Vk/Vk/VK: Vulkan
	KHR     : Khronos �iVulkan�AOpenCL�̃f�B���F���b�p�[

���@Vulkan��̃I�u�W�F�N�g�𐶐�����Ƃ��A�قƂ�ǈȉ��̒i�K�Ŏ������܂��B

                                                                  ��F�C���X�^���X	
	1.) �I�u�W�F�N�g���\���́uCreateInfo�v���쐬���܂��B        VkInstanceCreateInfo createInfo{};
	2.) �\���̂̎�ނ�I�����܂��B                                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	3.) �\���̂̐ݒ�E�t���b�O���`���܂��B                      createInfo.flags = 0;
	4.) CreateInfo�\���̂Ɋ�Â��ăI�u�W�F�N�g���̂𐶐����܂��B  vkCreateInstance(&createInfo, nullptr, &m_Instance):

��	Vulkan��p�̃f�o�b�O�@�\�uValidation Layer�v�u�o���f�[�V�������C���[�v�ɂ��āF
	���G�ł����A�ȒP�ɃC���[�W����ƁA�摜�ҏW�\�t�g�iPhotoshop�AGIMP�Ȃǁj�̃��C���[�Ɠ����悤�ɏd�˂Ă���̕����z�����₷���ł��B
	
	��F����A
	�@�����C���[�@�F����
		�����C���[�A�F����
		�@�����C���[�B�F�G���[�i�f�o�b�O���b�Z���W���[�\���A�G���[�n���h�����O�j

���@Vulkan�̃V�F�[�_�[�R�[�h�ɂ��āF
	GLSL��HLSL�Ȃǂ̃n�C���x���V�F�[�_�[����iHLSL�͂܂���HighLevelShadingLanguage)�ƈ���āA
	Vulkan�̃V�F�[�_�[�R�[�h��SPIR-V�Ƃ����o�C�g�R�[�h�t�H�[�}�b�g�Œ�`����Ă��܂��B

	GPU�ɂ��n�C���x���V�F�[�_�[�R�[�h�̓ǂݕ����قȂ�ꍇ������A
	����GPU�ŃV�F�[�_�[�����܂������Ă�����GPU�œ������ʂ��o����Ƃ͌���܂���B
	�@�B��̃��[���x�������Q�[�W�ł͂Ȃ����AHLSL�Ƌ@�B��̊Ԃ̃C���^�[���f�B�G�C�g���x��
	����Ƃ��ăN���X�v���b�g�t�H�[�������Ɍ����Ă��܂��B

	�Ȃ��AGLSL�Ȃǂ̃V�F�[�_�[��SPIR-V�ɕϊ�����c�[�����g����Vulkan�Ŏg����悤�ɂȂ�͂��ł��B

��	Vulkan�̂�����R�}���h�i�`��A�������]���j�Ȃǂ��֐��Œ��ڌĂ΂�܂���B�I�u�W�F�N�g�Ɠ����悤��
	�R�}���h�o�b�t�@�[�𐶐����āA���̃R�}���h���R�}���h�o�b�t�@�[�ɓo�^�i�i�[�j���܂��B
	���G�ȃZ�b�g�A�b�v�̑���ɁA�R�}���h�����O�ɏ����ł��܂��B

��	�t���O�����g�V�F�[�_�[�F�@�s�N�Z���V�F�[�_�[�Ɠ����ł��B

���@Vulkan��Bool�֐��Ɍ����āAtrue�Efalse�ł͂Ȃ��AVK_TRUE�EVK_FALSE���g�p���邱�Ƃł��B
	�������AVulkan��p VK_SUCCESS�AVK_ERROR_�����R�[���o�b�N���p�ӂ���Ă��܂��B

���@�X�e�[�W���O�o�b�t�@�[�̎g�p�̗��R
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT�F ��ԗ��z�I�ȃ������[�t�H�[�}�b�g���A���[�J���������[�����ڃA�N�Z�X�ł��܂���B
	�����ŁA���̃������[�^�C�v���g����CPU��́u�X�e�[�W���O�o�b�t�@�[�v�𐶐����A���_�z��i�f�[�^�j�ɓn���܂��B
	�����ăX�e�[�W���O�o�b�t�@�[�̏������[�J���������[�Ɋi�[����Ă��钸�_�o�b�t�@�[�Ɉڂ��܂��B

=======================================================================*/
#include "VulkanFramework.h"

#define TINYOBJLOADER_IMPLEMENTATION        // tinyobjloader���f���ǂݍ���
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL             // glm�^��Hash
#define GLM_FORCE_RADIANS                   // glm::rotate�֐������W�A���ŏ�������ݒ�
#define GLM_FORCE_DEPTH_ZERO_TO_ONE	        // GLM�̃v���W�F�N�V�����}�g���b�N�X��Z�\�[�g�l�� -1.0�`1.0�iOpenGL�Ή��̂��߁j
// Vulkan�Ή��ɂ��邽�߂� 0.0�`1.0 �ɐݒ肷��K�v������܂�
// GLM uses depth ranges of -1.0 to 1.0 in accordance with OpenGL standards.
// To use GLM projection matrices in Vulkan for depth buffering, set values to 0.0�`1.0

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>       // ���ԊǗ��G�t���[�����[�g�Ɋւ�炸�W�I���g���[��90����]�����܂�
                        // time manager: will allow us to rotate geometry 90 degress regardless of frame rate

#include <iostream>     // ��{I/O
#include <vector>       // std::vector
#include <set>          // std::set
#include <array>        // std::array
#include <map>          // std::map
#include <unordered_map>// std::unordered_map ���_�d��������邽��
#include <cstring>      // strcpy, �Ȃ�
#include <optional>     // C++17 and above
#include <algorithm>    // std::min/max : chooseSwapExtent()
#include <cstdint>      // UINT32_MAX   : in chooseSwapExtent()
#include <stdexcept>    // std::runtime error�A�Ȃ�
#include <cstdlib>      // EXIT_SUCCESS�EEXIT_FAILURE : main()
#include <fstream>      // �V�F�[�_�[�̃o�C�i���f�[�^��ǂݍ��ށ@for loading shader binary data
#include <glm/glm.hpp>  // glm::vec2, vec3 : Vertex�\����

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "Asset/Model/viking_room.obj";
const std::string TEXTURE_PATH = "Asset/Texture/viking_room.png";

// �����ɏ��������t���[���̍ő吔 
// how many frames should be processed concurrently 
const int MAX_FRAMES_IN_FLIGHT = 2;		

// Vulkan�̃o���f�[�V�������C���[�FSDK��̃G���[�`�F�b�N�d�g��
// Vulkan Validation layers: SDK's own error checking implementation
const std::vector<const char*> validationLayers =				
{
	"VK_LAYER_KHRONOS_validation"
};

// Vulkan�G�N�X�e���V�����i�����SwapChain)
const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME    // �뎚������邽�߂̃}�N����`
};

// NDEBUG = Not Debug	
#ifdef NDEBUG					
// �o���f�[�V�������C���[����
const bool enableValidationLayers = false;
#else
// �o���f�[�V�������C���[�L��
const bool enableValidationLayers = true;
#endif

//====================================================================================
// 00X : �f�o�b�O�p�֐�
// Debug-related Functions
//====================================================================================

// �f�o�b�O���b�Z���W���[����
// �G�N�X�e���V�����֐��F�����I�Ƀ��[�h����Ă��܂���B
// �֐��A�h���X��vkGetInstanceProcedureAddr()�œ���ł��܂��B
// extension function; not automatically loaded. Need to specify address via	
// vkGetInstanceProcedureAddr()

// �N���X�O�Œ�`�F�f�o�b�O���b�Z���W���[����
VkResult CreateDebugUtilsMessengerEXT(        
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr
	(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

// �N���X�O�Œ�`�F�f�o�b�O���b�Z���W���[�폜
void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

// �f�o�b�O�R�[���o�b�N
VKAPI_ATTR VkBool32 VKAPI_CALL CVulkanFramework::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,     // �G���[�̏d�v���G��r�I�y���[�^�Ŕ�ׂ��܂��B
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	// boolean indicating if Vulkan call that triggered the validation layer message should be aborted
	// if callback returns true, call is aborted with VK_ERROR_VALIDATION_FAILED_EXT error. 
	// Should always return VK_FALSE unless testing validation layer itself.
	return VK_FALSE;
}

// �f�o�b�O���b�Z�[�W�\���̂ɕK�v�ȃG�N�X�e���V������������֐�
void CVulkanFramework::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};    // �f�o�b�O���b�Z���W���[���\����
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|*/ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

}

// �f�o�b�O���b�Z�[�W�ݒ�
void CVulkanFramework::setupDebugMessenger()
{
	if (enableValidationLayers == false)    // �f�o�b�O���[�h�ł͂Ȃ��ꍇ�A�������܂�  Only works in debug mode
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

// �o���f�[�V�������C���[���T�|�[�g����Ă��邩�̊m�F
bool CVulkanFramework::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);                   // �o���f�[�V�������C���[�������

	std::vector<VkLayerProperties> availableLayers(layerCount);                 // �J�E���g�ɂ��x�N�g���𐶐����܂�
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());    // �����x�N�g���ɑ��

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			// ���C���[���������̃o���f�[�V�������C���[�ɍ����Ă�ꍇ if layer name matches existing validation layer name
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;		// ���������t���b�O
				break;
			}
		}

		// ���܂ł̃��C���[�������̃��C���[���X�g�Ɍ�����������A�S�Ẵ��C���[���m�F����܂Ń��[�v�������܂�
		// as long as layer is found in the list, loop will continue until all validation layers have been verified
		if (layerFound == false)
		{
			return false;
		}
	}
	return true;
}

// �K�v�ȃG�N�X�e���V�����̈ꗗ�i�f�o�b�O�@�\���I���E�I�t�ɂ���ĈقȂ�܂��j
// returns the list of extensions based on whether validation layers are enabled or not
std::vector<const char*> CVulkanFramework::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers == true)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}


//====================================================================================
// To be migrated later
//====================================================================================

void CVulkanFramework::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

// ���C�����[�v
void CVulkanFramework::mainLoop()
{
	while (glfwWindowShouldClose(m_Window) == false)
	{
		glfwPollEvents();    // �C�x���g�ҋ@  Update/event checker
		drawFrame();         // �t���[���`��
	}

	// �v���O�����I���i��Еt���j�̑O�ɁA���ɓ����Ă��鏈�����ς܂��܂��B
	// let logical device finish operations before exiting the main loop 
	vkDeviceWaitIdle(m_LogicalDevice);
}




//====================================================================================
// 10X : �I�u�W�F�N�g�����E�������p�֐�
// Object Creation/Initialization Functions
//====================================================================================

// �E�C���h�E������
void CVulkanFramework::initWindow()
{
	glfwInit();    // GLFW������

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);    // OPENGL�R���e�N�X�g���쐬���Ȃ��I
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);       // ���[�U�[�ŃE�B���h�E���T�C�Y�L��

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);    // ��L�̃p�����[�^�ŃE�B���h�E�𐶐����܂�
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

// Vulkan������
void CVulkanFramework::initVulkan()
{
	createInstance();			    // �C���X�^���X����			
	setupDebugMessenger();          // �f�o�b�O�R�[���o�b�N�ݒ�
	createSurface();                // �E�C���h�E�T�[�t�F�X����
	pickPhysicalDevice();           // Vulkan�ΏۃO���t�B�b�N�X�J�[�h�̑I��
	createLogicalDevice();          // �O���t�B�b�N�X�J�[�h�ƃC���^�[�t�F�[�X����f�o�C�X�ݒ�
	createSwapChain();              // SwapChain����
	createImageViews();             // SwapChain�p�̉摜�r���[����
	createRenderPass();             // �����_�[�p�X
	createDescriptorSetLayout();    // ���\�[�X�ŃX�N���v�^�[���C�A�E�g 
	createGraphicsPipeline();       // �O���t�B�b�N�X�p�C�v���C������
	createColorResources();         // �J���[���\�[�X�����iMSAA)
	createDepthResources();         // �f�v�X���\�[�X����
	createFramebuffers();           // �t���[���o�b�t�@�����i�f�v�X���\�[�X�̌�j
	createCommandPool();            // �R�}���h�o�b�t�@�[���i�[����v�[���𐶐�
	createTextureImage();           // �e�N�X�`���[�}�b�s���O�p�摜����
	createTextureImageView();       // �e�N�X�`���[���A�N�Z�X���邽�߂̃C���[�W�r���[����
	createTextureSampler();         // �e�N�X�`���[�T���v���[����
	loadModel();                    // ���f���f�[�^��ǂݍ���
	createVertexBuffer();           // ���_�o�b�t�@�[����
	createIndexBuffer();		      // �C���f�b�N�X�o�b�t�@�[����
	createUniformBuffers();         // ���j�t�H�[���o�b�t�@�[����
	createDescriptorPool();         // �f�X�N���v�^�[�Z�b�g���i�[����v�[���𐶐�
	createDescriptorSets();         // �f�X�N���v�^�[�Z�b�g�𐶐�
	createCommandBuffers();         // �R�}���h�o�b�t�@�[����
	createSyncObjects();            // ���������I�u�W�F�N�g����
}

// Vulkan�C���X�^���X���� Create Vulkan Instance
void CVulkanFramework::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())    // �f�o�b�O���[�h�̐ݒ�Ńo���f�[�V�������C���[�@�\���T�|�[�g����Ȃ��ꍇ
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};    // Vulkan�A�v���P�[�V�������\���̂𐶐�
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	// �f�o�b�O���[�h�I���̏ꍇ�A�o���f�[�V�������C���[�����`���܂�
	// if validation layers is on (debug mode), include validation layer names in instantiation
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

	// ��L�̍\���̂̏��Ɋ�Â��Ď��ۂ̃C���X�^���X�𐶐����܂��B
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance!");
	}
}

// �T�[�t�F�X�����iGLFW�j
// Surface Creation
void CVulkanFramework::createSurface()
{
	if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
}

// �����f�o�C�X�i�O���t�B�b�N�X�J�[�h��I���jSelect compatible GPU
void CVulkanFramework::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);    // Vulkan�Ή��̃f�o�C�X�iGPU)�𐔂���
	if (deviceCount == 0)                                             // ������Ȃ������ꍇ�A�G���[�\��
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);                     // ������GPU�Ɋ�Â��ĕ����f�o�C�X�x�N�g���𐶐�
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());   // �f�o�C�X�����x�N�g���ɑ��

	for (const VkPhysicalDevice device : devices)     // ���������f�o�C�X�͂�낤�Ƃ��Ă��鏈���ɓK�؂��ǂ������m�F���܂�
	{                                                 // evaluate each physical device if suitable for the operation to perform
		if (isDeviceSuitable(device) == true)
		{
			m_PhysicalDevice = device;
			m_MSAASamples = getMaxUseableSampleCount();   // �I�������T���v���r�b�g�̍ő吔���l��
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

// ���W�J���f�o�C�X���� Create Logical Device to interface with GPU
void CVulkanFramework::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);    // ���W�J���f�o�C�X�L���[�����@Preparing logical device queue

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;               // ���W�J���f�o�C�X�L���[�������
	std::set<uint32_t> uniqueQueueFamilies =
	{ indices.graphicsFamily.value(), indices.presentFamily.value() };	 // �L���[��ށi���݁F�O���t�B�b�N�X�A�v���[���e�[�V�����j

	float queuePriority = 1.0f;    // �D��x�G0.0f�i��j�`1.0f�i���j

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;    // Anisotropy�L��
	deviceFeatures.sampleRateShading = VK_TRUE;    // �T���v���V�F�[�f�B���O�L��


	VkDeviceCreateInfo createInfo{};    // ���W�J���f�o�C�X�������\����
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();    // �p�����[�^�̃|�C���^�[�@pointer to the logical device queue info (above)

	createInfo.pEnabledFeatures = &deviceFeatures;             // currently empty (will revisit later)

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// ��L�̃p�����[�^�Ɋ�Â��Ď��ۂ̃��W�J���f�o�C�X�𐶐����܂��B
	// Creating the logical device itself
	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);    //�@�O���t�B�b�N�X�L���[ graphics queue
	vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);      //�@�v���[���e�[�V�����L���[ presentation queue
}

// �X���b�v�`�F�C�������i�摜�̐؂�ւ��j
void CVulkanFramework::createSwapChain()
{
	// GPU��SwapChain�T�|�[�g����ǂݍ���
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// +1�̗��R�F
	// SwapChain���玟�̃C���[�W���l������O�ɁA���܂ɂ̓h���C�o�[�̓��������̏I����҂��Ȃ��Ƃ����܂���B
	// �����ŁA�]�T��1�i�ȏ�j�̃C���[�W��p�ӂł���悤�ɐݒ肷��̂͂������߂ł��B

	// why +1?
	// sometimes we may have to wait on the driver to perform internal operations before we can acquire
	// another image to render to. Therefore it is recommended to request at least one more image than the minimum.
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0                 // zero here means there is no maximum!
		&& imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};    // SwapChain�������\����
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;                                // stereoscopic3D �ȊO�Ȃ�u�P�v 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    // SwapChain�𗘗p���鏈���@what operations to use swap chain for 

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)    // �O���t�B�b�N�X�ƃv���[���e�[�V�����L���[���قȂ�ꍇ
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else    // �O���t�B�b�N�X�ƃv���[���e�[�V�����L���[�������i����̃f�o�C�X�͂����������ɐݒ肳��Ă��܂��j�B
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	// �p�t�H�[�}���X�œK
		createInfo.queueFamilyIndexCount = 0;        // �C�� optional
		createInfo.pQueueFamilyIndices = nullptr;    // �C�� optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;    // �A���t�@�`�����l���F�s����
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;               // TRUE : �I�N���[�W�������ꂽ�s�N�Z���̐F�𖳎��@don't care about color of obscured pixels
	createInfo.oldSwapchain = VK_NULL_HANDLE;	// ���SwapChain�̂݁@for now, only create one swap chain

	// ��L�̏��Ɋ�Â���SwapChain�𐶐����܂��B
	if (vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);                    // SwapChain�̃C���[�W�����l��
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());   // ����m_SwapChainImages�ɑ��

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

// �v���O�����p�C���[�W�r���[����
void CVulkanFramework::createImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());    // �C���[�W�J�E���g�ɂ���ăx�N�g���T�C�Y��ύX���܂�allocate enough size to fit all image views 					
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = createImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

// �����_�[�p�X����
void CVulkanFramework::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};           // �J���[�A�^�b�`�����g
	colorAttachment.format = m_SwapChainImageFormat;    // SwapChain�t�H�[�}�b�g�Ɠ����@format of color attachment = format of swap chain images
	colorAttachment.samples = m_MSAASamples;             // �}���`�T���v�����O�r�b�g��

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;    // �����_�����O�O�̏��͂ǂ�����t���b�O�@
															  // what to do with data before rendering

	// VK_ATTACHMENT_LOAD_OP_LOAD       : �����̏���ۑ��@Preserve existing contents of attachment
	// VK_ATTACHMENT_LOAD_OP_CLEAR      : �N���A����i���݁F���jClear the values to a constant at the start (in this case, clear to black)
	// VK_ATTACHMENT_LOAD_OP_DONT_CARE  : ���𖳎��@Existing contents are undefined; we don't care about them

	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	// �����_�����O��̏��͂ǂ�����t���b�O what to do with data after rendering

	// VK_ATTACHMENT_STORE_OP_STORE     : ����ۑ��@Rendered contents will be stored in memory and can be read later
	// VK_ATTACHMENT_STORE_OP_DONT_CARE : ���𖳎��@Contents of the framebuffer will be undefined ater the rendering operation

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;     // �X�e���V���o�b�t�@�[���g���Ă��Ȃ�
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // not using stencil buffer

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                 // �����_�����O�O�̃C���[�W���C�A�E�g image layout before render pass
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;    // �����_�����O�O�̃C���[�W���C�A�E�g layout to automatically transition to after pass

	// ��ʂ̃��C�A�E�g�� Common Layouts:
	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : �J���[�A�^�b�`�����g�Ƃ��Ďg��    Images used as color attachment
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR          : SwapChain�ŕ`�悵�܂�              Images to be presented in the swap chain
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL     : �������[�R�s�[���Z�Ƃ��Ďg��      Images to be used as destination for a memory copy operation

	// �A�^�b�`�����g���t���b�N�X�C���f�b�N�X
	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;    // attachment reference index; 0 = first index (only one attachment for now)
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// �}���`�T���v�����O�ō쐬�����C���[�W�͒��ڕ`��ł��܂���B���ʂ̃C���[�W�Ƃ��Ă܂Ƃ߂Ă���`��ł��܂�
	// ���L�̃T�u�p�X�p�����[�^ subpass.pResolveAttachments�y�ѕK�v�ȏ����ŏ������܂�
	// Multisampled Images cannot be presented directly (VK_IMAGE_LAYOUT_PRESENT_SRC_KHR). 
	// First resolve them into a regular image, then they can be presented via subpass.pResolveAttachments below
	VkAttachmentDescription colorAttachmentResolve{};    // �J���[�A�^�b�`�����gResolve(���\�[���u�G�����j���\����
	colorAttachmentResolve.format = m_SwapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;    // �}���`�T���v����A�C���[�W��1�r�b�g�T���v���ɕϊ�
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // ���\�[���u��A�v���[���g���邱�Ƃ��ł��܂�

	VkAttachmentReference colorAttachmentResolveReference{};
	colorAttachmentResolveReference.attachment = 2;
	colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// �f�v�X�A�^�b�`�����g
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = m_MSAASamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// �f�v�X�A�^�b�`�����g���t�@�����X
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;    // �A�^�b�`�����g�Ɠ���

	// �T�u�p�X
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentReference;
	subpass.pResolveAttachments = &colorAttachmentResolveReference;

	// �T�u�p�X�ˑ��֌W
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;    // �T�u�p�X�C���f�b�N�X 0 subpass index 0 (our first and only subpass)
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// �����_�[�p�X���\���̐���
	// attachments�FcreateCommandBuffers()��clearValues���ԂƓ����ɂ��邱��
	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	// ��L�̍\���̂̏��Ɋ�Â��Ď��ۂ̃����_�[�p�X�𐶐����܂��B
	if (vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");
	}
}

// ���\�[�X���C�A�E�g�F�ǂ�ȃ��\�[�X�i�o�b�t�@�[�A�C���[�W���Ȃǁj���O���t�B�b�N�X�p�C�v���C���ɃA�N�Z�X�������邩
void CVulkanFramework::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};    // UniversalBufferObject���C�A�E�g�o�C���f�B���O���\����
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;               // MVP�g�����X�t�H�[����1�̃o�b�t�@�[�I�u�W�F�N�g�Ɋi�[����Ă��܂�
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;               // �C�� optional, image sampling�p
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;    // �Q�Ƃł���V�F�[�_�[�X�e�[�W�i���݁A���_�V�F�[�_�[�ŃX�N���v�^�[�j

	// �����C���[�W�T���v���[�p Combined Image Sampler
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;    // VERTEX_BIT: Heightmap, etc

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_LogicalDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout!");
	}
}

// �O���t�B�b�N�X�p�C�v���C������
void CVulkanFramework::createGraphicsPipeline()
{
	const std::vector<char> vertShaderCode = readFile("shaders/vert.spv");    // ���_�V�F�[�_�[�O���t�@�C���̓ǂݍ���
	const std::vector<char> fragShaderCode = readFile("shaders/frag.spv");    // �t���O�����g�V�F�[�_�[�O���t�@�C���̓ǂݍ���

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);     // ���_�V�F�[�_�[���W���[�������i���_�f�[�^�A�F�f�[�^�܂߁j
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);     // �t���O�����g�V�F�[�_�[���W���[������

	// �V�F�[�_�X�e�[�W�F�p�C�v���C���ŃV�F�[�_�[�𗘗p����i�K	
	// Shader Stages: Assigning shader code to its specific pipeline stage
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};                    // ���_�V�F�[�_�[�X�e�[�W���\����
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;                   // enum for programmable stages in Graphics Pipeline: Intro
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";                                       // ���_�V�F�[�_�[�G���g���[�|�C���g�֐���(shaders.vert)

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};                    // �t���O�����g�V�F�[�_�[�X�e�[�W���\����
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";                                       // �t���O�����g�V�F�[�_�[�G���g���[�|�C���g�֐���(shaders.frag)

	// �p�C�v���C�������̃^�C�~���O�Ŏg����`�ɂ��܂��B
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };    // �V�F�[�_�[�\���̔z��

	// �p�C�v���C�������̍ۂɕK�v�Ȓi�K necessary steps in creating a graphics pipeline

	// 1.) ���_�C���v�b�g�F���_�V�F�[�_�[�ɓn����钸�_���̃t�H�[�}�b�g
	// Vertex Input: Format of the vertex data to be passed to the vertex shader

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};    // ���_�C���v�b�g���\����
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// Vertex�\���̂�VertexBindingDescription��VertexAttributeDescription�ɎQ�Ƃ��܂�
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	// 2.) �C���v�b�g�A�Z���u���[�F ���_����ǂ�ȃW�I���g���[���`�悳��邩�A�����ăg�|���W�[�����o�[�ݒ�
	// Input Assembly: What kind of geometry will be drawn from the vertices, topology member settings

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    // POINT_LIST�ALINE_LIST�ALINE_STRIP�ATRIANGLE_STRIP
	inputAssembly.primitiveRestartEnable = VK_FALSE;                 //�@STRIP�̏ꍇ�A���C���ƎO�p�𕪎U�ł��܂�

	// 3.) �r���[�|�[�g�E�V�U�[�l�p
	// Viewports and Scissor Rectangles

	// �r���[�|�[�g�F�C���[�W����t���[���o�b�t�@�[�܂ł́u�g�����X�t�H�[���v        Viewport: 'transformation' from the image to the framebuffer
	// �V�U�[�l�p�F�s�N�Z���f�[�^���i�[�����̈�G��ʏ�ŕ`�悳���u�t�B���^�[�v  Scissor rectangle: 'filter' in which region pixels will be stored. 

	VkViewport viewport{};                                // �r���[�|�[�g���\����
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_SwapChainExtent.width;      // �`�惌�]���[�V�����Ɠ���
	viewport.height = (float)m_SwapChainExtent.height;    // �`�惌�]���[�V�����Ɠ���
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};                     // �V�U�[�l�p���\����				
	scissor.offset = { 0, 0 };              // �I�t�Z�b�g�Ȃ�
	scissor.extent = m_SwapChainExtent;     // �t���[���o�b�t�@�[�S�̂�`�悷��ݒ� set to draw the entire framebuffer

	VkPipelineViewportStateCreateInfo viewportState{};    // �r���[�|�[�g�X�e�[�g�i��ԁj���\����
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;                 // �r���[�|�[�g�̃|�C���^�[
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;                   // �V�U�[�l�p�̃|�C���^�[

	// 4.) ���X�^���C�U�[�F���_�V�F�[�_�[����̃W�I���g���[�i�V�F�[�v�j���t���O�����g�i�s�N�Z���j�ɕϊ����ĐF��t���܂��B 
	// Rasterizer: Takes geomerty shaped from the vertex shader and turns it into fragments (pixels) to be colored by the fragment shader.

	VkPipelineRasterizationStateCreateInfo rasterizer{};    // ���X�^���C�U�[���\����
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	// VK_TRUE: �j�A�\�E�t�@�[�̈�̃t���O�����g���̂Ă��ɋ��E�ɃN�����v����܂��B�V���h�E�}�b�s���O�ɕ֗��B
	// VK_TRUE: fragments beyond near and far planes are clamped to them rather than discarding them; useful with shadow mapping

	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	// FILL: �t���O�����g�ɖ��ߍ��� fill area of polygon with fragments
	// LINE: ���C���ŕ`��i���C���[�t���[���j�@polygon edges drawn as lines (i.e. wireframe)
	// POINT: ���_�ŕ`��@polygon vertices are drawn as points
	// ��FILL�ȊO�̏ꍇ�A�����GPU�@�\���I���ɂ���K�v������܂��BUsing anything other than FILL requires enabling a GPU feature.

	rasterizer.lineWidth = 1.0f;    // ���C���̌����i�s�N�Z���P�ʁjLine thickness (in pixels)
	// ��1.0�ȏ�̌����̏ꍇ�A�����GPU�@�\���I���ɂ���K�v������܂��BUsing line width greater than 1.0f requires enabling a GPU feature.

	rasterizer.cullMode = VK_CULL_MODE_NONE;                    // �J�����O�ݒ�i�ʏ�FBackfaceCulling)
																// VK_CULL_MODE_BACK_BIT

	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;     // ���_�̏��Ԃɂ��\�ʁE���ʂ𔻒f����ݒ�i���v���E�����v���j
																// determines front-facing by vertex order (CLOCKWISE or COUNTER_CLOCKWISE) 
	rasterizer.depthBiasEnable = VK_FALSE;        // VK_TRUE: Depth�l�����i�V���h�E�}�b�s���O�jAdjusting depth values i.e. for shadow mapping
	rasterizer.depthBiasConstantFactor = 0.0f;    // �C�� optional
	rasterizer.depthBiasClamp = 0.0f;             // �C�� optional
	rasterizer.depthBiasSlopeFactor = 0.0f;       // �C�� optional

	// 5.) �}���`�T���v�����O�i�A���`�G�C���A�V���O�p�j 
	// Multisampling (method to perform anti-aliasing)

	VkPipelineMultisampleStateCreateInfo multisampling{};    // �}���`�T���v�����O���\����
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;             // �T���v���V�F�[�f�B���O�L��
	multisampling.rasterizationSamples = m_MSAASamples;      // �}���`�T���v�����O�L��
	multisampling.minSampleShading = 0.2f;                   // 1.0�ɋ߂���΋߂��قǃX���[�Y���������Ȃ�
	multisampling.pSampleMask = nullptr;                     // �C�� optional
	multisampling.alphaToCoverageEnable = VK_FALSE;          // �C�� optional
	multisampling.alphaToOneEnable = VK_FALSE;               // �C�� optionall

	// 6.)�@�f�v�X�E�X�e���V��
	// Depth and Stencil Testing 

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;     // �V�����t���O�����g�̃f�v�X���f�v�X�o�b�t�@�[�Ɣ�r���邩�idiscard)
	depthStencil.depthWriteEnable = VK_TRUE;    // �f�v�X�e�X�g�����i�����t���O�����g�̃f�v�X���f�v�X�o�b�t�@�[�ɏ������ނ�

	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;    // �f�v�X���Ⴂ�F�߂�
	depthStencil.depthBoundsTestEnable = VK_FALSE;       // �f�v�X�o�E���h�e�X�g�p�iTRUE: Bounds���̃t���O�����g�����ۗ����܂���j
	depthStencil.minDepthBounds = 0.0f;                  // �C�� optional
	depthStencil.maxDepthBounds = 1.0f;                  // �C�� optional

	depthStencil.stencilTestEnable = VK_FALSE;           // �X�e���V���o�b�t�@�[�p
	depthStencil.front = {};                             // �C�� optional
	depthStencil.back = {};                              // �C�� optional  

	// 7.) �J���[�u�����f�B���O�F�@�t���[���o�b�t�@�[�̊����̐F�ƍ����邩�A�A�O�ƐV�����l��Bitwise���Z�ō������邩
	// Color Blending: Either �@ mix the old value (in the framebuffer) and new value, or �A perform a bitwise operation with the two values

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};         // �J���[�u�����h���\����
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT      // �@�̃u�����f�B���O����
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;                        // ���L�̃R�����g�����������������B
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;     // �C�� optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;    // �C�� optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                // �C�� optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;     // �C�� optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;    // �C�� optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                // �C�� optional

	// blendEnable��VK_TRUE�̏ꍇ�̃u�����f�B���O�ݒ�i�^������j
	// if (blendEnable) 
	//	{
	//		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
	//		finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
	// }
	// else 
	// {
	//		finalColor = newColor;
	// }
	// finalColor = finalColor & colorWriteMask;

	// �A���t�@�u�����f�B���O�̗�i�^������j
	// Alpha Blending Implementation: (pseudocode)
	// finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
	// finalAlpha.a = newAlpha.a;

	// �A���t�@�u�����f�B���O�ݒ�F
	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};    // �p�C�v���C���J���[�u�����h�X�e�[�g���\����
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;       // VK_TRUE: �A�̃x���f�B���O����
	colorBlending.logicOp = VK_LOGIC_OP_COPY;     // �C�� optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;       // �C�� optional
	colorBlending.blendConstants[1] = 0.0f;       // �C�� optional
	colorBlending.blendConstants[2] = 0.0f;       // �C�� optional
	colorBlending.blendConstants[3] = 0.0f;       // �C�� optional

	// 8.) �_�C�i�~�b�N�X�e�[�g�i��ŏڂ������ׂ܂��j
	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	// 9.) �p�C�v���C�����C�A�E�g�i��ŏڂ������ׂ܂��j
	// Pipeline Layout (empty for now, revisit later)

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};     // �p�C�v���C�����C�A�E�g���\����
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;     // �ŃX�N���v�^�[�Z�b�g���C�A�E�g
	pipelineLayoutInfo.pushConstantRangeCount = 0;               // �C�� optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr;            // �C�� optional

	// ��L�̍\���̂̏��Ɋ�Â��Ď��ۂ̃p�C�v���C�����C�A�E�g�𐶐����܂��B
	if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	// 10.) �O���t�B�b�N�X�p�C�v���C���F�S���̒i�K��g�ݍ��킹�ăp�C�v���C���𐶐����܂��B
	// ���A���̑O�ɂ����̂悤�ɃO���t�B�b�N�X�p�C�v���C�����\���̂𐶐�����K�v������܂��B
	// Graphics Pipeline creation: Putting everything together to create the pipeline!!
	// ...but before that, we need to create a Pipeline info struct:

	VkGraphicsPipelineCreateInfo pipelineInfo{};    // �p�C�v���C�����\����
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;                    // �V�F�[�_�[�X�e�[�W�ɍ��킹��	 Make sure this info is aligned with Shader Stages above
	pipelineInfo.pStages = shaderStages;            // �V�F�[�_�[�X�e�[�W�z��̃|�C���^�[

	// ���܂ł̒i�K���p�C�v���C���\���̏��|�C���^�[�ɎQ�Ƃ��܂��B
	// reference all the structures described in the previous stages
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;             // �C�� optional

	// pipelineInfo.flags - ���݂Ȃ��B���L�� basePipelineHandle�Ɓ@basePipelineHandleIndex���������������B
	// none at the moment; see basePipelineHandle and basePipelineIndex below

	// �p�C�v���C�����C�A�E�g�F�\���̃|�C���^�[�ł͂Ȃ��AVulkan�n���h��
	// pipeline layout: Vulkan handle, NOT struct pointer
	pipelineInfo.layout = m_PipelineLayout;

	// �����_�[�p�X�ƃT�u�p�X�̎Q��
	// reference to the render pass and the subpass index
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	// pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT�̏ꍇ�F
	// �����̃p�C�v���C������̏���V���ȃp�C�v���C���Ɏg�p���܂��B
	// these values are only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag 
	// is also specified in the flags field of VkGraphicsPipelineCreateInfo
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;    // �C�� optional
	pipelineInfo.basePipelineIndex = -1;                 // �C�� optional

	// ��L�̍\���̂̏��Ɋ�Â��āA�悤�₭���ۂ̃O���t�B�b�N�X�p�C�v���C���������ł��܂��B
	// 2�ڂ̈����F�p�C�v���C���L���b�V��(��Œ��ׂ܂��j
	// Creating the actual graphics pipeline from data struct
	// Second argument: pipeline cache (revisit later)
	if (vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline!");
	}

	// �p�ς݂̃V�F�[�_�[���W���[�����폜���܂��B
	vkDestroyShaderModule(m_LogicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_LogicalDevice, vertShaderModule, nullptr);
}

// �}���`�T���v�����O�p�J���[�o�b�t�@�[�𐶐�
void CVulkanFramework::createColorResources()
{
	VkFormat colorFormat = m_SwapChainImageFormat;

	createImage(
		m_SwapChainExtent.width,
		m_SwapChainExtent.height,
		1,
		m_MSAASamples,
		colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_ColorImage,
		m_ColorImageMemory
	);

	// �}���`�T���v�����O�p�C���[�W�r���[�����̍ۂɃ~�b�v�}�b�v�́u1�v�ɐݒ肵�Ȃ��Ƃ����܂���iVulkan�̌��܂�j
	// ���̃C���[�W�r���[�̓e�N�X�`���[�Ƃ��Ďg��Ȃ��̂ŕ`��i���ɉe�����܂���
	// Mipmap levels must be set to 1 when creating an image with more than one sample per pixel,
	// as per Vulkan specifications.  As this image will not be used as a texture, it will not affect quality
	m_ColorImageView = createImageView(m_ColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

// �f�v�X���\�[�X����
void CVulkanFramework::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();
	createImage(
		m_SwapChainExtent.width,
		m_SwapChainExtent.height,
		1,
		m_MSAASamples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_DepthImage,
		m_DepthImageMemory
	);
	m_DepthImageView = createImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

// �t���[���o�b�t�@�[
void CVulkanFramework::createFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 3> attachments =
		{
			m_ColorImageView, m_DepthImageView, m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;          // �t���[���o�b�t�@�[�Ƒ�������ݒ� compatibility with framebuffer settings
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;                         // 1: 1�̃C���[�W�����Ȃ��ꍇ Swap chain images as single images

		if (vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}
}

// �R�}���h�o�b�t�@�[���i�[����R�}���h�v�[���𐶐�
void CVulkanFramework::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

	VkCommandPoolCreateInfo poolInfo{};    // �R�}���h�v�[�����\����
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();    // �`�悷�邽�߃O���t�B�b�N�X�L���[��I�����܂�
																			  // drawing commands: graphics queue family chosen
	poolInfo.flags = 0;    // optional �C��

	// ��L�̍\���̂̏��Ɋ�Â��Ď��ۂ̃R�}���h�v�[���𐶐����܂��B
	if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics command pool!");
	}
}

// �e�N�X�`���[�}�b�s���O�p�摜��p�ӂ��܂�
void CVulkanFramework::createTextureImage()
{
	int texWidth, texHeight, texChannels;

	// STBI_rgb_alpha: ���`���l�����Ȃ��ꍇ�A�����I�ɒǉ����܂��B
	stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	// max   : width��height����傫���T�C�Y�̕������
	// log2  : ���̒l������܂�2�ŏ��Z�ł��邩�i�e�~�b�v�}�b�v���x���͌��̃��x���̔����̂��߁j
	// floor : max�l % 2 != 0�@�̏ꍇ�Afloor�ɒ����Ȃ��ő吮���Ƃ��Ĉ����܂�
	// +1    : ���̃e�N�X�`���[���̂����x��0�Ƃ��܂�(���[�v������[i] -1)
	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(m_LogicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

	// �p�ς݃s�N�Z���z����폜
	stbi_image_free(pixels);

	// �e�N�X�`���[�C���[�W����
	createImage(
		texWidth,
		texHeight,
		m_MipLevels,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_TextureImage,
		m_TextureImageMemory
	);

	transitionImageLayout(
		m_TextureImage,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		m_MipLevels);

	// �R�s�[���������s
	copyBufferToImage(
		stagingBuffer,
		m_TextureImage,
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight)
	);

	// ��Еt��
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);

	// �~�b�v�}�b�v����
	generateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_MipLevels);
}

// createTextureImage()����̃C���[�W���C���[�W�r���[�𐶐�
void CVulkanFramework::createTextureImageView()
{
	m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

// �e�N�X�`���[�T���v���[�����FFiltering (Bilinear, Anisotropic�Ȃ�)�AAddressingMode�Ȃ�
void CVulkanFramework::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};    // �T���v���[���\����
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	// �o�C���j�A�t�B���^�����O (����VL_FILTER_NEAREST)
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	// UVW = XYZ; �e�N�X�`���[���W�ł悭�g���Ă��܂�
	// UVW often used in Texture Space Coordinates
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// VK_SAMPLER_ADDRESS_MODE_REPEAT: repeat texture
	// VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: mirror image
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: take color of edge closest to the coordinate beyond image dimensions
	// VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: Uses edge opposite to closest edge
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: returns solid color when sampling beyond dimensions
	// �����݁A�C���[�W�̊O�ŃT���v�����O���s��Ȃ����ߐݒ�𖳎����܂����A��Ԃ悭�g���Ă���̂�MODE_REPEAT�ł�


	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;    // �ő��

	// CLAMP_TO_BORDER�̏ꍇ�A�\�����ꂽ�J���[ 
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	// Texel���W [0, 1)
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	// Percentage-Closer Filtering�p
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	// �~�b�v�}�b�s���O�p
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	// samplerInfo.minLod = static_cast<float>(m_MipLevels / 2);      // �~�b�v�}�b�v�e�X�g mipmap test
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(m_MipLevels);

	if (vkCreateSampler(m_LogicalDevice, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

// ���f���̃��[�h����
void CVulkanFramework::loadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, MODEL_PATH.c_str()))    // Triangulate Faces by default
	{
		throw std::runtime_error(warn + error);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	// �S�Ă̎O�p��Iterate���āA1�̃��f���ɂ܂Ƃ߂܂�
	// Iterate over all the shapes to combine all the faces into a single model
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			vertex.pos = {
			attrib.vertices[3 * index.vertex_index + 0],
			attrib.vertices[3 * index.vertex_index + 1],
			attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0 - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			// ���_�d���t�B���^�[
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}
			m_Indices.push_back(uniqueVertices[vertex]);

			// �t�B���^�[�Ȃ�
			//m_Vertices.push_back(vertex);
			//m_Indices.push_back(m_Indices.size());
		}
	}
	// ���_���m�F�E��r
	// std::cout << "���_��: "  << m_Vertices.size() << std::endl;
}

// ���_�o�b�t�@�[����
void CVulkanFramework::createVertexBuffer()
{
	// ���_�P�� ���@�z��̗v�f��
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	// �X�e�[�W���O�o�b�t�@�[�FCPU�������[��Վ��o�b�t�@�[�B���_�f�[�^�ɓn����A�ŏI�I�Ȓ��_�o�b�t�@�[�ɓn���܂��B
	// Staging buffer: temporary buffer in CPU memory that takes in vertex array and sends it to the final vertex buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;	// void�|�C���^�[�G���񃁃����[�}�b�v�Ɏg�p���܂�

	// �����������[�Ƀ}�b�v
	vkMapMemory(m_LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

	// ���_�o�b�t�@�[�𐶐����܂�
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,		// ��ԗ��z�I�ȃ������[�t�H�[�}�b�g���A���ʂ�CPU���A�N�Z�X�ł��܂���B
		m_VertexBuffer,
		m_VertexBufferMemory);

	// ���_�f�[�^���X�e�[�W���O�o�b�t�@�[���璸�_�o�b�t�@�[�Ɉڂ�
	copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	// �p�ς݂̃X�e�[�W���O�o�b�t�@�[�ƃ������[�̌�Еt��
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);
}

// �C���f�b�N�X�o�b�t�@�[�����F���_�o�b�t�@�[�Ƃقړ����i�Ⴂ�͔Ԍ�@�@�A�A�ŕ\������Ă��܂�
void CVulkanFramework::createIndexBuffer()
{
	// �C���f�b�N�X�P�ʁ@���@�z��̗v�f��
	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();    // �ύX�_�@�@�A�A

	// �X�e�[�W���O�o�b�t�@�[�F���_�o�b�t�@�[�Ɠ���
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;
	vkMapMemory(m_LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), (size_t)bufferSize);        // �ύX�_�@�B vertices.data() --> indices.data()
	vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

	// �C���f�b�N�X�o�b�t�@�[�𐶐����܂�
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,    // �ύX�_�@�C (VK_BUFFER_USAGE_VERTEX_BIT����INDEX_BIT��)
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_IndexBuffer,            // �ύX�_�@�D  �C���f�b�N�X�o�b�t�@�[
		m_IndexBufferMemory);     // �ύX�_�@�E�@�C���f�b�N�X�o�b�t�@�[�������[

	// �C���f�b�N�X�f�[�^���X�e�[�W���O�o�b�t�@�[����C���f�b�N�X�o�b�t�@�[�Ɉڂ�
	copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);    // �ύX�_�@�F�@�R�s�[����C���f�b�N�X�o�b�t�@�[��

	// �p�ς݂̃X�e�[�W���O�o�b�t�@�[�ƃ������[�̌�Еt��
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);
}

// ���j�t�H�[���o�b�t�@�[�F�V�F�[�_�[�p��UBO(Uniform Buffer Object)�f�[�^
void CVulkanFramework::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(m_SwapChainImages.size());
	m_UniformBuffersMemory.resize(m_SwapChainImages.size());

	// �t���[������UBO�g�����X�t�H�[������K�p����
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_UniformBuffers[i],
			m_UniformBuffersMemory[i]
		);
	}
}

// �f�X�N���v�^�[�Z�b�g���i�[����ŃX�N���v�^�[�v�[���𐶐�
void CVulkanFramework::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};    // �e�t���[����1�̃f�X�N���v�^�[��p�ӂ��܂�
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());


	VkDescriptorPoolCreateInfo poolInfo{};    // �f�X�N���v�^�[�v�[���������\����
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

	if (vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}

// �f�X�N���v�^�[�Z�b�g�i�g�����X�t�H�[�����j����
void CVulkanFramework::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), m_DescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;    // �f�X�N���v�^�[�v�[��
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	m_DescriptorSets.resize(m_SwapChainImages.size());
	if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_UniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);    // ���S�ɏ㏑���FVK_WHOLE_SIZE�Ɠ���

		// Combined Image Sampler
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TextureImageView;
		imageInfo.sampler = m_TextureSampler;


		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};    // �f�X�N���v�^�[�̐ݒ�E�R���t�B�O���[�V�������\����
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;                // ���j�t�H�[���o�b�t�@�[�o�C���f�B���O�C���f�b�N�X�u0�v
		descriptorWrites[0].dstArrayElement = 0;           // �z����g���Ă��Ȃ��ꍇ�A�u0�v
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		// �f�X�N���v�^�[�Z�b�g���X�V���܂�
		vkUpdateDescriptorSets(m_LogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

// �R�}���h�v�[���̏�񂩂�R�}���h�o�b�t�@�[����
void CVulkanFramework::createCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());    // �t���[���o�b�t�@�[�T�C�Y�ɍ��킹��

	VkCommandBufferAllocateInfo allocInfo{};                    // �������[���蓖�ď��\����
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// PRIMARY:	�L���[�ɒ��ړn���܂����A���̃R�}���h�o�b�t�@�[����Ăяo���܂���B
	// can be submitted to queue for execution, but cannot be called from other command buffers
	// SECONDARY:�@�L���[�ɒ��ړn���܂��񂪁A�v���C�}���[�R�}���h�o�b�t�@�[����Ăяo���܂��B
	// cannot be submitted directly, but can be called from primary command buffers
	allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

	if (vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	// �R�}���h�o�b�t�@�[�o�^�J�n Starting command buffer recording
	for (size_t i = 0; i < m_CommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};       // �R�}���h�o�b�t�@�[�J�n���\����
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;                        // �C�Ӂ@optional
		beginInfo.pInheritanceInfo = nullptr;       // �p���FSECONDARY�̏ꍇ�̂݁i�ǂ̃R�}���h�o�b�t�@�[����Ăяo�����j
													// only for secondary command buffers (which state to inherit from)

		if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		// �����_�[�p�X�J�n
		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo{};		// �����_�[�p�X���\����
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0, 0 };

		// �p�t�H�[�}���X�̍œK���̂��߁A�����_�[�̈���A�^�b�`�����g�T�C�Y�ɍ��킹�܂��B
		// match render area to size of attachments for best performance
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		// createRenderPass(): VK_ATTACHMENT_LOAD_OP_CLEAR�̃N���A�l (clearColor)
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };    // ��
		clearValues[1].depthStencil = { 1.0f, 0 };            // �f�v�X�X�e���V���N���A�l (1.0f: �t�@�[ Far Plane)

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// ���ۂ̃����_�[�p�X���J�n���܂�
		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// �O���t�B�b�N�X�p�C�v���C���ƂȂ��܂�
		vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// ���_�o�b�t�@�[�����o�C���h������`��̏����͊����ł�
		VkBuffer vertexBuffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, vertexBuffers, offsets);

		// �C���f�b�N�X�o�b�t�@�[
		vkCmdBindIndexBuffer(m_CommandBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);    // VK_INDEX_TYPE_UINT16

		// �f�X�N���v�^�[�Z�b�g���o�C���h���܂�
		vkCmdBindDescriptorSets(
			m_CommandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0,
			1,
			&m_DescriptorSets[i],
			0,
			nullptr)
			;

		// �`��R�}���h�i�C���f�b�N�X�o�b�t�@�[�j
		vkCmdDrawIndexed(m_CommandBuffers[i], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
		// �����@�F�R�}���h�o�b�t�@�[
		//     �A�F���_���i���_�o�b�t�@�[�Ȃ��ł����_��`�悵�Ă��܂��B�j
		//     �B�F�C���X�^���X���i�C���X�^���X�����_�����O�p�j
		//     �C�F�C���f�b�N�X�o�b�t�@�[�̍ŏ��_����̃I�t�Z�b�g
		//     �C�F�C���f�b�N�X�o�b�t�@�[�ɑ����I�t�Z�b�g (�g�����͂܂��s���j
		//     �C�F�C���X�^���X�̃I�t�Z�b�g�i�C���X�^���X�����_�����O�p�j

		// arguments
		// first    : commandBuffer
		// second   : vertexCount  : even without vertex buffer, still drawing 3 vertices (triangle)
		// third    : instanceCount: used for instanced rendering, otherwise 1)
		// fourth   : firstIndexOffset : offset to start of index buffer (1 means GPU reads from second index)
		// fifth    : indexAddOffset   : offset to add to indices (not sure what this is for)
		// sixth    : instanceOffset   : used in instanced rendering

		// �����_�[�p�X���I�����܂�
		vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

// ���������̐�p�I�u�W�F�N�g����
void CVulkanFramework::createSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS
			|| vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization objects for a frame!");
		}
	}
}



//====================================================================================
// 15X : �ėp�I�u�W�F�N�g�����E�w���p�[�֐�
// All-purpose object creation / helper functions
//====================================================================================

// �ėp�C���[�W�r���[�����֐�
VkImageView CVulkanFramework::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;

	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_LogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view!");
	}

	return imageView;
}

// �ėp�C���[�W�����֐�
void CVulkanFramework::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = numSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_LogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_LogicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate image memory!");
	}

	vkBindImageMemory(m_LogicalDevice, image, imageMemory, 0);
}

// �T�|�[�g����Ă���i�K�p�ł���j��ԗ��z�ȃt�H�[�}�b�g���������܂��iTiling��Features�ɂ���ĈقȂ�܂��j
VkFormat CVulkanFramework::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	throw std::runtime_error("Failed to find supported format!");
}

// �V�F�[�_�[�R�[�h���p�C�v���C���Ɏg�p����ۂɃV�F�[�_�[���W���[����wrap����K�v������܂��B
VkShaderModule CVulkanFramework::createShaderModule(const std::vector<char>& code)
{
	// specify a pointer to the buffer with the bytecode and length

	VkShaderModuleCreateInfo createInfo{};    // �V�F�[�_�[���W���[�����\����
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();        // �T�C�Y

	// �o�C�g�R�[�h�̃T�C�Y�̓o�C�g�ŕ\���܂����A�o�C�g�R�[�h�|�C���^�[��uint32_t�^�ł��B
	// �����ŁA�|�C���^�[��recast����K�v������܂��B
	// size of bytecode is in bytes, but bytecode pointer is uint32_t pointer rather than a char pointer.
	// thus, we need to recast the pointer as below.
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	// ��L�̍\���̂Ɋ�Â��ăV�F�[�_�[���W���[�𐶐����܂��B
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module!");
	}
	return shaderModule;
}

// �t�@�C���ǂݍ���
std::vector<char> CVulkanFramework::readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate    // ate: �K������t�@�C����ǂށ@start reading at EOF 
		| std::ios::binary);                      // binary: �o�C�i���t�@�C���Ƃ��Ĉ��� read file as binary file (avoid text transformations)

	if (file.is_open() == false)
	{
		throw std::runtime_error("Failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();     // telg(): �C���v�b�g�X�g���[���ʒu��߂� returns input stream position. 
	std::vector<char> buffer(fileSize);         // �K��(EOF: End of File) ����ǂݍ��ނƎ��ۂ̃t�@�C���T�C�Y�����ł��܂��B
												// EOF essentially gives us the size of the file for the buffer

	file.seekg(0);                              // �t�@�C���̓��ɖ߂�@return to beginning of file
	file.read(buffer.data(), fileSize);         // read(x, y): y�܂œǂݍ��݁Ax�o�b�t�@�[�ɃA�T�C�����܂� 
												// read up to count y and assign to buffer x

	// ����A�S�Ẵo�C�g����C�ɓǂݍ��� in this case, read all the bytes at once

	// std::cout << fileSize << std::endl;		// �t�@�C���v���p�e�B�[�ŃT�C�Y�������Ă邩���m�F�ł��܂��B
												// check file byte size with actual file (properties)
	file.close();

	return buffer;
}

// �ėp�o�b�t�@�[�����֐�
void CVulkanFramework::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};                          // �o�b�t�@�[���\����
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;                                 // ���[�X�P�[�X�F���_�o�b�t�@�[
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;       // ���L���[�h�iSwapChain�����Ɠ����j�F����O���t�B�b�N�X�L���[��p

	// ��L�̍\���̂Ɋ�Â��Ď��ۂ̃o�b�t�@�[�𐶐����܂��B
	if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};    // �������[���蓖�ď��\����
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	// ��L�̍\���̂Ɋ�Â��Ď��ۂ̃������[�m�ۏ��������s���܂��B
	if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	// �m�ۂ��ꂽ�������[���蓖�Ă𒸓_�o�b�t�@�[�Ƀo�C���h���܂�
	vkBindBufferMemory(m_LogicalDevice, buffer, bufferMemory, 0);
}

// �o�b�t�@�[�R�s�[�֐�
void CVulkanFramework::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	// �R�s�[�̈�m��
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;    // �C�� optional
	copyRegion.dstOffset = 0;    // �C�� optional
	copyRegion.size = size;

	// �R�s�[���̃o�b�t�@�[�̒��g���R�s�[��̃o�b�t�@�[�ɃR�s�[����R�}���h���L�^���܂�
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

// �o�b�t�@�[�����C���[�W�Ɉڂ�
void CVulkanFramework::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};    // �R�s�[���\����
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	// �R�s�[����
	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(commandBuffer);
}

// �C���[�W���C�A�E�g�����̃��C�A�E�g�ɑJ�ڂ��܂�
void CVulkanFramework::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	// �L���[������ς������ꍇ�A�ȉ��̐ݒ肪�K�v�ł�
	// �f�t�H���g�ł͂���܂���I�@�K���ݒ肷�邱��
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("Unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,              // ����VK_DEPENDENCY_BY_REGION_BIT
		0, nullptr,     // �������[�o���A
		0, nullptr,     // �o�b�t�@�[�������[�o���A
		1, &barrier     // �C���[�W�������o���A�i���ݎg�p���j
	);

	endSingleTimeCommands(commandBuffer);
}

// ��񂾂��g�p�\��̃R�}���h���J�n���܂�
VkCommandBuffer CVulkanFramework::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

// ��񂾂��g�p�\��̃R�}���h���I�������܂�
void CVulkanFramework::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_GraphicsQueue);

	vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
}

// �~�b�v�}�b�v�����֐�
void CVulkanFramework::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// �n���ꂽ�t�H�[�}�b�g��Linear Blitting���T�|�[�g���邩���m�F
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("Texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	// �~�b�v�}�b�v�������[�v
	for (uint32_t i = 1; i < mipLevels; i++)    // ���[�v��1����n�߂�̂͒��ӓ_
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;    // Optimal: �N�I���e�B�[�œK��
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		// �摜����~�b�v�}�b�v�𐶐������p���\����
		VkImageBlit blit{};                                // blit: Bit Block Transfer
		blit.srcOffsets[0] = { 0, 0, 0 };                  // blit���̃I�t�Z�b�g�i�͈́j Source Offset
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		// blit��̃I�t�Z�b�g�i�̈�j Destination Offset
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = {
				mipWidth > 1 ? mipWidth / 2 : 1,     // �������Z�q�ł̃~�b�v�}�b�v�v�Z
				mipHeight > 1 ? mipHeight / 2 : 1,     // conditional/tertiary operator mipmap calculation
				1
		};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(
			commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR    // ���
		);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		// �~�b�v�}�b�v������A�~�b�v�}�b�v�T�C�Y / 2�i�[�����Z������邽�߁A�l��1���`�F�b�N�j
		// �e�N�X�`���[�̏c�Ɖ����ꓙ���Ȃ��ꍇ�A�ǂ��炩�����1�̂܂܂ɂȂ�܂�
		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	// �Ō�̃~�b�v�}�b�v���x����VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL����VK_IMAGE_SHADER_READ_ONLY_OPTIMAL��
	// (�Ō�̃��x���̓��[�v�ɓ���Ȃ��������߁j
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

//====================================================================================
// 20X : �X�V�p�֐�
// Update Functions
//====================================================================================

// �E�B���h�E���T�C�Y�R�[���o�b�N
void CVulkanFramework::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	CVulkanFramework* app = reinterpret_cast<CVulkanFramework*>(glfwGetWindowUserPointer(window));
	app->CVulkanFramework::m_FramebufferResized = true;
}

// SwapChain���E�B���h�E�T�[�t�F�X�ɑΉ����Ă��Ȃ��ꍇ�i�E�C���h�E���T�C�Y�j�ASwapChain���Đ���
// �K�v������܂��BSwapChain���̓E�B���h�E�T�C�Y�Ɉˑ�����I�u�W�F�N�g��SwapChain�Ɠ�����
// �Đ������Ȃ��Ƃ����܂���B
// Recreate swap chain and all creation functions for the object that depend on the swap chain or the window size.
// This step is to be implemented when the swap chain is no longer compatible with the window surface;
// i.e. window size changing (and thus the extent values are no longer consistent)
void CVulkanFramework::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_Window, &width, &height);
	while (width == 0 || height == 0)                       // �E�B���h�E���ŏ�����Ԃ̏ꍇ window is minimized
	{
		glfwGetFramebufferSize(m_Window, &width, &height);  // �ŏ����̏�O�����������܂ŃE�C���h�E��������U��~����
		glfwWaitEvents();                                   // window paused until window in foreground
	}

	vkDeviceWaitIdle(m_LogicalDevice);    // �g�p���̃��\�[�X�̏������I���܂ő҂��ƁBdo not touch resources that are still in use, wait for them to complete.

	cleanupSwapChain();         // SwapChain�̌�Еt��
	createSwapChain();          // SwapChain���̂��Đ���
	createImageViews();         // SwapChain���̉摜�Ɉˑ�
	createRenderPass();         // SwapChain���̉摜�̃t�H�[�}�b�g�Ɉˑ�
	createGraphicsPipeline();   // �r���[�|�[�g�AScissor�Ɉˑ�
	createColorResources();     // �`�揈���ɉe�����܂� 
	createDepthResources();     // �f�v�X�o�b�t�@�[���]���[�V�������E�C���h�E���T�C�Y�ɍ��킹�܂�
	createFramebuffers();       // SwapChain���̉摜�Ɉˑ�
	createUniformBuffers();     // SwapChain���̉摜�Ɉˑ�
	createDescriptorPool();     // SwapChain���̉摜�Ɉˑ�
	createDescriptorSets();     // SwapChain���̉摜�Ɉˑ�
	createCommandBuffers();     // SwapChain���̉摜�Ɉˑ�
}

// ���j�t�H�[���o�b�t�@�[�X�V�iUBO�j�F�}�g���b�N�X�g�����X�t�H�[���A�J�����ݒ�
void CVulkanFramework::updateUniformBuffer(uint32_t currentImage)
{
	//// startTime�AcurrentTime�̎��ۂ̃f�[�^�^: static std::chrono::time_point<std::chrono::steady_clock> 
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};  // MVP�g�����X�t�H�[�����\����

	//// M(Model: ���t���[���AZ����X����]������
	// ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//
	// V(View): �����@eye�ʒu, center�ʒu, up��
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// P(Projection): �����@45���o�[�e�B�J��FoV, �A�X�y�N�g��A�j�A�A�t�@�[�r���[�v���[��
	// arguments: field-of-view, aspect ratio, near and far view planes 
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);

	//// ���XGLM��OpelGL�ɑΉ����邽�߂ɐ݌v����Ă��܂��iY���̃N���b�v���W���t���ɂȂ��Ă��܂��j�B
	//// Vulkan�ɑΉ����邽�߂ɃN���b�v���W��Y�����u���ɖ߂��v��ł��B�������Ȃ��ƕ`��͂Ђ�����Ԃ���ԂɂȂ��Ă��܂��܂��B

	//// GLM was initially developed for OpenGL where the Y-coordinate of the clip coordinates is reversed.  
	//// To compensate, multiply the Y-coordinate of the projection matrix by -1 (reverting back to normal).  
	//// Not doing this results in an upside-down render.
	ubo.proj[1][1] *= -1;

	//// UBO�������݂̃��j�t�H�[���o�b�t�@�[�ɂ����܂�
	void* data;
	vkMapMemory(m_LogicalDevice, m_UniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_LogicalDevice, m_UniformBuffersMemory[currentImage]);
}

// �t���[����`��
void CVulkanFramework::drawFrame()
{
	// �t�F���X������҂��܂�
	vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	// SwapChain�������ꂽ�ꍇ  �i�����ꂽ�j
	// check if swap chain is out of date
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();    // SwapChain���Đ���
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	// ���j�t�H�[���o�b�t�@�[�X�V
	updateUniformBuffer(imageIndex);

	// ���݂̉摜���ȑO�̃t���[���Ŏg���Ă��邩�i�t�F���X��҂��Ă��邩�j
	// check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_LogicalDevice, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	// ���݂̉摜�����݂̃t���[���Ŏg���Ă���悤�Ɏ����B
	// mark the image as now being in use by this frame
	m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

	VkSubmitInfo submitInfo{};    // �L���[�����E��o���\����
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;    // ���s�O�ɑ҂Z�}�t�H�@semaphore to wait on before execution
	submitInfo.pWaitDstStageMask = waitStages;      // �҂�����p�C�v���C���X�e�[�W�@stage(s) of the pipeline to wait

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;    // �I����̂Ƃ��ɋN������Z�}�t�H  semaphores to signal once command buffer(s) have finished execution

	vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};    // �v���[���g���\����
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };      // SwapChain�\����
	presentInfo.swapchainCount = 1;                     // SwapChain���i���݁F�P�j
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	// �C�Ӂi������SwapChain�̏ꍇ�A�eSwapChain�̌��ʂ��i�[����z��j
	// array of VkResult values to check for each swap chain if presentation was successful if using multiple swap chains
	presentInfo.pResults = nullptr;

	// ���U���g��SwapChain�ɓn���ĕ`�悵�܂�  submit the result back to the swap chain to have it show on screen
	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR    // SwapChain���p�ꂽ
		|| result == VK_SUBOPTIMAL_KHR           // SwapChain���œK������Ă��Ȃ�
		|| m_FramebufferResized == true)         // �t���[���o�b�t�@�[�̃T�C�Y���ύX���ꂽ
	{
		m_FramebufferResized = false;
		recreateSwapChain();                  // SwapChain���Đ������܂�
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;    // ���̃t���[���Ɉړ��@advance to next frame
}

//====================================================================================
// 30X : �f�o�C�X�E�R���|�[�l���g�v���p�e�B�[�l���E�N�G���[�p�֐�
// Device/Component Property, Query Functions
//====================================================================================

// �g�p����GPU�̑Ή��ł���T���v���r�b�g�����l������֐�
VkSampleCountFlagBits CVulkanFramework::getMaxUseableSampleCount()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts =
		physicalDeviceProperties.limits.framebufferColorSampleCounts
		& physicalDeviceProperties.limits.framebufferDepthSampleCounts;


	// �ő吔�D��
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }    // ���݂�GPU: NVIDIA GeForce RTX 2070 SUPER
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

// SwapChain�T�|�[�g�m�F
SwapChainSupportDetails CVulkanFramework::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);    // �T�[�t�F�X�P�[�p�r���e�B surface capabilities

	uint32_t formatCount;                                                                   // �K�p�ł���t�H�[�}�b�g�J�E���g
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);         // �Ή����Ă���T�[�t�F�X�t�H�[�}�b�g���m�F 	
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);    // �K�p�ł���t�H�[�}�b�g���i�[�ł���悤�Ƀ��T�C�Y���܂��B
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());    // �f�[�^����
	}

	uint32_t presentModeCount;                                                              // �K�p�ł���v���[�g���[�h�J�E���g
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &presentModeCount, nullptr);    // �Ή����Ă���v���[���e�[�V�������[�h���m�F
	if (presentModeCount != 0)
	{
		// �K�p�ł���v���[���e�[�V�������[�h���i�[�ł���悤�Ƀ��T�C�Y���܂��B
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());    // �f�[�^����
	}
	return details;
}

// GPU����肽�������ɓK�؂��̊m�F
bool CVulkanFramework::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);     // VK_QUEUE_GRAPHICS_BIT��T���Ă��܂�

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;     // �Œ��1�̃C���[�W�t�H�[�}�b�g��1�̃v���[���e�[�V�������[�h������ł��܂�����
										// At least one supported image format and one supported presentation mode given the window surface
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	// �W�I���g���[�V�F�[�_�[�݂̂�I���������ꍇ�G�@sample if wanting to narrow down to geometry shaders:
	// return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;		
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

// ���W�J���f�o�C�X���G�N�X�e���V�����ɑΉ��ł��邩�̊m�F
bool CVulkanFramework::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;	// �G�N�X�e���V�����J�E���g
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);	// �g�p�\�G�N�X�e���V�����̃x�N�g��

	// �S�ẴG�N�X�e���V�����v���p�e�B��availableExtensions�ɑ�����܂�
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// �K�v�ȃG�N�X�e���V�����̃x�N�g��
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);    // �K�v�ȃG�N�X�e���V���������������ꍇ�Aset����폜���܂��B
															  // erase if required extension is in the vector
	}

	bool isEmpty = requiredExtensions.empty();    // �S�Ă̕K�v�ȃG�N�X�e���V���������������ꍇ�ATrue	
	return isEmpty;                               // if all the required extension were present (and thus erased), returns true
}

// �L���[��ތ����E�I��
QueueFamilyIndices CVulkanFramework::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);                 // �L���[��ނ����

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);                         // �L���[��ރJ�E���g�ɂ��x�N�g���𐶐����܂�
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());    // �����x�N�g���ɑ��

	int i = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

		if (presentSupport = VK_TRUE)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())    // �L���[��ނ����������ꍇ�Abreak / If queueFamily is found, exit early
		{
			break;
		}
		i++;
	}
	return indices;
}

// �T�[�t�F�X�t�H�[�}�b�g�I��
VkSurfaceFormatKHR CVulkanFramework::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB    // sRGB���g����Ȃ���󂵂܂��B
			&& availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	// ����ȊO�A�ŏ����������t�H�[�}�b�g���g���Ă������ł� otherwise, just use the first format that is specified
	return  availableFormats[0];

}

// �X���b�v�v���[���g���[�h��I��
VkPresentModeKHR CVulkanFramework::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)    // �g���v���o�b�t�@�����O�@triple buffering (less latency)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;    // if VK_PRESENT_MODE_MAILBOX_KHR is not available, use a guaranteed available mode
}

// ���]���[�V�����ݒ�  extent = resolution of the swap chain images
VkExtent2D CVulkanFramework::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites)
{
	// Vulkan���s�N�Z���P�ʂŐݒ肳��Ă��܂����X�N���[�����]���[�V������2D���W�ŕ\���Ă��܂��B
	// �s�N�Z���P�ʂł̃X�N���[�����]���[�V�������l�����邽�߂�glfwGetFramebufferSize()���g���K�v������܂��B
	// Vulkan works with pixel units, but the screen resolution (WIDTH, HEIGHT) is in screen coordinates.
	// Use glfwGetFramebufferSize() to query the resolution of the window in pixel before matching to min/max image extent

	// UINT32_MAX: uint32_t�̍ő�l maximum value of uint32_t
	// �����Ŏg���ƁA�����I�Ɉ�ԗ��z�ȃ��]���[�V������I�����Ă���܂��B
	// special value to indicate that we will be picking the resolution that best matches the window
	if (capabilites.currentExtent.width != UINT32_MAX)
	{
		return capabilites.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilites.minImageExtent.width,
			std::min(capabilites.maxImageExtent.width, actualExtent.width));

		actualExtent.height = std::max(capabilites.minImageExtent.height,
			std::min(capabilites.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

// �f�v�X�t�H�[�}�b�g���������܂�
VkFormat CVulkanFramework::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

// �K�؂ȃ������[�^�C�v������
uint32_t CVulkanFramework::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;    // �������[�^�C�v�A�������[�q�[�v
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	// �K�؂ȃ������[�^�C�v���l��
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i)    // corresponding bit is set to 1
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)     // bitwise AND �_���� == ���� properties
		{
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type!");    // �K�؂ȃ������[�^�C�v��������Ȃ������ꍇ
}

// �n���ꂽ�f�v�X�t�H�[�}�b�g���X�e���V���R���|�[�l���g�����Ă��邩
bool CVulkanFramework::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

//====================================================================================
// 40X : �I���E��Еt���֐�
// Cleanup Functions
//====================================================================================

// SwapChain���Đ�������O�ɌÂ�SwapChain���폜����֐�
// before recreating swap chain, call this to clean up older versions of it
void CVulkanFramework::cleanupSwapChain()
{
	vkDestroyImageView(m_LogicalDevice, m_ColorImageView, nullptr);
	vkDestroyImage(m_LogicalDevice, m_ColorImage, nullptr);
	vkFreeMemory(m_LogicalDevice, m_ColorImageMemory, nullptr);

	vkDestroyImageView(m_LogicalDevice, m_DepthImageView, nullptr);
	vkDestroyImage(m_LogicalDevice, m_DepthImage, nullptr);
	vkFreeMemory(m_LogicalDevice, m_DepthImageMemory, nullptr);

	for (VkFramebuffer framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);
	}

	// �R�}���h�v�[�����폜�����R�}���h�o�b�t�@�[���J���B�����̃v�[����V�����R�}���h�o�b�t�@�[�Ŏg���܂��B
	// Frees up command buffers without destroying the command pool; reuse the existing pool to allocate new command buffers
	vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()),
		m_CommandBuffers.data());

	vkDestroyPipeline(m_LogicalDevice, m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);

	for (VkImageView imageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(m_LogicalDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(m_LogicalDevice, m_UniformBuffers[i], nullptr);
		vkFreeMemory(m_LogicalDevice, m_UniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
}

// ��ЂÂ�
// 3�ڂ�nullptr: �C�ӂ̃R�[���o�b�N����
void CVulkanFramework::cleanup()
{
	cleanupSwapChain();
	
	vkDestroySampler(m_LogicalDevice, m_TextureSampler, nullptr);
	vkDestroyImageView(m_LogicalDevice, m_TextureImageView, nullptr);

	vkDestroyImage(m_LogicalDevice, m_TextureImage, nullptr);
	vkFreeMemory(m_LogicalDevice, m_TextureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(m_LogicalDevice, m_DescriptorSetLayout, nullptr);

	vkDestroyBuffer(m_LogicalDevice, m_IndexBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, m_IndexBufferMemory, nullptr);

	vkDestroyBuffer(m_LogicalDevice, m_VertexBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, m_VertexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);

	vkDestroyDevice(m_LogicalDevice, nullptr);

	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);

	glfwDestroyWindow(m_Window);	// uninit window
	glfwTerminate();				// uninit glfw
}








