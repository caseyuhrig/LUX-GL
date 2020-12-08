#pragma once

//#include "Base.h"
#include <random>
#include <xhash>

// Creator @TheCherno

namespace lux {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 eng(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	// "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
	// used to "uniquely" identify information. In Hazel, even though we use the term
	// GUID and UUID, at the moment we're simply using a randomly generated 64-bit
	// integer, as the possibility of a clash is low enough for now.
	// This may change in the future.
	class UUID
	{
	public:
		UUID()
			: m_UUID(s_UniformDistribution(eng))
		{
		}

		UUID(uint64_t uuid)
			: m_UUID(uuid)
		{
		}

		UUID(const UUID& other)
			: m_UUID(other.m_UUID)
		{
		}

		operator uint64_t () { return m_UUID; }
		operator const uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std {

	template <>
	struct hash<lux::UUID>
	{
		std::size_t operator()(const lux::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}