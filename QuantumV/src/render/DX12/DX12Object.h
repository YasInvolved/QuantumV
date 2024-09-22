#pragma once

#include "../Object.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>
#include <wrl/client.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace QuantumV {
	class DX12Object : public Object {
	public:
		DX12Object(const std::string& path);
		void AllocateBuffers(D3D12MA::Allocator* allocator, D3D12MA::Pool* vertexPool = nullptr, D3D12MA::Pool* indexPool = nullptr);
	private:
		D3D12MA::Allocation* m_vertexBufferAllocation;
		ComPtr<ID3D12Resource> m_vertexBuffer;
		
		D3D12MA::Allocation* m_indexBufferAllocation;
		ComPtr<ID3D12Resource> m_indexBuffer;
	};
}