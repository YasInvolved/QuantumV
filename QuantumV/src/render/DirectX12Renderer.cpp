#include "DirectX12Renderer.h"

#ifdef QV_RENDERER_DX12

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <array>
#include <future>
#include "d3dx12.h"
#include "QuantumV/core/Log.h"

using namespace DirectX;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
};

struct ConstantBuffer {
	XMMATRIX modelMatrix;
	XMMATRIX viewProjectionMatrix;
};

namespace QuantumV {
	void DirectX12Renderer::Init(void* window_handle, uint32_t width, uint32_t height) {
		HRESULT result = 0;
		m_width = width;
		m_height = height;
		m_hwnd = reinterpret_cast<HWND>(window_handle);

		// initialize
		#ifdef QV_DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
				debugController->EnableDebugLayer();
			}
			else {
				QV_CORE_CRITICAL("Failed to initialize debug controller");
			}
		}
		#endif

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)))) {
			QV_CORE_CRITICAL("Failed to create DXGI Factory");
		}

		{
			ComPtr<IDXGIAdapter> adapter;
			result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			if (FAILED(result)) {
				QV_CORE_WARN("Failed to find WARP compatible adapter. Falling back to regular one");
				result = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
				if (FAILED(result)) {
					QV_CORE_CRITICAL("Failed to find graphics adapter");
					return;
				}
			}

			result = adapter.As(&m_adapter);
			if (FAILED(result)) {
				QV_CORE_CRITICAL("Failed to create IDXGIAdapter4");
			}
		}

		if (FAILED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)))) {
			QV_CORE_CRITICAL("Failed to create DX12 Device");
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Width = m_width;
		swapchainDesc.Height = m_height;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = FALSE;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain1> swapchainTemp;
		m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &swapchainDesc, nullptr, nullptr, &swapchainTemp);
		m_factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
		swapchainTemp.As(&m_swapchain);

		m_currentFrameIndex = m_swapchain->GetCurrentBackBufferIndex();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		m_renderTargets.resize(m_frameCount);
		for (uint32_t i = 0; i < 2; i++) {
			m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
		m_commandList->Close();

		m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		// create root signature
		std::array<CD3DX12_ROOT_PARAMETER, 1> rootParams;
		rootParams[0].InitAsConstantBufferView(0);

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.NumParameters = rootParams.size();
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pParameters = rootParams.data();
		rootSignatureDesc.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

		ComPtr<ID3DBlob> compilation_error;
		auto vertex_future = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/dx12/VertexShader.cso", &m_vertexShader);
		auto pixel_future = std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/dx12/PixelShader.cso", &m_pixelShader);

		constexpr D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		
		vertex_future.wait();
		pixel_future.wait();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = { m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize() };
		psoDesc.PS = { m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize() };
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.NumRenderTargets = 1;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;

		m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

		// TODO: Delete this triangle boilerplate
		constexpr std::array<Vertex, 8> triangleVertices = {
			Vertex { { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Red
			Vertex { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Green
			Vertex { { 1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Blue
			Vertex { { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Yellow

			// Back face
			Vertex { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } }, // Magenta
			Vertex { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // Cyan
			Vertex { { 1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // White
			Vertex { { 1.0f, -1.0f,  1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f } }  // Gray
		};

		constexpr std::array<uint16_t, 36> triangleIndices = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};
	
		// create allocator
		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
			allocatorDesc.pAdapter = m_adapter.Get();
			allocatorDesc.pDevice = m_device.Get();

			D3D12MA::CreateAllocator(&allocatorDesc, &m_allocator);
		}

		{
			const uint32_t vertexBufferSize = sizeof(Vertex) * triangleVertices.size();

			D3D12_RESOURCE_DESC vertexBufferDesc = {};
			vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			vertexBufferDesc.Width = sizeof(Vertex) * triangleVertices.size();  // Size of the vertex buffer
			vertexBufferDesc.Height = 1;
			vertexBufferDesc.DepthOrArraySize = 1;
			vertexBufferDesc.MipLevels = 1;
			vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			vertexBufferDesc.SampleDesc.Count = 1;
			vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			vertexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC vertexBufferAllocDesc = {};
			vertexBufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			m_allocator->CreateResource(
				&vertexBufferAllocDesc,
				&vertexBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_vertexBufferAllocation,
				IID_PPV_ARGS(&m_vertexBuffer)
			);

			void* vertexData;
			CD3DX12_RANGE readRange(0, 0);
			m_vertexBuffer->Map(0, &readRange, &vertexData);
			memcpy(vertexData, triangleVertices.data(), vertexBufferSize);
			m_vertexBuffer->Unmap(0, nullptr);

			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
			m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		}

		{
			const uint32_t indexBufferSize = sizeof(uint16_t) * triangleIndices.size();

			D3D12_RESOURCE_DESC indexBufferDesc = {};
			indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			indexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			indexBufferDesc.Width = indexBufferSize;
			indexBufferDesc.Height = 1;
			indexBufferDesc.DepthOrArraySize = 1;
			indexBufferDesc.MipLevels = 1;
			indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			indexBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			indexBufferDesc.SampleDesc.Count = 1;

			D3D12MA::ALLOCATION_DESC indexBufferAllocDesc = {};
			indexBufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			m_allocator->CreateResource(
				&indexBufferAllocDesc,
				&indexBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_indexBufferAllocation,
				IID_PPV_ARGS(&m_indexBuffer)
			);

			void* indexData;
			CD3DX12_RANGE range(0, 0);
			m_indexBuffer->Map(0, &range, &indexData);
			memcpy(indexData, triangleIndices.data(), indexBufferSize);
			m_indexBuffer->Unmap(0, nullptr);

			m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.SizeInBytes = indexBufferSize;
			m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		}
		
		{
			m_cbSize = (sizeof(ConstantBuffer) + 255) & ~255;

			D3D12_RESOURCE_DESC cbDesc = {};
			cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			cbDesc.Format = DXGI_FORMAT_UNKNOWN;
			cbDesc.Width = m_cbSize;
			cbDesc.Height = 1;
			cbDesc.DepthOrArraySize = 1;
			cbDesc.MipLevels = 1;
			cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			cbDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			cbDesc.SampleDesc.Count = 1;

			D3D12MA::ALLOCATION_DESC cbAllocDesc = {};
			cbAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			m_allocator->CreateResource(
				&cbAllocDesc,
				&cbDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_constantBufferAllocation,
				IID_PPV_ARGS(&m_constantBuffer)
			);

			ConstantBuffer cbFilledData = {};

			XMVECTOR eyePosition = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
			XMVECTOR focusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
			
			float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

			XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

			XMMATRIX modelMatrix = XMMatrixIdentity();

			cbFilledData.viewProjectionMatrix = XMMatrixTranspose(viewProjectionMatrix);
			cbFilledData.modelMatrix = XMMatrixTranspose(modelMatrix);

			void* cbData;
			CD3DX12_RANGE range(0, 0);
			m_constantBuffer->Map(0, &range, &cbData);
			memcpy(cbData, &cbFilledData, sizeof(cbFilledData));
			m_constantBuffer->Unmap(0, nullptr);

			D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
			descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorHeapDesc.NumDescriptors = 1;
			descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			m_device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_constantBufferDescriptorHeap));

			m_constantBufferView.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
			m_constantBufferView.SizeInBytes = m_cbSize;

			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_constantBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			m_device->CreateConstantBufferView(&m_constantBufferView, cbvHandle);
		}
	}

	void DirectX12Renderer::Clear(float r, float g, float b, float a) {
		m_commandAllocator->Reset();
		m_commandList->Reset(m_commandAllocator.Get(), nullptr);

		CD3DX12_RESOURCE_BARRIER transitionToRender = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		m_commandList->ResourceBarrier(
			1,
			&transitionToRender
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_currentFrameIndex, m_rtvDescriptorSize);

		float clearColor[] = { r, g, b, a };
		m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		CD3DX12_RESOURCE_BARRIER transitionToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		m_commandList->ResourceBarrier(
			1,
			&transitionToPresent
		);

		m_commandList->Close();
		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_swapchain->Present(1, 0);
		WaitForPreviousFrame();
	}

	void DirectX12Renderer::WaitForPreviousFrame() {
		const uint64_t currentFenceValue = m_fenceValue;
		m_commandQueue->Signal(m_fence.Get(), currentFenceValue);
		m_fenceValue++;

		if (m_fence->GetCompletedValue() < currentFenceValue) {
			m_fence->SetEventOnCompletion(currentFenceValue, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		m_currentFrameIndex = m_swapchain->GetCurrentBackBufferIndex();
	}

	//void DirectX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
	//void DirectX12Renderer::BindPipeline(Pipeline* pipeline) {}
	void DirectX12Renderer::Draw(int vertex_count, int start_index) {
		m_commandAllocator->Reset();
		m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

		CD3DX12_RESOURCE_BARRIER transitionToRender = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		m_commandList->ResourceBarrier(1, &transitionToRender);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_currentFrameIndex, m_rtvDescriptorSize);

		float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		// TODO: Create viewport as it should be created
		m_commandList->RSSetViewports(1, new CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height)));
		m_commandList->RSSetScissorRects(1, new CD3DX12_RECT(0, 0, m_width, m_height));

		m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_commandList->IASetIndexBuffer(&m_indexBufferView);
		m_commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer->GetGPUVirtualAddress());

		m_commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

		CD3DX12_RESOURCE_BARRIER transitionToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		m_commandList->ResourceBarrier(1, &transitionToPresent);

		m_commandList->Close();
		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_swapchain->Present(1, 0);

		WaitForPreviousFrame();
	}

	//Shader* DirectX12Renderer::CreateShader(const std::string& shader_path) { return {}; }
	//Texture* DirectX12Renderer::CreateTexture(const std::string& texture_path) { return {}; }
	//VertexBuffer* DirectX12Renderer::CreateVertexBuffer(const void* data, size_t size) { return {}; }

	void DirectX12Renderer::Resize(uint32_t new_width, uint32_t new_height) {}
}

#endif