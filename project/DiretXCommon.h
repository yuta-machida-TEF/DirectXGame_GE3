#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include "WinApp.h"
#include"logger.h"
#include"StringUtility.h"
#include<array>

#include <chrono>



class DirectX;

//DirectX基盤
class DirectXCommon
{

public:

	HRESULT hr;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE DescHeap, UINT DESCRIPTOR, bool createShader);
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
	//コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;


	ID3D12Device* GetDxDevice()const { return device.Get(); }



	IDXGISwapChain4* GetSwapChain()const { return swapChain; }

	//コマンド
	//スワップチェーン
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//フェンス
	ID3D12Fence* fence = nullptr;
	//rtv
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//RTV Heap
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	//SRV Heap
	ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;
	//DSV Heap
	ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;

	//初期化
	void Initialize();
	void Device();
	void commandIze();//コマンド関連の初期化
	void swapIze();//スワップチェーンの生成
	void depthIze(ID3D12Device* device, int32_t width, int32_t height);//深度バッファの生成
	void DescriptorIze();//各種デスクリプタヒープの生成
	void RenderIze();//レンダーターゲットビューの初期化
	void viewRectangle();//ビューポート短形
	void ShortRectangle();//シザリング短形
	void dxcCommon();//DXCコンパイラの生成
	void ImguiIze(ID3D12Device* device);//IMGuiの初期化
	
	//スワップチェーンリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2>swapChainResources;

	//SRVの指定番号のCPUデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	//SRVの指定番号のGPUデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();
	
private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>CreateDescriptorHeap
	(
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeap, 
		UINT Descriptors, 
		bool HeapShader);

	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource>CreateTextureResource(const DirectX::TexMetadata& metadata);

	Microsoft::WRL::ComPtr<ID3D12Resource>UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);


	//WindowsAPI
	WinApp* winApp = nullptr;

	//指定番号のCPUデスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

private:

	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	void CreateDescriptorHeap();
	void RenderTargetView();
	void DepthStencilView();
	void ViewPort();

};
