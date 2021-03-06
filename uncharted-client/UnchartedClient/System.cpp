#include "System.h"



System::System(HINSTANCE hInstance)
{
	g_pApplicationHandle = this;

	m_applicationName	= "DirectX11Demo";
	m_hInstance			= hInstance;
	m_hwnd				= NULL;
	m_pInput			= nullptr;
	m_pGraphics			= nullptr;
}


System::System(const System &other)
{
}


System::~System()
{
}


bool System::Init()
{
	INT screenWidth = 0;
	INT screenHeight = 0;

	if (!InitWindows(screenWidth, screenHeight))
		return false;

	m_pInput = new Input();
	if (!m_pInput)
		return false;
	m_pInput->Init(m_hInstance, m_hwnd, screenWidth, screenHeight);

	m_pGraphics = new Graphics();
	if (!m_pGraphics)
		return false;
	return m_pGraphics->Init(screenWidth, screenHeight, m_hwnd);
}


void System::Shutdown()
{
	if (m_pGraphics) {
		m_pGraphics->Shutdown();
		Memory::SafeDelete(m_pGraphics);
	}

	if (m_pInput) {
		m_pInput->Shutdown();
		Memory::SafeDelete(m_pInput);
	}

	ShutdownWindows();
}


int System::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (!Frame())
				break;
		}
	}

	return static_cast<int>(msg.wParam);
}


bool System::Frame()
{
	if (!m_pInput->Frame())
		return false;

	if (m_pInput->IsKeyDown(VK_ESCAPE))
		return false;

	return m_pGraphics->Frame(m_pInput);
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool System::InitWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra		= 0;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.cbWndExtra		= 0;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance		= m_hInstance;
	wcex.lpfnWndProc	= WndProc;
	wcex.lpszClassName	= "DXWNDCLASS";
	wcex.lpszMenuName	= NULL;
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wcex))
		return false;

	if (FULL_SCREEN) {
		screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		dmScreenSettings.dmPelsWidth	= static_cast<UINT>(screenWidth);
		dmScreenSettings.dmPelsHeight	= static_cast<UINT>(screenHeight);
		dmScreenSettings.dmSize			= sizeof(DEVMODE);

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			return false;
	}
	else {
		screenWidth  = 1080;
		screenHeight = 720;
	}

	m_hwnd = CreateWindow("DXWNDCLASS", m_applicationName, WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2, screenWidth, screenHeight,
		NULL, NULL, m_hInstance, NULL);

	if (!m_hwnd)
		return false;

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return true;
}


void System::ShutdownWindows()
{
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass("DXWNDCLASS", m_hInstance);
	m_hInstance = NULL;

	g_pApplicationHandle = nullptr;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		if (g_pApplicationHandle)
			return g_pApplicationHandle->MessageHandler(hwnd, msg, wParam, lParam);
		else
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}