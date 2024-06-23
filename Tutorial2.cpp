#include "Tutorial2.h"

#include <Application.h>
#include <CommandQueue.h>
#include <Helpers.h>
#include <Window.h>
#include "Texture.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3dx12.h>
#include <d3dcompiler.h>

#include <algorithm> // For std::min and std::max.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

using namespace DirectX;


// Clamp a value between a min and max range.
template<typename T>
constexpr const T& clamp(const T& val, const T& min, const T& max)
{
    return val < min ? min : val > max ? max : val;
}

// Vertex data for a colored cube.
struct VertexPosColorNormal
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal; 
    XMFLOAT3 Color;
};

struct VertexPosColor
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};


static VertexPosColorNormal g_Vertices[24] = {
    // Front Face
    {{-1.f, -1.f,  1.f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  1.0f,  1.0f} },
    {{ 1.f, -1.f,  1.f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  1.0f,  1.0f} },
    {{ 1.f,  1.f,  1.f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  1.0f,  1.0f} },
    {{-1.f,  1.f,  1.f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  1.0f,  1.0f} },
    // Back Face
    {{-1.f, -1.f, -1.f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, 1.0f}},
    {{-1.f,  1.f, -1.f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, 1.0f}},
    {{ 1.f,  1.f, -1.f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, 1.0f}},
    {{ 1.f, -1.f, -1.f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, 1.0f}},
    // Top Face
    {{-1.f,  1.f, -1.f}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  1.0f}},
    {{-1.f,  1.f,  1.f}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  1.0f}}, 
    {{ 1.f,  1.f,  1.f}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  1.0f}}, 
    {{ 1.f,  1.f, -1.f}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  1.0f}},
    // Bottom Face
    {{-1.f, -1.f, -1.f}, {-0.0f, -1.0f,  0.0f}, {0.0f, 1.0f,  0.0f}}, 
    {{ 1.f, -1.f, -1.f}, {-0.0f, -1.0f,  0.0f}, {0.0f, 1.0f,  0.0f}},
    {{ 1.f, -1.f,  1.f}, {-0.0f, -1.0f,  0.0f}, {0.0f, 1.0f,  0.0f}},
    {{-1.f, -1.f,  1.f}, {-0.0f, -1.0f,  0.0f}, {0.0f, 1.0f,  0.0f}},
    // Right Face
    {{ 1.f, -1.f, -1.f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.f,  1.f, -1.f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.f,  1.f,  1.f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.f, -1.f,  1.f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}},
    // Left Face
    {{-1.f, -1.f, -1.f}, {-1.0f,  0.0f,  0.0f}, {1.0f,  1.0f,  0.0f}},
    {{-1.f, -1.f,  1.f}, {-1.0f,  0.0f,  0.0f}, {1.0f,  1.0f,  0.0f}},
    {{-1.f,  1.f,  1.f}, {-1.0f,  0.0f,  0.0f}, {1.0f,  1.0f,  0.0f}},
    {{-1.f,  1.f, -1.f}, {-1.0f,  0.0f,  0.0f}, {1.0f,  1.0f,  0.0f}}
};


static VertexPosColor g_VerticesWhite[8] = {
    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 0
    { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 1
    { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 2
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 3
    { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 4
    { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 5
    { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.f, 1.f, 1.f) }, // 6
    { XMFLOAT3(1.0f, -1.0f,  1.0f),  XMFLOAT3(1.f, 1.f, 1.f) }  // 7
};

static WORD g_Indicies[36] =
{
    // Front Face
    0, 1, 2, 0, 2, 3,
    // Back Face
    4, 5, 6, 4, 6, 7,
    // Top Face
    8, 9, 10, 8, 10, 11,
    // Bottom Face
    12, 13, 14, 12, 14, 15,
    // Right Face
    16, 17, 18, 16, 18, 19,
    // Left Face
    20, 21, 22, 20, 22, 23
};

static WORD g_IndiciesWhite[36] =
{
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};

Tutorial2::Tutorial2(const std::wstring& name, int width, int height, bool vSync)
    : super(name, width, height, vSync)
    , m_ScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    , m_Viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
    , m_FoV(45.0)
    , m_ContentLoaded(false)
{
}

void Tutorial2::UpdateBufferResource(
    ComPtr<ID3D12GraphicsCommandList2> commandList,
    ID3D12Resource** pDestinationResource,
    ID3D12Resource** pIntermediateResource,
    size_t numElements, size_t elementSize, const void* bufferData,
    D3D12_RESOURCE_FLAGS flags)
{
    auto device = Application::Get().GetDevice();

    size_t bufferSize = numElements * elementSize;

    // Assign this to a temporary to prevent l-value issues
    auto resourceDescriptorHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferResource = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);

    // Create a committed resource for the GPU resource in a default heap.
    ThrowIfFailed(device->CreateCommittedResource(
        &resourceDescriptorHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferResource,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(pDestinationResource)));

    // Create an committed resource for the upload.
    if (bufferData)
    {
        // Assign this to a temporary to prevent l-value issues
        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
        auto bufferResource = CD3DX12_RESOURCE_DESC::Buffer(bufferSize); 

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &bufferResource,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(pIntermediateResource)));

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = bufferData;
        subresourceData.RowPitch = bufferSize;
        subresourceData.SlicePitch = subresourceData.RowPitch;

        UpdateSubresources(commandList.Get(),
            *pDestinationResource, *pIntermediateResource,
            0, 0, 1, &subresourceData);
    }
}


bool Tutorial2::LoadContent()
{
    auto device = Application::Get().GetDevice();
    auto commandQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = commandQueue->GetCommandList();

    // Upload vertex buffer data.
    ComPtr<ID3D12Resource> intermediateVertexBuffer;
    UpdateBufferResource(commandList,
        &m_VertexBuffer, &intermediateVertexBuffer,
        _countof(g_Vertices), sizeof(VertexPosColorNormal), g_Vertices);

    // Create the vertex buffer view.
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.SizeInBytes = sizeof(g_Vertices);
    m_VertexBufferView.StrideInBytes = sizeof(VertexPosColorNormal);


    // Upload index buffer data.
    ComPtr<ID3D12Resource> intermediateIndexBuffer;
    UpdateBufferResource(commandList,
        &m_IndexBuffer, &intermediateIndexBuffer,
        _countof(g_Indicies), sizeof(WORD), g_Indicies);

    // Create index buffer view.
    m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
    m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    m_IndexBufferView.SizeInBytes = sizeof(g_Indicies);

    // Load the texture for object 1
    Texture tex = Texture("D:/Gamedevving/Repositories/VisboerDX12/VisboerDX12/VisboerDX12/Textures/brolyweed.png");

    // Upload second vertex buffer data.
    ComPtr<ID3D12Resource> secondIntermediateVertexBuffer;
    UpdateBufferResource(commandList,
        &m_VertexBuffer2, &secondIntermediateVertexBuffer,
        _countof(g_VerticesWhite), sizeof(VertexPosColor), g_VerticesWhite);

    // Create the vertex buffer view.
    m_VertexBufferView2.BufferLocation = m_VertexBuffer2->GetGPUVirtualAddress();
    m_VertexBufferView2.SizeInBytes = sizeof(g_VerticesWhite);
    m_VertexBufferView2.StrideInBytes = sizeof(VertexPosColor);

    // Upload index buffer data for other object.
    ComPtr<ID3D12Resource> secondIntermediateIndexBuffer;
    UpdateBufferResource(commandList,
        &m_IndexBuffer2, &secondIntermediateIndexBuffer,
        _countof(g_IndiciesWhite), sizeof(WORD), g_IndiciesWhite);

    // Create index buffer view for other object.
    m_IndexBufferView2.BufferLocation = m_IndexBuffer2->GetGPUVirtualAddress();
    m_IndexBufferView2.Format = DXGI_FORMAT_R16_UINT;
    m_IndexBufferView2.SizeInBytes = sizeof(g_IndiciesWhite);


    // Create the descriptor heap for the depth-stencil view.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

    // Load the vertex shader.
    ComPtr<ID3DBlob> vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

    // Load the pixel shader.
    ComPtr<ID3DBlob> pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

    // Create the vertex input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // Create a root signature.
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Allow input layout and deny unnecessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    // A single 32-bit constant root parameter that is used by the vertex shader.
    CD3DX12_ROOT_PARAMETER1 rootParameters[4];
    rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    // Store the model matrix again as a quick hack 
    rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);

    // Set root parameter for light
    rootParameters[2].InitAsConstants(sizeof(XMFLOAT3) / 4, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL);

    // Set the view position
    rootParameters[3].InitAsConstants(sizeof(XMFLOAT3) / 4, 3, 0, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    // Serialize the root signature.
    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
        featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
    // Create the root signature.
    ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

    struct PipelineStateStream
    {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
        CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
        CD3DX12_PIPELINE_STATE_STREAM_VS VS;
        CD3DX12_PIPELINE_STATE_STREAM_PS PS;
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
        CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    } pipelineStateStream;

    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 1;
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineStateStream.pRootSignature = m_RootSignature.Get();
    pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
    pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
    pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
    pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineStateStream.RTVFormats = rtvFormats;

    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
        sizeof(PipelineStateStream), &pipelineStateStream
    };
    ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

    // Compile new shaders for second pipeline state object 
    {

        // Load the vertex shader.
        ComPtr<ID3DBlob> vertexShaderBlob2;
        ThrowIfFailed(D3DReadFileToBlob(L"LightVertexShader.cso", &vertexShaderBlob2));

        // Load the pixel shader.
        ComPtr<ID3DBlob> pixelShaderBlob2;
        ThrowIfFailed(D3DReadFileToBlob(L"LightPixelShader.cso", &pixelShaderBlob2));

        // Create the vertex input layout
        D3D12_INPUT_ELEMENT_DESC inputLayout2[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob2.Get());
        pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob2.Get());
        pipelineStateStream.InputLayout = { inputLayout2, _countof(inputLayout2) };

        D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc2 = {
         sizeof(PipelineStateStream), &pipelineStateStream
        };

        ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc2, IID_PPV_ARGS(&m_PipelineStateUnlit)));
    }

    auto fenceValue = commandQueue->ExecuteCommandList(commandList);
    commandQueue->WaitForFenceValue(fenceValue);

    m_ContentLoaded = true;

    // Resize/Create the depth buffer.
    ResizeDepthBuffer(GetClientWidth(), GetClientHeight());

    return true;
}

void Tutorial2::ResizeDepthBuffer(int width, int height)
{
    if (m_ContentLoaded)
    {
        // Flush any GPU commands that might be referencing the depth buffer.
        Application::Get().Flush();

        width = std::max(1, width);
        height = std::max(1, height);

        auto device = Application::Get().GetDevice();

        // Resize screen dependent resources.
        // Create a depth buffer.
        D3D12_CLEAR_VALUE optimizedClearValue = {};
        optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        optimizedClearValue.DepthStencil = { 1.0f, 0 };

        // Assign this to a temporary to prevent l-value issues
        auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resource = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
            1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL); 

        ThrowIfFailed(device->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &resource,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &optimizedClearValue,
            IID_PPV_ARGS(&m_DepthBuffer)
        ));

        // Update the depth-stencil view.
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
        dsv.Format = DXGI_FORMAT_D32_FLOAT;
        dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsv.Texture2D.MipSlice = 0;
        dsv.Flags = D3D12_DSV_FLAG_NONE;

        device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
            m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
    }
}

void Tutorial2::OnResize(ResizeEventArgs& e)
{
    if (e.Width != GetClientWidth() || e.Height != GetClientHeight())
    {
        super::OnResize(e);

        m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f,
            static_cast<float>(e.Width), static_cast<float>(e.Height));

        ResizeDepthBuffer(e.Width, e.Height);
    }
}

void Tutorial2::UnloadContent()
{
    m_ContentLoaded = false;
}

void Tutorial2::OnUpdate(UpdateEventArgs& e)
{
    static uint64_t frameCount = 0;
    static double totalTime = 0.0;

    super::OnUpdate(e);

    totalTime += e.ElapsedTime;
    frameCount++;

    if (totalTime > 1.0)
    {
        double fps = frameCount / totalTime;

        char buffer[512];
        sprintf_s(buffer, "FPS: %f\n", fps);
        OutputDebugStringA(buffer);

        frameCount = 0;
        totalTime = 0.0;
    }

    // Update the model matrix.
    float angle = static_cast<float>(e.TotalTime * 90.0);
    const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

    //m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
    m_ModelMatrix = XMMatrixIdentity() * XMMatrixScaling(3.f, 3.f, 3.f); 

    // Update the model matrix of the second object
    m_ModelMatrix2 = XMMatrixIdentity() * XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(cosf(x) * 10.f, 2.f, sinf(x) * 10.f);

    // Update the view matrix.
    x += 0.001f; 

    const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    m_ViewMatrix = XMMatrixLookAtLH(m_EyePosition, focusPoint, upDirection);

    // Update the projection matrix.
    float aspectRatio = GetClientWidth() / static_cast<float>(GetClientHeight());
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FoV), aspectRatio, 0.1f, 100.0f);
}

// Transition a resource
void Tutorial2::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
    Microsoft::WRL::ComPtr<ID3D12Resource> resource,
    D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(),
        beforeState, afterState);

    commandList->ResourceBarrier(1, &barrier);
}

// Clear a render target.
void Tutorial2::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
    D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
    commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void Tutorial2::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
    D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
    commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void Tutorial2::OnRender(RenderEventArgs& e)
{
    super::OnRender(e);

    auto commandQueue = Application::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    auto commandList = commandQueue->GetCommandList();

    UINT currentBackBufferIndex = m_pWindow->GetCurrentBackBufferIndex();
    auto backBuffer = m_pWindow->GetCurrentBackBuffer();
    auto rtv = m_pWindow->GetCurrentRenderTargetView();
    auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

    // Clear the render targets.
    {
        TransitionResource(commandList, backBuffer,
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

        ClearRTV(commandList, rtv, clearColor);
        ClearDepth(commandList, dsv);
    }

    commandList->SetPipelineState(m_PipelineState.Get());
    commandList->SetGraphicsRootSignature(m_RootSignature.Get());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    commandList->IASetIndexBuffer(&m_IndexBufferView);

    commandList->RSSetViewports(1, &m_Viewport);
    commandList->RSSetScissorRects(1, &m_ScissorRect);

    commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

    // Update the MVP matrix
    XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
    mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);

    // Set the MVP matrix in the shader
    commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

    // Set the model for the hack I mentioned earlier
    commandList->SetGraphicsRoot32BitConstants(1, sizeof(XMMATRIX) / 4, &m_ModelMatrix, 0);

    // Set the light position in the shader
    XMVECTOR lightPos = m_ModelMatrix2.r[3]; 
    XMFLOAT3 light; 
    XMStoreFloat3(&light, lightPos); 

    commandList->SetGraphicsRoot32BitConstants(2, sizeof(XMFLOAT3) / 4, &light, 0); 

    // Set the view position in the shader 
    commandList->SetGraphicsRoot32BitConstants(3, sizeof(XMFLOAT3) / 4, &m_EyePosition, 0);

    commandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);

    // Render a second object but at a different position 

    // Bind unlit pipeline state objects for lighting objects, only works on command queues of type DIRECT
    commandList->SetPipelineState(m_PipelineStateUnlit.Get()); 

    // Set Vertex and Index buffers
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView2);
    commandList->IASetIndexBuffer(&m_IndexBufferView2);

    // Update the MVP matrix
    mvpMatrix = XMMatrixMultiply(m_ModelMatrix2, m_ViewMatrix); 
    mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix); 

    // Set the MVP matrix in the shader
    commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

    commandList->DrawIndexedInstanced(_countof(g_IndiciesWhite), 1, 0, 0, 0);

    // Present
    {
        TransitionResource(commandList, backBuffer,
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

        currentBackBufferIndex = m_pWindow->Present();

        commandQueue->WaitForFenceValue(m_FenceValues[currentBackBufferIndex]);
    }
}

void Tutorial2::OnKeyPressed(KeyEventArgs& e)
{
    super::OnKeyPressed(e);

    switch (e.Key)
    {
    case KeyCode::Escape:
        Application::Get().Quit(0);
        break;
    case KeyCode::Enter:
        if (e.Alt)
        {
    case KeyCode::F11:
        m_pWindow->ToggleFullscreen();
        break;
        }
    case KeyCode::V:
        m_pWindow->ToggleVSync();
        break;
    }
}

void Tutorial2::OnMouseWheel(MouseWheelEventArgs& e)
{
    m_FoV -= e.WheelDelta;
    m_FoV = clamp(m_FoV, 12.0f, 90.0f);

    char buffer[256];
    sprintf_s(buffer, "FoV: %f\n", m_FoV);
    OutputDebugStringA(buffer);
}
