#include "../Core/MOE.h"
#include "Scene.h"
#include <string>
#include "../Core/Stream.h"
#include "../Gfx/Graphics.h"
#include "../SceneGraph/SceneGraph.h"
#include "../SceneGraph/MrzLoader.h"
#include "../SceneGraph/AnimLoader.h"
#include "../SceneGraph/Util.h"
#include "../Gfx/SceneGraphRender.h"

namespace {
#if MOE_PLATFORM_WINDOWS
	void replace(std::string& src, const char* pszKey, const char* pszRepWord)
	{
		size_t nPos = 0;
		while((nPos = src.find(pszKey, nPos)) != std::string::npos){
			src.replace(nPos, sizeof(pszKey), pszRepWord);
        }
        return;
	}
#endif
    
    std::string getResourcePath(const s8* path)
	{
#if MOE_PLATFORM_WINDOWS
		const s8* dirchar = "\\";
#else
		const s8* dirchar = "/";
#endif
        std::string dirpath;
        std::string spath = std::string(path);
#if MOE_PLATFORM_WINDOWS
		replace(spath, "/", "\\");
#endif
		size_t np;
		if ((np = spath.rfind(dirchar)) != std::string::npos)
		{
			dirpath = spath.substr(0, np+1);
		}
		return dirpath;
	}
}

namespace MOE {
    class Scene::Impl{
    public:
        Impl(Graphics* mg, const s8* name, const s8* path)
        : g(mg)
        {
            m_render = new SceneGraphRender(g);
            m_width = 640;
            m_height = 480;
            m_name = std::string(name);
            m_path = std::string(path);
            m_root = 0;
            m_anim = 0;
            m_cameranode = 0;
            const size_t p = m_path.rfind(".MRZ");
            if (p == m_path.size()-4) {
                MOELogD("Loading MRZ:[%s]", m_path.c_str());
                
                m_render->SetResourcePath(getResourcePath(path).c_str());

                // load MRZ
                MrzLoader loader;
                Stream st(m_path.c_str(), Stream::MODE_INPUT_BINARY_ONMEMORY);
                if (!st.IsOpened())
				{
					MOELogE("nofile MRZ. [%s]", m_path.c_str());
					return;
				}
                m_root = loader.Load(&st);

                if (!m_root)
                {
                    MOELogE("can't load MRZ. [%s]", m_path.c_str());
                    return;
                }
                MOELogD("Loaded MRZ. [%s]", m_path.c_str());
                
                m_cameranode = static_cast<const SceneGraph::Transform*>(SceneGraph::FindNode(m_root, "Camera"));
                if (!m_cameranode)
                    MOELogW("can't find Camera node.");
                
                // load Anim
                std::string animfile(path);
                size_t p = animfile.rfind(".");
                if (p != std::string::npos)
                {
                    animfile = animfile.substr(0,p);
                    animfile += "_mrz.anim";
                }
                if (animfile != "") {
                    MOE::Stream ast(animfile.c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
                    MOE::AnimLoader aloader;
                    MOE::Animation* anim = aloader.Load(&ast);
                    m_anim = anim;
                    if (!anim)
                        MOELogE("not found Animation. [%s]", animfile.c_str());
                }

            } else {
                MOELogE("Not supported type. -> %s", path);
            }
        }
        ~Impl()
        {
            delete m_root;
        }
        
        void Update(f64 demotime, f64 scenetime, const std::map<std::string,Math::vec4>& sps)
        {
            // Animation
            if (m_anim)
                m_anim->Animate(m_root, scenetime);

            // View
            using namespace MOE::SceneGraph;
            using namespace MOE::Math;
            f32 fov = 60.0;
            matrix view = Identity();
            if (m_cameranode)
            {
                const matrix cammat = GetParentMatrix(m_cameranode);
                const vec3 cpos = (cammat * vec4(0,0,0,1)).xyz();
                const vec3 ctar = (cammat * vec4(0,0,-1,1)).xyz();
                const vec3 cup  = (cammat * vec4(0,1,0,0)).xyz();
                view = LookAt(cpos, ctar, cup);
                if (m_cameranode->GetType() == NODETYPE_TRANSFORM)
                {
                    const Transform* tr = static_cast<const Transform*>(m_cameranode);
                    s32 n = tr->GetChildCount();
                    for (s32 i = 0; i < n; ++i){
                        const Node* nd = tr->GetChild(i);
                        if (nd->GetType() == NODETYPE_CAMERA){
                            const Camera* camnode = static_cast<const Camera*>(nd);
                            const float asp = static_cast<f32>(m_height) / static_cast<f32>(m_width);
                            fov = camnode->GetFov() * asp;
                            break;
                        }
                    }
                }
            }
            const matrix proj = PerspectiveFov(fov, m_width/static_cast<f32>(m_height), 0.1, 1000.0);

            m_render->SetProjMatrix(proj);
            m_render->SetViewMatrix(view);
            m_render->SetUniform("time", vec4(scenetime,scenetime,scenetime,scenetime));
            const auto eit = sps.end();
            for (auto it = sps.begin(); it != eit; ++it)
                m_render->SetUniform(it->first.c_str(), it->second);
            m_render->UpdateBuffers(m_root);
        }
        
        void Render(f64 demotime, ProgramObject* prg)
        {
            m_render->Draw(m_root, prg);
        }
        
        void Resize(int w, int h)
        {
            m_width = w;
            m_height = h;
        }
        
        const s8* GetName() const
        {
            return m_name.c_str();
        }
        
    private:
        std::string m_name;
        std::string m_path;
        SceneGraph::Node* m_root;
        const SceneGraph::Transform* m_cameranode;
        Animation* m_anim;
        Graphics* g;
        SceneGraphRender* m_render;
        int m_width, m_height;
    };
    
// -----------------------------------------------------------------------------
    Scene::Scene(Graphics* mg, const s8* name, const s8* path) : m_imp(mnew Impl(mg,name,path)) {}
    Scene::~Scene() { delete m_imp; }
    void Scene::Update(f64 demotime, f64 scenetime, const std::map<std::string,Math::vec4>& sps) { m_imp->Update(demotime,scenetime, sps); }
    void Scene::Render(f64 demotime, ProgramObject* prg) { m_imp->Render(demotime, prg); }
    void Scene::Resize(s32 w, s32 h) { m_imp->Resize(w, h); };
    const s8* Scene::GetName() const { return m_imp->GetName(); };
} // MOE

