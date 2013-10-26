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

extern "C" {
    #include <lua/lua.h>
    #include <lua/lualib.h>
    #include <lua/lauxlib.h>
}

namespace {
    template<typename T>
    void clearmap(T& buffers)
    {
        auto eit = buffers.end();
        for (auto it = buffers.begin(); it != eit; ++it)
            delete it->second;
        buffers.clear();
    }
    template<typename T>
    void clearvector(T& buffers)
    {
        auto eit = buffers.end();
        for (auto it = buffers.begin(); it != eit; ++it)
            delete (*it);
        buffers.clear();
    }

    template<typename T> T luaX_Cast(lua_State* L, int i) { return lua_tonumber(L, i); }
    template<> std::string luaX_Cast<std::string>(lua_State* L, int i) { return std::string(lua_tostring(L, i)); }
    template<typename T>
    T eval(lua_State* L, const char* str, ...)
    {
        char buf[128] = {};
        
        va_list args;
        va_start(args, str);
        vsprintf(buf, str, args);
        va_end(args);
        
        luaL_dostring(L, buf);
        T val = luaX_Cast<T>(L, -1);
        lua_pop(L, 1);
        return val;
    }
    
    void dumpStack(lua_State* L)
    {
        const int stackSize = lua_gettop(L);
        for(int i = stackSize; i >= 1; i-- ) {
            int type = lua_type(L, i);
            printf("Stack[%2d-%10s] : ", i, lua_typename(L,type) );
            switch( type ) {
                case LUA_TNUMBER:  printf("%f", lua_tonumber(L, i) ); break;
                case LUA_TBOOLEAN: printf("%s", lua_toboolean(L, i) ? "true" : "false"); break;
                case LUA_TSTRING:  printf("%s", lua_tostring(L, i) ); break;
                case LUA_TNIL:     break;
                default:           printf("%s", lua_typename(L, type)); break;
            }
            printf("\n");
        }
        printf("\n");
    }
    int getTableNum(lua_State* L, const char* tablename)
    {
        std::string buf = std::string("local n = 0; for i,v in pairs(")+std::string(tablename)+std::string(") do n=n+1 end return n;");
        luaL_dostring(L,buf.c_str());
        int n = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return n;
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
            if (scene != "") {
                ProcessInfo* pi = mnew ProcessInfo(dt_start,dt_end,sc_start, sc_end, m_scenes[scene]);
                m_processes.push_back(pi);
            }
        }
    }
    
    void createRenderEffects(lua_State* L)
    {
        const int rendernum = getTableNum(L, "Render");
        for (int i = 0; i < rendernum; ++i)
        {
            const std::string src  = eval<std::string>(L, "return Render[%d].src",i+1);
            const std::string target = eval<std::string>(L, "return Render[%d].target[1]",i+1);
            std::string shader = "default";
            shader = eval<std::string>(L, "return Render[%d].shader",i+1);
            const int dt_start = eval<int>(L, "return Render[%d].demotime[1]",i+1);
            const int dt_end   = eval<int>(L, "return Render[%d].demotime[2]",i+1);
            if (src != "" && target != "") {
                RenderEffectInfo* re = mnew RenderEffectInfo(dt_start, dt_end, m_scenes[src], m_buffers[target], shader);
                m_renderEffects.push_back(re);
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
    }
    b8 loadLua(const s8* luafile)
    {
        Stream st(luafile, MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
        
        if (!st.IsOpened())
            return false;
        
        m_respath = getResourcePath(luafile);
        
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        
        setparams(L);
        int r = luaL_dostring(L, static_cast<const s8*>(st.GetData()));
        if (r){
            const char *err = lua_tostring(L, -1);
            printf("ERROR: %s\n", err);
            return false;
        }
        createFromParams(L);
        
        lua_close(L);
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
    }
    void Update(f64 time)
    {
        auto eit = m_processes.end();
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
                pi->scene->Update(time, ltime);
            }
        }
    }
    void Render(f64 time)
    {
        auto eit = m_renderEffects.end();
        for (auto it = m_renderEffects.begin(); it != eit; ++it)
        {
            RenderEffectInfo* re = (*it);
            if (re->demo_startTime <= time
            &&  re->demo_endTime   >  time) {
                //const f64 demospantime  = (*it)->demo_endTime - (*it)->demo_startTime;
                Scene* sc = re->scene;
                if (!sc)
                    continue;
                sc->Render(time);
            }
        }
    }
    
    void SetMatrix(const s8* name, const Math::matrix4x4& mat)
    {
        m_render->SetUniform(name, mat);
    }
    
    void Resize(s32 w, s32 h)
    {
        auto eit = m_scenes.end();
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
        std::string name;
        f64 val;
    };
    
    class ProcessInfo
    {
    public:
        ProcessInfo(f64 demo_st, f64 demo_et, f64 scene_st, f64 scene_et, Scene* sc)
        {
            demo_startTime  = demo_st;
            demo_endTime    = demo_et;
            scene_startTime = scene_st;
            scene_endTime   = scene_et;
            scene = sc;
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
        RenderEffectInfo(f64 demo_st, f64 demo_et, Scene* sc, EffectBuffer* eb, const std::string& ovShader)
        {
            demo_startTime = demo_st;
            demo_endTime   = demo_et;
            scene = sc;
            effectBuffer = eb;
            overrideShader = ovShader;
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
