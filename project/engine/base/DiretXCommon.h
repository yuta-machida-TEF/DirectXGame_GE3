
#pragma once
#include"WinApp.h"
#include<Windows.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include<format>
#include<cassert>
#include<array>
#include"logger.h"
#include"StringUtility.h"
#include<chrono>
#include <thread>
#include "externals/DirectXTex/DirectXTex.h"
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>

using namespace Logger;
using namespace StringUtility;


//DirectX基盤
class DirectXCommon
{
public:
	void Initialize(WinApp* winApp);

	void CreateDrive();//デバイスの生成
	void CreateCommand();//コマンド関連の生成
	void CreateSwapChan();//スワップチェーンの生成
	void CreateDepth();//深度バッファの生成
	void CreateDescriptorHeapRTV();//各種デスクリプタヒープの生成
	void CreateHeapType();//レンダーターゲットビューの初期化
	void CreateFence();//深度ステンシルビューの初期化
	void CreateView();//ビューポート短形の生成
	void CreateScissor();//シンリング短形の生成
	void CreateDXC();//DXCコンパイラの生成
	void CreateImGui();//ImGuiの初期化

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();



	//コマンド関連の初期化
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList;

	//スワップチェーンの初期化
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	//各種デスクリプタヒープの初期化
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvDescriptorHeap;
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device>device;
	// SRV用デスクリプタサイズ
	UINT descriptorSizeSRV = 0;
	//フェンス値
	UINT64 fenceVal = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	// 指定番号のCPUデスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize, uint32_t index);
	// 指定番号のGPUデスクリプタハンドルを取得する
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

	//スワップチェーンリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>swapChainResources[2];

	//シェーダーコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob>CompileShader(const std::wstring& filePath, const wchar_t* profile);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//getter
	ID3D12Device* GetDrive() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	Microsoft::WRL::ComPtr<ID3D12Fence>fence;
	HANDLE fenceEvent = nullptr;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

private:

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
		CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
			bool shaderVisible);

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;


	//ビューポート
	D3D12_VIEWPORT viewport;
	//シザリング短形
	D3D12_RECT scissorRect;

	//WindowsAPI
	WinApp* winApp = nullptr;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 2>rtvHandles{};

	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//記録時間
	std::chrono::steady_clock::time_point reference_;



};
