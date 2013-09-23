#include "RenderBuffer.h"

namespace MOE {
    
    class RenderBuffer::Impl{
    public:
        Impl()
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
//        FrameBufferObject* m_fbo;
    }
    
    // -----------------------------------------------------------------------------
    RenderBuffer::RenderBuffer() : m_imp(new Impl) {}
    RenderBuffer::~RenderBuffer() { delete m_imp; }
    void RenderBuffer::RenderBegin()       { m_imp->RenderBegin();  }
    void RenderBuffer::RenderEnd()         { m_imp->RenderEnd();    }
    s32 RenderBuffer::GetTextureID() const { m_imp->GetTextureID(); }
    s32 RenderBuffer::GetWidth() const     { m_imp->GetWidth();     }
    s32 RenderBuffer::GetHeight() const    { m_imp->GetHeight();    }

} // MOE
