#pragma once

#include <cstdint>
#include <string>
#include "../core/Window.h"

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() = default;

		// Initializes rendering system
		virtual void Init(const Window* window, uint32_t width, uint32_t height) = 0;

		// Clears render target
		virtual void Clear(float r, float g, float b, float a) = 0;

		// set viewport
		// virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		// bind pipeline (DX12 PSO or Vulkan Pipeline Layout)
		// virtual void BindPipeline(Pipeline* pipeline) = 0;

		// draw
		virtual void Draw(int vertex_count, int start_index = 0) = 0;

		//virtual Shader* CreateShader(const std::string& shader_path) = 0;
		//virtual Texture* CreateTexture(const std::string& texture_path) = 0;
		//virtual VertexBuffer* CreateVertexBuffer(const void* data, size_t size) = 0;

		virtual void Resize(uint32_t new_width, uint32_t new_height) = 0;
	};
}