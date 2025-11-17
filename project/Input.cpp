#include "Input.h"
#include "WinApp.h"
#include<cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;
	////借りてきたWinAppのインスタンスを記録
	//this->winApp_ = winApp;

	//CoInitializeEx(0, COINIT_MULTITHREADED);
	//DirectInputのインスタンス生成
	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイス生成
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
	HRESULT result;

	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	//キーボードの入力状態の更新
	result = keyboard->Acquire();
	//全キーの入力情報を取得する
	result = keyboard->GetDeviceState(sizeof(key), key);

}

bool Input::TriggerKey(BYTE keyNumber)
{
	//指定キーを押していればtrueを返す
	if (key[keyNumber])
	{
		return true;
	}

	return false;
}
