#include "../Core/MOE.h"
#include "Demo.h"
#include "Scene.h"
#include "EffectBuffer.h"
#include "../Core/Stream.h"
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "../Gfx/SceneGraphRender.h"
#include "../Gfx/ShaderProgramObject.h"
#include "../Core/LuaUtil.h"

namespace {
    template<typename T>
    void clearmap(T& buffers)
    {
        const auto eit = buffers.end();
        for (auto it = buffers.begin(); it != eit; ++it)
            delete it->second;
        buffers.clear();
    }
    template<typename T>
    void clearvector(T& buffers)
    {
        const auto eit = buffers.end();
        for (auto it = buffers.begin(); it != eit; ++it)
            delete (*it);
        buffers.clear();
    }
    
    std::string getResourcePath(const s8* path)
	{
#if MOE_PLATFORM_WINDOWS
		const s8* dirchar = "\\";
#else
		const s8* dirchar = "/";
#endif
        std::string dirpath;
        std::string spath = std::string(path);
		size_t np;
		if ((np = spath.rfind(dirchar)) != std::string::npos)
		{
			dirpath = spath.substr(0, np+1);
		}
		return dirpath;
	}

    std::vector<std::string> split(const std::string& astr, const std::string& delim)
    {
        std::vector<std::string> result;
        std::string str(astr);
        size_t cutAt;
        while( (cutAt = str.find_first_of(delim)) != str.npos )
        {
            if(cutAt > 0)
            {
                result.push_back(str.substr(0, cutAt));
            }
            str = str.substr(cutAt + 1);
        }
        if(str.length() > 0)
        {
            result.push_back(str);
        }
        return result;
    }
}

namespace MOE {

class Demo::Impl{
private:
    
    void createBuffers(Graphics* g, lua_State* L)
    {
        const int bufnum = getTableNum(L, "Buffer");
        for (int i = 0; i < bufnum; ++i)
        {
            const std::string name  = eval<std::string>(L, "return Buffer[%d].name",i+1);
            const std::string color = eval<std::string>(L, "return Buffer[%d].color",i+1);
            const std::string depth = eval<std::string>(L, "return Buffer[%d].depth",i+1);
            const int w     = eval<int>(L, "return Buffer[%d].width",i+1);
            const int h     = eval<int>(L, "return Buffer[%d].height",i+1);
            if (name != "" && w != 0 && h != 0) {
                EffectBuffer* eb = mnew EffectBuffer(g, name.c_str(), color.c_str(), depth.c_str(), w, h);
                m_buffers[name] = eb;
            }
        }
    }
    
    void createScenes(Graphics* g, lua_State* L)
    {
        const int scenenum = getTableNum(L, "Scene");
        for (int i = 0; i < scenenum; ++i)
        {
            const std::string name  = eval<std::string>(L, "return Scene[%d].name",i+1);
            const std::string path = eval<std::string>(L, "return Scene[%d].path",i+1);
            if (name != "" && path != "") {
                std::string fullpath = m_respath + path;
                MOE::Scene* sc = mnew MOE::Scene(g, name.c_str(), fullpath.c_str());
                m_scenes[name] = sc;
            }
        }
    }
    
    void createProcesses(lua_State* L)
    {
        const int processnum = getTableNum(L, "Process");
        for (int i = 0; i < processnum; ++i)
        {
            const std::string scene  = eval<std::string>(L, "return Process[%d].scene",i+1);
            const int sc_start = eval<int>(L, "return Process[%d].scenetime[1]",i+1);
            const int sc_end   = eval<int>(L, "return Process[%d].scenetime[2]",i+1);
            const int dt_start = eval<int>(L, "return Process[%d].demotime[1]",i+1);
            const int dt_end   = eval<int>(L, "return Process[%d].demotime[2]",i+1);
            char paramstr[128] = {};
            std::map<std::string,std::string> vals;
            sprintf(paramstr, "Process[%d].vec4", i+1);
			getTableValues(L, paramstr, vals);
            std::vector<ShaderParam> params;
			const auto eit = vals.end();
			for (auto it = vals.begin(); it != eit; ++it)
			{
                const std::vector<std::string> sp = split(it->second, ",");
                Math::vec4 v(0,0,0,0);
                v.x = atoi(sp[0].c_str());
                v.y = atoi(sp[1].c_str());
                v.z = atoi(sp[2].c_str());
                v.w = atoi(sp[3].c_str());
                params.push_back(ShaderParam(it->first, v));
            }

            if (scene != "") {
                ProcessInfo* pi = mnew ProcessInfo(dt_start,dt_end,sc_start, sc_end, m_scenes[scene],params);
                m_processes.push_back(pi);
            }
        }
    }
    
    void createRenderEffects(lua_State* L)
    {
        std::map<std::string,std::string> vals;
        const int rendernum = getTableNum(L, "Render");
        for (int i = 0; i < rendernum; ++i)
        {
            const std::string src  = eval<std::string>(L, "return Render[%d].src",i+1);
            const std::string target = eval<std::string>(L, "return Render[%d].target[1]",i+1);
            std::string shader = "default";
            shader = eval<std::string>(L, "return Render[%d].shader",i+1);
            const int dt_start = eval<int>(L, "return Render[%d].demotime[1]",i+1);
            const int dt_end   = eval<int>(L, "return Render[%d].demotime[2]",i+1);
            char paramstr[128] = {};
            vals.clear();
            sprintf(paramstr, "Render[%d].tex", i+1);
			getTableValues(L, paramstr, vals);
            std::vector<ShaderParam> sparams;
			auto eit = vals.end();
			for (auto it = vals.begin(); it != eit; ++it)
			{
                EffectBuffer* eb = m_buffers[it->second];
                sparams.push_back(ShaderParam(it->first, eb));
            }
            vals.clear();
            sprintf(paramstr, "Render[%d].vec4", i+1);
			getTableValues(L, paramstr, vals);
            std::vector<ShaderParam> params;
			eit = vals.end();
			for (auto it = vals.begin(); it != eit; ++it)
			{
                const std::vector<std::string> sp = split(it->second, ",");
                Math::vec4 v(0,0,0,0);
                v.x = atoi(sp[0].c_str());
                v.y = atoi(sp[1].c_str());
                v.z = atoi(sp[2].c_str());
                v.w = atoi(sp[3].c_str());
                sparams.push_back(ShaderParam(it->first, v));
            }

            if (src != "" && target != "") {
                RenderEffectInfo* re = mnew RenderEffectInfo(dt_start, dt_end, m_scenes[src], m_buffers[target], shader, sparams);
                m_renderEffects.push_back(re);
            }
        }
    }
    void createOverridePrograms(Graphics* g)
    {
        const auto eit = m_renderEffects.end();
        for (auto it = m_renderEffects.begin(); it != eit; ++it)
        {
            const std::string sdr = (*it)->overrideShader;
            if (sdr != "default" && !m_ovprgs[sdr])
            {
                ProgramObject* prg = mnew ProgramObject(g);
                std::string fpath = m_respath + sdr + ".fx";
                Stream fxdata(fpath.c_str(), Stream::MODE_INPUT_BINARY_ONMEMORY);
                if (fxdata.IsOpened()) {
                    MOELogD("Loadfx:[%s]",fpath.c_str());
                    prg->LoadFromMemory(static_cast<const s8*>(fxdata.GetData()));
                    m_ovprgs[sdr] = prg;
                }
            }
        }
    }
    void setparams(lua_State* L)
    {
        // set param
        lua_pushnumber(L, 1920);
        lua_setglobal(L, "screen_width");
        lua_pushnumber(L, 1080);
        lua_setglobal(L, "screen_height");
    }
    
    void createFromParams(lua_State* L)
    {
        createBuffers(g, L);
        createScenes(g, L);
        createProcesses(L);
        createRenderEffects(L);
        createOverridePrograms(g);
    }
    b8 loadLua(const s8* luafile)
    {
        Stream st(luafile, MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        
        if (!st.IsOpened())
            return false;
        
        m_respath = getResourcePath(luafile);
        
        lua_State* L = createLua();
        setparams(L);
        doLua(L, static_cast<const s8*>(st.GetData()));
        createFromParams(L);
        
        closeLua(L);
        return true;
    }

public:
    Impl(Graphics* mg) : g(mg)
    {
    }
    ~Impl()
    {
        Clear();
    }
   
    b8 Load(const s8* demolua)
    {
        Clear();
        return loadLua(demolua);
    }
    void Clear()
    {
        clearmap(m_buffers);
        clearmap(m_scenes);
        clearvector(m_processes);
        clearvector(m_renderEffects);
        clearmap(m_ovprgs);
    }
    void Update(f64 time)
    {
        std::map<std::string, Math::vec4> vec4s;
        const auto eit = m_processes.end();
        for (auto it = m_processes.begin(); it != eit; ++it)
        {
            ProcessInfo* pi = (*it);
            if (pi->demo_startTime <= time
            &&  pi->demo_endTime   >  time) {
                const f64 demospantime  = pi->demo_endTime - (*it)->demo_startTime;
                const f64 scenedirtime  = pi->scene_endTime - (*it)->scene_startTime;
                const f64 scenespantime = fabs(pi->scene_endTime - (*it)->scene_startTime);
                const f64 scenedir      = scenedirtime >= 0.0 ? 1.0 : -1.0;
                const f64 ltime = (time - pi->demo_startTime) * scenedir * scenespantime / demospantime + pi->scene_startTime;
                vec4s.clear();
                const auto peit = pi->shaderparam.end();
                for (auto pit = pi->shaderparam.begin(); peit != pit; ++pit)
                    vec4s[(*pit).m_name] = (*pit).m_val;
                pi->scene->Update(time, ltime, vec4s);
            }
        }
    }
    void Render(f64 time)
    {
        const auto eit = m_renderEffects.end();
        for (auto it = m_renderEffects.begin(); it != eit; ++it)
        {
            RenderEffectInfo* re = (*it);
            if (re->demo_startTime <= time
            &&  re->demo_endTime   >  time) {
                //const f64 demospantime  = (*it)->demo_endTime - (*it)->demo_startTime;
                const f32 effecttime  = static_cast<f32>(time - (*it)->demo_startTime);
                Scene* sc = re->scene;
                if (!sc)
                    continue;
                if (re->effectBuffer)
                    re->effectBuffer->RenderBegin();
                ProgramObject* pg = m_ovprgs[re->overrideShader];
                if (pg) {
                    pg->Bind();
                    int acttex = 1;
                    for (int p = 0; p < re->shaderparam.size(); ++p){
                        EffectBuffer* eb = re->shaderparam[p].m_eb;
                        if (eb) {
                            eb->BindTexture(acttex); // temporary binding
                            pg->SetUniform(re->shaderparam[p].m_name.c_str(), acttex);
                            ++acttex;
                        } else {
                            const Math::vec4& v = re->shaderparam[p].m_val;
                            pg->SetUniform(re->shaderparam[p].m_name.c_str(), v.x, v.y, v.z, v.w);
                        }
                        pg->SetUniform("time", effecttime);
                    }
                    pg->Unbind();
                }
                sc->Render(time, pg);
                if (re->effectBuffer)
                    re->effectBuffer->RenderEnd();
            }
        }
    }
    
    void SetMatrix(const s8* name, const Math::matrix4x4& mat)
    {
        m_render->SetUniform(name, mat);
    }
    
    void Resize(s32 w, s32 h)
    {
        const auto eit = m_scenes.end();
        for (auto it = m_scenes.begin(); it != eit; ++it)
        {
            if (it->second)
                it->second->Resize(w, h);
        }
    }
    
private:
    
    class ShaderParam
    {
    public:
        ShaderParam(const std::string& name, const Math::vec4& val){
            m_name = name;
            m_val = val;
            m_eb = 0;
        };
        ShaderParam(const std::string& name, EffectBuffer* eb){
            m_name = name;
            m_val = Math::vec4(0,0,0,0);
            m_eb = eb;
        };

        std::string m_name;
        Math::vec4 m_val;
        EffectBuffer* m_eb;
    };
    
    class ProcessInfo
    {
    public:
        ProcessInfo(f64 demo_st, f64 demo_et, f64 scene_st, f64 scene_et, Scene* sc, std::vector<ShaderParam>& sp)
        {
            demo_startTime  = demo_st;
            demo_endTime    = demo_et;
            scene_startTime = scene_st;
            scene_endTime   = scene_et;
            scene = sc;
            shaderparam = sp;
        }
        ~ProcessInfo(){}
        f64 demo_startTime;
        f64 demo_endTime;
        f64 scene_startTime;
        f64 scene_endTime;
        Scene* scene;
        std::vector<ShaderParam> shaderparam;
    };
    class RenderEffectInfo
    {
    public:
        RenderEffectInfo(f64 demo_st, f64 demo_et, Scene* sc, EffectBuffer* eb,
                         const std::string& ovShader, const std::vector<ShaderParam>& sparams)
        {
            demo_startTime = demo_st;
            demo_endTime   = demo_et;
            scene = sc;
            effectBuffer = eb;
            overrideShader = ovShader;
            shaderparam = sparams;
        }
        ~RenderEffectInfo(){}
        f64 demo_startTime;
        f64 demo_endTime;
        Scene* scene;
        EffectBuffer* effectBuffer;
        std::string overrideShader;
        std::vector<ShaderParam> shaderparam;
    };
  
    Graphics* g;
    std::map<std::string, EffectBuffer*> m_buffers;
    std::map<std::string, Scene*> m_scenes;
    std::vector<ProcessInfo*> m_processes;
    std::vector<RenderEffectInfo*> m_renderEffects;
    SceneGraphRender* m_render;
    std::string m_respath;
    std::map<std::string, ProgramObject*> m_ovprgs;
};

// -----------------------------------------------------------------------------
Demo::Demo(Graphics* mg) : m_imp(new Impl(mg)) {}
Demo::~Demo() { delete m_imp; }
    
b8 Demo::Load(const s8* demolua) { return m_imp->Load(demolua); }
void Demo::Clear()               { m_imp->Clear();      }
void Demo::Update(f64 time)      { m_imp->Update(time); }
void Demo::Render(f64 time)      { m_imp->Render(time); }
void Demo::SetMatrix(const s8* name, const Math::matrix4x4& mat){ m_imp->SetMatrix(name,mat); }
void Demo::Resize(s32 w, s32 h)  { m_imp->Resize(w, h); }
} // MOE
