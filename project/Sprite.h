#include"SpriteCommon.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <cmath>
#include <string>
#include"MathUtils.h"
#pragma once

class SpriteCommon;

class Sprite
{
public:


	struct Transform {
		MyMath::Vector3 scale;
		MyMath::Vector3 rotate;
		MyMath::Vector3 translate;
	};

	//頂点データ
	struct VertexData
	{
		MyMath::Vector4 position;
		MyMath::Vector2 texcoord;
		MyMath::Vector3 normal;
	};

	//マテリアルデータ
	struct Material
	{
		MyMath::Vector4 color;
		/*int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransfrom;*/
	};

	struct TransformationMatrix
	{
		MyMath::Matrix4x4 WVP;
		MyMath::Matrix4x4 World;
	};

	//getter
	const MyMath::Vector2& GetPostion() const { return postion; }
	float GetRotation()const { return rotation; }
	const MyMath::Vector4& GetColor() const { return materialData->color; }
	const MyMath::Vector2& GetSize() const { return size; }
	const MyMath::Vector2& GetAnchorPoint() const { return anchorPoint; }
	const bool IsFlipsX() const { return isFilpX_; }
	const bool IsFlipsY() const { return isFlipY_; }
	const MyMath::Vector2& GetTextureLeftTop() const { return textureLeftTop; }
	const MyMath::Vector2& GetTextureSize() const { return textureSize; }

	//setter
	void Setposition(const MyMath::Vector2& postion) { this->postion = postion; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const MyMath::Vector4& color) { materialData->color = color; }
	void SetSize(const MyMath::Vector2& size) { this->size = size; }
	void SetAnchorPoint(const MyMath::Vector2& anchoPoint) { this->anchorPoint; }
	void SetFilpsX(bool isFilpsX) { this->isFilpX_ = isFilpsX; }
	void SetFilpsY(bool isFilpsY) { this->isFlipY_ = isFilpsY; }
	void SetTextureLeftTop(const MyMath::Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	void SetTextureSize(const MyMath::Vector2& textureSize) { this->textureSize; }


	void Initiailze(SpriteCommon* spriteCommon, std::string textureFilePath);
	void Update();
	void Draw();

private:

	SpriteCommon* spriteCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource;
	//頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource>IndexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;
	
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transforMationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};

	Microsoft::WRL::ComPtr<ID3D12Resource> textrueFileResource;

	MyMath::Vector2 postion = { 1.0f,1.0f };
	float rotation = 0.0f;
	MyMath::Vector2 size = { 640.0f,360.0f };

	//テクスチャ番号
	uint32_t textureIndex = 0;

	MyMath::Vector2 anchorPoint = { 0.5f,0.5f };

	//左右フリップ
	bool isFilpX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	//テクスチャ左上座標
	MyMath::Vector2 textureLeftTop = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	MyMath::Vector2 textureSize = { 100.0f,100.0f };

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

	MyMath::Transform transform
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
};
