#pragma once

#include <utility>

namespace AmberEditor::Context
{
	class IDisplay
	{
	public:
		virtual ~IDisplay() = default;

		virtual void Clear() const = 0;
		virtual void Present() const = 0;
		virtual void UpdateDisplayTexture(const void* p_data, uint32_t  p_rowSize) const = 0;
		virtual std::pair<uint16_t, uint16_t> GetSize() const = 0;
	};
}
