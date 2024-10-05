#include <QuantumV/core/Log.h>
#include "ObjectFactory.h"
#include "D3D12/Object.h"

namespace QuantumV {
	std::future<Ref<IObject>> ObjectFactory::CreateObjectAsync(const std::string& filepath, const Ref<IAllocator> allocator, std::optional<const std::string> materialPath) {
		return std::async(std::launch::async, [filepath, allocator, materialPath]() -> Ref<IObject> {
			if (allocator->GetRenderAPI() == RenderAPI::D3D12) {
				return std::make_shared<D3D12::Object>(filepath, allocator, materialPath);
			}
			else {
				return nullptr;
			}
		});
	}
}