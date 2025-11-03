#include<Windows.h>
#include <wrl.h>
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
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "Input.h"
using namespace Microsoft::WRL;






void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	//CoInitializeEx(0, COINIT_MULTITHREADED);
	//DirectInputのインスタンス生成
	ComPtr<IDirectInput8>directInput = nullptr;
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイス生成
	ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	//キーボードの入力状態の更新

}
