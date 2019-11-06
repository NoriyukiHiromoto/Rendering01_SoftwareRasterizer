/*
 * MIT License
 *  Copyright (c) 2019 SPARKCREATIVE
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  @author Noriyuki Hiromoto <hrmtnryk@sparkfx.jp>
*/

//======================================================================================================
//
//======================================================================================================
#include <Application/Application.h>
#include <Renderer/FrameBuffer.h>
#include <Misc/Timer.h>

//======================================================================================================
//
//======================================================================================================
class DIBBuffer
{
private:
	Color*	_pSurface;
	int32	_Width;
	int32	_Height;
	HBITMAP	_hBitmap;
	HWND	_hWnd;
	HDC		_hWindowDC;
	HDC		_hSurfaceDC;

public:
	Color* Surface()
	{
		return _pSurface;
	}
	HDC SurfaceDC()
	{
		return _hSurfaceDC;
	}
	int32 Width()
	{
		return _Width;
	}
	int32 Height()
	{
		return _Height;
	}
	void Create(HWND hWnd, HDC hWindowDC, int32 w, int32 h)
	{
		_Width = w;
		_Height = h;

		BITMAPINFOHEADER BmpInfoHeader = { sizeof(BITMAPINFOHEADER) };
		BmpInfoHeader.biWidth = +_Width;
		BmpInfoHeader.biHeight = -_Height;	// BMPはYが下から上だけど高さをマイナスにすると上から下になる
		BmpInfoHeader.biPlanes = 1;
		BmpInfoHeader.biBitCount = 32;
		BmpInfoHeader.biCompression = BI_RGB;

		_hWnd = hWnd;
		_hWindowDC = hWindowDC;
		_hBitmap = ::CreateDIBSection(_hWindowDC, (BITMAPINFO*)&BmpInfoHeader, DIB_RGB_COLORS, (void**)&_pSurface, nullptr, 0);
		_hSurfaceDC = ::CreateCompatibleDC(_hWindowDC);
		::SelectObject(_hSurfaceDC, _hBitmap);
	}
	void Release()
	{
		::ReleaseDC(_hWnd, _hSurfaceDC);
		::DeleteObject(_hBitmap);
		::ReleaseDC(_hWnd, _hWindowDC);
	}
};

//======================================================================================================
//
//======================================================================================================
static Application _App;
static bool _RequireSaveScene;

//======================================================================================================
//
//======================================================================================================
static void SaveToBMP(const wchar_t* pFileName, const FrameBuffer<Color>& DIBBuff)
{
	BITMAPINFOHEADER BmpIH = { sizeof(BITMAPINFOHEADER) };
	BmpIH.biWidth = DIBBuff.GetWidth();
	BmpIH.biHeight = DIBBuff.GetHeight();
	BmpIH.biPlanes = 1;
	BmpIH.biBitCount = 32;
	BmpIH.biCompression = BI_RGB;

	BITMAPFILEHEADER BmpH;
	BmpH.bfType = 'MB';
	BmpH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(uint32) * BmpIH.biWidth * BmpIH.biHeight;
	BmpH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	HANDLE hFile = ::CreateFile(L"ScreenShot.bmp", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD WriteSize;
		::WriteFile(hFile, &BmpH, sizeof(BITMAPFILEHEADER), &WriteSize, nullptr);
		::WriteFile(hFile, &BmpIH, sizeof(BITMAPINFOHEADER), &WriteSize, nullptr);
		const Color* pPixel = DIBBuff.GetPixelPointer() + (BmpIH.biWidth * BmpIH.biHeight);
		for (int32 y = 0; y < BmpIH.biHeight; ++y)
		{
			pPixel -= BmpIH.biWidth;
			::WriteFile(hFile, pPixel, sizeof(uint32) * BmpIH.biWidth, &WriteSize, nullptr);
		}
		::CloseHandle(hFile);
	}
}

//======================================================================================================
//
//======================================================================================================
LRESULT CALLBACK MessageProc(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam)
{
	static POINT CursorPosition;

	switch (Msg)
	{
		//---------------------------------------------
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&CursorPosition);
		break;
		//---------------------------------------------
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		::ReleaseCapture();
		break;
		//---------------------------------------------
	case WM_MOUSEMOVE:
		{
			POINT NewCursorPosition;
			::GetCursorPos(&NewCursorPosition);
			const int32 mx = NewCursorPosition.x - CursorPosition.x;
			const int32 my = NewCursorPosition.y - CursorPosition.y;
			CursorPosition = NewCursorPosition;
			if (wParam & MK_LBUTTON)
			{
				_App.OnLefeMouseDrag(mx, my);
			}
			if (wParam & MK_RBUTTON)
			{
				_App.OnRightMouseDrag(mx, my);
			}
			if (wParam & MK_MBUTTON)
			{
				_App.OnWheelMouseDrag(mx, my);
			}
		}
		break;
		//---------------------------------------------
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case VK_RETURN:
			_RequireSaveScene = true;
			break;
		}
		break;
		//---------------------------------------------
	case WM_CREATE:
		::SetCursor(LoadCursor(nullptr, IDC_ARROW));
		break;
		//---------------------------------------------
	case WM_CLOSE:
		::PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
		//---------------------------------------------
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//======================================================================================================
//
//======================================================================================================
int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int32)
{
	wchar_t ModuleFileName[MAX_PATH];
	WNDCLASS WindowClass;
	HWND hWnd;
	MSG Msg;
	Timer Timer;

	//---------------------------------------------------------
	// カレントディレクトリ設定
	//---------------------------------------------------------
	GetModuleFileName(hInstance, ModuleFileName, MAX_PATH);
	auto Length = (int32)wcslen(ModuleFileName);
	while (ModuleFileName[Length] != L'\\')
	{
		if (--Length < 0)
		{
			break;
		}
	}
	ModuleFileName[Length + 1] = L'\0';
	SetCurrentDirectory(ModuleFileName);

	//------------------------------------------------------------
	// ウィンドウクラス
	//------------------------------------------------------------
	WindowClass.style = CS_DBLCLKS;
	WindowClass.lpfnWndProc = MessageProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = nullptr;
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WindowClass.lpszMenuName = nullptr;
	WindowClass.lpszClassName = APPLICATION_TITLE;
	RegisterClass(&WindowClass);

	//------------------------------------------------------------
	// ウィンドウ生成
	//------------------------------------------------------------
	auto x = 0;
	auto y = 0;
	auto w = SCREEN_WIDTH;
	auto h = SCREEN_HEIGHT;
	auto Style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	RECT Rect = { 0, 0, w, h };
	::AdjustWindowRect(&Rect, Style, FALSE);
	w = Rect.right - Rect.left;
	h = Rect.bottom - Rect.top;
	x = ::GetSystemMetrics(SM_CXSCREEN) / 2 - w / 2;
	y = ::GetSystemMetrics(SM_CYSCREEN) / 2 - h / 2;

	hWnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,
		APPLICATION_TITLE,
		APPLICATION_TITLE,
		Style,
		x, y, w, h,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	::ShowWindow(hWnd, SW_NORMAL);
	::UpdateWindow(hWnd);

	//--------------------------------------------------------------------------
	// 初期化処理
	//--------------------------------------------------------------------------
	if (!_App.OnInitialize())
	{
		return 1;
	}

	//--------------------------------------------------------------------------
	// バッファ生成
	//--------------------------------------------------------------------------
	auto hWindowDC = ::GetDC(hWnd);

	DIBBuffer DIBBuff;
	DIBBuff.Create(hWnd, hWindowDC, SCREEN_WIDTH, SCREEN_HEIGHT);

	ColorBuffer ColorBuff(DIBBuff.Surface(), DIBBuff.Width(), DIBBuff.Height());
	DepthBuffer DepthBuff(nullptr, SCREEN_WIDTH, SCREEN_HEIGHT);

	ColorBuff.Clear(0xFF000000);
	DepthBuff.Clear(1.0f);

	//--------------------------------------------------------------------------
	// メッセージループ
	//--------------------------------------------------------------------------
	bool IsRunning = true;
	while (IsRunning)
	{
		//------------------------------------------------------
		// ウインドウメッセージ処理
		//------------------------------------------------------
		while (::PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (Msg.message == WM_QUIT)
			{
				IsRunning = false;
				break;
			}
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}

		//------------------------------------------------------
		// FPS計算
		//------------------------------------------------------
		{
			static auto FPS = 0U;
			static auto PreTime = Timer.GetMicro();
			auto NowTime = Timer.GetMicro();
			if (NowTime - PreTime >= 1000000 / 4)
			{
				wchar_t Text[300];
				swprintf_s(Text, 300, L"%s (FPS:%.1lf) (Polygon: %u/frame) (Vertex: %u/frame)",
					APPLICATION_TITLE,
					(fp32)FPS * 1000000.0f / (fp32)(NowTime - PreTime),
					_App.GetTriangleCount(),
					_App.GetVertexCount());
				::SetWindowText(hWnd, Text);
				FPS = 0;
				PreTime = NowTime;
			}
			FPS++;
		}

		//------------------------------------------------------
		// メイン処理
		//------------------------------------------------------
		{
			// バッファを画面に転送してクリアする（必要ならBMP出力も
			::BitBlt(
				hWindowDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
				DIBBuff.SurfaceDC(), 0, 0, SRCCOPY);
			if (_RequireSaveScene)
			{
				_RequireSaveScene = false;
				SaveToBMP(L"ScreenShot.bmp", ColorBuff);
			}
			ColorBuff.Clear(0xFF000000);
			// 深度バッファをクリアする
			DepthBuff.Clear(1.0f);

			// フレームのdeltaを求める
			static auto PreTime = Timer.GetMicro();
			auto NowTime = Timer.GetMicro();
			auto FrameTime = (fp32)(NowTime - PreTime) / 1000000.0f;
			PreTime = NowTime;

			// フレームの更新処理
			_App.OnUpdate(FrameTime);
			_App.OnRendering(&ColorBuff, &DepthBuff);
		}
	}

	//--------------------------------------------------------------------------
	// バックバッファ解放
	//--------------------------------------------------------------------------
	DIBBuff.Release();

	//--------------------------------------------------------------------------
	// 解放
	//--------------------------------------------------------------------------
	_App.OnFinalize();

	return 0;
}

//======================================================================================================
//
//======================================================================================================
int32 main()
{
	return WinMain(::GetModuleHandle(nullptr), nullptr, nullptr, 0);
}
