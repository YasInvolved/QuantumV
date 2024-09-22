#include "DX12Object.h"

namespace QuantumV {
	DX12Object::DX12Object(const std::string& path)
		: Object(path) 
	{

	}

	void DX12Object::AllocateBuffers(D3D12MA::Allocator* allocator, D3D12MA::Pool* vertexPool, D3D12MA::Pool* indexPool) {
		if (!GetIsLoaded()) return;

		const uint32_t vertexBufferSize = sizeof(Vertex) * m_vertices.size();
		const uint32_t indexBufferSize = sizeof(uint32_t) * m_indices.size();
		{
			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = vertexBufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC bufferAllocDesc = {};
			bufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			bufferAllocDesc.CustomPool = vertexPool;

			allocator->CreateResource(
				&bufferAllocDesc,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_vertexBufferAllocation,
				IID_PPV_ARGS(&m_vertexBuffer)
			);
		}

		{
			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = indexBufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC bufferAllocDesc = {};
			bufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			bufferAllocDesc.CustomPool = indexPool;

			allocator->CreateResource(
				&bufferAllocDesc,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_indexBufferAllocation,
				IID_PPV_ARGS(&m_indexBuffer)
			);
		}

		constexpr D3D12_RANGE readRange = {0, 0};

		void* bufferData;
		m_vertexBuffer->Map(0, &readRange, &bufferData);
		memcpy(bufferData, m_vertices.data(), vertexBufferSize);
		m_vertexBuffer->Unmap(0, nullptr);

		m_indexBuffer->Map(0, &readRange, &bufferData);
		memcpy(bufferData, m_indices.data(), indexBufferSize);
		m_indexBuffer->Unmap(0, nullptr);
	}
}