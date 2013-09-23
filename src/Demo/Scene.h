#ifndef __INCLUDE_DEMO_SCENE_H__
#define __INCLUDE_DEMO_SCENE_H__

#include "../Core/Type.h"

namespace MOE {
    class Scene{
    public:
        Scene();
        virtual ~Scene();
        
        void Update(f64 demotime, f64 scenetime);
        void Render(f64 demotime, f64 scenetime);
    private:
        class Impl;
        Impl* m_imp;
    };
} // MOE

#endif // __INCLUDE_DEMO_SCENE_H__