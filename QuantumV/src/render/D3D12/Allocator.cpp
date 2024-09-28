#pragma once

#ifdef QV_PLATFORM_WINDOWS

#include "Allocator.h"
#include <utility>

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
		BufferHandle handle = {};

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
		D3D12ResourceComPtr buffer;
		m_allocator->CreateResource(
			&allocDesc,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&allocation,
			IID_PPV_ARGS(&buffer)
		);
		m_allocations.push_back(allocation);

		handle.buffer = std::move(buffer);
		return handle;
	}

	VertexBufferHandle Allocator::AllocateVertexBuffer(size_t size) {
		VertexBufferHandle handle = {};

		{
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
			D3D12ResourceComPtr buffer;
			m_allocator->CreateResource(
				&allocDesc,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&allocation,
				IID_PPV_ARGS(&buffer)
			);
			m_allocations.push_back(allocation);
			handle.buffer = std::move(buffer);
		}

		D3D12VertexBufferView view = {};
		view.BufferLocation = handle.GetD3D12Resource()->GetGPUVirtualAddress();
		view.SizeInBytes = size;
		view.StrideInBytes = sizeof(Vertex);

		handle.view = std::move(view);

		return handle;
	}

	IndexBufferHandle Allocator::AllocateIndexBuffer(size_t size) {
		IndexBufferHandle handle = {};

		{
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

			D3D12ResourceComPtr buffer;
			D3D12MA::Allocation* allocation = nullptr;
			m_allocator->CreateResource(
				&allocDesc,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&allocation,
				IID_PPV_ARGS(&buffer)
			);
			m_allocations.push_back(allocation);
			handle.buffer = std::move(buffer);
		}

		D3D12IndexBufferView view = {};
		view.BufferLocation = handle.GetD3D12Resource()->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = size;

		handle.view = std::move(view);

		return handle;
	}
}

#endif