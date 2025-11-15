#include<Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <dxgi1_6.h> 
#include <d3d12.h>
#include<string>
#include<format>
#include<cassert>
#include<dxgidebug.h>
#include "externals/DirectXTex/DirectXTex.h"
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include "Input.h"






void Input::Initialize(WinApp* winApp)
{
	HRESULT result;
	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	//CoInitializeEx(0, COINIT_MULTITHREADED);
	//DirectInputのインスタンス生成
	/*ComPtr<IDirectInput8>directInput = nullptr;*/
	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイス生成
	//ComPtr<IDirectInputDevice8>keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}

void Input::Update()
{
	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	//キーボードの入力状態の更新
	keyboard->Acquire();

	//全キーの入力情報を取得する
	keyboard->GetDeviceState(sizeof(key), key);

}

//bool PushKey(BYTE keyNumber)
//{
//	//指定キーを押していればtrueを返す
//	BYTE key[256] = {};
//	if (key[keyNumber])
//	{
//		return true;
//	}
//
//
//	return false;
//}

bool TriggerKey(BYTE keyNumber)
{
	BYTE key[256] = {};
	if(key[keyNumber])
	{
		OutputDebugStringA("Hit SPACE\n");
		return true;
	}

	return false;
}
