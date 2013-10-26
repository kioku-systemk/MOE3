#include "../Core/MOE.h"
#include "EffectBuffer.h"
#include "../Gfx/Graphics.h"

namespace MOE {
    
    class EffectBuffer::Impl{
    public:
        Impl(Graphics* mg, const s8* name, const s8* color, const s8* depth, s32 width, s32 height)
        : g(mg)
        {
            
        }
        ~Impl()
        {
            
        }
        
        void RenderBegin()
        {
            
        }
        void RenderEnd()
        {
            
        }
        
        s32 GetTextureID() const
        {
            return 0; // TODO
        }
        s32 GetWidth() const
        {
            return 0; // TODO
        }
        s32 GetHeight() const
        {
            return 0; // TODO
        }

    private:
        Graphics* g;
//        FrameBufferObject* m_fbo;
    };
    
    // -----------------------------------------------------------------------------
    EffectBuffer::EffectBuffer(Graphics* mg, const s8* name, const s8* color, const s8* depth, s32 width, s32 height)
        : m_imp(new Impl(mg, name,color,depth,width,height)) {}
    EffectBuffer::~EffectBuffer() { delete m_imp; }
    void EffectBuffer::RenderBegin()       { m_imp->RenderBegin();  }
    void EffectBuffer::RenderEnd()         { m_imp->RenderEnd();    }
    s32 EffectBuffer::GetTextureID() const { return m_imp->GetTextureID(); }
    s32 EffectBuffer::GetWidth() const     { return m_imp->GetWidth();     }
    s32 EffectBuffer::GetHeight() const    { return m_imp->GetHeight();    }

} // MOE
