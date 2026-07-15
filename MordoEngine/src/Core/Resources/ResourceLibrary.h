#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <utility>

template<typename T>
class ResourceLibrary
{
private:
	std::unordered_map<std::string, T> m_Resources;

public:
	template<typename... Args>
	T& Load(const std::string& id, Args&&... args)
	{
		auto [it, inserted] = m_Resources.try_emplace(id, std::forward<Args>(args)...);
		if (!inserted) {
			throw std::runtime_error("Resource already loaded: " + id);
		}
		return it->second;
	}

	T& Get(const std::string& id)
	{
		auto it = m_Resources.find(id);
		if (it == m_Resources.end()) {
			throw std::runtime_error("Resource not found: " + id);
		}
		return it->second;
	}

	bool Exists(const std::string& id) const
	{
		return m_Resources.find(id) != m_Resources.end();
	}
};