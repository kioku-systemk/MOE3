#ifndef __INCLUDE_DEMO_SCENE_H__
#define __INCLUDE_DEMO_SCENE_H__

#include "../Core/Type.h"
#include <map>
#include <string>

namespace MOE {
    class Graphics;
    class ProgramObject;
    namespace Math {
        struct vec4;
    }
    
    class Scene{
    public:
        Scene(Graphics* mg, const s8* name, const s8* path);
        virtual ~Scene();
        
        void Update(f64 demotime, f64 scenetime, const std::map<std::string,Math::vec4>& sps);
        void Render(f64 demotime, ProgramObject* prg);
        
        void Resize(s32 w, s32 h);
        const s8* GetName() const;
    private:
        class Impl;
        Impl* m_imp;
    };
} // MOE

#endif // __INCLUDE_DEMO_SCENE_H__