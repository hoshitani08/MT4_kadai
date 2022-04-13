#include "FBX.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);
	float4 shadecolor = { 0,0,0,0 };

	// 平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++)
	{
		if (!dirLights[i].active)
		{
			continue;
		}
		//Lambert反射
		float3 light = normalize(dirLights[i].lightv); //ライトの向き
		float diffuse = saturate(dot(light, input.normal));
		float brightness = diffuse + 0.3f;

		// 全て加算する
		shadecolor += float4(brightness, brightness, brightness, 1.0f);
	}
	//陰影とテクスチャの色を合成
	return shadecolor * texcolor;
}