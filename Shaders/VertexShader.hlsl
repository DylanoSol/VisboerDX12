struct ModelViewProjection
{
    matrix MVP;
};

struct JustModelMatrix
{
    matrix MODEL; 
};

struct LightPos
{
    float3 POS; 
};

struct ViewPos
{
    float3 POS; 
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);
ConstantBuffer<JustModelMatrix> ModelCB : register(b1);
ConstantBuffer<LightPos> LightPosCB : register(b2);
ConstantBuffer<ViewPos> ViewPosCB : register(b3); 

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
    float4 LightDir : LIGHTDIRECTION; 
    float4 ViewDir : VIEWDIRECTION; 
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    float4 position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0)); 
    float4 worldPosition = mul(ModelCB.MODEL, float4(IN.Position, 1.0)); 
    
    OUT.Position = position;
    
    OUT.Color = float4(IN.Color, 1.0);
    
    OUT.Normal = mul(ModelCB.MODEL, float4(IN.Normal, 1.0));
    
    OUT.LightDir = float4(normalize(LightPosCB.POS - worldPosition.xyz), 1.0);
    
    OUT.ViewDir = float4(normalize(ViewPosCB.POS - worldPosition.xyz), 1.0);

    return OUT;
}