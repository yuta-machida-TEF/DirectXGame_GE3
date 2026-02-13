#pragma once
//DirectInputインクルード
#include<Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include "engine/base/WinApp.h"


class Input
{
public:


	//初期化
	void Initialize(WinApp* winApp);
	void Update();

	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//キーボードデバイスの生成
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

private:
	
	ComPtr<IDirectInputDevice8>keyboard;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput = nullptr;

	//全キーの状態
	BYTE key[256] = {};
	BYTE keyPre[356] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;

};