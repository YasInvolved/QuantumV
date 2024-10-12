#include <QuantumV/resources/ResourceManager.h>

namespace QuantumV {
	void ResourceManager::unloadResource(const std::string& name) {
		std::lock_guard<std::mutex> lock(m_resourceMutex);
		if (m_resources.find(name) != m_resources.end()) {
			m_resources[name]->Unload();
			m_resources.erase(name);
			m_resourceTypes.erase(name);
		}
	}

	void ResourceManager::unloadAll() {
		std::lock_guard<std::mutex> lock(m_resourceMutex);
		for (const auto& [name, resource] : m_resources) {
			resource->Unload();
		}

		m_resources.clear();
		m_resourceTypes.clear();
	}
}