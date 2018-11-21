#pragma once
#include "D3D12Util.h"


class ShadowMap
{
public:
	//构造函数
	ShadowMap(ID3D12Device *device, UINT width, UINT height);
	~ShadowMap() = default;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;
	ShadowMap(const ShadowMap& rhs) = delete;


	//访问接口
	UINT Width() const;
	UINT Height() const;
	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() const;
	D3D12_VIEWPORT Viewport() const;
	D3D12_RECT ScissorRect() const;

	void BuildDescriptor(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv
	);

	//重置大小，主要是shadowmap的宽度和高度
	void OnResize(UINT newWidth,UINT newHeight);

	//创建真正的descriptor
	void BuildDescriptors();

	//以深度模板的格式创建id3d12resource 资源
	void BuildResource();

private:

	//成员变量
	ID3D12Device * md3dDevice = nullptr;
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;


	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
};