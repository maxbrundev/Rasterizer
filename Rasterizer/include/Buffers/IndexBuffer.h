#pragma once

#include <vector>

namespace Buffers
{
	struct IndexBuffer
	{
		std::vector<uint32_t> Indices;

		IndexBuffer() = default;

		IndexBuffer(const std::vector<uint32_t>& p_indices) : Indices(p_indices)
		{
		}
	};
}