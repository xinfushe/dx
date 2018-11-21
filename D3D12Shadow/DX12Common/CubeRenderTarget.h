#pragma once
#include "D3D12Util.h"

enum CubeMapFace :int
{
	PositiveX = 0,
	NegativeX = 1,
	PositiveY = 2,
	NegativeY = 3,
	PositiveZ = 4,
	NegativeZ = 5
};

class CubeRenderTarget
{
public:
	CubeRenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
	~CubeRenderTarget() = default;
	CubeRenderTarget(const CubeRenderTarget& rhs) = delete;
	CubeRenderTarget& operator=(const CubeRenderTarget& rhs) = delete;

	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv(int faceIndex);

	D3D12_VIEWPORT Viewport() const;
	D3D12_RECT SicssorRect() const;

	void BuildDescriptor(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv[6]
	);

	void OnResize(UINT width, UINT height);
private:
	void BuildDescriptor();
	void BuildResource();

private:
	ID3D12Device * md3dDevice;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv[6];

	Microsoft::WRL::ComPtr<ID3D12Resource> mCubeMap = nullptr;
};