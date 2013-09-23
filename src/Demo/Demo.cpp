#include "Demo.h"
#include "Scene.h"
#include "RenderBuffer.h"
#include "../Core/Stream.h"
#include <string>
#include <vector>
#include <map>

namespace MOE {

class Demo::Impl{
public:
    Impl(Graphics* mg) : g(mg)
    {
        
    }
    ~Impl()
    {
        
    }
    
    void createBuffers()
    {
        
    }
    
    void createScenes()
    {
        
    }
    
    b8 Load(const s8* demolua)
    {
        // load lua
        
        // creates
        createBuffers();
        createScenes();
        
        return false;
    }
    void Clear()
    {
        std::map<std::string, RenderBuffer*>::iterator bit,beit = m_buffers.end();
        for (bit = m_buffers.begin(); bit != beit; ++bit)
        {
            delete bit->second;
        }
        m_buffers.clear();
        std::map<std::string, Scene*>::iterator sit,seit = m_scenes.end();
        for (sit = m_scenes.begin(); sit != seit; ++sit)
        {
            delete sit->second;
        }
        m_scenes.clear();
        
        m_sceneinfo.clear();
    }
    void Update(f64 time)
    {
        std::vector<SceneInfo*>::iterator it,eit = m_sceneinfo.end();
        for (it = m_sceneinfo.begin(); it != eit; ++it)
        {
            if ((*it)->demo_startTime <= time
                &&  (*it)->demo_endTime   >  time) {
                const f64 demospantime  = (*it)->demo_endTime - (*it)->demo_startTime;
                const f64 scenedirtime  = (*it)->scene_endTime - (*it)->scene_startTime;
                const f64 scenespantime = fabs((*it)->scene_endTime - (*it)->scene_startTime);
                const f64 scenedir      = scenedirtime >= 0.0 ? 1.0 : -1.0;
                const f64 ltime = (time - (*it)->demo_startTime) * scenedir * scenespantime / demospantime + (*it)->scene_startTime;
                (*it)->scene->Update(time, ltime);
            }
        }
    }
    void Render(f64 time)
    {
        std::vector<SceneInfo*>::iterator it,eit = m_sceneinfo.end();
        for (it = m_sceneinfo.begin(); it != eit; ++it)
        {
            if ((*it)->demo_startTime <= time
            &&  (*it)->demo_endTime   >  time) {
                const f64 demospantime  = (*it)->demo_endTime - (*it)->demo_startTime;
                const f64 scenedirtime  = (*it)->scene_endTime - (*it)->scene_startTime;
                const f64 scenespantime = fabs((*it)->scene_endTime - (*it)->scene_startTime);
                const f64 scenedir      = scenedirtime >= 0.0 ? 1.0 : -1.0;
                const f64 ltime = (time - (*it)->demo_startTime) * scenedir * scenespantime / demospantime + (*it)->scene_startTime;
                (*it)->scene->Render(time, ltime);
            }
        }
    }
    
private:
    Graphics* g;
    std::map<std::string, RenderBuffer*> m_buffers;
    std::map<std::string, Scene*> m_scenes;
    std::vector<SceneInfo*> m_sceneinfo;
    
    class ShaderParam
    {
    public:
        std::string name;
        f64 val;
    };
    
    class SceneInfo
    {
    public:
        f64 demo_startTime;
        f64 demo_endTime;
        f64 scene_startTime;
        f64 scene_endTime;
        Scene* scene;
        RenderBuffer* targetbuffer;
        std::vector<ShaderParam> shaderparam;
    };
    
}

// -----------------------------------------------------------------------------
Demo::Demo(Graphics* mg) : m_imp(new Impl(mg)) {}
Demo::~Demo() { delete m_imp; }
    
b8 Demo::Load(const s8* demolua) { return m_imp->Load(demolua); }
void Demo::Clear()               { m_imp->Clear();      }
void Demo::Update(f64 time)      { m_imp->Update(time); }
void Demo::Render(f64 time)      { m_imp->Render(time); }
    
} // MOE
