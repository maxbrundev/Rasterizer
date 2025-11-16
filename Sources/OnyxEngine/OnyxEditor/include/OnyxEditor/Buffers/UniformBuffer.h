#pragma once
#include <cstdint>

namespace OnyxEditor::Buffers
{
    class UniformBuffer
    {
    public:
        UniformBuffer(size_t p_size, std::uint32_t p_bindingPoint = 0);
        ~UniformBuffer();

        void Bind() const;
        void Unbind() const;

        template<typename T>
        void SetSubData(const T& p_data, size_t p_offset)
        {
            SetSubData(&p_data, sizeof(T), p_offset);
        }

        void SetSubData(const void* p_data, size_t p_dataSize, size_t p_offset);

    private:
        uint32_t m_bufferID;
        uint32_t m_bindingPoint;
        size_t m_size;
    };
}
