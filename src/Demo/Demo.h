#ifndef __INCLUDE_DEMO_DEMO_H__
#define __INCLUDE_DEMO_DEMO_H__

#include "../Core/Type.h"

namespace MOE {
    class Graphics;
    
    class Demo{
    public:
        Demo(Graphics* mg);
        ~Demo();
        
        b8 Load(const s8* demolua);
        void Clear();
        
        void Update(f64 time);
        void Render(f64 time);
        
    private:
        class Impl;
        Impl* m_imp;
    };
} // MOE

#endif // __INCLUDE_DEMO_DEMO_H__