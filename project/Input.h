//DirectInputインクルード
#include<Windows.h>
#include<wrl.h>
using namespace Microsoft::WRL;
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#pragma once

class Input
{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//初期化
	void Initialize(HINSTANCE hInstance,HWND hwnd);
	void Update();

	//キーボードデバイスの生成
	//前回の全てキーの状態
	BYTE keyPre[256] = {};

private:
	
	ComPtr<IDirectInputDevice8>keyboard;

};

