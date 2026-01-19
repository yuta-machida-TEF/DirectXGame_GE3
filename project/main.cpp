
#include<string>
#include<dxgidebug.h>
#include "Input.h"
//DirectInputインクルード
#pragma comment(lib, "dxcompiler.lib")
#include<fstream>
#include<sstream>
#include"DiretXCommon.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




//
////std::string ConverString(const std::wstring& str)
////{
////
////}
//
////2.DIrectX12のTextureResourceを作る
//ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
//{
//	//1.metadataを基にResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = UINT(metadata.width);//Textureの幅
//	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
//	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
//	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き or 配列Textureの配列数
//	resourceDesc.Format = metadata.format;//TextureのFormat
//	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元
//
//	//2.利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
//	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
//	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置
//
//	//3.Resourceを生成
//	ID3D12Resource* resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,//Heapの設定
//		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
//		&resourceDesc,//Resourceの設定
//		D3D12_RESOURCE_STATE_GENERIC_READ,//初回のResourceState。Textureは基本読むだけ
//		nullptr,//Clear最適値。使わないのでnullptr
//		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
//	assert(SUCCEEDED(hr));
//	return resource;
//}
//

//void Log(const std::string& message)
//{
//	OutputDebugStringA(message.c_str());
//}
//




IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	
	IDxcIncludeHandler* includeHandler)
{

}

//1.Textureデータを読む
//DirectX::ScratchImage LoadTexture(const std::string& filePath)
//{
//	//テクスチャファイルを読んでプログラムで扱えるようにする
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConverString(filePath);
//	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//	assert(SUCCEEDED(hr));
//
//	//ミニマップの作成
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
//		image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//
//	//ミニマップ付きのデータを返す
//	return mipImages;
//}

//

//WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	////ポインタ
	WinApp* winApp = nullptr;

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();
	winApp->Update();

	//ポインタ
	Input* input = nullptr;

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//ポインタ
	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

#ifdef _DEBUG//DEBUGはCreateWindowの直後

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		//デバックレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG




#ifdef _DEBUG
	//ID3D12InfoQueue* infoQueue = nullptr;
	//if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	//{
	//	//ヤバいエラー時に止まる
	//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	//	//エラー時に止まる
	//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	//	//警告時に止まる
	//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	//	//抑制するメッセージのID
	//	D3D12_MESSAGE_ID denyIds[] = {
	//		//Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
	////		//https://stackoberflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
	//		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	//	};
	//	//抑制するレベル
	//	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	//	D3D12_INFO_QUEUE_FILTER filter{};
	//	filter.DenyList.NumIDs = _countof(denyIds);
	//	filter.DenyList.pIDList = denyIds;
	//	filter.DenyList.NumSeverities = _countof(severities);
	//	filter.DenyList.pSeverityList = severities;
	//	//指定したメッセージの表示を抑制する
	//	infoQueue->PushStorageFilter(&filter);

	//	//解放
	//	infoQueue->Release();
	//}
#endif // DEBUG
	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものではないので、ShaderVisbleはtrue
	//ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));


	//=============================PSO===========================
		//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始める
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//ConstantBuffer < Material> : register(b0);

	//RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さの配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う//b0のと一致する
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド//bというのはConstantBufferのこと

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号を使う
	//DescriptorTable
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;//パイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0-1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためにアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = MakeIdentity4x4();

	//シリアライズしてバイナリする
	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	/*hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr))
		{
			Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}*/
		//バイナリを元に生成
		ID3D12RootSignature* roolSignatrue = nullptr;
		/*hr = device->CreateRootSignature(0,
			signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&roolSignatrue));
		assert(SUCCEEDED(hr));*/

		//InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);


		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		//すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;
		//RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		//Shaderをコンパイルする
		IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/shader/Object3D.VS.hlsl",
			L"vs_6_0", dxcUtlis, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);
		
		
		IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/shader/Object3D.PS.hlsl",
			L"ps_6_0", dxcUtlis, dxcCompiler, includeHandler);
		assert(pixelShaderBlob != nullptr);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = roolSignatrue;//RootSignatrue

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };//VertexShader
		graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizeerState
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジ(形状)のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType =
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定(気にしなくて良い)
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		
		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		//Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込むします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//比較関数はLassEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		
		//実際に生成
		ID3D12PipelineState* graphicsPipelineState = nullptr;
	/*	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
			IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));*/
		
		
		//モデル読み込み
		//ModelData modelData = LoadObjFile("resources", "plane.obj");
		//
		////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		//ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
		//マテリアルにデータを書き込む
		Vector4* materialData = nullptr;
		//書き込むためのアドレスを取得
		//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		//今回は赤を書き込んでみる
		*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		
		
		
		//Transform変数を作る。
		Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };
		
		
		//Textureを読んで転送する
		//DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
		/*DirectX::ScratchImage mipImages = LoadTexture(modelData.material.textrueFilePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
		UploadTextureData(textureResource, mipImages);*/
		
		//metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		//srvDesc.Format = metadata.format;
		//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		//
		////SRVを作成するDescriptorHeapの場所を決める
		//
		//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		////先頭はImGuiが使っているのでその次を使う
		//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		////SRVの生成
		//device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

		////DSVHeapの先頭にDSVを作る
		//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
		//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture

		////Sprite用の頂点リソースを作る
		//ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
		//リーソスの先頭のアドレスを作成する
		//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
		////使用するリソースのサイズは頂点4つ分のサイズ
		//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
		////1頂点あたりのサイズ
		//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

		VertexData* vertexDataSprite = nullptr;
		//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
		////1枚目の三角形
		//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
		//vertexDataSprite[0].texcoord = { 0.0f,1.0f };

		//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
		//vertexDataSprite[1].texcoord = { 0.0f,0.0f };

		//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
		//vertexDataSprite[2].texcoord = { 1.0f,1.0f };

		//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
		//vertexDataSprite[3].texcoord = { 1.0f,0.0f };

		////2枚目の三角形

		//vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
		//vertexDataSprite[4].texcoord = { 1.0f,0.0f };
		//vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
		//vertexDataSprite[5].texcoord = { 1.0f,1.0f };

		////Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		//ID3D12Resource* transformtionMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
		//データを書き込む
		Matrix4x4* transformationMatrixDataSPrite = nullptr;
		//書き込むためのアドレスを取得
		//transformtionMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSPrite));
		//単位行列を書き込んでおく
		*transformationMatrixDataSPrite = MakeIdentity4x4();

		Transform transformSprite({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

		//ID3D12Resource* indexResourceSprice = CreateBufferResource(device, sizeof(uint32_t) * 6);
		D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
		//リソースの先頭のアドレスから使う
		//indexBufferViewSprite.BufferLocation = indexResourceSprice->GetGPUVirtualAddress();
		//使用するリソースのサイズはインデックス6つ分のサイズ
		indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
		//インデックスはuint32_tとする
		indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

		//インデックスリソースにデータを書き込む
		uint32_t* indexDataSprite = nullptr;
		/*indexResourceSprice->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
		indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
		indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;*/

		//頂点リソースを作る
		//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();//リソースの先頭のアドレスから使う
		//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());//使用するリソースのサイズは頂点のサイズ
		//vertexBufferView.StrideInBytes = sizeof(VertexData);//1頂点あたりのサイズ
		
		//頂点リソースにデータを書き込む
		VertexData* vertexData = nullptr;
		//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
		//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());//頂点データをリソースにコピー

		//ウィンドウのxボタンが押されるまでループ
	while (true) {
		//描画前処理
		dxCommon->PreDraw();
		//描画後処理
		dxCommon->PostDraw();
		//Windowにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMessage())
		{
			break;
		}

		input->Update();
		if (input->PushKey(DIK_SPACE))
		{
			OutputDebugStringA("Hit SPACE\n");
		}

		if (input->TriggerKey(DIK_SPACE))
		{

		}

		/*ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();*/

		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		*wvpData = worldViewProjectionMatrix;
		
		//SPrite用のWorldViewProjectionMatrixを作る
		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
		*transformationMatrixDataSPrite = worldViewProjectionMatrixSprite;


		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		ImGui::ShowDemoWindow();

		//ImGuiの内部コマンドを生成する
		ImGui::Render();



		//commandList->RSSetViewports(1, &viewport);//Viewportを設定
		//commandList->RSSetScissorRects(1, &scissorRect);//Scirssorを設定
		//RootSignatrueを設定。PSPに設定しているけど別途設定が必要
		//commandList->SetGraphicsRootSignature(roolSignatrue);
		//commandList->SetPipelineState(graphicsPipelineState);//PSOを設定
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
		////形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		////マテリアルCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		//commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());//これをいれないと描画ができない
		//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		//transform.rotate.y += 0.03f;


		//描画!(DrawCall/ドローコール)。3頂点で1つのインタランス。インタランスについては今後
		//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		////Spriteの描画。変更が必要なものだけ変更する
		//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);//VBVを設定
		////TransformationMatrixCBufferの場所を設定
		//commandList->SetGraphicsRootConstantBufferView(1, transformtionMatrixResourceSprite->GetGPUVirtualAddress());
		//インデックスを指定
		//commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
		
		//実際のcommandListのImGuiの描画コマンドを積む
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
		
		//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
		//今回はRenderTargetからPresentにする
		//TransitionBarrierを振る



		//コマンドリストの内容を確定させる。すべてのコマンドをつんでからCloseすること
		//hr = commandList->Close();
		//assert(SUCCEEDED(hr));


	}

	//Log(ConverString(std::format(L"WSTRING{}\n", L"abc")));


	//CloseHandle(fenceEvent);
	//fence->Release();
	//rtvDescriptorHeap->Release();
	//swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//commandQueue->Release();
	//device->Release();
	////useAdapter->Release();
	//dxgiFactory->Release();
	//vertexResource->Release();
	//graphicsPipelineState->Release();
	//signatureBlob->Release();
	//if (errorBlob)
	//{
	//	errorBlob->Release();
	//}
	//roolSignatrue->Release();
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();
	//materialResource->Release();
	//
	//transformtionMatrixResourceSprite->Release();
	//vertexResourceSprite->Release();
	//
	//wvpResource->Release();
	//textureResource->Release();
	//dsvDescriptorHeap->Release();
	//srvDescriptorHeap->Release();
	//commandList->Release();
	//commandAllocator->Release();
	//indexResourceSprice->Release();
	//
	//////ImGuiの終了処理。詳細はさして重要ではないので解説は省略する
	//////こういうもんである。初期化と逆順に行う
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();



#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG

	//警告時に止まる
	//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

	//リソースリークチェック
	/*IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	CoUninitialize();*/

	//入力解放
	delete input;

	//WindowAPIの終了処理
	winApp->Finalize();

	//WindowAPI解放
	delete winApp;

	winApp = nullptr;
	//DirectXの初期化
	delete dxCommon;

	return 0;
}
