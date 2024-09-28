#include <QuantumV/core/Log.h>
#include "ObjectFactory.h"
#include "D3D12/Object.h"

namespace QuantumV {
	std::future<IObject*> ObjectFactory::CreateObjectAsync(const std::string& filepath, IAllocator* allocator) {
		return std::async(std::launch::async, [filepath, allocator]() -> IObject* {
			if (allocator->GetRenderAPI() == RenderAPI::D3D12) {
				return new D3D12::Object(filepath, allocator);
			}
			else {
				return nullptr;
			}
		});
	}
}