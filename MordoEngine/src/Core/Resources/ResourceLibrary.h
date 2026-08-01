#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <utility>

/// The one owner of a kind of resource, keyed by a string id everything else
/// borrows by.
///
/// Backed by an unordered_map, which is node based: a reference handed out by
/// Load or Get stays valid however many resources are added afterwards. That is
/// what lets a model hold a Texture* for the life of the program, and it is the
/// reason this is not a vector.
///
/// @tparam T Resource type. Constructed in place from the arguments given to
///           Load, so it does not need to be movable or copyable.
template<typename T>
class ResourceLibrary
{
private:
	std::unordered_map<std::string, T> m_Resources;

public:
	/// Constructs a resource in place and registers it under an id.
	///
	/// @param id   Key everything else will ask for. Must not already exist.
	/// @param args Forwarded to T's constructor.
	/// @return Reference to the stored resource, valid for the life of the
	///         library.
	/// @throws std::runtime_error if the id is already taken. Call Exists first
	///         when several callers may want the same resource, since the
	///         duplicate is the caller's mistake and not a recoverable state.
	template<typename... Args>
	T& Load(const std::string& id, Args&&... args)
	{
		auto [it, inserted] = m_Resources.try_emplace(id, std::forward<Args>(args)...);
		if (!inserted)
		{
			throw std::runtime_error("Resource already loaded: " + id);
		}
		return it->second;
	}

	/// @throws std::runtime_error if nothing is registered under the id. Loading
	///         is a startup step, so by the time anything asks, a miss is a bug
	///         rather than a case to handle.
	T& Get(const std::string& id)
	{
		auto it = m_Resources.find(id);
		if (it == m_Resources.end())
		{
			throw std::runtime_error("Resource not found: " + id);
		}
		return it->second;
	}

	/// The guard for anything optional: a resource that may legitimately be
	/// absent is asked about here before Get is allowed to throw.
	bool Exists(const std::string& id) const
	{
		return m_Resources.find(id) != m_Resources.end();
	}
};
