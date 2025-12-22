#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>

#pragma once

//DirectX基盤
class DirectXCommon
{
	void Initialize();

	void CreateDrive();//デバイスの生成
	void CreateCommand();//コマンド関連の生成
	void CreateSwapChan();//スワップチェーンの生成
	void CreateDepth();//深度バッファの生成
	void CreateDescriptorHeap();//各種デスクリプタヒープの生成
	void CreateHeapType();//レンダーターゲットビューの初期化
	void CreateFence();//深度ステンシルビューの初期化
	void CreateView();//ビューポート短形の生成
	void CreateScissor();//シンリング短形の生成
	void CreateDXC();//DXCコンパイラの生成
	void CreateImGui();//ImGuiの初期化


};
