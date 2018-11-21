//-------------------------------------------------------------
//-------------------  shadow base shader ------------------------
//时间：   2018-11-20   by:淡一抹夕霞
//说明：shadow 简单shader
//-------------------------------------------------------------

#include "Common.hlsl"




struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentU : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float3 PosW : POSITION1;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
};



//顶点着色器
VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
    MaterialData matData = gMaterialData[gMaterialIndex];

    //将顶点转换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    //保存世界坐标
    vout.PosW = posW.xyz;

    //将normal 和tangent 转换到世界空间
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);

    //将顶点转换到齐次裁剪空间
    vout.PosH = mul(posW, gViewProj);

    //材质变换
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

     //将顶点转换到shadowmap空间 以便与shadowmap进行深度比较
    vout.ShadowPosH = mul(posW, gShadowTransform);


    return vout;
}

//像素着色器
float4 PS(VertexOut pin):SV_Target
{
    //获取材质信息
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;
	
    // 获取贴图透明度 
    diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
   //裁剪透明度过低的像素
    clip(diffuseAlbedo.a - 0.1f);
#endif

	// 经过线性插值，要重新单位化法向量
    pin.NormalW = normalize(pin.NormalW);
	
    float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    //计算normalmap的向量
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);


    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Light terms.
    float4 ambient = gAmbientLight * diffuseAlbedo;

    //计算shadowFactor
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor = CalcShadowFactor(pin.ShadowPosH);

    const float shininess = (1.0f - roughness) * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        bumpedNormalW, toEyeW, shadowFactor,3,0,0);

    float4 litColor = ambient + directLight;

	// 添加cubeMap反射
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
	
    litColor.a = diffuseAlbedo.a;

    return litColor;
}