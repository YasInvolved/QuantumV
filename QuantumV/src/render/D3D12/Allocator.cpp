#pragma once

#ifdef QV_PLATFORM_WINDOWS

#include "Allocator.h"
#include <utility>
#include <fstream>
#include <QuantumV/core/Log.h>

namespace QuantumV::D3D12 {
	Allocator::Allocator(IDXGIAdapter4* adapter, ID3D12Device10* device) {
		D3D12MA::ALLOCATOR_DESC desc = {};
		desc.pAdapter = adapter;
		desc.pDevice = device;

		D3D12MA::CreateAllocator(&desc, &m_allocator);
	}

	Allocator::~Allocator() {
		for (const auto& allocation : m_allocations) {
			allocation.second->Release();
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
		auto allocationId = xg::newGuid();

		m_allocations.emplace(std::make_pair(allocationId, allocation));

		handle.buffer = std::move(buffer);
		handle.allocationId = allocationId;
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
			auto allocationId = xg::newGuid();

			m_allocations.emplace(std::make_pair(allocationId, allocation));
			handle.buffer = std::move(buffer);
			handle.allocationId = allocationId;
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
			auto allocationId = xg::newGuid();

			m_allocations.emplace(std::make_pair(allocationId, allocation));
			handle.buffer = std::move(buffer);
			handle.allocationId = allocationId;
		}

		D3D12IndexBufferView view = {};
		view.BufferLocation = handle.GetD3D12Resource()->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = size;

		handle.view = std::move(view);

		return handle;
	}

	ImageHandle Allocator::AllocateImage(size_t width, size_t height) {
		ImageHandle handle = {};

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.MipLevels = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.Width = width;
		bufferDesc.Height = height;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12ResourceComPtr image;
		D3D12MA::Allocation* allocation = nullptr;
		m_allocator->CreateResource(
			&allocDesc,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&allocation,
			IID_PPV_ARGS(&image)
		);
		auto allocationId = xg::newGuid();

		m_allocations.emplace(std::make_pair(allocationId, allocation));
		handle.image = std::move(image);
		handle.allocationId = allocationId;

		return handle;
	}

	void Allocator::GenerateMemoryDump() {
		WCHAR* buffer;
		m_allocator->BuildStatsString(&buffer, true);
		std::wstring dump(&buffer[1]);
		std::wofstream dumpFile("memdump_d3d12.json");
		if (dumpFile.good()) {
			QV_CORE_TRACE("Saving memory dump to: memdump_d3d12.json");
			dumpFile << dump; // dunno this generation is a little bit broken on the library side
		}
		dumpFile.close();
		m_allocator->FreeStatsString(buffer);
	}

	void Allocator::Free(BufferHandle buffer) {
		auto allocation = m_allocations[buffer.allocationId];
		allocation->Release();
	}

	void Allocator::Free(ImageHandle image) {
		auto allocation = m_allocations[image.allocationId];
		allocation->Release();
	}
}

#endif