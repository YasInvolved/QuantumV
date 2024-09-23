#pragma once

#include "QVMath.h"
#include <array>
#include <d3d12.h>

struct Vertex {
	glm::vec3 position;
	glm::vec4 color;

	static constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 2> GetElementDescDX12() {
		constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 2> desc = {
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			},
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 12,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			}
		};

		return desc;
	}

	// vulkan desc too
};