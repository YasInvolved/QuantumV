#pragma once

#include <QuantumV/core/Base.h>
#include <QuantumV/resources/Resource.h>
#include <QuantumV/core/Log.h>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <mutex>

namespace QuantumV {
	class QV_API ResourceManager {
	public:
		ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		template <typename T>
		Ref<T> loadResource(const std::string& name, const std::string& filePath) {
			std::lock_guard<std::mutex> lock(m_resourceMutex);

			const std::type_info& typeInfo = typeid(T);
			auto it = m_resources.find(name);
			if (it != m_resources.end()) {
				if (m_resourceTypes[name] != &typeInfo) {
					QV_CORE_ERROR("Resource {} already loaded with a different type", name);
					return nullptr;
				}

				return std::static_pointer_cast<T>(it->second);
			}

			auto resource = std::make_shared<T>();
			try {
				resource->Load(filePath);
			}
			catch (const std::exception& e) {
				QV_CLIENT_ERROR("Failed to load resource {0}: {1}", name, e.what());
				return nullptr;
			}

			m_resources[name] = resource;
			m_resourceTypes[name] = &typeInfo;

			return resource;
		}

		void unloadResource(const std::string& name);
		void unloadAll();
	private:
		std::mutex m_resourceMutex;
		std::unordered_map<std::string, Ref<Resource>> m_resources;
		std::unordered_map<std::string, const std::type_info*> m_resourceTypes;
	};
}