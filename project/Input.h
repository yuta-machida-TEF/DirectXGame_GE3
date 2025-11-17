//DirectInputインクルード
#pragma once
#include <windows.h>

#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

class WinApp;

class Input
{
public:

	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//初期化
	void Initialize(WinApp* winApp);
	void Update();

	//キーボードデバイスの生成
	//前回の全てキーの状態
	bool TriggerKey(BYTE keyNumber);

private:

	//キーボードのデバイス
	ComPtr<IDirectInput8>directInput;
	ComPtr<IDirectInputDevice8> keyboard;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//directInput

	//keyboardDevice

	//各キー


};

