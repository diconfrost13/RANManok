default                                                                                                                                      b_pet_mummy.X    s_pet_mummy.x 
   pet_mummy          R            ?  ?  ?  ?  ?  ?   D   B   ?x     ?  ?  @?   ?   pet_mummy.dds             ?                  ?                  ?                  ?       	   STRIKE01   @   7  ¶¿e?Ê¼Å=¾EA¿S'¿¢Æ½         ÐÌÌ=fff?	   STRIKE02   @     pov?¨®?â{Ö½1Ta½B?O½°H¿             ?   ?        Ô-  pet_mummy_rb.dds                                                                                                                                                                                                                                                                           ? ? ?  ? ? ? ?  ?ÊÈH?ÊÈH?ÊÈH?    òðp>òðp>òðp>    >>>      ÄB   @   C                                                                                                                                                                                                                                                                                  ?  ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       SITION; 
	float2  m_vTex			: TEXCOORD0; 
	float3  m_vNor			: TEXCOORD1; 
	float3  m_vBinormal		: TEXCOORD2; 
	float	m_fFog			: FOG; 
};
VS_OUTPUT VShadeVS(VS_INPUT In, uniform int nNumBones)
{
	VS_OUTPUT   Out;
	float3      vPos = 0.0f;
	float3      vNormal = 0.0f;    
	float3      vBinormal = 0.0f;    
	float       LastWeight = 0.0f;
	int4 IndexVector = D3DCOLORtoUBYTE4(In.m_vBlendIndices);
	float  ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>    pet_mummy_rb.dds                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   C_vBinormal = mul(vBinormal, g_matView); 
	Out.m_vTex  = In.m_vTex.xy;
	Out.m_fFog = saturate((g_vFOG.x - Out.m_vPos.z) / g_vFOG.y); 
	return Out;
}
float3    tTangent ( float3 vTangent, float3 vNormal, float fShift )  
{  
	float3 vShiftedTangent = vTangent + (fShift * vNormal);  
	return normalize( vShiftedTangent );  
}  
float StrandSpecular2( float3 vTangent, float3 vHalf, float fExponent )  
{  
	float fDotTangentHalf = dot( vTangent, vHalf );  
	float fSinTangentHalf = sqrt(1.f - (fDotTangentHalf*fDotTangentHalf));  
	float fDirAtten = smoothstep( -1.f, ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   C { compile vs_2_0 VShadeVS(1), 
compile vs_2_0 VShadeVS(2),
compile vs_2_0 VShadeVS(3),
compile vs_2_0 VShadeVS(4) }; 
technique runtime_Skin
{
	pass midd   	{
		VertexShader = (vsArray[g_nCurNumBones]);
		PixelShader = compile ps_2_0 PS_2();
	}
	pass high
	{
		VertexShader = (vsArray[g_nCurNumBones]);
		PixelShader = compile ps_2_0 PS_2();
	}
}
technique runtime_Object
{
	pass middle
	{
		VertexShader = compile vs_2_0 VS_OBJECT();
		PixelShader = compile ps_2_0 PS_2();
	}
	pass high
	{
		VertexShader = compile vs_2_0 VS_OBJECT();
		PixelShader =  ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   C/ Specular, CubeMap Factor ¾ò±â. 
	float3 vSpecularCubemap = tex2D( BackBufferSampler_3rd, vProjTex ).xyz; 
	vSpecularCubemap.x *= 2.f; 
	vSpecularCubemap.y    56.f; 
 
	float3 vAddColor = 0.f; 
	if ( nPL1==2 ) 
	{ 
		float fLightAmount = In.m_vACubeSpot1Spot2.x; 
		float vLightAmountNEW = CascadeShadowSpot( In.m_vShadowProjectionPos, ShadowTexSampler_PL1, 1024.f, g_vPos_Range_CL_TOOL[0].w );  
		{ 
			int i=0; 
			float3 afPosDir = g_vPos_Range_CL_TOOL[i].xyz - In.m_vPosViewSpace; 
			float fLength = length( afPosDir ); 
			// ³»Àû ±¸ÇÔ. 
			afPosDir = norm ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   Cxyz * fLightAmount; 
	} 
	if ( nPL2==2 ) 
	{ 
		float fLightAmount = In.m_vACubeSpot1Spot2.y; 
		float vLightAmountNEW = CascadeShadowSpot( In.m_vShadowPro   _3rd, ShadowTexSampler_PL2, 1024.f, g_vPos_Range_CL_TOOL[1].w );  
		{ 
			int i=1; 
			float3 afPosDir = g_vPos_Range_CL_TOOL[i].xyz - In.m_vPosViewSpace; 
			float fLength = length( afPosDir ); 
			// ³»Àû ±¸ÇÔ. 
			afPosDir = normalize( afPosDir ); 
			float fDotPL = saturate( dot( afPosDir, vNor ) ); 
			// ¼±Çü °¨¼è, 2Â÷ °¨¼è, Áö¼ö °¨¼è Àû¿ë 
			float fAttenuationSub = g_vAtt_CosHalfThetaSubPhi_CL_T ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   C--------------------------- 
//					technique 
 
technique runtime_3 
{ 
	pass Spot_LBuffer_PL1_ON 
	{ 
		VertexShader = compile vs_3_0 VS_L_3( 2, 0 );    ixelShader = compile ps_3_0 PS_L_3( 2, 0, false ); 
	} 
	pass Spot_LBuffer_PL2_ON 
	{ 
		VertexShader = compile vs_3_0 VS_L_3( 0, 2 ); 
		PixelShader = compile ps_3_0 PS_L_3( 0, 2, false ); 
	} 
	pass Spot_LBuffer_PL1_PL2_ON 
	{ 
		VertexShader = compile vs_3_0 VS_L_3( 2, 2 ); 
		PixelShader = compile ps_3_0 PS_L_3( 2, 2, false ); 
	} 
	pass Spot_LBuffer_PL1_ON_AlphaTex 
	{ 
		VertexShader = compile ? ? ?  ? ? ? ?  ?ÉÈH?ÉÈH?ÉÈH?    ñðp>ñðp>ñðp>    >>>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ?  ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ÄB   @   Cader = compile vs_3_0 VS_3( 1, 0 ); 
		PixelShader = compile ps_3_0 PS_3( true, 1, 0 ); 
	} 
	pass albedo_PL2_ON_AlphTex 
	{ 
		VertexShader = compile vs_3          