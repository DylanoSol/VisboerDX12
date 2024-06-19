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

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);
ConstantBuffer<JustModelMatrix> Model : register(b1);
ConstantBuffer<LightPos> LightPosCB : register(b2);

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
    float4 Position : SV_Position;
};

// Function to calculate the inverse transpose of a float4x4 matrix
float4x4 InverseTranspose(float4x4 mat)
{
    float4 c0 = mat[0];
    float4 c1 = mat[1];
    float4 c2 = mat[2];
    float4 c3 = mat[3];

    float4 r0 = float4(c0.x, c1.x, c2.x, 0.0);
    float4 r1 = float4(c0.y, c1.y, c2.y, 0.0);
    float4 r2 = float4(c0.z, c1.z, c2.z, 0.0);
    float4 r3 = float4(0.0, 0.0, 0.0, 1.0);

    float4x4 adjoint;
    adjoint[0] = (cross(r1.xyz, r2.xyz), 1.0);
    adjoint[1] = (-cross(r0.xyz, r2.xyz), 1.0);
    adjoint[2] = (cross(r0.xyz, r1.xyz), 1.0);
    adjoint[3] = float4(0.0, 0.0, 0.0, 1.0);

    float det = dot(c0, adjoint[0]);

    float4x4 result;
    result[0] = adjoint[0] / det;
    result[1] = adjoint[1] / det;
    result[2] = adjoint[2] / det;
    result[3] = r3;

    return transpose(result);
}

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    float4 position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0)); 
    float4 worldPosition = mul(Model.MODEL, float4(IN.Position, 1.0)); 
    
    // Calculate normal matrix to transform the normal to world space
    float3x3 normalMatrix; 
    normalMatrix[0] = Model.MODEL[0].xyz; 
    normalMatrix[1] = Model.MODEL[1].xyz;
    normalMatrix[2] = Model.MODEL[2].xyz;
    
    normalMatrix = InverseTranspose(Model.MODEL); 
    
    OUT.Position = position;
    
    OUT.Color = float4(IN.Color, 1.0);
    
    OUT.Normal = float4(mul(normalMatrix, IN.Normal), 1.0);
    
    OUT.LightDir = float4(normalize(LightPosCB.POS - worldPosition.xyz), 1.0);

    return OUT;
}