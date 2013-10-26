#include "../Core/MOE.h"
#include "Scene.h"
#include <string>
#include "../Core/Stream.h"
#include "../Gfx/Graphics.h"
#include "../SceneGraph/SceneGraph.h"
#include "../SceneGraph/MrzLoader.h"
#include "../Gfx/SceneGraphRender.h"

namespace MOE {
    class Scene::Impl{
    public:
        Impl(Graphics* mg, const s8* name, const s8* path)
        : g(mg)
        {
            m_render = new SceneGraphRender(g);
            m_name = std::string(name);
            m_path = std::string(path);
            m_node = 0;
            const size_t p = m_path.rfind(".MRZ");
            if (p == m_path.size()-4) {
                MrzLoader loader;
                Stream st(path, Stream::MODE_INPUT_BINARY_ONMEMORY);
                if (!st.IsOpened())
                    return;
                m_node = loader.Load(&st);
            } else {
                MOELogD("Not supported type. -> %s", path);
            }
        }
        ~Impl()
        {
            delete m_node;
        }
        
        void Update(f64 demotime, f64 scenetime)
        {
            
            m_render->UpdateBuffers(m_node);
        }
        
        void Render(f64 demotime)
        {
            m_render->Draw(m_node);
        }
    private:
        std::string m_name;
        std::string m_path;
        SceneGraph::Node* m_node;
        Graphics* g;
        SceneGraphRender* m_render;
    };
    
// -----------------------------------------------------------------------------
    Scene::Scene(Graphics* mg, const s8* name, const s8* path) : m_imp(new Impl(mg,name,path)){}
    Scene::~Scene() { delete m_imp; }
    void Scene::Update(f64 demotime, f64 scenetime) { m_imp->Update(demotime,scenetime); }
    void Scene::Render(f64 demotime) { m_imp->Render(demotime); }
} // MOE

