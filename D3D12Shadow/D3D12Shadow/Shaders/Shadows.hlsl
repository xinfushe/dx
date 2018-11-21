
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	MaterialData matData = gMaterialData[gMaterialIndex];
	
    //转换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // 转换到齐次裁剪空间（但是这里不进行裁剪操作）
    vout.PosH = mul(posW, gViewProj);
	
	// 材质动画
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;
	
    return vout;
}

void PS(VertexOut pin) 
{
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
	
	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
    
    //丢弃透明度小于0.1的物体，并且应该尽早进行clip测试，这样就可以尽早退出shader
    clip(diffuseAlbedo.a - 0.1f);
#endif
}


