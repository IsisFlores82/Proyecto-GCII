Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState colorSampler : register(s0);

cbuffer cbChangerEveryFrame : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix projMatrix;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;



	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	float4 text = colorMap.Sample(colorSampler, pix.tex0);
	float intensity = 0.4;
	if (text.a < 0.1)
	//if(text.r >= intensity && text.g >= intensity && text.b >= intensity)
	{
		clip(-1);
		return 0;
	}
	else {
		//aqui se calcula la iluminacion difusa
		float4 textnorm = normalMap.Sample(colorSampler, pix.tex0);
		float4 LuzAmbiental = float4(1.0, 1.0, 1.0, 1);
		float FA = 0.3;
		///////////////////////////////////////////////
		// aqui se desarrolla el elemento ambiental
		///////////////////////////////////////////////
		float4 AportAmb = LuzAmbiental * FA;
		//////////////////////////////////////////
		//aqui se desarrolla el elemento difuso //
		//////////////////////////////////////////
		float3 DirLuz = float3(30, 10, 30);
		float4 LuzDifusa = float4(1, 1, 1, 1);
		float FAD = 1;
	
		float3 bump = normalize(2.0 * textnorm - 1.0);
	
		float FALL = dot(normalize(bump), normalize(DirLuz));
		float4 AportLuzDif = saturate(LuzDifusa * FALL * FAD);
		text = text * (AportAmb + AportLuzDif );
		text.a = 1;
		return text; //* (aportAmb + aportDif);
	}
}