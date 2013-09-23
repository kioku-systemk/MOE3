#include "Scene.h"

namespace MOE {
    class Scene::Impl{
    public:
        Impl()
        {
            
        }
        ~Impl()
        {
            
        }
        
        void Update(f64 time)
        {
            
        }
        
        void Render(f64 time)
        {
            
        }
    private:
        
    }
    
// -----------------------------------------------------------------------------
    Scene::Scene() : m_imp(new Impl) {}
    Scene::~Scene() { delete m_imp; }
    void Scene::Update(f64 time) { m_imp->Update(time); }
    void Scene::Render(f64 time) { m_imp->Render(time); }
} // MOE

