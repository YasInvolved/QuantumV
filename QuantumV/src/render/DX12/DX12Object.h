#pragma once

#include "../IObject.h"
#include "../Primitives.h"
#include "DX12Renderer.h"
#include <string>
#include <tiny_obj_loader.h>
#include <vector>

namespace QuantumV {
	class DX12Object : public IObject {
	public:
		DX12Object(DX12Renderer* renderer, const std::string& path);

		void Load() override;
		void Create() override;
		void Destroy() override;
		//void Bind() override;
		//void UnBind() override;
	private:
		DX12Renderer* m_renderer;
		std::string m_path;
		tinyobj::ObjReader m_reader;

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		D3D12MA::Allocation* m_vbAllocation = nullptr;
		ComPtr<ID3D12Resource> m_vb;
		D3D12_VERTEX_BUFFER_VIEW m_vbView = {};

		D3D12MA::Allocation* m_ibAllocation = nullptr;
		ComPtr<ID3D12Resource> m_ib;
		D3D12_INDEX_BUFFER_VIEW m_ibView = {};

		bool m_loaded = false;
	};
}