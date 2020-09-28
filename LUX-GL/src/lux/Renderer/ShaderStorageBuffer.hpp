#pragma once

#include <string>
#include <map>

#include "lux/Types.hpp"


namespace lux {

    class ShaderStorageBuffer
    {
    public:
        ShaderStorageBuffer(std::string name, unsigned int binding_point, unsigned int _size, const void* data);
        ~ShaderStorageBuffer();
        void Publish(const void* data);
        void Publish(uint32_t offset, uint32_t size, const void* data);
        void Bind() const;
        void Unbind() const;
    private:
        std::string _name;
        uint32_t _ssbo_ID, _binding_point, _size;
        std::map <std::string, byte_offset> _layout;
    };
}