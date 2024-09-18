#pragma once

#include <cstdint>
#include <string>

namespace QuantumV {
	class IRenderer {
	public:
		virtual ~IRenderer() = default;

		// Initializes rendering system
		virtual void Init(void* window_handle, uint32_t width, uint32_t height) = 0;

		// Clears render target
		virtual void Clear(float r, float g, float b, float a) = 0;

		// draw
		virtual void Draw(int vertex_count, int start_index = 0) = 0;

		//virtual Shader* CreateShader(const std::string& shader_path) = 0;
		//virtual Texture* CreateTexture(const std::string& texture_path) = 0;
		//virtual VertexBuffer* CreateVertexBuffer(const void* data, size_t size) = 0;

		virtual void Resize(uint32_t new_width, uint32_t new_height) = 0;
	};
}