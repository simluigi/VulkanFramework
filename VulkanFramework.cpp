/*======================================================================
Vulkan Presentation : VulkanFramework.cpp
Author:			Sim Luigi
Last Modified:	2020.12.13

<< 気づいたこと・メモ >>　※間違っている場合、教えていただければ幸いです。

＊　VulkanAPIのネーミングコンベンションについて：
	
	小文字「 k 」： 関数          vkCreateInstance()   
	大文字「 K 」： データ型      VkInstance
	全て大文字　 ： マクロ        VK_KHR_SWAPCHAIN_EXTENSION_NAME

	Vk/Vk/VK: Vulkan
	KHR     : Khronos （Vulkan、OpenCLのディヴェロッパー

＊　Vulkan上のオブジェクトを生成するとき、ほとんど以下の段階で実装します。

                                                                  例：インスタンス	
	1.) オブジェクト情報構造体「CreateInfo」を作成します。        VkInstanceCreateInfo createInfo{};
	2.) 構造体の種類を選択します。                                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	3.) 構造体の設定・フラッグを定義します。                      createInfo.flags = 0;
	4.) CreateInfo構造体に基づいてオブジェクト自体を生成します。  vkCreateInstance(&createInfo, nullptr, &m_Instance):

＊	Vulkan専用のデバッグ機能「Validation Layer」「バリデーションレイヤー」について：
	複雑ですが、簡単にイメージすると、画像編集ソフト（Photoshop、GIMPなど）のレイヤーと同じように重ねているの方が想像しやすいです。
	
	例：処理A
	　→レイヤー�@：正常
		→レイヤー�A：正常
		　→レイヤー�B：エラー（デバッグメッセンジャー表示、エラーハンドリング）

＊　Vulkanのシェーダーコードについて：
	GLSLやHLSLなどのハイレベルシェーダー言語（HLSLはまさにHighLevelShadingLanguage)と違って、
	VulkanのシェーダーコードはSPIR-Vというバイトコードフォーマットで定義されています。

	GPUによるハイレベルシェーダーコードの読み方が異なる場合があり、
	あるGPUでシェーダーがうまく動いても他のGPUで同じ結果が出せるとは限りません。
	機械語のローレベルランゲージではないが、HLSLと機械語の間のインターメディエイトレベル
	言語としてクロスプラットフォーム処理に向いています。

	なお、GLSLなどのシェーダーはSPIR-Vに変換するツールを使えばVulkanで使えるようになるはずです。

＊	Vulkanのあらゆるコマンド（描画、メモリ転送）などが関数で直接呼ばれません。オブジェクトと同じように
	コマンドバッファーを生成して、そのコマンドをコマンドバッファーに登録（格納）します。
	複雑なセットアップの代わりに、コマンドが事前に処理できます。

＊	フラグメントシェーダー：　ピクセルシェーダーと同じです。

＊　VulkanのBool関数に限って、true・falseではなく、VK_TRUE・VK_FALSEを使用することです。
	同じく、Vulkan専用 VK_SUCCESS、VK_ERROR_○○コールバックも用意されています。

＊　ステージングバッファーの使用の理由
	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT： 一番理想的なメモリーフォーマットが、ローカルメモリーが直接アクセスできません。
	そこで、そのメモリータイプが使えるCPU上の「ステージングバッファー」を生成し、頂点配列（データ）に渡します。
	そしてステージングバッファーの情報をローカルメモリーに格納されている頂点バッファーに移します。

=======================================================================*/
#include "VulkanFramework.h"

#define TINYOBJLOADER_IMPLEMENTATION        // tinyobjloaderモデル読み込み
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL             // glm型のHash
#define GLM_FORCE_RADIANS                   // glm::rotate関数をラジアンで処理する設定
#define GLM_FORCE_DEPTH_ZERO_TO_ONE	        // GLMのプロジェクションマトリックスのZソート値は -1.0〜1.0（OpenGL対応のため）
// Vulkan対応にするために 0.0〜1.0 に設定する必要があります
// GLM uses depth ranges of -1.0 to 1.0 in accordance with OpenGL standards.
// To use GLM projection matrices in Vulkan for depth buffering, set values to 0.0〜1.0

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>       // 時間管理；フレームレートに関わらずジオメトリーが90°回転させます
                        // time manager: will allow us to rotate geometry 90 degress regardless of frame rate

#include <iostream>     // 基本I/O
#include <vector>       // std::vector
#include <set>          // std::set
#include <array>        // std::array
#include <map>          // std::map
#include <unordered_map>// std::unordered_map 頂点重複を避けるため
#include <cstring>      // strcpy, など
#include <optional>     // C++17 and above
#include <algorithm>    // std::min/max : chooseSwapExtent()
#include <cstdint>      // UINT32_MAX   : in chooseSwapExtent()
#include <stdexcept>    // std::runtime error、など
#include <cstdlib>      // EXIT_SUCCESS・EXIT_FAILURE : main()
#include <fstream>      // シェーダーのバイナリデータを読み込む　for loading shader binary data
#include <glm/glm.hpp>  // glm::vec2, vec3 : Vertex構造体

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "Asset/Model/viking_room.obj";
const std::string TEXTURE_PATH = "Asset/Texture/viking_room.png";

// 同時に処理されるフレームの最大数 
// how many frames should be processed concurrently 
const int MAX_FRAMES_IN_FLIGHT = 2;		

// Vulkanのバリデーションレイヤー：SDK上のエラーチェック仕組み
// Vulkan Validation layers: SDK's own error checking implementation
const std::vector<const char*> validationLayers =				
{
	"VK_LAYER_KHRONOS_validation"
};

// Vulkanエクステンション（今回はSwapChain)
const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME    // 誤字を避けるためのマクロ定義
};

// NDEBUG = Not Debug	
#ifdef NDEBUG					
// バリデーションレイヤー無効
const bool enableValidationLayers = false;
#else
// バリデーションレイヤー有効
const bool enableValidationLayers = true;
#endif

//====================================================================================
// 00X : デバッグ用関数
// Debug-related Functions
//====================================================================================

// デバッグメッセンジャー生成
// エクステンション関数：自動的にロードされていません。
// 関数アドレスをvkGetInstanceProcedureAddr()で特定できます。
// extension function; not automatically loaded. Need to specify address via	
// vkGetInstanceProcedureAddr()

// クラス外で定義：デバッグメッセンジャー生成
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

// クラス外で定義：デバッグメッセンジャー削除
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

// デバッグコールバック
VKAPI_ATTR VkBool32 VKAPI_CALL CVulkanFramework::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,     // エラーの重要さ；比較オペレータで比べられます。
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

// デバッグメッセージ構造体に必要なエクステンションを代入する関数
void CVulkanFramework::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};    // デバッグメッセンジャー情報構造体
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|*/ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

}

// デバッグメッセージ設定
void CVulkanFramework::setupDebugMessenger()
{
	if (enableValidationLayers == false)    // デバッグモードではない場合、無視します  Only works in debug mode
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

// バリデーションレイヤーがサポートされているかの確認
bool CVulkanFramework::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);                   // バリデーションレイヤー数を特定

	std::vector<VkLayerProperties> availableLayers(layerCount);                 // カウントによりベクトルを生成します
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());    // 情報をベクトルに代入

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			// レイヤー名が既存のバリデーションレイヤーに合ってる場合 if layer name matches existing validation layer name
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;		// 見つかったフラッグ
				break;
			}
		}

		// 今までのレイヤーが既存のレイヤーリストに見つかった限り、全てのレイヤーが確認するまでループが続きます
		// as long as layer is found in the list, loop will continue until all validation layers have been verified
		if (layerFound == false)
		{
			return false;
		}
	}
	return true;
}

// 必要なエクステンションの一覧（デバッグ機能がオン・オフによって異なります）
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

// メインループ
void CVulkanFramework::mainLoop()
{
	while (glfwWindowShouldClose(m_Window) == false)
	{
		glfwPollEvents();    // イベント待機  Update/event checker
		drawFrame();         // フレーム描画
	}

	// プログラム終了（後片付け）の前に、既に動いている処理を済ませます。
	// let logical device finish operations before exiting the main loop 
	vkDeviceWaitIdle(m_LogicalDevice);
}




//====================================================================================
// 10X : オブジェクト生成・初期化用関数
// Object Creation/Initialization Functions
//====================================================================================

// ウインドウ初期化
void CVulkanFramework::initWindow()
{
	glfwInit();    // GLFW初期化

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);    // OPENGLコンテクストを作成しない！
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);       // ユーザーでウィンドウリサイズ有効

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);    // 上記のパラメータでウィンドウを生成します
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

// Vulkan初期化
void CVulkanFramework::initVulkan()
{
	createInstance();			    // インスタンス生成			
	setupDebugMessenger();          // デバッグコールバック設定
	createSurface();                // ウインドウサーフェス生成
	pickPhysicalDevice();           // Vulkan対象グラフィックスカードの選択
	createLogicalDevice();          // グラフィックスカードとインターフェースするデバイス設定
	createSwapChain();              // SwapChain生成
	createImageViews();             // SwapChain用の画像ビュー生成
	createRenderPass();             // レンダーパス
	createDescriptorSetLayout();    // リソースでスクリプターレイアウト 
	createGraphicsPipeline();       // グラフィックスパイプライン生成
	createColorResources();         // カラーリソース生成（MSAA)
	createDepthResources();         // デプスリソース生成
	createFramebuffers();           // フレームバッファ生成（デプスリソースの後）
	createCommandPool();            // コマンドバッファーを格納するプールを生成
	createTextureImage();           // テクスチャーマッピング用画像生成
	createTextureImageView();       // テクスチャーをアクセスするためのイメージビュー生成
	createTextureSampler();         // テクスチャーサンプラー生成
	loadModel();                    // モデルデータを読み込み
	createVertexBuffer();           // 頂点バッファー生成
	createIndexBuffer();		      // インデックスバッファー生成
	createUniformBuffers();         // ユニフォームバッファー生成
	createDescriptorPool();         // デスクリプターセットを格納するプールを生成
	createDescriptorSets();         // デスクリプターセットを生成
	createCommandBuffers();         // コマンドバッファー生成
	createSyncObjects();            // 処理同期オブジェクト生成
}

// Vulkanインスタンス生成 Create Vulkan Instance
void CVulkanFramework::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())    // デバッグモードの設定でバリデーションレイヤー機能がサポートされない場合
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};    // Vulkanアプリケーション情報構造体を生成
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

	// デバッグモードオンの場合、バリデーションレイヤー名を定義します
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

	// 上記の構造体の情報に基づいて実際のインスタンスを生成します。
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance!");
	}
}

// サーフェス生成（GLFW）
// Surface Creation
void CVulkanFramework::createSurface()
{
	if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
}

// 物理デバイス（グラフィックスカードを選択）Select compatible GPU
void CVulkanFramework::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);    // Vulkan対応のデバイス（GPU)を数える
	if (deviceCount == 0)                                             // 見つからなかった場合、エラー表示
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);                     // 数えたGPUに基づいて物理デバイスベクトルを生成
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());   // デバイス情報をベクトルに代入

	for (const VkPhysicalDevice device : devices)     // 見つかったデバイスはやろうとしている処理に適切かどうかを確認します
	{                                                 // evaluate each physical device if suitable for the operation to perform
		if (isDeviceSuitable(device) == true)
		{
			m_PhysicalDevice = device;
			m_MSAASamples = getMaxUseableSampleCount();   // 選択したサンプルビットの最大数を獲得
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

// ロジカルデバイス生成 Create Logical Device to interface with GPU
void CVulkanFramework::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);    // ロジカルデバイスキュー準備　Preparing logical device queue

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;               // ロジカルデバイスキュー生成情報
	std::set<uint32_t> uniqueQueueFamilies =
	{ indices.graphicsFamily.value(), indices.presentFamily.value() };	 // キュー種類（現在：グラフィックス、プレゼンテーション）

	float queuePriority = 1.0f;    // 優先度；0.0f（低）〜1.0f（高）

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
	deviceFeatures.samplerAnisotropy = VK_TRUE;    // Anisotropy有効
	deviceFeatures.sampleRateShading = VK_TRUE;    // サンプルシェーディング有効


	VkDeviceCreateInfo createInfo{};    // ロジカルデバイス生成情報構造体
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();    // パラメータのポインター　pointer to the logical device queue info (above)

	createInfo.pEnabledFeatures = &deviceFeatures;             // currently empty (will revisit later)

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// 上記のパラメータに基づいて実際のロジカルデバイスを生成します。
	// Creating the logical device itself
	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);    //　グラフィックスキュー graphics queue
	vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);      //　プレゼンテーションキュー presentation queue
}

// スワップチェイン生成（画像の切り替え）
void CVulkanFramework::createSwapChain()
{
	// GPUのSwapChainサポート情報を読み込む
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// +1の理由：
	// SwapChainから次のイメージを獲得する前に、たまにはドライバーの内部処理の終了を待たないといけません。
	// そこで、余裕な1つ（以上）のイメージを用意できるように設定するのはおすすめです。

	// why +1?
	// sometimes we may have to wait on the driver to perform internal operations before we can acquire
	// another image to render to. Therefore it is recommended to request at least one more image than the minimum.
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0                 // zero here means there is no maximum!
		&& imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};    // SwapChain生成情報構造体
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;                                // stereoscopic3D 以外なら「１」 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    // SwapChainを利用する処理　what operations to use swap chain for 

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)    // グラフィックスとプレゼンテーションキューが異なる場合
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else    // グラフィックスとプレゼンテーションキューが同じ（現代のデバイスはこういう風に設定されています）。
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	// パフォーマンス最適
		createInfo.queueFamilyIndexCount = 0;        // 任意 optional
		createInfo.pQueueFamilyIndices = nullptr;    // 任意 optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;    // アルファチャンネル：不透明
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;               // TRUE : オクルージョンされたピクセルの色を無視　don't care about color of obscured pixels
	createInfo.oldSwapchain = VK_NULL_HANDLE;	// 一つのSwapChainのみ　for now, only create one swap chain

	// 上記の情報に基づいてSwapChainを生成します。
	if (vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);                    // SwapChainのイメージ数を獲得
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());   // 情報をm_SwapChainImagesに代入

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

// プログラム用イメージビュー生成
void CVulkanFramework::createImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());    // イメージカウントによってベクトルサイズを変更しますallocate enough size to fit all image views 					
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = createImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

// レンダーパス生成
void CVulkanFramework::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};           // カラーアタッチメント
	colorAttachment.format = m_SwapChainImageFormat;    // SwapChainフォーマットと同じ　format of color attachment = format of swap chain images
	colorAttachment.samples = m_MSAASamples;             // マルチサンプリングビット数

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;    // レンダリング前の情報はどうするフラッグ　
															  // what to do with data before rendering

	// VK_ATTACHMENT_LOAD_OP_LOAD       : 既存の情報を保存　Preserve existing contents of attachment
	// VK_ATTACHMENT_LOAD_OP_CLEAR      : クリアする（現在：黒）Clear the values to a constant at the start (in this case, clear to black)
	// VK_ATTACHMENT_LOAD_OP_DONT_CARE  : 情報を無視　Existing contents are undefined; we don't care about them

	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	// レンダリング後の情報はどうするフラッグ what to do with data after rendering

	// VK_ATTACHMENT_STORE_OP_STORE     : 情報を保存　Rendered contents will be stored in memory and can be read later
	// VK_ATTACHMENT_STORE_OP_DONT_CARE : 情報を無視　Contents of the framebuffer will be undefined ater the rendering operation

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;     // ステンシルバッファーを使っていない
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // not using stencil buffer

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                 // レンダリング前のイメージレイアウト image layout before render pass
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;    // レンダリング前のイメージレイアウト layout to automatically transition to after pass

	// 一般のレイアウト種 Common Layouts:
	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : カラーアタッチメントとして使う    Images used as color attachment
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR          : SwapChainで描画します              Images to be presented in the swap chain
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL     : メモリーコピー演算として使う      Images to be used as destination for a memory copy operation

	// アタッチメントレフレックスインデックス
	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;    // attachment reference index; 0 = first index (only one attachment for now)
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// マルチサンプリングで作成したイメージは直接描画できません。普通のイメージとしてまとめてから描画できます
	// 下記のサブパスパラメータ subpass.pResolveAttachments及び必要な準備で処理します
	// Multisampled Images cannot be presented directly (VK_IMAGE_LAYOUT_PRESENT_SRC_KHR). 
	// First resolve them into a regular image, then they can be presented via subpass.pResolveAttachments below
	VkAttachmentDescription colorAttachmentResolve{};    // カラーアタッチメントResolve(リソールブ；解決）情報構造体
	colorAttachmentResolve.format = m_SwapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;    // マルチサンプル後、イメージを1ビットサンプルに変換
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // リソールブ後、プレゼントすることができます

	VkAttachmentReference colorAttachmentResolveReference{};
	colorAttachmentResolveReference.attachment = 2;
	colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// デプスアタッチメント
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = m_MSAASamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// デプスアタッチメントレファレンス
	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;    // アタッチメントと同じ

	// サブパス
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentReference;
	subpass.pResolveAttachments = &colorAttachmentResolveReference;

	// サブパス依存関係
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;    // サブパスインデックス 0 subpass index 0 (our first and only subpass)
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// レンダーパス情報構造体生成
	// attachments：createCommandBuffers()のclearValues順番と同じにすること
	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	// 上記の構造体の情報に基づいて実際のレンダーパスを生成します。
	if (vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");
	}
}

// リソースレイアウト：どんなリソース（バッファー、イメージ情報など）をグラフィックスパイプラインにアクセスを許可するか
void CVulkanFramework::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};    // UniversalBufferObjectレイアウトバインディング情報構造体
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;               // MVPトランスフォームは1つのバッファーオブジェクトに格納されています
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;               // 任意 optional, image sampling用
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;    // 参照できるシェーダーステージ（現在、頂点シェーダーでスクリプター）

	// 合成イメージサンプラー用 Combined Image Sampler
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

// グラフィックスパイプライン生成
void CVulkanFramework::createGraphicsPipeline()
{
	const std::vector<char> vertShaderCode = readFile("shaders/vert.spv");    // 頂点シェーダー外部ファイルの読み込み
	const std::vector<char> fragShaderCode = readFile("shaders/frag.spv");    // フラグメントシェーダー外部ファイルの読み込み

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);     // 頂点シェーダーモジュール生成（頂点データ、色データ含め）
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);     // フラグメントシェーダーモジュール生成

	// シェーダステージ：パイプラインでシェーダーを利用する段階	
	// Shader Stages: Assigning shader code to its specific pipeline stage
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};                    // 頂点シェーダーステージ情報構造体
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;                   // enum for programmable stages in Graphics Pipeline: Intro
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";                                       // 頂点シェーダーエントリーポイント関数名(shaders.vert)

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};                    // フラグメントシェーダーステージ情報構造体
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";                                       // フラグメントシェーダーエントリーポイント関数名(shaders.frag)

	// パイプライン生成のタイミングで使える形にします。
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };    // シェーダー構造体配列

	// パイプライン生成の際に必要な段階 necessary steps in creating a graphics pipeline

	// 1.) 頂点インプット：頂点シェーダーに渡される頂点情報のフォーマット
	// Vertex Input: Format of the vertex data to be passed to the vertex shader

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};    // 頂点インプット情報構造体
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// Vertex構造体のVertexBindingDescriptionとVertexAttributeDescriptionに参照します
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	// 2.) インプットアセンブリー： 頂点からどんなジオメトリーが描画されるか、そしてトポロジーメンバー設定
	// Input Assembly: What kind of geometry will be drawn from the vertices, topology member settings

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    // POINT_LIST、LINE_LIST、LINE_STRIP、TRIANGLE_STRIP
	inputAssembly.primitiveRestartEnable = VK_FALSE;                 //　STRIPの場合、ラインと三角を分散できます

	// 3.) ビューポート・シザー四角
	// Viewports and Scissor Rectangles

	// ビューポート：イメージからフレームバッファーまでの「トランスフォーム」        Viewport: 'transformation' from the image to the framebuffer
	// シザー四角：ピクセルデータが格納される領域；画面上で描画される「フィルター」  Scissor rectangle: 'filter' in which region pixels will be stored. 

	VkViewport viewport{};                                // ビューポート情報構造体
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_SwapChainExtent.width;      // 描画レゾルーションと同じ
	viewport.height = (float)m_SwapChainExtent.height;    // 描画レゾルーションと同じ
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};                     // シザー四角情報構造体				
	scissor.offset = { 0, 0 };              // オフセットなし
	scissor.extent = m_SwapChainExtent;     // フレームバッファー全体を描画する設定 set to draw the entire framebuffer

	VkPipelineViewportStateCreateInfo viewportState{};    // ビューポートステート（状態）情報構造体
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;                 // ビューポートのポインター
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;                   // シザー四角のポインター

	// 4.) ラスタライザー：頂点シェーダーからのジオメトリー（シェープ）をフラグメント（ピクセル）に変換して色を付けます。 
	// Rasterizer: Takes geomerty shaped from the vertex shader and turns it into fragments (pixels) to be colored by the fragment shader.

	VkPipelineRasterizationStateCreateInfo rasterizer{};    // ラスタライザー情報構造体
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	// VK_TRUE: ニア―・ファー領域のフラグメントが捨てずに境界にクランプされます。シャドウマッピングに便利。
	// VK_TRUE: fragments beyond near and far planes are clamped to them rather than discarding them; useful with shadow mapping

	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	// FILL: フラグメントに埋め込む fill area of polygon with fragments
	// LINE: ラインで描画（ワイヤーフレーム）　polygon edges drawn as lines (i.e. wireframe)
	// POINT: 頂点で描画　polygon vertices are drawn as points
	// ※FILL以外の場合、特定なGPU機能をオンにする必要があります。Using anything other than FILL requires enabling a GPU feature.

	rasterizer.lineWidth = 1.0f;    // ラインの厚さ（ピクセル単位）Line thickness (in pixels)
	// ※1.0以上の厚さの場合、特定なGPU機能をオンにする必要があります。Using line width greater than 1.0f requires enabling a GPU feature.

	rasterizer.cullMode = VK_CULL_MODE_NONE;                    // カリング設定（通常：BackfaceCulling)
																// VK_CULL_MODE_BACK_BIT

	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;     // 頂点の順番により表面・裏面を判断する設定（時計回り・反時計回り）
																// determines front-facing by vertex order (CLOCKWISE or COUNTER_CLOCKWISE) 
	rasterizer.depthBiasEnable = VK_FALSE;        // VK_TRUE: Depth値調整（シャドウマッピング）Adjusting depth values i.e. for shadow mapping
	rasterizer.depthBiasConstantFactor = 0.0f;    // 任意 optional
	rasterizer.depthBiasClamp = 0.0f;             // 任意 optional
	rasterizer.depthBiasSlopeFactor = 0.0f;       // 任意 optional

	// 5.) マルチサンプリング（アンチエイリアシング用） 
	// Multisampling (method to perform anti-aliasing)

	VkPipelineMultisampleStateCreateInfo multisampling{};    // マルチサンプリング情報構造体
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;             // サンプルシェーディング有効
	multisampling.rasterizationSamples = m_MSAASamples;      // マルチサンプリング有効
	multisampling.minSampleShading = 0.2f;                   // 1.0に近ければ近いほどスムーズ率が高くなる
	multisampling.pSampleMask = nullptr;                     // 任意 optional
	multisampling.alphaToCoverageEnable = VK_FALSE;          // 任意 optional
	multisampling.alphaToOneEnable = VK_FALSE;               // 任意 optionall

	// 6.)　デプス・ステンシル
	// Depth and Stencil Testing 

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;     // 新しいフラグメントのデプスをデプスバッファーと比較するか（discard)
	depthStencil.depthWriteEnable = VK_TRUE;    // デプステストを合格したフラグメントのデプスをデプスバッファーに書き込むか

	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;    // デプスが低い：近い
	depthStencil.depthBoundsTestEnable = VK_FALSE;       // デプスバウンドテスト用（TRUE: Bounds内のフラグメントしか保留しません）
	depthStencil.minDepthBounds = 0.0f;                  // 任意 optional
	depthStencil.maxDepthBounds = 1.0f;                  // 任意 optional

	depthStencil.stencilTestEnable = VK_FALSE;           // ステンシルバッファー用
	depthStencil.front = {};                             // 任意 optional
	depthStencil.back = {};                              // 任意 optional  

	// 7.) カラーブレンディング：�@フレームバッファーの既存の色と混ぜるか、�A前と新しい値をBitwise演算で合成するか
	// Color Blending: Either �@ mix the old value (in the framebuffer) and new value, or �A perform a bitwise operation with the two values

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};         // カラーブレンド情報構造体
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT      // �@のブレンディング方式
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;                        // 下記のコメント欄をご覧ください。
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;     // 任意 optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;    // 任意 optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                // 任意 optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;     // 任意 optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;    // 任意 optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                // 任意 optional

	// blendEnableがVK_TRUEの場合のブレンディング設定（疑似言語）
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

	// アルファブレンディングの例（疑似言語）
	// Alpha Blending Implementation: (pseudocode)
	// finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
	// finalAlpha.a = newAlpha.a;

	// アルファブレンディング設定：
	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};    // パイプラインカラーブレンドステート情報構造体
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;       // VK_TRUE: �Aのベンディング方式
	colorBlending.logicOp = VK_LOGIC_OP_COPY;     // 任意 optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;       // 任意 optional
	colorBlending.blendConstants[1] = 0.0f;       // 任意 optional
	colorBlending.blendConstants[2] = 0.0f;       // 任意 optional
	colorBlending.blendConstants[3] = 0.0f;       // 任意 optional

	// 8.) ダイナミックステート（後で詳しく調べます）
	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	// 9.) パイプラインレイアウト（後で詳しく調べます）
	// Pipeline Layout (empty for now, revisit later)

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};     // パイプラインレイアウト情報構造体
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;     // でスクリプターセットレイアウト
	pipelineLayoutInfo.pushConstantRangeCount = 0;               // 任意 optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr;            // 任意 optional

	// 上記の構造体の情報に基づいて実際のパイプラインレイアウトを生成します。
	if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	// 10.) グラフィックスパイプライン：全部の段階を組み合わせてパイプラインを生成します。
	// が、その前にいつものようにグラフィックスパイプライン情報構造体を生成する必要があります。
	// Graphics Pipeline creation: Putting everything together to create the pipeline!!
	// ...but before that, we need to create a Pipeline info struct:

	VkGraphicsPipelineCreateInfo pipelineInfo{};    // パイプライン情報構造体
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;                    // シェーダーステージに合わせる	 Make sure this info is aligned with Shader Stages above
	pipelineInfo.pStages = shaderStages;            // シェーダーステージ配列のポインター

	// 今までの段階をパイプライン構造体情報ポインターに参照します。
	// reference all the structures described in the previous stages
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;             // 任意 optional

	// pipelineInfo.flags - 現在なし。下記の basePipelineHandleと　basePipelineHandleIndexをご覧ください。
	// none at the moment; see basePipelineHandle and basePipelineIndex below

	// パイプラインレイアウト：構造体ポインターではなく、Vulkanハンドル
	// pipeline layout: Vulkan handle, NOT struct pointer
	pipelineInfo.layout = m_PipelineLayout;

	// レンダーパスとサブパスの参照
	// reference to the render pass and the subpass index
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	// pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BITの場合：
	// 既存のパイプラインからの情報を新たなパイプラインに使用します。
	// these values are only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag 
	// is also specified in the flags field of VkGraphicsPipelineCreateInfo
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;    // 任意 optional
	pipelineInfo.basePipelineIndex = -1;                 // 任意 optional

	// 上記の構造体の情報に基づいて、ようやく実際のグラフィックスパイプラインが生成できます。
	// 2つ目の引数：パイプラインキャッシュ(後で調べます）
	// Creating the actual graphics pipeline from data struct
	// Second argument: pipeline cache (revisit later)
	if (vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline!");
	}

	// 用済みのシェーダーモジュールを削除します。
	vkDestroyShaderModule(m_LogicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_LogicalDevice, vertShaderModule, nullptr);
}

// マルチサンプリング用カラーバッファーを生成
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

	// マルチサンプリング用イメージビュー生成の際にミップマップは「1」に設定しないといけません（Vulkanの決まり）
	// このイメージビューはテクスチャーとして使わないので描画品質に影響しません
	// Mipmap levels must be set to 1 when creating an image with more than one sample per pixel,
	// as per Vulkan specifications.  As this image will not be used as a texture, it will not affect quality
	m_ColorImageView = createImageView(m_ColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

// デプスリソース生成
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

// フレームバッファー
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
		framebufferInfo.renderPass = m_RenderPass;          // フレームバッファーと相当する設定 compatibility with framebuffer settings
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;                         // 1: 1つのイメージしかない場合 Swap chain images as single images

		if (vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}
}

// コマンドバッファーを格納するコマンドプールを生成
void CVulkanFramework::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

	VkCommandPoolCreateInfo poolInfo{};    // コマンドプール情報構造体
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();    // 描画するためグラフィックスキューを選択します
																			  // drawing commands: graphics queue family chosen
	poolInfo.flags = 0;    // optional 任意

	// 上記の構造体の情報に基づいて実際のコマンドプールを生成します。
	if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics command pool!");
	}
}

// テクスチャーマッピング用画像を用意します
void CVulkanFramework::createTextureImage()
{
	int texWidth, texHeight, texChannels;

	// STBI_rgb_alpha: αチャネルがない場合、自動的に追加します。
	stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	// max   : widthとheightから大きいサイズの方を基準に
	// log2  : その値を何回まで2で除算できるか（各ミップマップレベルは元のレベルの半分のため）
	// floor : max値 % 2 != 0　の場合、floorに超えない最大整数として扱います
	// +1    : 元のテクスチャー自体をレベル0とします(ループ処理は[i] -1)
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

	// 用済みピクセル配列を削除
	stbi_image_free(pixels);

	// テクスチャーイメージ生成
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

	// コピー処理を実行
	copyBufferToImage(
		stagingBuffer,
		m_TextureImage,
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight)
	);

	// 後片付け
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);

	// ミップマップ生成
	generateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_MipLevels);
}

// createTextureImage()からのイメージがイメージビューを生成
void CVulkanFramework::createTextureImageView()
{
	m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

// テクスチャーサンプラー生成：Filtering (Bilinear, Anisotropicなど)、AddressingModeなど
void CVulkanFramework::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};    // サンプラー情報構造体
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	// バイリニアフィルタリング (他はVL_FILTER_NEAREST)
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	// UVW = XYZ; テクスチャー座標でよく使われています
	// UVW often used in Texture Space Coordinates
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// VK_SAMPLER_ADDRESS_MODE_REPEAT: repeat texture
	// VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: mirror image
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: take color of edge closest to the coordinate beyond image dimensions
	// VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: Uses edge opposite to closest edge
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: returns solid color when sampling beyond dimensions
	// ※現在、イメージの外でサンプリングを行わないため設定を無視しますが、一番よく使われているのはMODE_REPEATです


	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;    // 最大限

	// CLAMP_TO_BORDERの場合、表示されたカラー 
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	// Texel座標 [0, 1)
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	// Percentage-Closer Filtering用
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	// ミップマッピング用
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	// samplerInfo.minLod = static_cast<float>(m_MipLevels / 2);      // ミップマップテスト mipmap test
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(m_MipLevels);

	if (vkCreateSampler(m_LogicalDevice, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

// モデルのロード処理
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

	// 全ての三角をIterateして、1つのモデルにまとめます
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

			// 頂点重複フィルター
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}
			m_Indices.push_back(uniqueVertices[vertex]);

			// フィルターなし
			//m_Vertices.push_back(vertex);
			//m_Indices.push_back(m_Indices.size());
		}
	}
	// 頂点数確認・比較
	// std::cout << "頂点数: "  << m_Vertices.size() << std::endl;
}

// 頂点バッファー生成
void CVulkanFramework::createVertexBuffer()
{
	// 頂点単位 ＊　配列の要素数
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	// ステージングバッファー：CPUメモリー上臨時バッファー。頂点データに渡され、最終的な頂点バッファーに渡します。
	// Staging buffer: temporary buffer in CPU memory that takes in vertex array and sends it to the final vertex buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;	// voidポインター；今回メモリーマップに使用します

	// 情報をメモリーにマップ
	vkMapMemory(m_LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

	// 頂点バッファーを生成します
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,		// 一番理想的なメモリーフォーマットが、普通にCPUがアクセスできません。
		m_VertexBuffer,
		m_VertexBufferMemory);

	// 頂点データをステージングバッファーから頂点バッファーに移す
	copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	// 用済みのステージングバッファーとメモリーの後片付け
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);
}

// インデックスバッファー生成：頂点バッファーとほぼ同じ（違いは番後　�@、�Aで表示されています
void CVulkanFramework::createIndexBuffer()
{
	// インデックス単位　＊　配列の要素数
	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();    // 変更点　�@、�A

	// ステージングバッファー：頂点バッファーと同じ
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
	memcpy(data, m_Indices.data(), (size_t)bufferSize);        // 変更点　�B vertices.data() --> indices.data()
	vkUnmapMemory(m_LogicalDevice, stagingBufferMemory);

	// インデックスバッファーを生成します
	createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,    // 変更点　�C (VK_BUFFER_USAGE_VERTEX_BITからINDEX_BITに)
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_IndexBuffer,            // 変更点　�D  インデックスバッファー
		m_IndexBufferMemory);     // 変更点　�E　インデックスバッファーメモリー

	// インデックスデータをステージングバッファーからインデックスバッファーに移す
	copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);    // 変更点　�F　コピー先をインデックスバッファーに

	// 用済みのステージングバッファーとメモリーの後片付け
	vkDestroyBuffer(m_LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice, stagingBufferMemory, nullptr);
}

// ユニフォームバッファー：シェーダー用のUBO(Uniform Buffer Object)データ
void CVulkanFramework::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(m_SwapChainImages.size());
	m_UniformBuffersMemory.resize(m_SwapChainImages.size());

	// フレームずつUBOトランスフォームをを適用する
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

// デスクリプターセットを格納するでスクリプタープールを生成
void CVulkanFramework::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};    // 各フレームに1つのデスクリプターを用意します
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());


	VkDescriptorPoolCreateInfo poolInfo{};    // デスクリプタープール生成情報構造体
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

	if (vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}

// デスクリプターセット（トランスフォーム情報）生成
void CVulkanFramework::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), m_DescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;    // デスクリプタープール
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
		bufferInfo.range = sizeof(UniformBufferObject);    // 完全に上書き：VK_WHOLE_SIZEと同じ

		// Combined Image Sampler
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TextureImageView;
		imageInfo.sampler = m_TextureSampler;


		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};    // デスクリプターの設定・コンフィグレーション情報構造体
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;                // ユニフォームバッファーバインディングインデックス「0」
		descriptorWrites[0].dstArrayElement = 0;           // 配列を使っていない場合、「0」
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

		// デスクリプターセットを更新します
		vkUpdateDescriptorSets(m_LogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

// コマンドプールの情報からコマンドバッファー生成
void CVulkanFramework::createCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());    // フレームバッファーサイズに合わせる

	VkCommandBufferAllocateInfo allocInfo{};                    // メモリー割り当て情報構造体
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// PRIMARY:	キューに直接渡せますが、他のコマンドバッファーから呼び出せません。
	// can be submitted to queue for execution, but cannot be called from other command buffers
	// SECONDARY:　キューに直接渡せませんが、プライマリーコマンドバッファーから呼び出せます。
	// cannot be submitted directly, but can be called from primary command buffers
	allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

	if (vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	// コマンドバッファー登録開始 Starting command buffer recording
	for (size_t i = 0; i < m_CommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};       // コマンドバッファー開始情報構造体
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;                        // 任意　optional
		beginInfo.pInheritanceInfo = nullptr;       // 継承：SECONDARYの場合のみ（どのコマンドバッファーから呼び出すか）
													// only for secondary command buffers (which state to inherit from)

		if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		// レンダーパス開始
		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo{};		// レンダーパス情報構造体
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0, 0 };

		// パフォーマンスの最適化のため、レンダー領域をアタッチメントサイズに合わせます。
		// match render area to size of attachments for best performance
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		// createRenderPass(): VK_ATTACHMENT_LOAD_OP_CLEARのクリア値 (clearColor)
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };    // 黒
		clearValues[1].depthStencil = { 1.0f, 0 };            // デプスステンシルクリア値 (1.0f: ファー Far Plane)

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// 実際のレンダーパスを開始します
		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// グラフィックスパイプラインとつなぎます
		vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// 頂点バッファー情報をバインドしたら描画の準備は完成です
		VkBuffer vertexBuffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, vertexBuffers, offsets);

		// インデックスバッファー
		vkCmdBindIndexBuffer(m_CommandBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);    // VK_INDEX_TYPE_UINT16

		// デスクリプターセットをバインドします
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

		// 描画コマンド（インデックスバッファー）
		vkCmdDrawIndexed(m_CommandBuffers[i], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
		// 引数�@：コマンドバッファー
		//     �A：頂点数（頂点バッファーなしでも頂点を描画しています。）
		//     �B：インスタンス数（インスタンスレンダリング用）
		//     �C：インデックスバッファーの最初点からのオフセット
		//     �C：インデックスバッファーに足すオフセット (使い道はまだ不明）
		//     �C：インスタンスのオフセット（インスタンスレンダリング用）

		// arguments
		// first    : commandBuffer
		// second   : vertexCount  : even without vertex buffer, still drawing 3 vertices (triangle)
		// third    : instanceCount: used for instanced rendering, otherwise 1)
		// fourth   : firstIndexOffset : offset to start of index buffer (1 means GPU reads from second index)
		// fifth    : indexAddOffset   : offset to add to indices (not sure what this is for)
		// sixth    : instanceOffset   : used in instanced rendering

		// レンダーパスを終了します
		vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

// 処理同期の専用オブジェクト生成
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
// 15X : 汎用オブジェクト生成・ヘルパー関数
// All-purpose object creation / helper functions
//====================================================================================

// 汎用イメージビュー生成関数
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

// 汎用イメージ生成関数
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

// サポートされている（適用できる）一番理想なフォーマットを検索します（TilingとFeaturesによって異なります）
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

// シェーダーコードをパイプラインに使用する際にシェーダーモジュールにwrapする必要があります。
VkShaderModule CVulkanFramework::createShaderModule(const std::vector<char>& code)
{
	// specify a pointer to the buffer with the bytecode and length

	VkShaderModuleCreateInfo createInfo{};    // シェーダーモジュール情報構造体
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();        // サイズ

	// バイトコードのサイズはバイトで表しますが、バイトコードポインターはuint32_t型です。
	// そこで、ポインターをrecastする必要があります。
	// size of bytecode is in bytes, but bytecode pointer is uint32_t pointer rather than a char pointer.
	// thus, we need to recast the pointer as below.
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	// 上記の構造体に基づいてシェーダーモジューを生成します。
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_LogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module!");
	}
	return shaderModule;
}

// ファイル読み込み
std::vector<char> CVulkanFramework::readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate    // ate: 尻尾からファイルを読む　start reading at EOF 
		| std::ios::binary);                      // binary: バイナリファイルとして扱う read file as binary file (avoid text transformations)

	if (file.is_open() == false)
	{
		throw std::runtime_error("Failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();     // telg(): インプットストリーム位置を戻す returns input stream position. 
	std::vector<char> buffer(fileSize);         // 尻尾(EOF: End of File) から読み込むと実際のファイルサイズを特定できます。
												// EOF essentially gives us the size of the file for the buffer

	file.seekg(0);                              // ファイルの頭に戻る　return to beginning of file
	file.read(buffer.data(), fileSize);         // read(x, y): yまで読み込み、xバッファーにアサインします 
												// read up to count y and assign to buffer x

	// 今回、全てのバイトを一気に読み込む in this case, read all the bytes at once

	// std::cout << fileSize << std::endl;		// ファイルプロパティーでサイズが合ってるかが確認できます。
												// check file byte size with actual file (properties)
	file.close();

	return buffer;
}

// 汎用バッファー生成関数
void CVulkanFramework::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};                          // バッファー情報構造体
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;                                 // ユースケース：頂点バッファー
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;       // 共有モード（SwapChain生成と同じ）：今回グラフィックスキュー専用

	// 上記の構造体に基づいて実際のバッファーを生成します。
	if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};    // メモリー割り当て情報構造体
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	// 上記の構造体に基づいて実際のメモリー確保処理を実行します。
	if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	// 確保されたメモリー割り当てを頂点バッファーにバインドします
	vkBindBufferMemory(m_LogicalDevice, buffer, bufferMemory, 0);
}

// バッファーコピー関数
void CVulkanFramework::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	// コピー領域確保
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;    // 任意 optional
	copyRegion.dstOffset = 0;    // 任意 optional
	copyRegion.size = size;

	// コピー元のバッファーの中身をコピー先のバッファーにコピーするコマンドを記録します
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

// バッファー情報をイメージに移す
void CVulkanFramework::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};    // コピー情報構造体
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	// コピー処理
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

// イメージレイアウトを次のレイアウトに遷移します
void CVulkanFramework::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	// キュー所属を変えたい場合、以下の設定が必要です
	// デフォルトではありません！　必ず設定すること
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
		0,              // 又はVK_DEPENDENCY_BY_REGION_BIT
		0, nullptr,     // メモリーバリア
		0, nullptr,     // バッファーメモリーバリア
		1, &barrier     // イメージメモリバリア（現在使用中）
	);

	endSingleTimeCommands(commandBuffer);
}

// 一回だけ使用予定のコマンドを開始します
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

// 一回だけ使用予定のコマンドを終了させます
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

// ミップマップ生成関数
void CVulkanFramework::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// 渡されたフォーマットがLinear Blittingをサポートするかを確認
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

	// ミップマップ生成ループ
	for (uint32_t i = 1; i < mipLevels; i++)    // ループは1から始めるのは注意点
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;    // Optimal: クオリティー最適化
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

		// 画像からミップマップを生成する専用情報構造体
		VkImageBlit blit{};                                // blit: Bit Block Transfer
		blit.srcOffsets[0] = { 0, 0, 0 };                  // blit元のオフセット（範囲） Source Offset
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		// blit先のオフセット（領域） Destination Offset
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = {
				mipWidth > 1 ? mipWidth / 2 : 1,     // 条件演算子でのミップマップ計算
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
			VK_FILTER_LINEAR    // 補間
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

		// ミップマップ生成後、ミップマップサイズ / 2（ゼロ除算を避けるため、値は1をチェック）
		// テクスチャーの縦と横が一等しない場合、どちらかが先に1のままになります
		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	// 最後のミップマップレベルをVK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMALからVK_IMAGE_SHADER_READ_ONLY_OPTIMALに
	// (最後のレベルはループに入らなかったため）
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
// 20X : 更新用関数
// Update Functions
//====================================================================================

// ウィンドウリサイズコールバック
void CVulkanFramework::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	CVulkanFramework* app = reinterpret_cast<CVulkanFramework*>(glfwGetWindowUserPointer(window));
	app->CVulkanFramework::m_FramebufferResized = true;
}

// SwapChainがウィンドウサーフェスに対応していない場合（ウインドウリサイズ）、SwapChainを再生成
// 必要があります。SwapChain又はウィンドウサイズに依存するオブジェクトはSwapChainと同時に
// 再生成しないといけません。
// Recreate swap chain and all creation functions for the object that depend on the swap chain or the window size.
// This step is to be implemented when the swap chain is no longer compatible with the window surface;
// i.e. window size changing (and thus the extent values are no longer consistent)
void CVulkanFramework::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_Window, &width, &height);
	while (width == 0 || height == 0)                       // ウィンドウが最小化状態の場合 window is minimized
	{
		glfwGetFramebufferSize(m_Window, &width, &height);  // 最小化の場外から解除するまでウインドウ処理を一旦停止する
		glfwWaitEvents();                                   // window paused until window in foreground
	}

	vkDeviceWaitIdle(m_LogicalDevice);    // 使用中のリソースの処理を終了まで待つこと。do not touch resources that are still in use, wait for them to complete.

	cleanupSwapChain();         // SwapChainの後片付け
	createSwapChain();          // SwapChain自体を再生成
	createImageViews();         // SwapChain内の画像に依存
	createRenderPass();         // SwapChain内の画像のフォーマットに依存
	createGraphicsPipeline();   // ビューポート、Scissorに依存
	createColorResources();     // 描画処理に影響します 
	createDepthResources();     // デプスバッファーレゾルーションがウインドウリサイズに合わせます
	createFramebuffers();       // SwapChain内の画像に依存
	createUniformBuffers();     // SwapChain内の画像に依存
	createDescriptorPool();     // SwapChain内の画像に依存
	createDescriptorSets();     // SwapChain内の画像に依存
	createCommandBuffers();     // SwapChain内の画像に依存
}

// ユニフォームバッファー更新（UBO）：マトリックストランスフォーム、カメラ設定
void CVulkanFramework::updateUniformBuffer(uint32_t currentImage)
{
	//// startTime、currentTimeの実際のデータ型: static std::chrono::time_point<std::chrono::steady_clock> 
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};  // MVPトランスフォーム情報構造体

	//// M(Model: 毎フレーム、Z軸にX°回転させる
	// ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//
	// V(View): 引数　eye位置, center位置, up軸
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// P(Projection): 引数　45°バーティカルFoV, アスペクト比、ニア、ファービュープレーン
	// arguments: field-of-view, aspect ratio, near and far view planes 
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);

	//// 元々GLMはOpelGLに対応するために設計されています（Y軸のクリップ座標が逆さになっています）。
	//// Vulkanに対応するためにクリップ座標のY軸を「元に戻す」訳です。こうしないと描画はひっくり返す状態になってしまいます。

	//// GLM was initially developed for OpenGL where the Y-coordinate of the clip coordinates is reversed.  
	//// To compensate, multiply the Y-coordinate of the projection matrix by -1 (reverting back to normal).  
	//// Not doing this results in an upside-down render.
	ubo.proj[1][1] *= -1;

	//// UBO情報を現在のユニフォームバッファーにうつします
	void* data;
	vkMapMemory(m_LogicalDevice, m_UniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_LogicalDevice, m_UniformBuffersMemory[currentImage]);
}

// フレームを描画
void CVulkanFramework::drawFrame()
{
	// フェンス処理を待ちます
	vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	// SwapChainがすたれた場合  （すたれた）
	// check if swap chain is out of date
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();    // SwapChainを再生成
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	// ユニフォームバッファー更新
	updateUniformBuffer(imageIndex);

	// 現在の画像が以前のフレームで使われているか（フェンスを待っているか）
	// check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_LogicalDevice, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	// 現在の画像が現在のフレームで使われているように示す。
	// mark the image as now being in use by this frame
	m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

	VkSubmitInfo submitInfo{};    // キュー同期・提出情報構造体
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;    // 実行前に待つセマフォ　semaphore to wait on before execution
	submitInfo.pWaitDstStageMask = waitStages;      // 待たせるパイプラインステージ　stage(s) of the pipeline to wait

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;    // 終了後のときに起動するセマフォ  semaphores to signal once command buffer(s) have finished execution

	vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};    // プレゼント情報構造体
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };      // SwapChain構造体
	presentInfo.swapchainCount = 1;                     // SwapChain数（現在：１）
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	// 任意（複数のSwapChainの場合、各SwapChainの結果を格納する配列）
	// array of VkResult values to check for each swap chain if presentation was successful if using multiple swap chains
	presentInfo.pResults = nullptr;

	// リザルトをSwapChainに渡して描画します  submit the result back to the swap chain to have it show on screen
	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR    // SwapChainが廃れた
		|| result == VK_SUBOPTIMAL_KHR           // SwapChainが最適化されていない
		|| m_FramebufferResized == true)         // フレームバッファーのサイズが変更された
	{
		m_FramebufferResized = false;
		recreateSwapChain();                  // SwapChainを再生成します
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;    // 次のフレームに移動　advance to next frame
}

//====================================================================================
// 30X : デバイス・コンポーネントプロパティー獲得・クエリー用関数
// Device/Component Property, Query Functions
//====================================================================================

// 使用中のGPUの対応できるサンプルビット数を獲得する関数
VkSampleCountFlagBits CVulkanFramework::getMaxUseableSampleCount()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts =
		physicalDeviceProperties.limits.framebufferColorSampleCounts
		& physicalDeviceProperties.limits.framebufferDepthSampleCounts;


	// 最大数優先
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }    // 現在のGPU: NVIDIA GeForce RTX 2070 SUPER
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

// SwapChainサポート確認
SwapChainSupportDetails CVulkanFramework::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);    // サーフェスケーパビリティ surface capabilities

	uint32_t formatCount;                                                                   // 適用できるフォーマットカウント
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);         // 対応しているサーフェスフォーマットを確認 	
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);    // 適用できるフォーマットを格納できるようにリサイズします。
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());    // データを代入
	}

	uint32_t presentModeCount;                                                              // 適用できるプレゼトモードカウント
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &presentModeCount, nullptr);    // 対応しているプレゼンテーションモードを確認
	if (presentModeCount != 0)
	{
		// 適用できるプレゼンテーションモードを格納できるようにリサイズします。
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());    // データを代入
	}
	return details;
}

// GPUがやりたい処理に適切かの確認
bool CVulkanFramework::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);     // VK_QUEUE_GRAPHICS_BITを探しています

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;     // 最低限1つのイメージフォーマットと1つのプレゼンテーションモードが特定できましたか
										// At least one supported image format and one supported presentation mode given the window surface
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	// ジオメトリーシェーダーのみを選択したい場合；　sample if wanting to narrow down to geometry shaders:
	// return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;		
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

// ロジカルデバイスがエクステンションに対応できるかの確認
bool CVulkanFramework::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;	// エクステンションカウント
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);	// 使用可能エクステンションのベクトル

	// 全てのエクステンションプロパティをavailableExtensionsに代入します
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// 必要なエクステンションのベクトル
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);    // 必要なエクステンションを見つかった場合、setから削除します。
															  // erase if required extension is in the vector
	}

	bool isEmpty = requiredExtensions.empty();    // 全ての必要なエクステンションが見つかった場合、True	
	return isEmpty;                               // if all the required extension were present (and thus erased), returns true
}

// キュー種類検索・選択
QueueFamilyIndices CVulkanFramework::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);                 // キュー種類を特定

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);                         // キュー種類カウントによりベクトルを生成します
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());    // 情報をベクトルに代入

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

		if (indices.isComplete())    // キュー種類が見つかった場合、break / If queueFamily is found, exit early
		{
			break;
		}
		i++;
	}
	return indices;
}

// サーフェスフォーマット選択
VkSurfaceFormatKHR CVulkanFramework::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB    // sRGBが使えるなら洗濯します。
			&& availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	// それ以外、最初見つかったフォーマットを使ってもいいです otherwise, just use the first format that is specified
	return  availableFormats[0];

}

// スワッププレゼントモードを選択
VkPresentModeKHR CVulkanFramework::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)    // トリプルバッファリング　triple buffering (less latency)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;    // if VK_PRESENT_MODE_MAILBOX_KHR is not available, use a guaranteed available mode
}

// レゾルーション設定  extent = resolution of the swap chain images
VkExtent2D CVulkanFramework::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites)
{
	// Vulkanがピクセル単位で設定されていますがスクリーンレゾルーションは2D座標で表しています。
	// ピクセル単位でのスクリーンレゾルーションを獲得するためにglfwGetFramebufferSize()を使う必要があります。
	// Vulkan works with pixel units, but the screen resolution (WIDTH, HEIGHT) is in screen coordinates.
	// Use glfwGetFramebufferSize() to query the resolution of the window in pixel before matching to min/max image extent

	// UINT32_MAX: uint32_tの最大値 maximum value of uint32_t
	// ここで使うと、自動的に一番理想なレゾルーションを選択してくれます。
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

// デプスフォーマットを検索します
VkFormat CVulkanFramework::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

// 適切なメモリータイプを検索
uint32_t CVulkanFramework::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;    // メモリータイプ、メモリーヒープ
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	// 適切なメモリータイプを獲得
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i)    // corresponding bit is set to 1
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)     // bitwise AND 論理積 == 引数 properties
		{
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type!");    // 適切なメモリータイプが見つからなかった場合
}

// 渡されたデプスフォーマットがステンシルコンポーネントがついているか
bool CVulkanFramework::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

//====================================================================================
// 40X : 終了・後片付け関数
// Cleanup Functions
//====================================================================================

// SwapChainを再生成する前に古いSwapChainを削除する関数
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

	// コマンドプールを削除せずコマンドバッファーを開放。既存のプールを新しいコマンドバッファーで使います。
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

// 後片づけ
// 3つ目のnullptr: 任意のコールバック引数
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









