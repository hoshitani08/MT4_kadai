#include "FBX.hlsli"

SkinOutput ComputeSkin(VSInput input)
{
	//ゼロクリア
	SkinOutput output = (SkinOutput)0;

	uint iBone;//計算するボーン番号
	float weight;//ボーンウェイト(重み)
	matrix m;//スキニング行列

	//ボーン0
	iBone = input.boneIndices.x;
	weight = input.boneWeights.x;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);
	output.tangent += weight * mul((float3x3)m, input.tangent);
	output.biNormal += weight * mul((float3x3)m, input.biNormal);

	//ボーン1
	iBone = input.boneIndices.y;
	weight = input.boneWeights.y;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);
	output.tangent += weight * mul((float3x3)m, input.tangent);
	output.biNormal += weight * mul((float3x3)m, input.biNormal);

	//ボーン2
	iBone = input.boneIndices.z;
	weight = input.boneWeights.z;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);
	output.tangent += weight * mul((float3x3)m, input.tangent);
	output.biNormal += weight * mul((float3x3)m, input.biNormal);

	//ボーン3
	iBone = input.boneIndices.w;
	weight = input.boneWeights.w;
	m = matSkinning[iBone];
	output.pos += weight * mul(m, input.pos);
	output.normal += weight * mul((float3x3)m, input.normal);
	output.tangent += weight * mul((float3x3)m, input.tangent);
	output.biNormal += weight * mul((float3x3)m, input.biNormal);

	return output;
}

VSOutput main(VSInput input)
{
	//スキニング計算
	SkinOutput skinned = ComputeSkin(input);
	//法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));
	//ピクセルシェーダーに渡す値
	VSOutput output;
	//行列による座標変換
	output.svpos = mul(mul(viewproj, world), skinned.pos);
	//ワールド座標を次のステージに渡す
	output.worldpos = mul(world, skinned.pos).xyz;
	//ワールド法線を次のステージに渡す
	output.normal = wnormal.xyz;

	float4 wtangent = normalize(mul(world, float4(skinned.tangent, 0)));
	float4 wbiNormal = normalize(mul(world, float4(skinned.biNormal, 0)));

	output.tangent = wtangent.xyz;
	output.biNormal = wbiNormal.xyz;
	//入力値をそのまま次のステージに渡す
	output.uv = input.uv;

	return output;
}