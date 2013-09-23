#ifndef __INCLUDE_DEMO_RENDERBUFFER_H__
#define __INCLUDE_DEMO_RENDERBUFFER_H__

#include "../Core/Type.h"

namespace MOE {
    class RenderBuffer{
    public:
        RenderBuffer(const s8* name, const s8* color, const s8* depth, s32 width, s32 height);
        ~RenderBuffer();
        
        void RenderBegin();
        void RenderEnd();
        
        s32 GetTextureID() const;
        s32 GetWidth() const;
        s32 GetHeight() const;

    private:
        class Impl;
        Impl* m_imp;
    };
} // MOE

#endif // __INCLUDE_DEMO_RENDERBUFFER_H__