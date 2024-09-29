#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <variant>
#include <array>

enum class RenderAPI {
	D3D12,
	Vulkan
};

#ifdef QV_PLATFORM_WINDOWS
#include <wrl/client.h>
#include <d3d12.h>
using D3D12ResourceComPtr = Microsoft::WRL::ComPtr<ID3D12Resource>;
using D3D12VertexBufferView = D3D12_VERTEX_BUFFER_VIEW;
using D3D12IndexBufferView = D3D12_INDEX_BUFFER_VIEW;
constexpr RenderAPI preferredApi = RenderAPI::D3D12;
#else
using D3D12ResourceComPtr = std::monostate;
using D3D12VertexBufferView = std::monstate;
using D3D12IndexBufferView = std::monostate;
constexpr RenderAPI preferredApi = RenderAPI::Vulkan;
#endif

struct BufferHandle {
	std::variant<D3D12ResourceComPtr, VkBuffer> buffer;

	constexpr bool isD3D12Buffer() const {
		return std::holds_alternative<D3D12ResourceComPtr>(buffer);
	}

	constexpr bool isVulkanBuffer() const {
		return std::holds_alternative<VkBuffer>(buffer);
	}

	D3D12ResourceComPtr GetD3D12Resource() const {
		return std::get<D3D12ResourceComPtr>(buffer);
	}

	VkBuffer GetVulkanBuffer() const {
		return std::get<VkBuffer>(buffer);
	}
};

struct VertexBufferHandle : public BufferHandle {
	std::variant<D3D12VertexBufferView, VkBufferView> view;

	D3D12VertexBufferView GetD3D12VertexBufferView() const 
	{
		return std::get<D3D12VertexBufferView>(view);
	}

	VkBufferView GetVulkanVertexBufferView() const {
		return std::get<VkBufferView>(view);
	}
};

struct IndexBufferHandle : public BufferHandle {
	std::variant<D3D12IndexBufferView, VkBufferView> view;

	D3D12IndexBufferView GetD3D12IndexBufferView() const {
		return std::get<D3D12IndexBufferView>(view);
	}

	VkBufferView GetVulkanIndexBufferView() const {
		return std::get<VkBufferView>(view);
	}
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;

#ifdef QV_PLATFORM_WINDOWS
	static constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 3> GetElementDescDX12() noexcept {
		constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 3> desc = {
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			},
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = "NORMAL",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			},
			D3D12_INPUT_ELEMENT_DESC {
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			}
		};

		return desc;
	}
#endif
};