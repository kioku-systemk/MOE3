#include "../Core/MOE.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Graphics.h"
#include "ShaderProgramObject.h"

#define SHADER_STR(STR)      (std::string(shaderMacro)+(STR))
#define SHADER_HIGH_STR(STR) (std::string(shaderMacro_high)+(STR))


static const s8* shaderMacro =  "\
#ifdef GL_ES\n\
precision mediump float;\n\
#define LOWP lowp\n\
#define MEDIUMP mediump\n\
#define HIGHP highp\n\
#else\n\
#define LOWP\n\
#define MEDIUMP\n\
#define HIGHP\n\
#endif\n";

static const s8* shaderMacro_high =  "\
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
	s32 bufSize;
	g->GetShaderiv(shader, VG_INFO_LOG_LENGTH , &bufSize);
	if (bufSize > 1)
	{
		s8 *infoLog;
		infoLog = mnew s8[bufSize];
		if (infoLog != NULL)
		{
			s32 length;
			g->GetShaderInfoLog(shader, bufSize, &length, infoLog);
			//cout << "InfoLog:" << endl << infoLog << endl << endl;
			MOELogE("InfoLog:\n%s\n",infoLog);
			mdeleteArray(infoLog);
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
		infoLog = mnew s8[bufSize];
		if (infoLog != NULL)
		{
			s32 length;
			g->GetProgramInfoLog(program, bufSize, &length, infoLog);
			//cout << "InfoLog:" << endl << infoLog << endl << endl;
			MOELogE("InfoLog:\n%s\n",infoLog);
			mdeleteArray(infoLog);
		}
		else
		{
			//cout << "Could not allocate InfoLog buffer." << endl;
			MOELogE("Could not allocate InfoLog buffer.");
		}
	}
}

};// namespace


namespace MOE {
	
ShaderObject::ShaderObject(Graphics* mg, bool float_highprofile)
{
	g = mg;
	m_shader = 0;
	m_highprofile = float_highprofile;
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
	if (m_highprofile)
		prgSource = SHADER_HIGH_STR(programSource);
	else
		prgSource = SHADER_STR(programSource);
	
	m_source = prgSource;

	const char* s = prgSource.c_str();
	s32 l = static_cast<s32>(prgSource.length());
	
	m_shader = g->CreateShader(shaderType);
	g->ShaderSource( m_shader, 1, &s, &l );
	if ( g->GetError() != VG_NO_ERROR )
	{
		//cout << "cannot set shader source: " << prgSource << endl;
		MOELogE("cannot set shader source: %s", prgSource.c_str());
		return false;
	}

	// compile
	s32 compiled = 0;
	g->CompileShader(m_shader);
	g->GetShaderiv(m_shader, VG_COMPILE_STATUS, &compiled);
	printShaderInfoLog(g, m_shader);
	if (!compiled)
	{
		//cout << "Compile is failed" << endl;
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

void ProgramObject::Bind()
{
	g->GetIntegerv(VG_CURRENT_PROGRAM, &m_oldProgram); 	
	g->UseProgram(m_program);
	m_binding = true;
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
void ProgramObject::SetUniform(const char* name, const s32 i0)
{
	if (m_program)
		g->Uniform1i(g->GetUniformLocation(m_program, name), i0);
}

void ProgramObject::SetUniform(const char* name, const s32 i0, const s32 i1)
{
	if (m_program)
		g->Uniform2i(g->GetUniformLocation(m_program, name), i0, i1);
}

void ProgramObject::SetUniform(const char* name, const s32 i0, const s32 i1, const s32 i2)
{
	if (m_program)
		g->Uniform3i(g->GetUniformLocation(m_program, name), i0, i1, i2);
}

void ProgramObject::SetUniform(const char* name, const s32 i0, const s32 i1, const s32 i2, const s32 i3)
{
	if (m_program)
		g->Uniform4i(g->GetUniformLocation(m_program, name), i0, i1, i2, i3);
}

void ProgramObject::SetUniform(const char* name, const s32 num, const s32* i_array)
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
void ProgramObject::SetUniform(const char* name, const f32 f0)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform1f(loc, f0);
	}
}

void ProgramObject::SetUniform(const char* name, const f32 f0, const f32 f1)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform2f(loc, f0, f1);
	}
}

void ProgramObject::SetUniform(const char* name, const f32 f0, const f32 f1, const f32 f2)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform3f(loc, f0, f1, f2);
	}
}

void ProgramObject::SetUniform(const char* name, const f32 f0, const f32 f1, const f32 f2, const f32 f3)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform4f(loc, f0, f1, f2, f3);
	}
}

void ProgramObject::SetUniform(const char* name, const s32 num, const f32* f_array)
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
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			(g->*UniFuncs[num])(loc, num, f_array);
	}
}
	
void ProgramObject::SetUniformArray(const char* name, const s32 num, const f32* f_array)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform1fv(loc, num, f_array);
	}
}

void ProgramObject::SetUniformArray4f(const char* name, const s32 num, const f32* f_array)
{
	if (m_program)
	{
		const s32 loc = g->GetUniformLocation(m_program, name);
		if (loc >= 0)
			g->Uniform4fv(loc, num, f_array);
	}
}

// matrix
void ProgramObject::SetUniformMatrix2x2(const char* name, const s32 count, const bool transpose, const f32* val)
{
	if (m_program)
		g->UniformMatrix2fv(g->GetUniformLocation(m_program, name), count, transpose, val);
}
void ProgramObject::SetUniformMatrix3x3(const char* name, const s32 count, const bool transpose, const f32* val)
{
	if (m_program)
		g->UniformMatrix3fv(g->GetUniformLocation(m_program, name), count, transpose, val);
}
void ProgramObject::SetUniformMatrix4x4(const char* name, const s32 count, const bool transpose, const f32* val)
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

	

} // namespace MOE
