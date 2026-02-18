#include"engine/base/DiretXCommon.h"
#pragma once

//スプライト共通部
class SpriteCommon
{
public:

	void Initiailze(DirectXCommon* dxCommon);
	DirectXCommon* GetDxCommon() const { return dxCommon_; }
	DirectXCommon* commandList_ = nullptr;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	//共通描画設定
	void CommonDrawSetting();


private:

	////バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> roolSignatrue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState = nullptr;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインステートの作成
	void CreateGraphicsPipelineState();

	DirectXCommon* dxCommon_;
};

