#include "../Core/MOE.h"
#include "ShaderProgramObject.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
//#include <stdlib.h>
//#include <stdio.h>

#include "Graphics.h"
#include "../Core/Log.h"

#include "../Core/LuaUtil.h"

#define SHADER_STR(STR)      (std::string(shaderMacro)+(STR))
#define SHADER_HIGH_STR(STR) (std::string(shaderMacro_high)+(STR))

#define GLHEADER "#version 120\n"

static const s8* shaderMacro_high = GLHEADER "\
#ifdef GL_ES\n\
precision highp float;\n\
#define LOWP lowp\n\
#define MEDIUMP mediump\n\
#define HIGHP highp\n\
#else\n\
#define LOWP\n\
#define MEDIUMP\n\
#define HIGHP\n\
#endif\n";


using namespace std;

namespace
{

// シェーダの情報を表示する
void printShaderInfoLog(MOE::Graphics* g, GLuint shader)
{
	s32 bufSize = 0;
	g->GetShaderiv(shader, VG_INFO_LOG_LENGTH , &bufSize);
	if (bufSize > 1)
	{
		s8 *infoLog;
		infoLog = new s8[bufSize];
		if (infoLog != NULL)
		{
			s32 length;
			g->GetShaderInfoLog(shader, bufSize, &length, infoLog);
			//cout << "InfoLog:" << endl << infoLog << endl << endl;
			MOELogE("InfoLog:\n%s\n",infoLog);
			delete [] infoLog;
		}
		else
		{
			//cout << "Could not allocate InfoLog buffer." << endl;
			MOELogE("Could not allocate InfoLog buffer.");
		}
	}
}

// プログラムの情報を表示する
void printProgramInfoLog(MOE::Graphics* g, GLuint program)
{
	s32 bufSize;
	g->GetProgramiv(program, VG_INFO_LOG_LENGTH , &bufSize);
	if (bufSize > 1)
	{
		s8 *infoLog;
		infoLog = new s8[bufSize];
		if (infoLog != NULL)
		{
			s32 length;
			g->GetProgramInfoLog(program, bufSize, &length, infoLog);
			//cout << "InfoLog:" << endl << infoLog << endl << endl;
			MOELogE("InfoLog:\n%s\n",infoLog);
			delete [] infoLog;
		}
		else
		{
			//cout << "Could not allocate InfoLog buffer." << endl;
			MOELogE("Could not allocate InfoLog buffer.");
		}
	}
}

    
    
    struct StrVal {
        const s8* str;
        u32 val;
    };
    const StrVal blendFunc_tables [] = {
        {"ZERO", 0},       {"ONE",  1},
        {"SRC_COLOR"          , VG_SRC_COLOR},
        {"ONE_MINUS_SRC_COLOR", VG_ONE_MINUS_SRC_COLOR},
        {"SRC_ALPHA"          , VG_SRC_ALPHA},
        {"ONE_MINUS_SRC_ALPHA", VG_ONE_MINUS_SRC_ALPHA},
        {"DST_ALPHA"          , VG_DST_ALPHA},
        {"ONE_MINUS_DST_ALPHA", VG_ONE_MINUS_DST_ALPHA},
        {"DST_COLOR"          , VG_DST_COLOR},
        {"ONE_MINUS_DST_COLOR", VG_ONE_MINUS_DST_COLOR},
        {"SRC_ALPHA_SATURATE" , VG_SRC_ALPHA_SATURATE},
        {"CONSTANT_COLOR"          , VG_CONSTANT_COLOR},
        {"ONE_MINUS_CONSTANT_COLOR", VG_ONE_MINUS_CONSTANT_COLOR},
        {"CONSTANT_ALPHA"          , VG_CONSTANT_ALPHA},
        {"ONE_MINUS_CONSTANT_ALPHA", VG_ONE_MINUS_CONSTANT_ALPHA},
        {0,0}
    };
    
    const StrVal cullMode_tables [] = {
        {"ALL",   VG_FRONT_AND_BACK},
        {"BACK",  VG_BACK},
        {"FRONT", VG_FRONT},
        {0,0}
    };
    
    const StrVal faceMode_tables [] = {
        {"CW",  VG_CW},
        {"CCW", VG_CCW},
        {0,0}
    };
    b8 getLuaEnumValue(lua_State* L, const char* varname, u32& val, const StrVal* tables)
    {
        b8 ret = true;
        lua_getglobal(L, varname);
        if ( lua_isstring(L, -1) ){
            std::string sv(lua_tostring(L, -1));
            s32 i = 0;
            while (tables[i].str){
                if (std::string(tables[i].str) == sv){
                    val = tables[i].val;
                    break;
                }
                ++i;
            }
        } else {
            ret = false;
        }
        lua_pop(L, 1);
        return ret;
    }
    
};// namespace


namespace MOE {
	
ShaderObject::ShaderObject(Graphics* mg)
{
	g = mg;
	m_shader = 0;
}

ShaderObject::~ShaderObject()
{
	Release();
}


bool ShaderObject::LoadFromFile(const std::string& filename, SHADERTYPE shaderType)
{
	std::ifstream fin(filename.c_str(), ios::binary);
	if (fin.fail())
	{
		//cout << "cannot open shader file: " << filename << endl;
		MOELogE("cannot open shader file: %s", filename.c_str());
		return false;
	}
	std::ostringstream str_out;
	str_out << fin.rdbuf();
	std::string fileBuf = str_out.str();
	fin.close();
	return LoadFromMemory(fileBuf, shaderType);
}

bool ShaderObject::LoadFromMemory(const std::string& programSource, SHADERTYPE shaderType)
{
	std::string prgSource;
	prgSource = SHADER_HIGH_STR(programSource);
	
	m_source = prgSource;

	const char* s = prgSource.c_str();
	int l = static_cast<int>(prgSource.length());
	
	m_shader = g->CreateShader(shaderType);
	g->ShaderSource( m_shader, 1, &s, &l );
	if ( g->GetError() != VG_NO_ERROR )
	{
		//cout << "cannot set shader source: " << prgSource << endl;
		MOELogE("cannot set shader source: %s", prgSource.c_str());
		return false;
	}

	// compile
	int compiled = 0;
	g->CompileShader(m_shader);
	g->GetShaderiv(m_shader, VG_COMPILE_STATUS, &compiled);
	printShaderInfoLog(g, m_shader);
	if (!compiled)
	{
		MOELogE("Compile is failed");
		return false;
	}

	return true;
}

void ShaderObject::Release()
{
	if (m_shader)
	{
		g->DeleteShader(m_shader);
		m_shader = 0;
	}
}


ProgramObject::ProgramObject(Graphics* mg)
{
	g = mg;
	m_oldProgram = 0;
	m_program = 0;
	m_prio = 0;
	m_binding = false;
}

ProgramObject::ProgramObject(Graphics* mg, const ShaderObject& vertexShader, const ShaderObject& fragmentShader)
{
	g = mg;
	m_oldProgram = 0;
	m_program = 0;
	m_binding = false;
	Link(vertexShader, fragmentShader);
}

bool ProgramObject::Attach(const ShaderObject& vertexShader, const ShaderObject& fragmentShader)
{
	u32 program = g->CreateProgram();
	g->AttachShader(program, vertexShader.GetShader());
	g->AttachShader(program, fragmentShader.GetShader());
    m_program = program;
    return true;
}
    
bool ProgramObject::Attach(const ShaderObject& vertexShader, const ShaderObject& fragmentShader, const ShaderObject& geometryShader)
{
    u32 program = g->CreateProgram();
    g->AttachShader(program, vertexShader.GetShader());
    g->AttachShader(program, fragmentShader.GetShader());
    g->AttachShader(program, geometryShader.GetShader());
    m_program = program;
    return true;
}

bool ProgramObject::Link(const ShaderObject& vertexShader, const ShaderObject& fragmentShader)
{
    // program existence
    if (!m_program)
    {
        Attach(vertexShader, fragmentShader);
    }
    
	/* シェーダプログラムのリンク */
	g->LinkProgram(m_program);
	GLint linked;
	g->GetProgramiv(m_program, VG_LINK_STATUS, &linked);
	printProgramInfoLog(g, m_program);
	if (linked == VG_FALSE)
	{
		//cout << "Link error." << endl;
		MOELogE("Link error.");
		return false;
	}
	return true;
}

bool ProgramObject::Link(const ShaderObject& vertexShader, const ShaderObject& fragmentShader, const ShaderObject& geometryShader)
{
    // program existence
    if (!m_program)
    {
        Attach(vertexShader, fragmentShader, geometryShader);
    }
    
    /* シェーダプログラムのリンク */
    g->LinkProgram(m_program);
    GLint linked;
    g->GetProgramiv(m_program, VG_LINK_STATUS, &linked);
    printProgramInfoLog(g, m_program);
    if (linked == VG_FALSE)
    {
        //cout << "Link error." << endl;
        MOELogE("Link error.");
        return false;
    }
    return true;
}
    
bool ProgramObject::LoadFromMemory(const std::string &fxSource)
{
    lua_State* L = createLua();
    if (!doLua(L, fxSource.c_str())){
        closeLua(L);
        return false;
    }
    
    std::string vtxShader = eval<std::string>(L, "return VertexShader");
    std::string frgShader = eval<std::string>(L, "return FragmentShader");
    std::string geoShader = eval<std::string>(L, "return GeometryShader");
    
    int pri = 0;
    eval<int>(L, pri, "return Priority");
 	m_prio = pri;
    
    b8 depthtest = false;
    eval<b8>(L, depthtest, "return DepthTest");
    m_depthtest = depthtest;

    b8 blend = false;
    eval<b8>(L, blend, "return Blend");
    m_blend = blend;

    b8 cullface = true;
    eval<b8>(L, cullface, "return CullFace");
    m_cullface = cullface;

    b8 depthmask = false;
    eval<b8>(L, depthmask, "return DepthMask");
    m_depthmask = depthmask;

    m_blendfunc_src = VG_ONE;
	m_blendfunc_dst = VG_ZERO;
	u32 blendfunc_src = VG_SRC_ALPHA;
	u32 blendfunc_dst = VG_ONE_MINUS_SRC_ALPHA;
	if (getLuaEnumValue(L, "BlendSrc", blendfunc_src, blendFunc_tables))
		m_blendfunc_src = blendfunc_src;
	if (getLuaEnumValue(L, "BlendDst", blendfunc_dst, blendFunc_tables))
		m_blendfunc_dst = blendfunc_dst;

    m_cullmode = VG_BACK;
	u32 cullmode = VG_BACK;
	if (getLuaEnumValue(L, "CullMode", cullmode, cullMode_tables))
		m_cullmode = cullmode;

    m_frontface = VG_CCW;
	VGenum frontface = VG_CCW;
	if (getLuaEnumValue(L, "FrontFace", frontface, faceMode_tables))
		m_frontface = frontface;
    
    closeLua(L);
    
    ShaderObject vertexShader(g);
	ShaderObject fragmentShader(g);
    ShaderObject geometryShader(g);
	vertexShader.LoadFromMemory(vtxShader.c_str(), ShaderObject::VERTEX_SHADER);
	fragmentShader.LoadFromMemory(frgShader.c_str(), ShaderObject::FRAGMENT_SHADER);
    b8 r;
    if (geoShader != "")
    {
        geometryShader.LoadFromMemory(geoShader.c_str(), ShaderObject::GEOMETRY_SHADER);
        r = Link(vertexShader,fragmentShader, geometryShader);
    }
    else
    {
        r = Link(vertexShader,fragmentShader);
    }
    return r;
}

void ProgramObject::Bind()
{
	g->GetIntegerv(VG_CURRENT_PROGRAM, &m_oldProgram); 	
	g->UseProgram(m_program);
	m_binding = true;
    
    if (m_depthtest) g->Enable(VG_DEPTH_TEST);
	else             g->Disable(VG_DEPTH_TEST);
    
	if (m_blend) {
		g->Enable(VG_BLEND);
		g->BlendFunc(m_blendfunc_src, m_blendfunc_dst);
	} else {
		g->Disable(VG_BLEND);
	}
    
    if (m_cullface) {
		g->Enable(VG_CULL_FACE);
		g->CullFace(m_cullmode);
	} else {
		g->Disable(VG_CULL_FACE);
	}
	
	if (m_depthmask) g->DepthMask(VG_TRUE);
	else             g->DepthMask(VG_FALSE);
    
	g->FrontFace(m_frontface);
}

void ProgramObject::Unbind()
{
	g->UseProgram(m_oldProgram);
	m_binding = false;
}

void ProgramObject::Release()
{
	if (m_program)
	{
		g->DeleteProgram(m_program);
		m_program = 0;
	}
}

// 1i - 4i
void ProgramObject::SetUniform(const char* name, const int i0)
{
	if (m_program)
	{
		s32 loc = g->GetUniformLocation(m_program, name);
		if (loc == -1){
			return;
		}
		g->Uniform1i(loc, i0);
	}
}

void ProgramObject::SetUniform(const char* name, const int i0, const int i1)
{
	if (m_program)
		g->Uniform2i(g->GetUniformLocation(m_program, name), i0, i1);
}

void ProgramObject::SetUniform(const char* name, const int i0, const int i1, const int i2)
{
	if (m_program)
		g->Uniform3i(g->GetUniformLocation(m_program, name), i0, i1, i2);
}

void ProgramObject::SetUniform(const char* name, const int i0, const int i1, const int i2, const int i3)
{
	if (m_program)
		g->Uniform4i(g->GetUniformLocation(m_program, name), i0, i1, i2, i3);
}

void ProgramObject::SetUniform(const char* name, const int num, const int* i_array)
{
	void (Graphics::*UniFuncs[]) (s32, s32, const s32*) = 
	{
		0,
		&Graphics::Uniform1iv,
		&Graphics::Uniform2iv,
		&Graphics::Uniform3iv,
		&Graphics::Uniform4iv
	};
	if (num <= 0 || num >= 5)
		return;

	if (m_program)
		(g->*UniFuncs[num])(g->GetUniformLocation(m_program, name), num, i_array);
}

// 1f - 4f
void ProgramObject::SetUniform(const char* name, const float f0)
{
	if (m_program)
		g->Uniform1f(g->GetUniformLocation(m_program, name), f0);
}

void ProgramObject::SetUniform(const char* name, const float f0, const float f1)
{
	if (m_program)
		g->Uniform2f(g->GetUniformLocation(m_program, name), f0, f1);
}

void ProgramObject::SetUniform(const char* name, const float f0, const float f1, const float f2)
{
	if (m_program)
		g->Uniform3f(g->GetUniformLocation(m_program, name), f0, f1, f2);
}

void ProgramObject::SetUniform(const char* name, const float f0, const float f1, const float f2, const float f3)
{
	if (m_program)
		g->Uniform4f(g->GetUniformLocation(m_program, name), f0, f1, f2, f3);
}

void ProgramObject::SetUniform(const char* name, const int num, const float* f_array)
{
	void (Graphics::*UniFuncs[]) (s32, s32, const f32*) = 
	{
		0,
		&Graphics::Uniform1fv,
		&Graphics::Uniform2fv,
		&Graphics::Uniform3fv,
		&Graphics::Uniform4fv
	};
	if (num <= 0 || num >= 5)
		return;

	if (m_program)
		(g->*UniFuncs[num])(glGetUniformLocation(m_program, name), num, f_array);
}

// matrix
void ProgramObject::SetUniformMatrix2x2(const char* name, const int count, const bool transpose, const float* val)
{
	if (m_program)
		g->UniformMatrix2fv(g->GetUniformLocation(m_program, name), count, transpose, val);
}
void ProgramObject::SetUniformMatrix3x3(const char* name, const int count, const bool transpose, const float* val)
{
	if (m_program)
		g->UniformMatrix3fv(g->GetUniformLocation(m_program, name), count, transpose, val);
}
void ProgramObject::SetUniformMatrix4x4(const char* name, const int count, const bool transpose, const float* val)
{
	if (m_program)
		g->UniformMatrix4fv(g->GetUniformLocation(m_program, name), count, transpose, val);
}

void ProgramObject::BindAttribLocation(u32 index, const char* name)
{
	if (m_program)
		g->BindAttribLocation(m_program, index, name);
}
	
u32 ProgramObject::GetAttribLocation(const char* name)
{
	if (!m_program)
		return -1;
	return g->GetAttribLocation(m_program, name);
}

unsigned int ProgramObject::GetID() const
{
	return m_program;
}
unsigned int ProgramObject::GetPriority() const
{
	return m_prio;
}
void ProgramObject::SetPriority(unsigned int prio)
{
	m_prio = prio;
}

} // namespace MOE
