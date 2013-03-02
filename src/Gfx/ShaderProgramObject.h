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
		VERTEX_SHADER   = 0x8B31  // GL_VERTEX_SHADER
	};

	ShaderObject(Graphics* mg, bool float_highprofile = false);
	~ShaderObject();

	bool LoadFromFile(const std::string& filename, SHADERTYPE shaderType);
	bool LoadFromMemory(const std::string& programSource, SHADERTYPE shaderType);

	void Release();
	u32 GetShader() const
	{
		return m_shader;
	}

private:
	u32 m_shader;
	std::string	m_source;
	Graphics* g;
	
	bool m_highprofile;
	
};

class ProgramObject
{
public:
	ProgramObject(Graphics* mg);
	ProgramObject(Graphics* mg, const ShaderObject& vertexShader, const ShaderObject& fragmentShader);

    bool Attach(const ShaderObject& vertexShader, const ShaderObject& fragmentShader);
	bool Link(const ShaderObject& vertexShader, const ShaderObject& fragmentShader);
	void Bind();
	void Unbind();

	void Release();

	// 1i - 4i
	void SetUniform(const s8* name, const s32 i0);
	void SetUniform(const s8* name, const s32 i0, const s32 i1);
	void SetUniform(const s8* name, const s32 i0, const s32 i1, const s32 i2);
	void SetUniform(const s8* name, const s32 i0, const s32 i1, const s32 i2, const s32 i3);
	void SetUniform(const s8* name, const s32 num, const s32* i_array);
	// 1f - 4f
	void SetUniform(const s8* name, const f32 f0);
	void SetUniform(const s8* name, const f32 f0, const f32 f1);
	void SetUniform(const s8* name, const f32 f0, const f32 f1, const f32 f2);
	void SetUniform(const s8* name, const f32 f0, const f32 f1, const f32 f2, const f32 f3);
	void SetUniform(const s8* name, const s32 num, const f32* f_array);
	// Array
	void SetUniformArray(const char* name, const s32 num, const f32* f_array);
	void SetUniformArray4f(const char* name, const s32 num, const f32* f_array);

	// matrix
	void SetUniformMatrix2x2(const s8* name, const s32 count, const bool transpose, const f32* val);
	void SetUniformMatrix3x3(const s8* name, const s32 count, const bool transpose, const f32* val);
	void SetUniformMatrix4x4(const s8* name, const s32 count, const bool transpose, const f32* val);

	void BindAttribLocation(u32 index, const s8* name);
	u32 GetAttribLocation(const s8* name);

private:
	u32 m_program;
	s32 m_oldProgram;
	bool m_binding;
	Graphics* g;
	
};
	
} // namespace MOE

#endif // INCLUDE_SHADERPROGRAMOBJECT_H

