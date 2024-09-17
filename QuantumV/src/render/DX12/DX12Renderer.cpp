#include "DX12Renderer.h"

#ifdef QV_RENDERER_DX12

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <future>
#include <array>
#include "d3dx12.h"
#include "QuantumV/core/Log.h"

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct ConstBuffer {
	DirectX::XMMATRIX worldViewMatrix;
};

// TODO: Delete this triangle boilerplate
// Vertex Buffer
constexpr static std::array<Vertex, 8> vertices = {
	Vertex{.position = {-0.5f, -0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
	Vertex{.position = {0.5f, -0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
	Vertex{.position = {0.5f, 0.5f, -0.5f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
	Vertex{.position = {-0.5f, 0.5f, -0.5f}, .color = {1.0f, 1.0f, 0.0f, 1.0f}},

	Vertex{.position = {-0.5f, -0.5f, 0.5f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
	Vertex{.position = {0.5f, -0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
	Vertex{.position = {0.5f, 0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
	Vertex{.position = {-0.5f, 0.5f, 0.5f}, .color = {1.0f, 1.0f, 0.0f, 1.0f}},
};

constexpr static std::array<uint16_t, 12> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

ConstBuffer cbSrcData = {};

namespace QuantumV {
	void DX12Renderer::Init(void* window_handle, uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		hwnd = reinterpret_cast<HWND>(window_handle);
		
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

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&this->factory)))) {
			QV_CORE_CRITICAL("Failed to create DXGI Factory");
		}

		if (FAILED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&this->device)))) {
			QV_CORE_CRITICAL("Failed to create DX12 Device");
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		this->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&this->commandQueue));

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Width = this->width;
		swapchainDesc.Height = this->height;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = FALSE;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain1> swapchainTemp;
		factory->CreateSwapChainForHwnd(this->commandQueue.Get(), this->hwnd, &swapchainDesc, nullptr, nullptr, &swapchainTemp);
		factory->MakeWindowAssociation(this->hwnd, DXGI_MWA_NO_ALT_ENTER);
		swapchainTemp.As(&this->swapchain);

		this->currentFrameIndex = this->swapchain->GetCurrentBackBufferIndex();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (uint32_t i = 0; i < 2; i++) {
			swapchain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}

		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->commandAllocator));

		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&this->commandList));
		this->commandList->Close();

		device->CreateFence(this->fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence));
		this->fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		CD3DX12_DESCRIPTOR_RANGE cbvRange;
		cbvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		std::array<CD3DX12_ROOT_PARAMETER, 1> rootParameters;
		rootParameters[0].InitAsDescriptorTable(1, &cbvRange, D3D12_SHADER_VISIBILITY_VERTEX);

		// create root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.NumParameters = rootParameters.size();
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pParameters = rootParameters.data();
		rootSignatureDesc.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/VertexShader.cso", &vertexShader);
		std::async(std::launch::async, D3DReadFileToBlob, L"assets/shaders/PixelShader.cso", &pixelShader);

		constexpr D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = rootSignature.Get();
		psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
		psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.NumRenderTargets = 1;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;

		device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

		// ----------------------- VERTEX BUFFER -----------------------

		const uint32_t vertexBufferSize = sizeof(Vertex) * vertices.size();

		CD3DX12_HEAP_PROPERTIES vertexHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC vertexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		device->CreateCommittedResource(
			&vertexHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&vertexResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer)
		);

		void* vertexData;
		CD3DX12_RANGE readVertexRange(0, 0);
		vertexBuffer->Map(0, &readVertexRange, &vertexData);
		memcpy(vertexData, vertices.data(), vertexBufferSize);
		vertexBuffer->Unmap(0, nullptr);

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = vertexBufferSize;
		vertexBufferView.StrideInBytes = sizeof(Vertex);

		// ----------------------- INDEX BUFFER -----------------------

		const uint32_t indexBufferSize = sizeof(uint16_t) * indices.size();

		CD3DX12_HEAP_PROPERTIES indexHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC indexResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
		device->CreateCommittedResource(
			&indexHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&indexResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)
		);

		void* indexData;
		CD3DX12_RANGE readIndexRange(0, 0);
		indexBuffer->Map(0, &readIndexRange, &indexData);
		memcpy(indexData, indices.data(), indexBufferSize);
		indexBuffer->Unmap(0, nullptr);

		// ----------------------- CONSTANT BUFFER -----------------------

		// Descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC cbDescHeap = {};
		cbDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbDescHeap.NumDescriptors = 1;
		cbDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		device->CreateDescriptorHeap(&cbDescHeap, IID_PPV_ARGS(&cbvHeap));

		const uint32_t cbSize = sizeof(ConstBuffer); // one constant buffer
		const uint32_t cbSizeAligned = (cbSize + 255) & ~255;

		CD3DX12_HEAP_PROPERTIES cbHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC cbResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSizeAligned);
		device->CreateCommittedResource(
			&cbHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer)
		);

		cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = cbSizeAligned;
	}

	void DX12Renderer::Clear(float r, float g, float b, float a) {
		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), nullptr);

		CD3DX12_RESOURCE_BARRIER transitionToRender = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		commandList->ResourceBarrier(
			1,
			&transitionToRender
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentFrameIndex, rtvDescriptorSize);

		float clearColor[] = { r, g, b, a };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		CD3DX12_RESOURCE_BARRIER transitionToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		commandList->ResourceBarrier(
			1,
			&transitionToPresent
		);

		commandList->Close();
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		swapchain->Present(1, 0);
		WaitForPreviousFrame();
	}

	void DX12Renderer::WaitForPreviousFrame() {
		const uint64_t currentFenceValue = fenceValue;
		commandQueue->Signal(fence.Get(), currentFenceValue);
		fenceValue++;

		if (fence->GetCompletedValue() < currentFenceValue) {
			fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
			WaitForSingleObject(fenceEvent, INFINITE);
		}

		currentFrameIndex = swapchain->GetCurrentBackBufferIndex();
	}

	//void DirectX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
	//void DirectX12Renderer::BindPipeline(Pipeline* pipeline) {}
	void DX12Renderer::Draw(int vertex_count, int start_index) {
		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), pipelineState.Get());

		CD3DX12_RESOURCE_BARRIER transitionToRender = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		commandList->ResourceBarrier(1, &transitionToRender);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentFrameIndex, rtvDescriptorSize);

		float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// TODO: Create viewport as it should be created
		commandList->RSSetViewports(1, new CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)));
		commandList->RSSetScissorRects(1, new CD3DX12_RECT(0, 0, width, height));

		commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

		// bind constant buffer
		const std::array<ID3D12DescriptorHeap*, 1> heaps = { cbvHeap.Get() };
		commandList->SetDescriptorHeaps(heaps.size(), heaps.data());
		commandList->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUDescriptorHandleForHeapStart());

		commandList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);

		CD3DX12_RESOURCE_BARRIER transitionToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTargets[currentFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		commandList->ResourceBarrier(1, &transitionToPresent);

		commandList->Close();
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		swapchain->Present(1, 0);

		WaitForPreviousFrame();
	}

	//Shader* DirectX12Renderer::CreateShader(const std::string& shader_path) { return {}; }
	//Texture* DirectX12Renderer::CreateTexture(const std::string& texture_path) { return {}; }
	//VertexBuffer* DirectX12Renderer::CreateVertexBuffer(const void* data, size_t size) { return {}; }

	void DX12Renderer::Resize(uint32_t new_width, uint32_t new_height) {}
}

#endif