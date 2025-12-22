#include "DiretXCommon.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<wrl.h>

using namespace Microsoft::WRL;

void DirectXCommon::Initialize()
{
	void CreateDrive();
	void CreateCommand();
	void CreateSwapChan();
	void CreateDepth();
	void CreateDescriptorHeap();
	void CreateHeapType();
	void CreateFence();
	void CreateView();
	void CreateScissor();
	void CreateDXC();
	void CreateImGui();
}

void DirectXCommon::CreateDrive()
{
	HRESULT hr;

	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	//メインスレッドではMTAでCOM利用
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	//使用するアダブタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	//よい順にアダブタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++)
	{
		//アダブターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		//assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダブタでなければ採用!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
	
			break;
		}
		useAdapter = nullptr;
	}
	//適切なアダブタが見つからなかったので起動できない
	//assert(useAdapter != nullptr);

	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featrueLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featrueLevels); i++)
	{
		hr = D3D12CreateDevice(useAdapter, featrueLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			
			break;
		}
	}

	//デバイスの生成がうまくいかなかったので起動できない
	//assert(device != nullptr);
	
}

void DirectXCommon::CreateCommand()
{
}

void DirectXCommon::CreateSwapChan()
{
}

void DirectXCommon::CreateDepth()
{
}

void DirectXCommon::CreateDescriptorHeap()
{
}

void DirectXCommon::CreateHeapType()
{
}

void DirectXCommon::CreateFence()
{
}

void DirectXCommon::CreateView()
{
}

void DirectXCommon::CreateScissor()
{
}

void DirectXCommon::CreateDXC()
{
}

void DirectXCommon::CreateImGui()
{
}
