#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "Input.h"

//DirectInputインクルード
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include<fstream>
#include<sstream>
#include "WinApp.h"






std::wstring ConverString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0,
		reinterpret_cast<const char*>(&str[0]), static_cast<int>
		(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]),
		static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConverString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0,
		str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>
		(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;

}


void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {

	
	/*if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}*/

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
	case WM_DESTROY:

		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}


IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcComppiler,
	IDxcIncludeHandler* includeHandler)
{
	//1,hlslファイルを読む
	//Log(ConverString(std::format(L"Resources/shader/Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
	hr = dxcComppiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler,//includeが含まれた諸々
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
	//成功したログを出す
	//Log(ConverString(std::format(L"Resources/shader/Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のパイナリを返却
	return shaderBlob;
}

//1.Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath)
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

	//ミニマップ付きのデータを返す
	return mipImages;
}


//3.TextureResourceにデータを転送する
void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
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

//MaterialData LoagMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
//{
//	//1.中で必要となる変数の宣言
//	MaterialData materialData;//構築するMaterialData
//	std::string line;//ファイルから読んだ1行を格納するもの
//	//2.ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());//とりあえず開けなかったら止める
//	//3.実際にファイルを読み、MaterialDataを構築していく
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		//identifierに応じた処理
//		if (identifier == "map_Kd")
//		{
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			materialData.textrueFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//	//4.MeterialDataを返す
//	return materialData;
//
//
//}
//
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
//{
//	//1.中で必要となる変数の宣言
//	ModelData modelData;//構築するModelData
//	std::vector<Vector4> positions;//位置
//	std::vector<Vector3> normals;//法線
//	std::vector<Vector2> texcoords;//テクスチャ座標
//	std::string line;//ファイルから読んだ1行を格納するもの
//	//2.ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
//	assert(file.is_open());//とりあえず開けなかったら止める
//	//3.実際にファイルを読み、MOdelDataを構築していく
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;//先頭の識別子を読む
//
//		//identifierに応じた処理
//		if (identifier == "v")
//		{
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			positions.push_back(position);
//		} else if (identifier == "vt")
//		{
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoords.push_back(texcoord);
//		} else if (identifier == "vn")
//		{
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		} else if (identifier == "f")
//		{
//			VertexData triangle[3];
//			//面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; faceVertex++)
//			{
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; element++)
//				{
//					std::string index;
//					std::getline(v, index, '/');//区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//				//要素へのIndexから,実際の要素の値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				position.x *= 1.0f;
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				texcoord.y = 1.0f - texcoord.y;
//				Vector3 normal = normals[elementIndices[2] - 1];
//				//normal.x *= 1.0f;
//			/*	VertexData vertex = { position, texcoord };
//				modelData.vertices.push_back(vertex);*/
//				triangle[faceVertex] = { position,texcoord };
//			}
//			//頂点を逆順で登録することで、回り順を逆にする
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib")
//		{
//			//materialTemplateLibararyファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//			modelData.material = LoagMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//	//4.ModelDataを返す
//	return modelData;
//}


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
	input->Update();


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
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う//b0のbと一致する
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

	////WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	////データを書き込む
	//Matrix4x4* wvpData = nullptr;
	////書き込むためにアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	////単位行列を書き込んでおく
	//*wvpData = MakeIdentity4x4();

	////シリアライズしてバイナリする
	//ID3D10Blob* signatureBlob = nullptr;
	//ID3D10Blob* errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature,
	//	D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr))
	//{
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	//バイナリを元に生成
	ID3D12RootSignature* roolSignatrue = nullptr;
	/*hr = device->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&roolSignatrue));*/
	//assert(SUCCEEDED(hr));

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
	/*IDxcBlob* vertexShaderBlob = CompileShader(L"Resources/shader/Object3D.VS.hlsl",
		L"vs_6_0", dxcUtlis, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);*/


	/*IDxcBlob* pixelShaderBlob = CompileShader(L"Resources/shader/Object3D.PS.hlsl",
		L"ps_6_0", dxcUtlis, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);*/

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = roolSignatrue;//RootSignatrue

	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	//vertexShaderBlob->GetBufferSize() };
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	//pixelShaderBlob->GetBufferSize() };//VertexShader
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
	/*hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));*/

	//Direct Input　初期化
	IDirectInput8* directInput = nullptr;
	/*hr = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));*/

	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	/*hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));*/

	//入力データ形式のセット
	//hr = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	//assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	/*hr = keyboard->SetCooperativeLevel(
		winApp->GetHInstance(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));*/

	////モデル読み込み
	//ModelData modelData = LoadObjFile("resources", "plane.obj");

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
	////マテリアルにデータを書き込む
	//Vector4* materialData = nullptr;
	////書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	////今回は赤を書き込んでみる
	//*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


	////ｼｻﾞｰ短形
	//D3D12_RECT scissorRect{};
	////基本的にビューボートと同じ短形が構成されるようにする
	//scissorRect.left = 0;
	//scissorRect.right = WinApp::kClientWidth;
	//scissorRect.top = 0;
	//scissorRect.bottom = WinApp::kClientHeight;

	////Transform変数を作る。
	//Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };


	//Textureを読んで転送する
	////DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	//DirectX::ScratchImage mipImages = LoadTexture(modelData.material.textrueFilePath);
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	//UploadTextureData(textureResource, mipImages);

	////metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	////先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	////SRVの生成
	//device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);



	////Sprite用の頂点リソースを作る
	//ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	////リーソスの先頭のアドレスを作成する
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点4つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	////1頂点あたりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexDataSprite = nullptr;
	//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	////1枚目の三角形
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	//
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };

	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };

	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };

	////2枚目の三角形
	//
	//vertexDataSprite[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	//vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	//vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[5].texcoord = { 1.0f,1.0f };

	////Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ID3D12Resource* transformtionMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
	////データを書き込む
	//Matrix4x4* transformationMatrixDataSPrite = nullptr;
	////書き込むためのアドレスを取得
	//transformtionMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSPrite));
	////単位行列を書き込んでおく
	//*transformationMatrixDataSPrite = MakeIdentity4x4();


	//ID3D12Resource* indexResourceSprice = CreateBufferResource(device, sizeof(uint32_t) * 6);
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprice->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprice->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	////頂点リソースを作る
	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();//リソースの先頭のアドレスから使う
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());//使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);//1頂点あたりのサイズ

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());//頂点データをリソースにコピー

	BYTE key[256]{};
	BYTE preKey[256]{};


	//ウィンドウのxボタンが押されるまでループ
	while (true) {
		//Windowにメッセージが来てたら最優先で処理させる
		if(winApp->ProcessMessage())
		{
			break;
		} 
		
		//描画後処理
		

//		ImGui_ImplDX12_NewFrame();
//		ImGui_ImplWin32_NewFrame();
//		ImGui::NewFrame();
//
//
//		ImGui::Begin("Sprite");
//		ImGui::ColorEdit4("material", &materialData->x, ImGuiColorEditFlags_AlphaPreview);
//		ImGui::DragFloat2("Sprite transform", &transformSprite.translate.x, 1.0f);
//		ImGui::End();
//
//		ImGui::Begin("Object");
//		ImGui::ColorEdit4("material", &materialData->x, ImGuiColorEditFlags_AlphaPreview);
//		ImGui::DragFloat("rotate.y", &transform.rotate.y, 0.1f);
//		ImGui::DragFloat3("transform", &transform.translate.x, 0.1f);
//		ImGui::End();
//		
//		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
//		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
//		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
//		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
//		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
//		*wvpData = worldViewProjectionMatrix;
//
//		//SPrite用のWorldViewProjectionMatrixを作る
//		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
//		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
//		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
//		Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
//		*transformationMatrixDataSPrite = worldViewProjectionMatrixSprite;
//
//
//		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
//		//ImGui::ShowDemoWindow();
//
//		//ImGuiの内部コマンドを生成する
//		ImGui::Render();
//		
//		//これから書き込むバックバッファのインデックスを取得
//		UINT backBuffetIndex = swapChain->GetCurrentBackBufferIndex();
//		//TransitionBarrierの設定
//		D3D12_RESOURCE_BARRIER barrier{};
//		//今回のバリアはTransitionF
//		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//		//Noneにしておく
//		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//		//バリアを張る対象のリソース。現在のバックバッファに対して行う
//		barrier.Transition.pResource = swapChainResources[backBuffetIndex];
//		//遷移前(現在)のResourceState
//		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//		//遷移後のResourceState
//		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
//
//		//TransitionBarrierを張る
//		commandList->ResourceBarrier(1, &barrier);		//TransitionBarrierを張る
//		//commandList->ResourceBarrier(1, &barrier);
//		//描画先のRTVを設定する
//		commandList->OMSetRenderTargets(1, &rtvHandles[backBuffetIndex], false, nullptr);
//		//指定した色で画面全体をクリアする
//		float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
//		commandList->ClearRenderTargetView(rtvHandles[backBuffetIndex], clearColor, 0, nullptr);
//
//		//描画先のRTVとDSVを設定する
//		/*D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
//		commandList->OMSetRenderTargets(1, &rtvHandles[backBuffetIndex], false, &dsvHandle);*/
//
//
//		//描画用のDescriptorHeapの設定
//		ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
//		commandList->SetDescriptorHeaps(1, descriptorHeaps);
//		//指定した深度で画面全体をクリアする
//		//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//
//		commandList->RSSetViewports(1, &viewport);//Viewportを設定
//		commandList->RSSetScissorRects(1, &scissorRect);//Scirssorを設定
//		//RootSignatrueを設定。PSPに設定しているけど別途設定が必要
//		commandList->SetGraphicsRootSignature(roolSignatrue);
//		commandList->SetPipelineState(graphicsPipelineState);//PSOを設定
//		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
//		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
//		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		//マテリアルCBufferの場所を設定
//		commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
//		commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());//これをいれないと描画ができない
//		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
//
//		//transform.rotate.y += 0.03f;
//		
//
//		//描画!(DrawCall/ドローコール)。3頂点で1つのインタランス。インタランスについては今後
//		//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
//
//		//Spriteの描画。変更が必要なものだけ変更する
//		commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);//VBVを設定
//		//TransformationMatrixCBufferの場所を設定
//		commandList->SetGraphicsRootConstantBufferView(1, transformtionMatrixResourceSprite->GetGPUVirtualAddress());
//		//インデックスを指定
//		commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
//		//描画! (DrawCall/ドローコール)
//		//commandList->DrawInstanced(6, 1, 0, 0);
//		//描画! (DrawCall/ドローコール)6個のインデックスを使用し1つのインタランスを描画。その他は当面で良い
//	    //commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
//
//
//		//実際のcommandListのImGuiの描画コマンドを積む
//		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
//
//		//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
//		//今回はRenderTargetからPresentにする
//		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
//		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
//		//TransitionBarrierを振る
//		commandList->ResourceBarrier(1, &barrier);
//
//
//
//		//コマンドリストの内容を確定させる。すべてのコマンドをつんでからCloseすること
//		hr = commandList->Close();
//		assert(SUCCEEDED(hr));
//		//GPUにコマンドリストの実行を行わせる
//		ID3D12CommandList* commandLists[] = { commandList };
//		commandQueue->ExecuteCommandLists(1, commandLists);
//		//GPUとOSに画面の交換を行うよう通知する
//		swapChain->Present(1, 0);
//		//Fenceの値を更新
//		fenceValue++;
//		//GPUがここまでたどり着いたときに、Fenceの値を指定した値を指定した値に代入するようにSignalを送る
//		commandQueue->Signal(fence, fenceValue);
//
//		//Fenceの値を指定したSignal値にたどり着いているか確認する
//		//GetCompletedValueの初期値はFence作成時に渡した初期値
//		if (fence->GetCompletedValue() < fenceValue)
//		{
//			//指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
//			fence->SetEventOnCompletion(fenceValue, fenceEvent);
//			//イベント待つ
//			WaitForSingleObject(fenceEvent, INFINITE);
//		}
//
//		//次のフレーム用のコマンドリストを準備
//		hr = commandAllocator->Reset();
//		assert(SUCCEEDED(hr));
//		hr = commandList->Reset(commandAllocator, nullptr);
//		assert(SUCCEEDED(hr));
//
//		
//
	}
//
//	Log(ConverString(std::format(L"WSTRING{}\n", L"abc")));
//
//
//	CloseHandle(fenceEvent);
//	fence->Release();
//	rtvDescriptorHeap->Release();
//	swapChainResources[0]->Release();
//	swapChainResources[1]->Release();
//	swapChain->Release();
//	commandQueue->Release();
//	device->Release();
//	useAdapter->Release();
//	dxgiFactory->Release();
//	vertexResource->Release();
//	graphicsPipelineState->Release();
//	signatureBlob->Release();
//	if (errorBlob)
//	{
//		errorBlob->Release();
//	}
//	roolSignatrue->Release();
//	pixelShaderBlob->Release();
//	vertexShaderBlob->Release();
//	materialResource->Release();
//
//	transformtionMatrixResourceSprite->Release();
//	vertexResourceSprite->Release();
//
//	wvpResource->Release();
//	textureResource->Release();
//	//depthStencilResource->Release();
//	dsvDescriptorHeap->Release();
//	srvDescriptorHeap->Release();
//	commandList->Release();
//	commandAllocator->Release();
//	indexResourceSprice->Release();
//
//	////ImGuiの終了処理。詳細はさして重要ではないので解説は省略する
//	////こういうもんである。初期化と逆順に行う
//	ImGui_ImplDX12_Shutdown();
//	ImGui_ImplWin32_Shutdown();
//	ImGui::DestroyContext();
//	
//
//
//#ifdef _DEBUG
//	debugController->Release();
//#endif // _DEBUG
	//CloseWindow(hwnd);

	//警告時に止まる
	//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	CoUninitialize();

	//入力解放
	delete input;

	//WindowAPIの終了処理
	winApp->Finalize();

	//WindowAPI解放
	delete winApp;
	winApp = nullptr;


	return 0;
}