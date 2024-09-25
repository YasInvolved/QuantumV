#pragma once

#include <vulkan/vulkan.h>
#include <variant>

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
	size_t size;
	size_t stride;

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

	D3D12VertexBufferView GetD3D12VertexBufferView() const {
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