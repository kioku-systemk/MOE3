#ifndef INCLUDE_SHADERPROGRAMOBJECT_H
#define INCLUDE_SHADERPROGRAMOBJECT_H

#include "../Core/MOE.h"
#include <string>

#define SHADER_INLINE(A) #A

namespace MOE {
	
class Graphics;

class ShaderObject
{
public:
	enum SHADERTYPE
	{
		FRAGMENT_SHADER = 0x8B30, // GL_FRAGMENT_SHADER
		VERTEX_SHADER   = 0x8B31,  // GL_VERTEX_SHADER
        GEOMETRY_SHADER = 0x8DD9
	};
	ShaderObject(Graphics* mg);
	~ShaderObject();

	bool LoadFromFile(const std::string& filename, SHADERTYPE shaderType);
	bool LoadFromMemory(const std::string& programSource, SHADERTYPE shaderType);

	void Release();
	unsigned int GetShader() const
	{
		return m_shader;
	}

private:
	unsigned int m_shader;
	std::string	m_source;
	Graphics* g;
};

class ProgramObject
{
public:
	ProgramObject(Graphics* mg);
	ProgramObject(Graphics* mg, const ShaderObject& vertexShader, const ShaderObject& fragmentShader);

    bool LoadFromMemory(const std::string& fxSource);
    
    bool Attach(const ShaderObject& vertexShader, const ShaderObject& fragmentShader);
	bool Link(const ShaderObject& vertexShader, const ShaderObject& fragmentShader);
    bool Attach(const ShaderObject& vertexShader, const ShaderObject& fragmentShader, const ShaderObject& geometryShader);
	bool Link(const ShaderObject& vertexShader, const ShaderObject& fragmentShader, const ShaderObject& geometryShader);
	void Bind();
	void Unbind();

	void Release();

	// 1i - 4i
	void SetUniform(const char* name, const int i0);
	void SetUniform(const char* name, const int i0, const int i1);
	void SetUniform(const char* name, const int i0, const int i1, const int i2);
	void SetUniform(const char* name, const int i0, const int i1, const int i2, const int i3);
	void SetUniform(const char* name, const int num, const int* i_array);
	// 1f - 4f
	void SetUniform(const char* name, const float f0);
	void SetUniform(const char* name, const float f0, const float f1);
	void SetUniform(const char* name, const float f0, const float f1, const float f2);
	void SetUniform(const char* name, const float f0, const float f1, const float f2, const float f3);
	void SetUniform(const char* name, const int num, const float* f_array);
	// matrix
	void SetUniformMatrix2x2(const char* name, const int count, const bool transpose, const float* val);
	void SetUniformMatrix3x3(const char* name, const int count, const bool transpose, const float* val);
	void SetUniformMatrix4x4(const char* name, const int count, const bool transpose, const float* val);

	void BindAttribLocation(u32 index, const char* name);
	u32 GetAttribLocation(const char* name);

	unsigned int GetID() const;
	unsigned int GetPriority() const;
	void SetPriority(unsigned int prio);
	
private:
	unsigned int m_program;
	int m_oldProgram;
	bool m_binding;
	unsigned int m_prio;
	Graphics* g;
	
    // Effect
    b8  m_depthtest;
    b8  m_blend;
    u32 m_blendfunc_src;
    u32 m_blendfunc_dst;
    b8  m_cullface;
    u32 m_cullmode;
    b8  m_depthmask;
    u32 m_frontface;
    
};
	
} // namespace MOE

#endif // INCLUDE_SHADERPROGRAMOBJECT_H

