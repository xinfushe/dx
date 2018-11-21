#include "Win32Application.h"
#include <WindowsX.h>

HWND Win32Application::m_hwnd = nullptr;


int Win32Application::Init(D3D12App *m_D3D12App, HINSTANCE hInstance, int nCmdShow)
{
	//------------------------------------------------------------
	//窗口初始化1：填写WNDCLASS结构体
	//------------------------------------------------------------
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	//------------------------------------------------------------
	//窗口初始化2：注册窗口
	//------------------------------------------------------------
	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return 0;
	}


	//------------------------------------------------------------
	//窗口初始化3：创建窗口
	//------------------------------------------------------------
	RECT windowRect = { 0, 0, static_cast<LONG>(m_D3D12App->GetWidth()), static_cast<LONG>(m_D3D12App->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;


	m_hwnd = CreateWindow(
		wc.lpszClassName,
		m_D3D12App->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,		
		nullptr,		
		hInstance,
		m_D3D12App);

	if (!m_hwnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	m_D3D12App->OnInit();

	//更新和显示窗口
	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	//窗口的消息循环部分放在了Run函数。
	

	Run(m_D3D12App);

	return 0;
}

int Win32Application::Run(D3D12App *m_D3D12App)
{
	m_D3D12App->Run();

	return 0;
}

LRESULT Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	D3D12App* m_D3D12App = reinterpret_cast<D3D12App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
		case WM_CREATE:
		{
			// 窗口创建时，将m_D3D12App存入窗口，之后主窗口程序完成CreateWindow的调用。
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			return 0;
		}
		//窗口激活及失效时游戏开始与暂停
		case WM_ACTIVATE:
		{
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				m_D3D12App->setPaused(true);
				m_D3D12App->GetTimer().Stop();
			}
			else
			{
				m_D3D12App->setPaused(false);
				m_D3D12App->GetTimer().Start();
			}

			return 0;
		}


		case WM_SIZE:
		{
			//根据窗口最小化的状态自动暂停游戏
			m_D3D12App->setWidth(LOWORD(lParam));
			m_D3D12App->setHeight(HIWORD(lParam));
			if (m_D3D12App->IsDevice())
			{
				if ( wParam == SIZE_MINIMIZED )
				{
					m_D3D12App->setPaused(true);
					m_D3D12App->setMinimized(true);
					m_D3D12App->setMaximized(false);
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_D3D12App->setPaused(false);
					m_D3D12App->setMinimized(false);
					m_D3D12App->setMaximized(true);
				}
				else if (wParam == SIZE_RESTORED)//窗口恢复
				{
					//从最小化状态恢复
					if (m_D3D12App->getMinimized())
					{
						m_D3D12App->setPaused(false);
						m_D3D12App->setMinimized(false);
						m_D3D12App->OnResize();
					}
					else if (m_D3D12App->getMaximized())
					{
						m_D3D12App->setPaused(false);
						m_D3D12App->setMaximized(false);
						m_D3D12App->OnResize();
					}
					else if (m_D3D12App->getResizing())
					{
						/*
							 这个分支是用于判断用户是否仍然在改变窗口大小的过程中，比如鼠标拖动窗口的大小等，
							 如果是，我们先不改变d3d12的缓冲区大小，等待用户完成修改后，再调用resize函数。
						*/
					}
					else
					{
						m_D3D12App->OnResize();
					}
				}
			}
			return 0;
		}

		//拖动窗口消息
		case WM_ENTERSIZEMOVE:
		{
			m_D3D12App->setPaused(true);
			m_D3D12App->setResizing(true);
			m_D3D12App->GetTimer().Stop();
			return 0;
		}

		case WM_EXITSIZEMOVE:
		{
			m_D3D12App->setPaused(false);
			m_D3D12App->setResizing(false);
			m_D3D12App->GetTimer().Start();
			m_D3D12App->OnResize();
			return 0;
		}

		case WM_MENUCHAR:
			// 关闭警报.
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYUP:
		{
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
				m_D3D12App->set4xMsaaState(!m_D3D12App->get4xMsaaState());

			m_D3D12App->OnKeyUp(static_cast<UINT8>(wParam));
			return 0;
		}

		case WM_KEYDOWN:
		{
			m_D3D12App->OnKeyDown(static_cast<UINT8>(wParam));
			return 0;
		}

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			 m_D3D12App->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			 return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			m_D3D12App->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			 return 0;
		case WM_MOUSEMOVE:
			m_D3D12App->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			 return 0;

	}

		

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);

}
