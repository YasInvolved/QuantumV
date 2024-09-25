#pragma once

#ifdef QV_PLATFORM_WINDOWS

#include "Allocator.h"

namespace QuantumV::D3D12 {
	Allocator::Allocator(IDXGIAdapter4* adapter, ID3D12Device10* device) {
		D3D12MA::ALLOCATOR_DESC desc = {};
		desc.pAdapter = adapter;
		desc.pDevice = device;

		D3D12MA::CreateAllocator(&desc, &m_allocator);
	}

	Allocator::~Allocator() {
		for (const auto& allocation : m_allocations) {
			allocation->Release();
		}
		m_allocator->Release();
	}

	BufferHandle Allocator::AllocateBuffer(size_t size) {
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.MipLevels = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.Width = size;
		bufferDesc.Height = 1;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		
		D3D12MA::Allocation* allocation = nullptr;
		BufferHandle handle;
		m_allocator->CreateResource(
			&allocDesc,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&allocation,
			IID_PPV_ARGS(&handle.GetD3D12Resource())
		);

		return handle;
	}

	VertexBufferHandle Allocator::AllocateVertexBuffer(size_t size) {
		VertexBufferHandle handle = {};
		return handle;
	}

	IndexBufferHandle Allocator::AllocateIndexBuffer(size_t size) {
		IndexBufferHandle handle = {};
		return handle;
	}
}

#endif