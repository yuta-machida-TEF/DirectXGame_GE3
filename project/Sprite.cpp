#include "Sprite.h"
#include"SpriteCommon.h"
#include "engine/base/DiretXCommon.h"
#include "engine/base/WinApp.h"

void Sprite::Initiailze(SpriteCommon* spriteCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->spriteCommon_ = spriteCommon;

	vertexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 4);
	IndexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点６つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//１頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = IndexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };

	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };

	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };

	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };

	IndexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	//マテリアル用のリソースを作る。今回はcolor１つ分のサイズを用意する
	materialResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は白を書き込んでみる
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//Sparite用のTransformationMatrix用のリソースを作る。Matrix4x4 １つ分のサイズを用意する
	transforMationMatrixResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	//書き込むためのアドレスを取得
	transforMationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//単位行列を書きこんでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();

	//Textueを読んで転送する
	DirectX::ScratchImage mipImages = spriteCommon_->GetDxCommon()->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textrueFileResource = spriteCommon_->GetDxCommon()->CreateTextureResource(metadata);
	spriteCommon_->GetDxCommon()->UploadTextureData(textrueFileResource, mipImages);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//２Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU = spriteCommon->GetDxCommon()->GetSRVCPUDescriptorHandle(1);
	textureSrvHandleGPU = spriteCommon->GetDxCommon()->GetSRVGPUDescriptorHandle(1);
	//SRVの生成
	spriteCommon_->GetDxCommon()->GetDrive()->CreateShaderResourceView(textrueFileResource.Get(), &srvDesc, textureSrvHandleCPU);


	transform = {};
	transform.scale = { 1.0f,1.0f,1.0f };
}

void Sprite::Update()
{
	transform.translate = { postion.x,postion.y ,0.0f };

	transform.rotate = { 0.0f,0.0f,rotation };


	//SPrite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	transformationMatrixData->WVP = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixData->World = worldMatrixSprite;
}

void Sprite::Draw()
{
	//Spriteの描画。変更が必要なものだけ変更する
	spriteCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	spriteCommon_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//VBVを設定
	//TransformationMatrixBufferの場所のを設定
	//マテリアルCBufferの場所を設定
	spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());//これをいれないと描画ができない
	spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transforMationMatrixResource->GetGPUVirtualAddress());
	spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	//描画!(DrawCall/ドローコール)。3頂点で1つのインタランス。インタランスについては今後
	spriteCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}


