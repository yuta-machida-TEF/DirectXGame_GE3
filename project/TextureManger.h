#include<string>//string::を定義するために必要なヘッターファイル
#include "externals/DirectXTex/DirectXTex.h"//Direct::を定義に必要なヘッターファイル
#include<wrl.h>//Microsoft::WRLの定義に必要なヘッターファイル
#include<d3d12.h>//ID3D12Resourceに必要なヘッターファイル
#include<engine/base/DiretXCommon.h>
#include<algorithm>
#include<cassert>

#pragma once
//テクスチャマネージャー
class TextureManager
{
public:
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);


	//初期化
	void Initialize(DirectXCommon* dxCommon_);
	//テクスチャファイルのパス</pparam>
	void LoadTexture(const std::string& filePath);

	//終了
	void Finalize();


private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator = (TextureManager&) = delete;

	//テクスチャ1枚分のデータ
	struct TextureData
	{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource>resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	};
	//テクスチャデータ
	std::vector<TextureData> textureDatas;

	DirectXCommon* dxCommon_ = nullptr;

};
