#include "ShaderStructs.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    
    float4 vertexObjectPos = input.myPosition.xyzw;
    float4 vertexWorldPos = mul(input.myTransform, vertexObjectPos);
    float4 vertexViewPos = mul(toCamera, vertexWorldPos);
    float4 vertexProjectionPos = mul(toProjection, vertexViewPos);
    
    returnValue.myPosition = vertexProjectionPos;
    
    float3x3 toWorldRotation = (float3x3) input.myTransform;
    float3 vertexWorldNormal = mul(toWorldRotation, input.myNormal.xyz);
    float3 vertexWorldTangent = mul(toWorldRotation, input.myTangent.xyz);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.myBiNormal.xyz);
    
    returnValue.myWorldPosition = vertexWorldPos;
    returnValue.myNormal = float4(vertexWorldNormal, 0);
    returnValue.myTangent = float4(vertexWorldTangent, 0);
    returnValue.myBiNormal = float4(vertexWorldBinormal, 0);
    returnValue.myUV = input.myUV;
    
    return returnValue;
}