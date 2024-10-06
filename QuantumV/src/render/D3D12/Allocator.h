#pragma once

#ifdef QV_PLATFORM_WINDOWS

#include <QuantumV/core/Base.h>
#include "../IAllocator.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include <unordered_map>

namespace QuantumV::D3D12 {
	class Allocator : public IAllocator {
	public:
		Allocator(IDXGIAdapter4* adapter, ID3D12Device10* device);
		~Allocator();

		BufferHandle AllocateBuffer(size_t size) override;
		VertexBufferHandle AllocateVertexBuffer(size_t size) override;
		IndexBufferHandle AllocateIndexBuffer(size_t size) override;
		ImageHandle AllocateImage(size_t width, size_t height) override;
		void GenerateMemoryDump() override;
		RenderAPI GetRenderAPI() override { return RenderAPI::D3D12; }
		void Free(BufferHandle buffer) override;
		void Free(ImageHandle image) override;
	private:
		D3D12MA::Allocator* m_allocator;
		std::unordered_map<xg::Guid, D3D12MA::Allocation*> m_allocations;
	};
}

#endif