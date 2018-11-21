//-------------------------------------------------------------
//-------------------d3d12ShadowMap----------------------------
//时间：2018-11-20       by:淡一抹夕霞
//说明：d3d12 ShadowMap Demo.
//-------------------------------------------------------------
#pragma once


#include "D3D12App.h"
#include "FrameResource.h"
#include "D3D12Camera.h"
#include "CubeRenderTarget.h"
#include "ShadowMap.h"

struct RenderItem
{
	RenderItem() = default;

	//世界矩阵
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//Texture Transform
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	//对应frameresource中的常量缓冲区的索引
	UINT ObjCBIndex = -1;

	//物体材质
	Material *Mat = nullptr;
	//引用几何体信息
	MeshGeometry *Geo = nullptr;

	//primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;

};



class ShadowMapApp :public D3D12App
{
public:
	ShadowMapApp(UINT width, UINT height, std::wstring name);
	~ShadowMapApp();
	ShadowMapApp(const ShadowMapApp& rhs) = delete;
	ShadowMapApp &operator=(const ShadowMapApp& rhs) = delete;


	virtual void OnInit() override;
private:
	//由于我们需要增加dsv或者rtv所以重载此函数
	virtual void CreateRtvAndDsvDescriptorHeaps() override;
	virtual void OnResize()override;
	virtual void OnUpdate(const GameTimer& gt)override;
	virtual void OnRender(const GameTimer& gt)override;
	virtual void CalculateFrameStats() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);

	void UpdateObjectCBs(const GameTimer& gt);//在这里更新常量缓冲区和material缓冲区
	void UpdateMainPassCB(const GameTimer& gt);//在这里更新公共变量缓冲区
	//更新阴影信息
	void UpdateShadowTransform(const GameTimer& gt);
	void UpdateShadowPassCB(const GameTimer& gt);

	void LoadTextures();//贴图资源加载
	void BuildRootSignature();//设置根签名
	void BuildDescriptorHeaps();//设置资源的descriptor
	void BuildShadersAndInputLayout();//设置hlsl的顶点格式
	void BuildShapeGeometry();//构建基本几何体
	void BuildSkullGeometry();//加载头骨信息
	void BuildPSOs();//创建管道状态
	void BuildFrameResources();//设置页面资源
	void BuildMaterials();//设置材质
	void BuildRenderItems();//建立绘制对象信息

	//绘制object
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	//绘制灯光摄像机的深度信息到 shadowmap
	void DrawSceneToShadowMap();

	//贴图的采样方式 我们使用常量贴图采样方式 这样我们可以直接使用根签名将常用的采样方式绑定到hlsl
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	//整个demo使用的srv资源的descriptor
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;//所有的顶点缓冲
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;//所有的材质
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;//所有的贴图
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;//所有的shader
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;//所有的PipelineState


	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// 需要渲染的所有物体
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// 区分不同管道状态 PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	//用于标识我们的sky 和  shadow资源的偏移量
	UINT mSkyTexHeapIndex = 0;
	UINT mShadowMapHeapIndex = 0;


	PassConstants mMainPassCB;  // 普通世界对象的公共信息
	PassConstants mShadowPassCB;// 灯光世界的公共信息

	//场景摄像机
	Camera  mCamera;

	//shadowmap  for our  shadowtest
	std::unique_ptr<ShadowMap> mShadowMap;
	//用于包裹场景的sphere
	DirectX::BoundingSphere mSceneBounds;

	//创建灯光为视点的渲染所需要的信息
	float mLightNearZ = 0.0f;
	float mLightFarZ = 0.0f;
	XMFLOAT3 mLightPosW;
	XMFLOAT4X4 mLightView = MathHelper::Identity4x4();
	XMFLOAT4X4 mLightProj = MathHelper::Identity4x4();
	XMFLOAT4X4 mShadowTransform = MathHelper::Identity4x4();

	//光的旋转角度
	float mLightRotationAngle = 0.0f;
	//光的方向
	XMFLOAT3 mBaseLightDirections[3] = {
		XMFLOAT3(0.57735f, -0.57735f, 0.57735f),
		XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
		XMFLOAT3(0.0f, -0.707f, -0.707f)
	};
	XMFLOAT3 mRotatedLightDirections[3];

	POINT mLastMousePos;

};