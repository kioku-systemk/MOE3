#include "../Core/MOE.h"
#include "EffectBuffer.h"
#include "../Gfx/Graphics.h"
#include "../Gfx/BufferObject.h"
#include "../Gfx/FrameBufferObject.h"

namespace MOE {
    
    class EffectBuffer::Impl{
    public:
        Impl(Graphics* mg, const s8* name, const s8* color, const s8* depth, s32 width, s32 height)
        : g(mg)
        {
            m_name = std::string(name);
            int color_comp = 4;
            int color_bit = 32;
            int depth_bit = -1;
            const std::string c(color);
            const std::string d(depth);
            if (c == "RGBA8"){
                color_comp = 4;
                color_bit = 32;
            } else if (c == "RGBA32F") {
                color_comp = 4;
                color_bit = 128;
            } else if (c == "R32F") {
                color_comp = 1;
                color_bit = 32;
            }
            
            if (d == "")
                depth_bit = -1;
            else if (d == "D24")
                depth_bit = 24;
            else if(d == "D16")
                depth_bit = 16;
            
            m_fbo = mnew FrameBufferObject(mg, width, height, color_comp, color_bit, depth_bit);
        }
        ~Impl()
        {
            delete m_fbo;
        }
        
        void RenderBegin()
        {
            m_fbo->Begin();
            g->Viewport(0, 0, m_fbo->Width(), m_fbo->Height());
        }
        void RenderEnd()
        {
            m_fbo->End();
        }
        
        // temporary.
        void BindTexture(int n)
        {
            g->ActiveTexture(VG_TEXTURE0 + n);
            m_fbo->BindTexture();
            g->ActiveTexture(VG_TEXTURE0);
        }
        
        s32 GetTextureID() const
        {
            return m_fbo->GetColorTexture();
        }
        
        s32 GetDepthTextureID() const
        {
            return m_fbo->GetColorTexture();
        }

        s32 GetWidth() const
        {
            return m_fbo->Width();
        }
        s32 GetHeight() const
        {
            return m_fbo->Height();
        }

    private:
        Graphics* g;
        
        std::string m_name;
        FrameBufferObject* m_fbo;
    };
    
    // -----------------------------------------------------------------------------
    EffectBuffer::EffectBuffer(Graphics* mg, const s8* name, const s8* color, const s8* depth, s32 width, s32 height)
        : m_imp(new Impl(mg, name,color,depth,width,height)) {}
    EffectBuffer::~EffectBuffer() { delete m_imp; }
    void EffectBuffer::RenderBegin()       { m_imp->RenderBegin();  }
    void EffectBuffer::RenderEnd()         { m_imp->RenderEnd();    }
    void EffectBuffer::BindTexture(int n)  { m_imp->BindTexture(n); }
    s32 EffectBuffer::GetTextureID() const { return m_imp->GetTextureID(); }
    s32 EffectBuffer::GetWidth() const     { return m_imp->GetWidth();     }
    s32 EffectBuffer::GetHeight() const    { return m_imp->GetHeight();    }

} // MOE
