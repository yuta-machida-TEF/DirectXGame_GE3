
#pragma once


//WindowsAPI
class WinApp
{
public:
	//静的メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, 
	UINT msg, WPARAM wparam, LPARAM lparam);


	void Initialize();
	void Update();

private:

};

