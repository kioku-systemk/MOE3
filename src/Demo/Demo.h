#ifndef __INCLUDE_DEMO_DEMO_H__
#define __INCLUDE_DEMO_DEMO_H__

#include "../Core/Type.h"

namespace MOE {
    class Graphics;
    namespace Math{
        struct matrix4x4;
        struct vec4;
    }
    class Demo{
    public:
        Demo(Graphics* mg);
        ~Demo();
        
        b8 Load(const s8* demolua);
        b8 Export(const s8* packfile);
        
        void Clear();
        void Update(f64 time);
        void Render(f64 time);
        
        void SetMatrix(const s8* name, const Math::matrix4x4& mat);
        void SetVec4(const s8* name, const Math::vec4& vec);
        void Resize(s32 w, s32 h);
        f64 GetDemoTime() const;
    private:
        class Impl;
        Impl* m_imp;
    };
} // MOE

#endif // __INCLUDE_DEMO_DEMO_H__