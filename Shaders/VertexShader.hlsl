struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Color : COLOR;
};

struct VertexShaderOutput
{
	float4 Color    : COLOR;
    float4 Normal : NORMAL; 
    float4 InPosition : INCOMINGPOSITION; 
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    float4 position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0)); 
    
    //OUT.WorldPos = mul(ModelCB.MODEL, float4(IN.Position, 1.0)); 
    OUT.InPosition = float4(IN.Position, 1.0); 
    
    OUT.Position = position;
    
    OUT.Color = float4(IN.Color, 1.0);
    
    OUT.Normal = float4(IN.Normal, 0.0);
    //OUT.Normal = mul(ModelCB.MODEL, float4(IN.Normal, 1.0));

    return OUT;
}