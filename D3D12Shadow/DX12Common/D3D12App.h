
//-------------------------------------------------------------
//------------------- D3D12程序框架  -------------------------
//时间：2018-9-6    by:   淡一抹夕霞
//说明：D3D程序使用的基础框架
//-------------------------------------------------------------
#pragma once
#include "D3D12Util.h"



class D3D12App
{
public:
	//初始化函数
	D3D12App(UINT width,UINT height,std::wstring name);
	D3D12App(const D3D12App& rhs) = delete;
	D3D12App& operator=(const D3D12App& rhs) = delete;
	virtual ~D3D12App();
	
	int Run();
public:
	//可以根据需求定制的函数
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();
	virtual void OnInit();//设备初始化，资源初始化
	virtual void OnDestroy();
	//时间控制和fps计算
	virtual void CalculateFrameStats();

	//画面更新和渲染函数
	virtual void OnUpdate(const GameTimer& gt) = 0;
	virtual void OnRender(const GameTimer& gt) = 0;

	//键盘消息处理函数
	virtual void OnKeyDown(UINT8 /*key*/) {}
	virtual void OnKeyUp(UINT8 /*key*/) {}


	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }



public:
	//获取窗口信息
	float  AspectRatio()const;
	int  GetWidth() const { return m_width; }
	int  GetHeight() const { return m_height; }

	const WCHAR* GetTitle() const { return m_title.c_str(); }
	GameTimer &GetTimer() { return m_Timer; }

	bool      getPaused() const { return m_AppPaused; };
	bool      getMinimized() const { return m_Minimized; };
	bool      getMaximized() const { return m_Maximized; };
	bool      getResizing() const { return m_Resizing; };
	bool      get4xMsaaState() const { return m_4xMsaaState; }
	bool      IsDevice() { return m_d3dDevice; }
	//设置窗口信息
	void      setWidth(int width) { m_width = width; }
	void      setHeight(int height) { m_height = height; }

	void      setPaused(bool isPaused) { m_AppPaused = isPaused; }
	void      setMinimized(bool isMinimized) { m_Minimized = isMinimized; }
	void      setMaximized(bool isMaximized) { m_Maximized = isMaximized; }
	void      setResizing(bool isResizing) { m_Resizing = isResizing; }
	void      set4xMsaaState(bool is4xMsaaState) { m_4xMsaaState = is4xMsaaState; }
	//获取适配器信息
	void GetAdpterInfo() {};

	
protected:

	void CreateCommandObjects();

	void CreateSwapChain();

	void FlushCommandQueue();//等待gpu执行完所有指令，使cpu和gpu并行

	//获取缓冲信息
	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	

	//适配器信息
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);


	std::wstring GetAssetFullPath(LPCWSTR assetName);//获取资产路径
	void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);
	void SetCustomWindowText(LPCWSTR text);


protected:

	//与窗口信息和时间函数配合使用
	bool      m_AppPaused = false;  // is the application paused?
	bool      m_Minimized = false;  // is the application minimized?
	bool      m_Maximized = false;  // is the application maximized?
	bool      m_Resizing = false;   // are the resize bars being dragged?
	bool      m_FullscreenState = false;// fullscreen enabled

	// Set true to use 4X MSAA (?.1.8).  The default is false.
	bool      m_4xMsaaState = false;    // 4X MSAA enabled
	UINT      m_4xMsaaQuality = 0;      // quality level of 4X MSAA

	//程序基础接口
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;

	//cpu与gpu的同步
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_CurrentFence = 0;

	

	//d3d12指令接口
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	//渲染管道相关接口
	static const int SwapChainBufferCount = 2;
	int m_CurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;//UI矩形区域，该区域不会进行图形绘制

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;
	UINT m_CbvSrvDescriptorSize = 0;
	//是否启用软件适配器
	bool m_useWarpDevice = false;
	
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//适配器信息
	std::vector<IDXGIAdapter*> m_AdapterList;

	//视口参数
	int m_width;
	int m_height;
	
	

	// Used gamrTimer
	GameTimer m_Timer;
	
	

private:
	//资产路径，加载hlsl文件等
	std::wstring m_assetsPath;

	//window 标题
	std::wstring m_title;

};
