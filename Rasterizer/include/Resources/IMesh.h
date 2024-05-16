#pragma once

#include <cstdint>

class IMesh
{
public:
	IMesh() = default;
	virtual ~IMesh() = default;

	virtual uint32_t GetVertexCount() = 0;
	virtual uint32_t GetIndexCount() = 0;
};
