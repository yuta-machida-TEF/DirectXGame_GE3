
#pragma once


//WindowsAPI
class WinApp
{
public:
	//静的メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, 
	UINT msg, WPARAM wparam, LPARAM lparam);

	//getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return wc.hInstance; }


	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	void Initialize();
	void Update();

private:

	//ウィンドウハンドル
	HWND hwnd = nullptr;
	//ウィンドウクラス
	WNDCLASS wc{};
};

