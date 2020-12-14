/*======================================================================
Vulkan Presentation : VulkanFramework.h
Author:			Sim Luigi
Last Modified:	2020.12.13
=======================================================================*/
#pragma once

#define GLFW_INCLUDE_VULKAN    // VulkanSDKをGLFWと一緒にインクルードします。
#include <GLFW/glfw3.h>        // replaces #include <vulkan/vulkan.h> and automatically bundles it with glfw include

#include <glm/glm.hpp>                      // glmインクルード
#include <glm/gtc/matrix_transform.hpp>     // モデルトランスフォーム
#include <glm/gtx/hash.hpp>

#include <array>
#include <optional>
#include <iostream>  // std::cerr, try to migrate out of debug callback

struct Vertex
{
	// glmライブラリーがシェーダーコードに合ってるC++データ型を用意してくれます。
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	// シェーダー情報がGPUに読み込まれた後、頂点シェーダーに渡す関数2つ
	// Two functions to tell Vulkan how to pass the shader data once it's been uploaded to the GPU

	// ① 頂点バインディング：情報の読み込む率　（stride, 頂点ごと・インスタンスごと）
	// Vertex Binding: Which rate to load data from memory throughout the vertices (stride, by vertex or by instance)
	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;                                // 配列1つだけ（インデックス 0）
		bindingDescription.stride = sizeof(Vertex);                    // stride: 次の要素までのバイト数  number of bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // RATE_INSTANCE（インスタンスレンダリング）

		return bindingDescription;
	}

	// ②アトリビュートデスクリプター：頂点バインディングから読み込んだ頂点データの扱い
	// Attribute Descriptor: how to handle vertex input
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		// attributeDescriptions[0]: 位置情報　Position
		// bindingDescriptionと同じ値: 頂点シェーダー (location = 0) in	
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;    // 3Dジオメトリー	
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// attributeDescriptions[1]: カラー情報　Color　（上記とほぼ同じ）
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	// オペレータオーバーライド: == 、頂点比較用（重複）
	// Operator Override for vertex comparison: equals == 
	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

// 頂点重複除きハッシュ関数（後でもっと勉強すること）
// Hash function for filtering duplicate vertices (study this later!)
namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator() (Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos)
				^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1)
				^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

// UBO (UniformBufferObject): マトリクス変換情報・MVP Transform
struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


// Vulkan上のあらゆる処理はキューで処理されています。処理によってキューの種類も異なります。
struct QueueFamilyIndices
{
	// optional: 情報が存在しているかしていないかのコンテナ（どちらでも可能）
	// optional contained value; value MAY or MAY NOT be present. (C++17)

	std::optional<uint32_t> graphicsFamily;   // グラフィック系キュー
	std::optional<uint32_t> presentFamily;    // プレゼント（描画）キュー

	// キューの各値がちゃんと存在しているか　check if value exists for all values
	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

// SwapChain詳細
// Swap Chain Details
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


// Main class
// メインクラス
class CVulkanFramework
{

public:

	// 000 デバッグコールバック
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,     // エラーの重要さ；比較オペレータで比べられます。
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void populateDebugMessengerCreateInfo
	    (VkDebugUtilsMessengerCreateInfoEXT& createInfo);    // 001 デバッグメッセンジャー用メッセージ生成
	void setupDebugMessenger();                              // 002 デバッグメッセンジャー設定
	bool checkValidationLayerSupport();                      // 003 バリデーションレイヤー対応確認
	std::vector<const char*> getRequiredExtensions();    	 // 004 バリデーションレイヤーエクステンションを獲得


	

	void run();         
	void mainLoop();    

	void initWindow();                   // 101 ウインドウ初期化
	void initVulkan();                   // 102 Vulkan初期化
	void createInstance();               // 103 Vulkanインスタンス初期化
	void createSurface();                // 104 GLFWサーフェス生成
	void pickPhysicalDevice();           // 105 Vulkan対応GPUを選択
	void createLogicalDevice();          // 106 ロジカルデバイス生成（デバイスお姉さん）
	void createSwapChain();              // 107 スワップチェイン生成
	void createImageViews();             // 108 イメージビュー生成
	void createRenderPass();             // レンダーパス
	void createDescriptorSetLayout();    // リソースでスクリプターレイアウト 
	void createGraphicsPipeline();       // グラフィックスパイプライン生成
	void createColorResources();         // カラーリソース生成（MSAA)
	void createDepthResources();         // デプスリソース生成
	void createFramebuffers();           // フレームバッファ生成（デプスリソースの後）
	void createCommandPool();            // コマンドバッファーを格納するプールを生成
	void createTextureImage();           // テクスチャーマッピング用画像生成
	void createTextureImageView();       // テクスチャーをアクセスするためのイメージビュー生成
	void createTextureSampler();         // テクスチャーサンプラー生成
	void loadModel();                    // モデルデータを読み込み
	void createVertexBuffer();           // 頂点バッファー生成
	void createIndexBuffer();		     // インデックスバッファー生成
	void createUniformBuffers();         // ユニフォームバッファー生成
	void createDescriptorPool();         // デスクリプターセットを格納するプールを生成
	void createDescriptorSets();         // デスクリプターセットを生成
	void createCommandBuffers();         // コマンドバッファー生成
	void createSyncObjects();            // 処理同期オブジェクト生成
	

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	static std::vector<char> readFile(const std::string& fileName);
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	//----------------

	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUseableSampleCount();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);
	VkFormat findDepthFormat();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	bool hasStencilComponent(VkFormat format);



	static void framebufferResizeCallback
	    (GLFWwindow* window, int width, int height);
	void recreateSwapChain();
	void updateUniformBuffer(uint32_t currentImage);
	void drawFrame();
	
	void cleanup();
	void cleanupSwapChain();





private:

	GLFWwindow*                     m_Window;                // WINDOWSではなくGLFW;　クロスプラットフォーム対応
	VkInstance                      m_Instance;              // インスタンス：アプリケーションとSDKのつながり

	VkDebugUtilsMessengerEXT        m_DebugMessenger;        // デバッグコールバック

	VkSurfaceKHR                    m_Surface;               // GLFW -> WSI (Windows System Integration) -> ウィンドウ生成

	VkPhysicalDevice    m_PhysicalDevice = VK_NULL_HANDLE;   // 物理デバイス（GPU・グラフィックスカード）
	VkDevice            m_LogicalDevice;                     // 物理デバイスとのシステムインターフェース
	VkQueue                         m_GraphicsQueue;         // グラフィックス専用キュー
	VkQueue                         m_PresentQueue;          // プレゼント（描画）専用キュー

	VkSwapchainKHR                  m_SwapChain;             // 表示する予定の画像のキュー
	std::vector<VkImage>            m_SwapChainImages;	     // キュー画像
	VkFormat                        m_SwapChainImageFormat;  // 画像フォーマット
	VkExtent2D                      m_SwapChainExtent;       // extent : 画像レゾルーション（通常、ウィンドウと同じ）

	std::vector<VkImageView>   m_SwapChainImageViews;        // VkImageのハンドル；画像を使用する際にアクセスする（ビューそのもの）
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;      // SwapChainのフレームバッファ

	VkRenderPass                    m_RenderPass;            // レンダーパス
	VkDescriptorSetLayout           m_DescriptorSetLayout;   // でスクリプターセットレイアウト
	VkPipelineLayout                m_PipelineLayout;        // グラフィックスパイプラインレイアウト
	VkPipeline                      m_GraphicsPipeline;      // グラフィックスパイプライン自体

	VkCommandPool                   m_CommandPool;           // CommandPool : コマンドバッファー、そしてその割り当てたメモリ管理、
	std::vector<VkCommandBuffer>    m_CommandBuffers;

	VkDescriptorPool                m_DescriptorPool;        // DescriptorPool : デスクリプターセット、そしてその割り当てたメモリ管理
	std::vector<VkDescriptorSet>    m_DescriptorSets;

	std::vector<Vertex>             m_Vertices;              // 頂点データ（モデル用）
	std::vector<uint32_t>           m_Indices;               // インデックスデータ（モデル用）

	VkBuffer                        m_VertexBuffer;          // 頂点バッファー
	VkDeviceMemory                  m_VertexBufferMemory;    // 頂点バッファーメモリー割り当て
	VkBuffer                        m_IndexBuffer;           // インデックスバッファー
	VkDeviceMemory                  m_IndexBufferMemory;     // インデックスバッファーメモリー割り当て

	std::vector<VkBuffer>           m_UniformBuffers;
	std::vector<VkDeviceMemory>     m_UniformBuffersMemory;

	VkImage                         m_DepthImage;            // Zソートなどのデプスバッファリング用　Depth Buffering
	VkDeviceMemory                  m_DepthImageMemory;
	VkImageView                     m_DepthImageView;

	uint32_t                        m_MipLevels;             // ミップマップ用
	VkImage                         m_TextureImage;          // テクスチャーマッピング用（Texel情報、など）
	VkDeviceMemory                  m_TextureImageMemory;
	VkImageView                     m_TextureImageView;
	VkSampler                       m_TextureSampler;

	VkSampleCountFlagBits           m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;    // マルチサンプリングビット数  Multisampling bit count 
	VkImage                         m_ColorImage;                             // マルチサンプリングバッファー用
	VkDeviceMemory                  m_ColorImageMemory;                       // マルチサンプリングバッファー用
	VkImageView                     m_ColorImageView;                         // マルチサンプリングバッファー用

	// Semaphore：簡単に「シグナル」。処理を同期するために利用します。
	// Fence: GPU-CPUの間の同期機能；ゲート見たいなストッパーである。
	std::vector<VkSemaphore>        m_ImageAvailableSemaphores;    // イメージ描画準備完了セマフォ
	std::vector<VkSemaphore>        m_RenderFinishedSemaphores;    // レンダリング完了セマフォ
	std::vector<VkFence>            m_InFlightFences;              // 起動中のフェンス
	std::vector<VkFence>            m_ImagesInFlight;              // 処理中の画像
	size_t                          m_CurrentFrame = 0;            // 現在こフレームカウンター

	bool m_FramebufferResized = false;    // ウウィンドウサイズが変更したか

};