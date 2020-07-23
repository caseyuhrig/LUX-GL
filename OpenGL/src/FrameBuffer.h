#pragma once

//#include <cstdint>
//#include <memory>
#include "ux/ux.h"

/*
template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
*/

struct FrameBufferSpecification
{
    uint32_t width, height;
    uint32_t Samples = 1;
};

class FrameBuffer
{
public:
    //virtual const FrameBufferSpecification& GetSpecification() const = 0;

    //static ux::Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
    inline static ux::Ref<FrameBuffer> Create(const FrameBufferSpecification& spec)
    {

    }
};