#include "../Core/MOE.h"
#include "Demo.h"
#include "Scene.h"
#include "EffectBuffer.h"
#include "../Core/Stream.h"
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "../Gfx/Graphics.h"
#include "../Gfx/SceneGraphRender.h"
#include "../Gfx/ShaderProgramObject.h"
#include "../Core/LuaUtil.h"
#include "../Core/Time.h"
#include "../Core/KdbImporterExporter.h"
#include "Sound.h"

const s32 exportFPS = 30;

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
            const f64 sc_start = eval<f64>(L, "return Process[%d].scenetime[1]",i+1);
            const f64 sc_end   = eval<f64>(L, "return Process[%d].scenetime[2]",i+1);
            const f64 dt_start = eval<f64>(L, "return Process[%d].demotime[1]",i+1);
            const f64 dt_end   = eval<f64>(L, "return Process[%d].demotime[2]",i+1);
            std::string camname= eval<std::string>(L, "return Process[%d].camera",i+1);
            if (camname == "")
                camname = "Camera";
            
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
                v.x = atof(sp[0].c_str());
                v.y = atof(sp[1].c_str());
                v.z = atof(sp[2].c_str());
                v.w = atof(sp[3].c_str());
                params.push_back(ShaderParam(it->first, v));
            }

            if (scene != "") {
                ProcessInfo* pi = mnew ProcessInfo(dt_start,dt_end,sc_start, sc_end, m_scenes[scene],params,camname);
                m_processes.push_back(pi);
            }
        }
    }
    
    double createRenderEffects(lua_State* L)
    {
        double maxendtime = 0;
        std::map<std::string,std::string> vals;
        const int rendernum = getTableNum(L, "Render");
        for (int i = 0; i < rendernum; ++i)
        {
            const std::string src  = eval<std::string>(L, "return Render[%d].src",i+1);
            const std::string target = eval<std::string>(L, "return Render[%d].target[1]",i+1);
            std::string shader = "default";
            shader = eval<std::string>(L, "return Render[%d].shader",i+1);
            const f64 dt_start = eval<f64>(L, "return Render[%d].demotime[1]",i+1);
            const f64 dt_end   = eval<f64>(L, "return Render[%d].demotime[2]",i+1);
            if (maxendtime < dt_end)
                maxendtime = dt_end;
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
                v.x = atof(sp[0].c_str());
                v.y = atof(sp[1].c_str());
                v.z = atof(sp[2].c_str());
                v.w = atof(sp[3].c_str());
                sparams.push_back(ShaderParam(it->first, v));
            }

            if (src != "" && target != "") {
                RenderEffectInfo* re = mnew RenderEffectInfo(dt_start, dt_end, m_scenes[src], m_buffers[target], shader, sparams);
                m_renderEffects.push_back(re);
            }
        }
        return maxendtime;
    }
    void createOverridePrograms(Graphics* g)
    {
        const auto eit = m_renderEffects.end();
        for (auto it = m_renderEffects.begin(); it != eit; ++it)
        {
            const std::string sdr = (*it)->overrideShader;
            if (sdr != "" && sdr != "default" && !m_ovprgs[sdr])
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
        lua_pushnumber(L, m_width);
        lua_setglobal(L, "screen_width");
        lua_pushnumber(L, m_height);
        lua_setglobal(L, "screen_height");
    }
    
    void createFromParams(lua_State* L)
    {
        createBuffers(g, L);
        createScenes(g, L);
        createProcesses(L);
        m_demoalltime = createRenderEffects(L);
        createOverridePrograms(g);
    }
    void loadSound(lua_State* L)
    {
        std::string sfile = eval<std::string>(L, "return soundfile");
        if (sfile != "") {
            m_sound = new Sound();
            std::string spath = m_respath + sfile;
            if (!m_sound->Load(spath.c_str())) {
                MOELogE("Load failed sound : %s\n",spath.c_str());
                delete m_sound;
                m_sound = 0;
            } else {
                MOELogI("Load sound : %s\n",spath.c_str());
            }
            
            assert(!m_ffthist);
            const s32 fnum = static_cast<s32>(GetDemoTime() * exportFPS);
            m_ffthist = mnew MOE::Math::vec4[fnum];
            m_ffthistnum = fnum;
            memset(m_ffthist, 0, m_ffthistnum*sizeof(MOE::Math::vec4));
        }
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
        
        loadSound(L);
        
        closeLua(L);
        
        Update(0);
        return true;
    }

public:
    Impl(Graphics* mg) : g(mg)
    {
        m_width  = 1920;
        m_height = 1080;
        m_demoalltime = 0;
        m_sound = 0;
        m_starttime = 0;
        m_playtime = 0;
        m_playing = false;
        m_fftvec = MOE::Math::vec4(0,0,0,0);
        m_ffthist = 0;
        m_ffthistnum = 0;
    }
    ~Impl()
    {
        Clear();
    }
    
    f64 GetDemoTime() const
    {
        return m_demoalltime;
    }
    
    f64 GetTime()
    {
        if (!m_sound) {
            if (m_playing)
                m_playtime = GetTimeCount() - m_starttime;
            return m_playtime;
        }else{
            return m_sound->GetPosTime();
        }
    }
   
    b8 Play()
    {
        if (m_playing)
            return true;
        m_playing = true;
        if (!m_sound)
            return false;
        m_starttime = GetTimeCount() - m_playtime;
        m_sound->Play();
        return true;
    }
    b8 IsPlaying() const
    {
        return m_playing;
    }
    b8 Stop()
    {
        if (!m_playing)
            return true;
        m_playing = false;
        if (!m_sound)
            return false;
        m_sound->Pause();
        return true;
    }
    void SetTime(f64 tm)
    {
        if (m_sound){
            m_sound->SetPosTime(tm);
        } else {
            m_starttime = GetTimeCount() - tm;
            m_playtime = tm;
        }
    }
    b8 Export(const s8* packfile)
    {
        // src base path -> m_respath

        KdbExporter exporter;
        return exporter.Export(packfile, m_respath);
    }
    b8 ExportFFTHistory(const s8* histfile)
    {
        FILE* fp = fopen(histfile, "wb");
        if (!fp)
            return false;
        
        fwrite(&m_ffthistnum, sizeof(s32), 1, fp);
        fwrite(m_ffthist, sizeof(MOE::Math::vec4)*m_ffthistnum, 1, fp);
        fclose(fp);
        return true;
    }
    b8 LoadFFTHistory(const s8* histfile)
    {
        FILE* fp = fopen(histfile, "rb");
        if (!fp)
            return false;
        
        fread(&m_ffthistnum, sizeof(s32), 1, fp);
        m_ffthist = mnew MOE::Math::vec4[m_ffthistnum];
        fread(m_ffthist, sizeof(MOE::Math::vec4)*m_ffthistnum, 1, fp);
        fclose(fp);
        return true;
    }
    
    b8 Load(const s8* demolua)
    {
        Clear();
        return loadLua(demolua);
    }
    void Clear()
    {
        delete m_sound; m_sound = 0;
        delete [] m_ffthist; m_ffthist = 0;
        m_ffthistnum = 0;
        clearmap(m_buffers);
        clearmap(m_scenes);
        clearvector(m_processes);
        clearvector(m_renderEffects);
        clearmap(m_ovprgs);
    }
    void Update(f64 time)
    {
        const f32* fft = 0;
        if (m_sound) {
            fft = m_sound->GetFFTData();
            const s32 ftm = static_cast<s32>(time * exportFPS);
            if (m_sound->IsPlaying()) {
                m_fftvec = MOE::Math::vec4(fft[8],fft[16],fft[32], fft[64]); // chois freq.
                if (ftm < m_ffthistnum)
                    m_ffthist[ftm] = m_fftvec;
            } else {
                if (ftm < m_ffthistnum)
                    m_fftvec = m_ffthist[ftm];
            }
            //printf("%.3f,%.3f,%.3f,%.3f\n",fftvec.x,fftvec.y,fftvec.z,fftvec.w);
        }
        std::map<std::string, Math::vec4> vec4s;
        const auto eit = m_processes.end();
        for (auto it = m_processes.begin(); it != eit; ++it)
        {
            ProcessInfo* pi = (*it);
            if (pi->demo_startTime <= time
            &&  pi->demo_endTime   >  time
            &&  pi->scene) {
                const f64 demospantime  = pi->demo_endTime - (*it)->demo_startTime;
                const f64 scenedirtime  = pi->scene_endTime - (*it)->scene_startTime;
                const f64 scenespantime = fabs(pi->scene_endTime - (*it)->scene_startTime);
                const f64 scenedir      = scenedirtime >= 0.0 ? 1.0 : -1.0;
                const f64 ltime = (time - pi->demo_startTime) * scenedir * scenespantime / demospantime + pi->scene_startTime;
                vec4s.clear();
                if (fft)
                    pi->scene->SetUniform("fft", m_fftvec);
                const auto peit = pi->shaderparam.end();
                for (auto pit = pi->shaderparam.begin(); peit != pit; ++pit)
                    vec4s[(*pit).m_name] = (*pit).m_val;
                pi->scene->Update(time, ltime, vec4s, pi->cameraname);
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
                if (re->effectBuffer)
                    re->effectBuffer->RenderBegin();
                else // backbuffer
                    g->Viewport(0, 0, m_width, m_height);
                
                Scene* sc = re->scene;
                if (!sc) { // clear
                    for (int p = 0; p < re->shaderparam.size(); ++p){
                        if (re->shaderparam[p].m_name == "color")
                            g->ClearColor(re->shaderparam[p].m_val.x,
                                          re->shaderparam[p].m_val.y,
                                          re->shaderparam[p].m_val.z,
                                          re->shaderparam[p].m_val.w);
                        if (re->shaderparam[p].m_name == "flag")
                            g->Clear(static_cast<int>(re->shaderparam[p].m_val.x) * GL_COLOR_BUFFER_BIT
                                    |static_cast<int>(re->shaderparam[p].m_val.y) * GL_DEPTH_BUFFER_BIT);
                    }
                } else {
                    ProgramObject* pg = m_ovprgs[re->overrideShader];
                    if (pg) {
                        pg->Bind();
                        int acttex = 1;
                        for (int p = 0; p < re->shaderparam.size(); ++p){
                            EffectBuffer* eb = re->shaderparam[p].m_eb;
                            if (eb) {
                                 // temporary binding
                                const int colortex = 2*acttex;
                                const int depthtex = 2*acttex+1;
                                eb->BindTexture(colortex);
                                eb->BindDepthTexture(depthtex);
                                eb->BindEnd();// dasai :(
                                pg->SetUniform(re->shaderparam[p].m_name.c_str(), colortex);
                                pg->SetUniform((re->shaderparam[p].m_name+"_depth").c_str(), depthtex);
                                ++acttex;
                            } else {
                                const Math::vec4& v = re->shaderparam[p].m_val;
                                pg->SetUniform(re->shaderparam[p].m_name.c_str(), v.x, v.y, v.z, v.w);
                            }
                        }
                        pg->SetUniform("time", effecttime,effecttime,effecttime,effecttime);
                        auto eit = m_vec4s.end();
                        for (auto it = m_vec4s.begin(); it != eit; ++it)
                        {
                            const MOE::Math::vec4& v4 = it->second;
                            pg->SetUniform(it->first.c_str(), v4.x,v4.y,v4.z,v4.w);
                        }
                        pg->Unbind();
                    }
                    sc->Render(time, pg);
                }
                if (re->effectBuffer)
                    re->effectBuffer->RenderEnd();
            }
        }
    }
    
    b8 Cache()
    {
        /*const auto eit = m_processes.end();
        for (auto it = m_processes.begin(); it != eit; ++it)
        {
            ProcessInfo* pi = (*it);
        }*/
        const auto eit = m_renderEffects.end();
        for (auto it = m_renderEffects.begin(); it != eit; ++it)
        {
            RenderEffectInfo* re = (*it);
            Update(re->demo_startTime);
            Render(re->demo_startTime);
        }
        Update(0);
        Render(0);
        
        return true;
    }
    
    void SetMatrix(const s8* name, const Math::matrix4x4& mat)
    {
        assert(0);// TODO:
    }
    void SetVec4(const s8* name, const Math::vec4& vec)
    {
        m_vec4s[name] = vec;
    }
    
    void ClearUniforms()
    {
        m_vec4s.clear();
    }
    
    void Resize(s32 w, s32 h)
    {
        m_width  = w;
        m_height = h;
        const auto eit = m_scenes.end();
        for (auto it = m_scenes.begin(); it != eit; ++it)
        {
            if (it->second)
                it->second->Resize(w, h);
        }
    }
    
    void ReloadEffectShaders()
    {
        clearmap(m_ovprgs);
        createOverridePrograms(g);
    }
    
    s32 GetProcessNum() const
    {
        return static_cast<s32>(m_processes.size());
    }
    s32 GetRenderEffectNum() const
    {
        return static_cast<s32>(m_renderEffects.size());
    }
    b8 GetProcessScene(s32 pi, std::string& scenename, f64& start_demotm, f64& end_demotm, f64& start_scntm, f64& end_scntm) const
    {
        if (pi < 0 || GetProcessNum() <= pi)
            return false;
        
        start_demotm = m_processes[pi]->demo_startTime;
        end_demotm   = m_processes[pi]->demo_endTime;
        start_scntm  = m_processes[pi]->scene_startTime;
        end_scntm    = m_processes[pi]->scene_endTime;
        scenename    = m_processes[pi]->scene->GetName();
        std::string camname = m_processes[pi]->cameraname;
        scenename += std::string(":") + camname;
        return true;
    }
    b8 GetRenderEffect(s32 ri, std::string& scenename, f64& start_demotm, f64& end_demotm, s32& texid, std::string& ovshader) const
    {
        if (ri < 0 || GetRenderEffectNum() <= ri)
            return false;

        start_demotm = m_renderEffects[ri]->demo_startTime;
        end_demotm   = m_renderEffects[ri]->demo_endTime;
        texid        = 0;
        if (m_renderEffects[ri]->effectBuffer)
            texid = m_renderEffects[ri]->effectBuffer->GetTextureID();
        ovshader     = m_renderEffects[ri]->overrideShader;
        scenename = "clear";
        if (m_renderEffects[ri]->scene)
            scenename    = m_renderEffects[ri]->scene->GetName();
        scenename += std::string(":") + ovshader;
        return true;
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
        ProcessInfo(f64 demo_st, f64 demo_et, f64 scene_st, f64 scene_et, Scene* sc, std::vector<ShaderParam>& sp, const std::string& camname)
        {
            demo_startTime  = demo_st;
            demo_endTime    = demo_et;
            scene_startTime = scene_st;
            scene_endTime   = scene_et;
            scene = sc;
            shaderparam = sp;
            cameraname = camname;
        }
        ~ProcessInfo(){}
        f64 demo_startTime;
        f64 demo_endTime;
        f64 scene_startTime;
        f64 scene_endTime;
        std::string cameraname;
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
    Sound* m_sound;
    b8 m_playing;
    s32 m_width, m_height;
    f64 m_starttime;
    f64 m_playtime;
    f64 m_demoalltime;
    MOE::Math::vec4 m_fftvec;
    MOE::Math::vec4* m_ffthist;
    s32 m_ffthistnum;
    std::map<std::string, EffectBuffer*> m_buffers;
    std::map<std::string, Scene*> m_scenes;
    std::vector<ProcessInfo*> m_processes;
    std::vector<RenderEffectInfo*> m_renderEffects;
    std::string m_respath;
    std::map<std::string, ProgramObject*> m_ovprgs;
    std::map<std::string, MOE::Math::vec4> m_vec4s;
};

// -----------------------------------------------------------------------------
Demo::Demo(Graphics* mg) : m_imp(new Impl(mg)) {}
Demo::~Demo() { delete m_imp; }
    
b8 Demo::Load(const s8* demolua)    { return m_imp->Load(demolua); }
b8 Demo::Export(const s8* packfile) { return m_imp->Export(packfile); }
void Demo::ReloadEffectShaders() { m_imp->ReloadEffectShaders(); }
void Demo::Clear()               { m_imp->Clear();      }
void Demo::Update(f64 time)      { m_imp->Update(time); }
void Demo::Render(f64 time)      { m_imp->Render(time); }
void Demo::SetMatrix(const s8* name, const Math::matrix4x4& mat){ m_imp->SetMatrix(name,mat); }
void Demo::SetVec4(const s8* name, const Math::vec4& vec){ m_imp->SetVec4(name,vec); }
void Demo::ClearUniforms()       { m_imp->ClearUniforms(); }
void Demo::Resize(s32 w, s32 h)  { m_imp->Resize(w, h); }
f64 Demo::GetDemoTime() const    { return m_imp->GetDemoTime(); }
f64 Demo::GetTime()              { return m_imp->GetTime(); }
b8 Demo::Play()                  { return m_imp->Play(); }
b8 Demo::IsPlaying() const       { return m_imp->IsPlaying(); }
b8 Demo::Stop()                  { return m_imp->Stop(); }
b8 Demo::Cache()                 { return m_imp->Cache(); }
void Demo::SetTime(f64 tm)       { return m_imp->SetTime(tm); }
    s32 Demo::GetProcessNum() const { return m_imp->GetProcessNum(); }
    s32 Demo::GetRenderEffectNum() const { return m_imp->GetRenderEffectNum(); }
    b8 Demo::GetProcessScene(s32 pi, std::string& scenename,
                             f64& start_demotm, f64& end_demotm, f64& start_scntm, f64& end_scntm) const
    { return m_imp->GetProcessScene(pi,scenename,start_demotm,end_demotm, start_scntm, end_scntm); }
    b8 Demo::GetRenderEffect(s32 ri, std::string& scenename, f64& start_demotm, f64& end_demotm,
                             s32& texid, std::string& ovshader) const
    { return m_imp->GetRenderEffect(ri, scenename, start_demotm,end_demotm, texid, ovshader); }

    
} // MOE
