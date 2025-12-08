#include "DiretXCommon.h"

void DirectXCommon::Initialize()
{
//	//DXGIファクトリーの生成
//	IDXGIFactory7* dxgiFactory = nullptr;
//	//メインスレッドではMTAでCOM利用
//	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
//	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
//	assert(SUCCEEDED(hr));
//
//	//使用するアダブタ用の変数。最初にnullptrを入れておく
//	IDXGIAdapter4* useAdapter = nullptr;
//	//よい順にアダブタを頼む
//	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
//		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
//		DXGI_ERROR_NOT_FOUND; i++)
//	{
//		//アダブターの情報を取得する
//		DXGI_ADAPTER_DESC3 adapterDesc{};
//		hr = useAdapter->GetDesc3(&adapterDesc);
//		assert(SUCCEEDED(hr));//取得できないのは一大事
//		//ソフトウェアアダブタでなければ採用!
//		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
//		{
//			Log(ConverString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));//ここのエラーは、0-5の補足教材にある
//			break;
//		}
//		useAdapter = nullptr;
//	}
//	//適切なアダブタが見つからなかったので起動できない
//	assert(useAdapter != nullptr);
//
//	ID3D12Device* device = nullptr;
//	//機能レベルとログ出力用の文字列
//	D3D_FEATURE_LEVEL featrueLevels[] = {
//		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
//	};
//	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
//	//高い順に生成できるか試していく
//	for (size_t i = 0; i < _countof(featrueLevels); i++)
//	{
//		hr = D3D12CreateDevice(useAdapter, featrueLevels[i], IID_PPV_ARGS(&device));
//		//指定した機能レベルでデバイスが生成できたかを確認
//		if (SUCCEEDED(hr))
//		{
//			//生成できたのでログ出力を行ってループを抜ける
//			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
//			break;
//		}
//	}
//
//	//デバイスの生成がうまくいかなかったので起動できない
//	assert(device != nullptr);
//	Log("Complete create D3D12Drivice!!!\n");//初期化完了のログをだす
//
//
//#ifdef _DEBUG
//	ID3D12InfoQueue* infoQueue = nullptr;
//	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
//	{
//		//ヤバいエラー時に止まる
//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//		//エラー時に止まる
//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//		//警告時に止まる
//		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
//		//抑制するメッセージのID
//		D3D12_MESSAGE_ID denyIds[] = {
//			//Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
//			//https://stackoberflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
//			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
//		};
//		//抑制するレベル
//		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
//		D3D12_INFO_QUEUE_FILTER filter{};
//		filter.DenyList.NumIDs = _countof(denyIds);
//		filter.DenyList.pIDList = denyIds;
//		filter.DenyList.NumSeverities = _countof(severities);
//		filter.DenyList.pSeverityList = severities;
//		//指定したメッセージの表示を抑制する
//		infoQueue->PushStorageFilter(&filter);
//
//		//解放
//		infoQueue->Release();
//	}

}
