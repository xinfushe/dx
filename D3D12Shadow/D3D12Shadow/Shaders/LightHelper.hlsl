//----------------------------------------------//
//时间：2018-9-29   by：淡一抹夕霞
//说明：光照计算shader
//----------------------------------------------//


#define MaxLights 16

struct Light
{
    //三大光源：方向光，点光源，聚光灯
    float3 Strength;//光照基本强度
    float FalloffStart;//衰减起始点 spot和point独有
    float3 Direction; //direction 和 spot 独有
    float FalloffEnd; //衰减结束点
    float3 Postion; //point spot 独有
    float SpotPower; //spot独有
};

struct Material
{
    float4 DiffuseAlbedo;//BaseColor
    float3 FresenIRO;//菲涅尔0°参数
    float Shininess;//光滑度 = 1 - roughness 
};


//计算衰减因子
float CalcAttenuation(float d,float falloffStart,float falloffEnd)
{
     // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

//计算菲涅尔近似方程的结果
float3 SchlickFresnel(float3 R0,float3 normal,float3 lightVec)
{

    //方程为：R(0)=R0+(1-R0)*(cos0)^5 其中cos0为halfVec与入射光线的夹角
    float cosIncidentAngle = saturate(dot(normal,lightVec));

    float f0 = 1.0f - cosIncidentAngle;

    float3 reflectPrecent = R0 + (1.0f - R0) * (f0*f0*f0*f0*f0);

    return reflectPrecent;


}

//计算光照通量
float3 BlinnPhong(float3 lightStrength,float3 lightVec,float3 normal,float3 toEye,Material mat)
{
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    //材质影响公式：（m+8）/8 * (cos0)^m   cos0 = dot(normal,halfVec)
    float roughnessFactory = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    //菲涅尔因子
    float3 fresnelFactory = SchlickFresnel(mat.FresenIRO ,halfVec ,lightVec);

    float3 specAlbedo = roughnessFactory * fresnelFactory;

    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    //返回镜面光和漫反射作用和的光强
    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;

   

}

//平行光计算函数
//由于平行光的方向都相同，所以光照强度只与角度有关联
float3 ComputDirectionalLight(Light L,Material mat,float3 normal,float3 toEye)
{
    float3 lightVec = -L.Direction;

    float lambertCos = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * lambertCos;


    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}


//点光源计算函数
//点光源方向是任意方向,光源本身有位置，不同位置需要计算，同时有衰减影响光照强度
float3 ComputPointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Postion - pos;
    float d = length(lightVec);
    if (d>L.FalloffEnd)
        return 0.0f;

    lightVec = lightVec / d;
    float lambetCos = max(dot(lightVec, normal), 0.0f);
    float att = CalcAttenuation(d,L.FalloffStart,L.FalloffEnd);//衰减因子

    float3 lightStrength = L.Strength * lambetCos * att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//聚光灯计算函数
//聚光灯拥有衰减半径和聚光灯衰减因子
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Postion - pos;
    float d = length(lightVec);
    if (d > L.FalloffEnd)
        return 0.0f;
    //单位化lightVec，计算兰伯特，距离衰减，聚光灯衰减
    lightVec = lightVec / d;
    float lambetCos = max(dot(lightVec, normal), 0.0f);
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);

    float3 lightStrength = L.Strength * lambetCos * att * spot;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);

 

}

float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor,int numDL,int numPL,int numSL)
{
    float3 result = 0.0f;

    int i = 0;
    int k = 0;

    for (i = 0; i < numDL; ++i)
    {
        result += ComputDirectionalLight(gLights[k], mat, normal, toEye) * shadowFactor;
        ++k;
    }

    for (i = 0; i < numPL; ++i)
    {
        result += ComputPointLight(gLights[k], mat, pos, normal, toEye);
        ++k;
    }

    for (i = 0; i < numSL; ++i)
    {
        result += ComputeSpotLight(gLights[k], mat, pos, normal, toEye);
        ++k;
    }


    return float4(result, 0.0f);
}


