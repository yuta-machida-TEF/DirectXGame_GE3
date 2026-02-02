#include "DiretXCommon.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<cassert>
#include"StringUtility.h"
#include"logger.h"


using namespace Microsoft::WRL;
using namespace StringUtility;
using namespace Logger;

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL検出
	assert(winApp);
	//メンバ変数に記録
	this->winApp = winApp;

	CreateDrive();
	CreateCommand();
	CreateSwapChan();
	CreateDepth();
	CreateDescriptorHeapRTV();
	CreateHeapType();
	CreateFence();
	CreateView();
	CreateScissor();
	CreateDXC();
	CreateImGui();

	HRESULT hr;

	// dxcUtils を生成
	hr = DxcCreateInstance(
		CLSID_DxcUtils,
		IID_PPV_ARGS(&dxcUtils_)
	);
	assert(SUCCEEDED(hr));

	// dxcCompiler を生成
	hr = DxcCreateInstance(
		CLSID_DxcCompiler,
		IID_PPV_ARGS(&dxcCompiler_)
	);
	assert(SUCCEEDED(hr));


	//// CommandQueue
	//D3D12_COMMAND_QUEUE_DESC queueDesc{};
	//hr = device_->CreateCommandQueue(
	//	&queueDesc,
	//	IID_PPV_ARGS(&commandQueue_)
	//);
	//assert(SUCCEEDED(hr));

	// CommandAllocator
	hr = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(hr));

	// CommandList
	hr = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(hr));

	// 最初は Close しておく（超重要）
	commandList_->Close();

}

void DirectXCommon::CreateDrive()
{
	HRESULT hr;

	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	//メインスレッドではMTAでCOM利用
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

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
	assert(useAdapter != nullptr);

	//ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featrueLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featrueLevels); i++)
	{
		hr = D3D12CreateDevice(useAdapter, featrueLevels[i], IID_PPV_ARGS(device.GetAddressOf()));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{

			break;
		}
	}

	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);

}

void DirectXCommon::CreateCommand()
{
	HRESULT hr;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	assert(SUCCEEDED(hr));

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(commandList.GetAddressOf()));
	assert(SUCCEEDED(hr));

}

void DirectXCommon::CreateSwapChan()
{
	HRESULT hr;


	swapChainDesc.Width = WinApp::kClientWidth;
	swapChainDesc.Height = WinApp::kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));


	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(),
		winApp->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));

}

//DepthStencilTextureを作る
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの幅
	resourceDesc.Height = height;//Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの敵
	resourceDesc.DepthOrArraySize = 1;//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる


	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態しておく
		&depthClearValue,//Clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));


	return resource;
}

void DirectXCommon::CreateDepth()
{
	ID3D12Resource* DepthResource = CreateDepthStencilTextureResource(
		device.Get(), WinApp::kClientWidth, WinApp::kClientHeight);
	// DSV用のヒープでデイスクリプタの数は１．DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap =
		CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format =
		DXGI_FORMAT_D24_UNORM_S8_UINT; // Format.基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// DSVHeapの先頭にDSVをつくる
	device->CreateDepthStencilView(
		DepthResource, &dsvDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}


//各種デスクリプタヒープの生成
void DirectXCommon::CreateDescriptorHeapRTV()
{


	//RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisbleはfalse
	rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものではないので、ShaderVisbleはtrue
	srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);


}

void DirectXCommon::CreateHeapType()
{
	HRESULT hr;

	//SwapChainからResourceを引っ張ってくる
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));

	//うまく取得できなけらば起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//SRV
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//RTV
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPU =
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// RTVハンドルの要素数を2個に変更する
	UINT RTVSize =
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 　裏表の2つ文
	for (uint32_t i = 0; i < 2; ++i) {
		D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = rtvCPU;
		CPUHandle.ptr += RTVSize * i;

		device->CreateRenderTargetView(swapChainResources[i].Get(), &rtvDesc, CPUHandle);
	}
}

// 指定した番号の CPU デスクリプタハンドルを取得
D3D12_CPU_DESCRIPTOR_HANDLE
DirectXCommon::GetCPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
	uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle =
		descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	handle.ptr += descriptorSize * index;
	return handle;
}


// 指定した番号の GPU デスクリプタハンドルを取得
D3D12_GPU_DESCRIPTOR_HANDLE
DirectXCommon::GetGPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
	uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handle =
		descriptorHeap->GetGPUDescriptorHandleForHeapStart();

	handle.ptr += descriptorSize * index;
	return handle;
}



Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	ID3D12DescriptorHeap* CreateDescriptorHeap(
		ID3D12Device * device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors, bool shaderVisible);
	{
		//ディスククリプタヒープの生成
		ID3D12DescriptorHeap* descriptorHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.Type = heapType;//レンダーターゲットビュー用
		descriptorHeapDesc.NumDescriptors = numDescriptors;//ダブルバッファように2つ。多くても構わない
		descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
		//ディスクリプタヒープが作られなかったので起動できない
		assert(SUCCEEDED(hr));
		return descriptorHeap;

	}
	return Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>();
}

void DirectXCommon::CreateFence()
{
	HRESULT hr;


	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));

	//FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}

void DirectXCommon::CreateView()
{
	//ビューボート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void DirectXCommon::CreateScissor()
{
	//ｼｻﾞｰ短形
	D3D12_RECT scissorRect{};
	//基本的にビューボートと同じ短形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;
}

void DirectXCommon::CreateDXC()
{
	HRESULT hr;

	//dxcCompilerを初期化
	IDxcUtils* dxcUtlis = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtlis));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtlis->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

}

// スワップチェーン設定
DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
void DirectXCommon::CreateImGui()
{
	//ImGuiの初期化。詳細はさして重要ではないので解説は省略する。
	//こういうもんである
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

}


Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{

    	//1,hlslファイルを読む
    Log(ConverString(std::format(L"Resources/shader/Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
    //hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr =dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    //読めなかったら止める
    assert(SUCCEEDED(hr));
    //読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTFT8の文字コードであることを通知
    //2.Compileする
    LPCWSTR arguments[] = {
    	 filePath.c_str(),//コンパイル対象のhlslファイル名
    	 L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
    	 L"-T",profile,//ShaderProfileの設定
    	 L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
    	 L"-Od",//最適化を外しておく
    	 L"-Zpr",//メモリレイアウトは行優先
    };
    //実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler_->Compile(
    	&shaderSourceBuffer,//読み込んだファイル
    	arguments,//コンパイルオプション
    	_countof(arguments),//コンパイルオプションの数
    	includeHandler_.Get(),//includeが含まれた諸々
    	IID_PPV_ARGS(&shaderResult)
    );
    //コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));
    
    	//3.警告・エラーがでていないか確認する
    	IDxcBlobUtf8* shaderError = nullptr;
    	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
    	{
    		Log(shaderError->GetStringPointer());
    		//警告・エラーダメゼッタイ
    		assert(false);
    
    	}
    
    	//4.Compile結果を受け取って返す
    	IDxcBlob* shaderBlob = nullptr;
    	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    	assert(SUCCEEDED(hr));
    	////成功したログを出す
    	Log(ConverString(std::format(L"Resources/shader/Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
    	//もう使わないリソースを解放
    	shaderSource->Release();
    	shaderResult->Release();
		includeHandler_->Release();
    	//実行用のパイナリを返却
    	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(size_t sizeInBytes)
{
	
	//頂点バッファビューを作成する
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リーソスの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリーソス。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;//リーソスのサイズ。こんかいはVector4を3頂点分
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateTextrueResource(const DirectX::TexMetadata& metadata)
{
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//2.利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	//3.Resourceを生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;

}

void DirectXCommon::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			UINT(img->rowPitch),//1ラインサイズ
			UINT(img->slicePitch)//1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

DirectX::ScratchImage DirectXCommon::LoadTexture(const std::string& filePath)
{


	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConverString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	
	return mipImages;
}

ModelData DirectXCommon::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//1.中で必要となる変数の宣言
	ModelData modelData;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める
	//3.実際にファイルを読み、MOdelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f")
		{
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; faceVertex++)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; element++)
				{
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから,実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				position.x *= 1.0f;
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				texcoord.y = 1.0f - texcoord.y;
				Vector3 normal = normals[elementIndices[2] - 1];
				normal.x *= 1.0f;
				VertexData vertex = { position, texcoord };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib")
		{
			//materialTemplateLibararyファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoagMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	//4.ModelDataを返す
	return modelData;
}

MaterialData DirectXCommon::LoagMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//1.中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//とりあえず開けなかったら止める
	//3.実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textrueFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//4.MeterialDataを返す
	return materialData;


}

UINT DirectXCommon::GetDescriptorHandleIncrementSize(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType) const {

	return device->GetDescriptorHandleIncrementSize(heapType);
}

void DirectXCommon::CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
{
	device->CreateShaderResourceView(
		resource,
		srvDesc,
		cpuHandle
	);
}

ID3D12RootSignature* DirectXCommon::CreateRootSignature(
	const D3D12_ROOT_SIGNATURE_DESC& desc
) {
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA(
				static_cast<char*>(errorBlob->GetBufferPointer())
			);
		}
		(false);
	}

	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(hr));

	return rootSignature;
}




void DirectXCommon::PreDraw()
{
	// バックバッファの番号取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// リソースバリアで書き込み可能に変更
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
	// 描画先のRTVとDSVを指定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(
		rtvDescriptorHeap,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
		backBufferIndex);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle =
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// 画面全体の色をクリア
	commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 画面全体の深度をクリア
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0,
		0, nullptr);
	// SRV用のデスクリプタヒープを指定する
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);
	// ビューポート領域の設定
	commandList->RSSetViewports(1, &viewport);
	// シザー矩形の設定
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw()
{
	// バックバッファの番号取得
	UINT BufferIndex = swapChain->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = swapChainResources[BufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	////GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain->Present(1, 0);
	//Fenceの値を更新
	fenceVal++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence.Get(), fenceVal);

	//Fenceの値を指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceVal)
	{
		//指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(fenceVal, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);



}
