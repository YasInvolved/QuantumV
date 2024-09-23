#include "DX12Object.h"
#include <QuantumV/core/Log.h>
#include <fstream>

namespace QuantumV {
	DX12Object::DX12Object(DX12Renderer* renderer, const std::string& path)
		: m_renderer(renderer), m_path(path)
	{
	}

	void DX12Object::Load() {
		if (m_loaded) return; // prevent double loading
		
		tinyobj::ObjReaderConfig config = {};
		config.mtl_search_path = "./";
		m_reader.ParseFromFile(m_path, config);
		if (!m_reader.Error().empty()) {
			QV_CORE_ERROR("Failed to load object {0}: {1}", m_path, m_reader.Error());
			return;
		}

		if (!m_reader.Warning().empty()) {
			QV_CORE_WARN(m_reader.Warning());
		}

		const tinyobj::attrib_t& attrib = m_reader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = m_reader.GetShapes();
		const std::vector<tinyobj::material_t>& materials = m_reader.GetMaterials();

		for (size_t s = 0; s < shapes.size(); s++) {
			const tinyobj::mesh_t& mesh = shapes[s].mesh;

			size_t index_offset = 0;
			for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
				int fv = mesh.num_face_vertices[f];

				for (size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx = mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

					m_vertices.push_back(Vertex{ {vx, vy, vz}, {0.52, 0.52, 0.52, 1.0f} });
					m_indices.push_back(index_offset + v);

					// TODO: normals and texcoords
				}

				index_offset += fv;
			}
		}

		m_loaded = true;

		Create();
	}

	void DX12Object::Create() {
		auto allocator = m_renderer->GetAllocator();
		const uint32_t vertexBufferSize = sizeof(Vertex) * m_vertices.size();
		const uint32_t indexBufferSize = sizeof(uint32_t) * m_indices.size();

		{
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Width = vertexBufferSize;
			desc.Height = 1;
			desc.MipLevels = 1;
			desc.DepthOrArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			allocator->CreateResource(
				&allocationDesc,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_vbAllocation,
				IID_PPV_ARGS(&m_vb)
			);

			m_vbView.BufferLocation = m_vb->GetGPUVirtualAddress();
			m_vbView.SizeInBytes = vertexBufferSize;
			m_vbView.StrideInBytes = sizeof(Vertex);
		}

		{
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Width = indexBufferSize;
			desc.Height = 1;
			desc.MipLevels = 1;
			desc.DepthOrArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			allocator->CreateResource(
				&allocationDesc,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_ibAllocation,
				IID_PPV_ARGS(&m_ib)
			);

			m_ibView.BufferLocation = m_ib->GetGPUVirtualAddress();
			m_ibView.Format = DXGI_FORMAT_R32_UINT;
			m_ibView.SizeInBytes = indexBufferSize;
		}

		WCHAR* data;
		allocator->BuildStatsString(&data, TRUE);

		std::wofstream dump("vram_dump.json");
		if (dump.good()) {
			dump << data+1;
		}
		dump.close();
	}

	void DX12Object::Destroy() {}
}