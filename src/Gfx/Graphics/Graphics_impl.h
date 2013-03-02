/*
 * Graphics_impl.h
 *
 * 低レベルグラフィックスライブラリ
 * Created by kioku on 11/07/06.
 *
 */

// !!! This is included by Graphics.h !!!
#ifndef INCLUDE_VGGRAPHICS_H
#error "You must include from Graphics.h. Do not include this file."
#else

#if MOE_PLATFORM_WINDOWS
#include <GL/gl.h>
#include <GL/glu.h>
#include "WGLExtention.h"
#elif MOE_PLATFORM_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
namespace {
	void InitGLExtention(){}
}
#else // Linux
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
namespace {
	void InitGLExtention(){}
}
#endif

#include <assert.h>

#ifndef USE_GLEXTFUNC
#define glBindFramebuffer                     glBindFramebufferEXT
#define glBindRenderbuffer                    glBindRenderbufferEXT
#define glCheckFramebufferStatus              glCheckFramebufferStatusEXT
#define glDeleteFramebuffers                  glDeleteFramebuffersEXT
#define glDeleteRenderbuffers                 glDeleteRenderbuffersEXT
#define glFramebufferRenderbuffer             glFramebufferRenderbufferEXT
#define glFramebufferTexture2D                glFramebufferTexture2DEXT
#define glGenerateMipmap                      glGenerateMipmapEXT
#define glGenFramebuffers                     glGenFramebuffersEXT
#define glGenRenderbuffers                    glGenRenderbuffersEXT
#define glGetFramebufferAttachmentParameteriv glGetFramebufferAttachmentParameterivEXT
#define glGetRenderbufferParameteriv          glGetRenderbufferParameterivEXT
#define glIsFramebuffer                       glIsFramebufferEXT
#define glIsRenderbuffer                      glIsRenderbufferEXT
#define glRenderbufferStorage                 glRenderbufferStorageEXT
#define glBlendFuncSeparate                   glBlendFuncSeparateEXT
#endif

#include "GraphicsProfiler.h"

#define ENABLE_PROFILER
#define PROFILER GraphicsProfiler::GetInstance()


//#define GL_DEBUG

#ifdef GL_DEBUG
#define GLDEBUG	GL_debug_func(this, __LINE__) 
#include <assert.h>
#include <stdio.h>
#else

#define GLDEBUG 

#endif

namespace MOE {

#ifdef GL_DEBUG
inline void GL_debug_func(Graphics* g, s32 line)
{
	VGenum e = g->GetError();
	if(e)
	{
#if MOE_PLATFORM_WINDOWS
		char buf[1024];
		sprintf(buf, "GL ERROR(0x%X):Line(%d) - %s\n", e, line, gluErrorString(e));
		OutputDebugStringA(buf);
#else
		printf("GL ERROR(0x%X):Line(%d) - %s\n", e, line, gluErrorString(e));
#endif
		assert(0);
	}
}
#endif
	
inline u32 getElementSize(VGenum element)
{
	const u32 idx = element - VG_BYTE;
	static const u32 elementsize_table[] = {
		1, // VG_BYTE
		1, // VG_UNSIGNED_BYTE
		2, // VG_SHORT
		2, // VG_UNSIGNED_SHORT
		4, // VG_INT
		4, // VG_UNSIGNED_INT
		4  // VG_FLOAT
	};
	if (idx >= sizeof(elementsize_table)/sizeof(u32))
		return 0;
	
	return elementsize_table[idx];
}
	
inline GraphicsProfiler::DrawType getDrawType(VGenum type)
{
	const u32 idx = type - VG_POINTS;
	static const GraphicsProfiler::DrawType type_table[] = {
		GraphicsProfiler::TYPE_POINTS,         // VG_POINTS
		GraphicsProfiler::TYPE_LINES,          // VG_LINES
		GraphicsProfiler::TYPE_LINE_LOOP,      // VG_LINE_LOOP
		GraphicsProfiler::TYPE_LINE_STRIP,     // VG_LINE_STRIP
		GraphicsProfiler::TYPE_TRIANGLES,      // VG_TRIANGLES
		GraphicsProfiler::TYPE_TRIANGLE_STRIP, // VG_TRIANGLE_STRIP
		GraphicsProfiler::TYPE_TRIANGLE_FAN,   // VG_TRIANGLE_FAN
	};
	if (idx >= sizeof(type_table)/sizeof(u32))
		return GraphicsProfiler::TYPE_UNKOWN;
	
	return type_table[idx];
}
	
inline u32 getInternalFormatSize(VGenum internalformat)
{
	if     (internalformat == VG_LUMINANCE)  return 1;
	else if(internalformat == VG_RGB565)     return 2;
	else if(internalformat == VG_RGBA)       return 4;
	else if(internalformat == VG_BGRA)       return 4;
	else if(internalformat == VG_RGB)        return 3;
	else return 0;
}

		
inline Graphics::Graphics()
{
#ifdef ENABLE_PROFILER
	m_bind_vertexbuffer = 0;
	m_bind_indexbuffer = 0;
	m_bind_texture = 0;
#endif
	InitGLExtention();
}

inline Graphics::~Graphics()
{
}
	
inline void Graphics::ActiveTexture (VGenum texture){ glActiveTexture(texture); GLDEBUG; }
inline void Graphics::AttachShader (u32 program, u32 shader){ glAttachShader(program, shader); GLDEBUG; }
inline void Graphics::BindAttribLocation (u32 program, u32 index, const s8* name){ glBindAttribLocation(program, index, name); GLDEBUG; }
inline void Graphics::BindBuffer (VGenum target, u32 buffer){ glBindBuffer(target, buffer); GLDEBUG;
#ifdef ENABLE_PROFILER
	if (target == VG_ARRAY_BUFFER)
	{
		m_bind_vertexbuffer = buffer;
		PROFILER.BindVertex(buffer);
	}
	else if (target == VG_ELEMENT_ARRAY_BUFFER)
	{
		m_bind_indexbuffer = buffer;
		PROFILER.BindIndex(buffer);
	}
#endif
}
inline void Graphics::BindFramebuffer (VGenum target, u32 framebuffer){ glBindFramebuffer(target, framebuffer); GLDEBUG; }
inline void Graphics::BindRenderbuffer (VGenum target, u32 renderbuffer){ glBindRenderbuffer(target, renderbuffer); GLDEBUG; }
inline void Graphics::BindTexture (VGenum target, u32 texture){ glBindTexture(target, texture); GLDEBUG;
#ifdef ENABLE_PROFILER
	m_bind_texture = texture;
	PROFILER.BindTexture(texture);
#endif
}
inline void Graphics::BlendColor (f32 red, f32 green, f32 blue, f32 alpha){ glBlendColor(red, green, blue, alpha); GLDEBUG; }
inline void Graphics::BlendEquation ( VGenum mode ){ glBlendEquation(mode); GLDEBUG; }
inline void Graphics::BlendEquationSeparate (VGenum modeRGB, VGenum modeAlpha){ glBlendEquationSeparate(modeRGB, modeAlpha); GLDEBUG; }
inline void Graphics::BlendFunc (VGenum sfactor, VGenum dfactor){ glBlendFunc(sfactor, dfactor); GLDEBUG; }
inline void Graphics::BlendFuncSeparate (VGenum srcRGB, VGenum dstRGB, VGenum srcAlpha, VGenum dstAlpha){ glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha); GLDEBUG; }
inline void Graphics::BufferData (VGenum target, s32 size, const void* data, VGenum usage){ glBufferData(target, size, data, usage); GLDEBUG;
#ifdef ENABLE_PROFILER
	if (target == VG_ARRAY_BUFFER)
		PROFILER.WriteVertex(m_bind_vertexbuffer, size);
	else if (target == VG_ELEMENT_ARRAY_BUFFER)
		PROFILER.WriteIndex(m_bind_indexbuffer, size);
#endif
}
inline void Graphics::BufferSubData (VGenum target, s32 offset, s32 size, const void* data){ glBufferSubData(target, offset, size, data); GLDEBUG;
#ifdef ENABLE_PROFILER
	if (target == VG_ARRAY_BUFFER)
		PROFILER.WriteVertex(m_bind_vertexbuffer, size);
	else if (target == VG_ELEMENT_ARRAY_BUFFER)
		PROFILER.WriteIndex(m_bind_indexbuffer, size);
#endif
}
inline VGenum Graphics::CheckFramebufferStatus (VGenum target){ return glCheckFramebufferStatus(target); GLDEBUG; }
inline void Graphics::Clear (u32 mask){ glClear(mask); GLDEBUG; }
inline void Graphics::ClearColor (f32 red, f32 green, f32 blue, f32 alpha){ glClearColor(red, green, blue, alpha); GLDEBUG; }
inline void Graphics::ClearDepthf (f32 depth){ assert(0); }//glClearDepthf(depth); GLDEBUG; }
inline void Graphics::ClearStencil (s32 s){ glClearStencil(s); GLDEBUG; }
inline void Graphics::ColorMask (u8 red, u8 green, u8 blue, u8 alpha){ glColorMask(red, green, blue, alpha); GLDEBUG; }
inline void Graphics::CompileShader (u32 shader){ glCompileShader(shader); GLDEBUG; }
inline void Graphics::CompressedTexImage2D (VGenum target, s32 level, VGenum internalformat, s32 width, s32 height, s32 border, s32 imageSize, const void* data){ glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data); GLDEBUG; }
inline void Graphics::CompressedTexSubImage2D (VGenum target, s32 level, s32 xoffset, s32 yoffset, s32 width, s32 height, VGenum format, s32 imageSize, const void* data){ glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data); GLDEBUG; }
inline void Graphics::CopyTexImage2D (VGenum target, s32 level, VGenum internalformat, s32 x, s32 y, s32 width, s32 height, s32 border){ glCopyTexImage2D(target, level, internalformat, x, y, width, height, border); GLDEBUG; }
inline void Graphics::CopyTexSubImage2D (VGenum target, s32 level, s32 xoffset, s32 yoffset, s32 x, s32 y, s32 width, s32 height){ glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height); GLDEBUG; }
inline u32  Graphics::CreateProgram (void){ return glCreateProgram(); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.CreateProgram(1);
#endif
}
inline u32  Graphics::CreateShader (VGenum type){ return glCreateShader(type); GLDEBUG; }
inline void Graphics::CullFace (VGenum mode){ glCullFace(mode); GLDEBUG; }
inline void Graphics::DeleteBuffers (s32 n, const u32* buffers){
#ifdef ENABLE_PROFILER
	for (s32 i = 0; i < n; i++)
		PROFILER.DeleteBuffer(buffers[i]);
#endif	
	glDeleteBuffers(n, buffers); GLDEBUG;
}
inline void Graphics::DeleteFramebuffers (s32 n, const u32* framebuffers){ glDeleteFramebuffers(n, framebuffers); GLDEBUG; }
inline void Graphics::DeleteProgram (u32 program){ glDeleteProgram(program); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.DeleteProgram(1);
#endif
}
inline void Graphics::DeleteRenderbuffers (s32 n, const u32* renderbuffers){ glDeleteRenderbuffers(n, renderbuffers); GLDEBUG; }
inline void Graphics::DeleteShader (u32 shader){ glDeleteShader(shader); GLDEBUG; }
inline void Graphics::DeleteTextures (s32 n, const u32* textures){ glDeleteTextures(n, textures); GLDEBUG;
#ifdef ENABLE_PROFILER
	for (s32 i = 0; i < n; i++)
		PROFILER.DeleteTexture(textures[i]);
#endif
}
inline void Graphics::DepthFunc (VGenum func){ glDepthFunc(func); GLDEBUG; }
inline void Graphics::DepthMask (u8 flag){ glDepthMask(flag); GLDEBUG; }
inline void Graphics::DepthRangef (f32 zNear, f32 zFar){ assert(0); }//glDepthRangef(zNear, zFar); GLDEBUG; }
inline void Graphics::DetachShader (u32 program, u32 shader){ glDetachShader(program, shader); GLDEBUG; }
inline void Graphics::Disable (VGenum cap){ glDisable(cap); GLDEBUG; }
inline void Graphics::DisableVertexAttribArray (u32 index){ glDisableVertexAttribArray(index); GLDEBUG; }
inline void Graphics::DrawArrays (VGenum mode, s32 first, s32 count){ glDrawArrays(mode, first, count); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.DrawVertex(m_bind_vertexbuffer, count, getDrawType(mode));
#endif
}
inline void Graphics::DrawElements (VGenum mode, s32 count, VGenum type, const void* indices){ glDrawElements(mode, count, type, indices); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.DrawIndex(m_bind_indexbuffer, count, getDrawType(mode));
#endif
}
inline void Graphics::Enable (VGenum cap){ glEnable(cap); GLDEBUG; }
inline void Graphics::EnableVertexAttribArray (u32 index){ glEnableVertexAttribArray(index); GLDEBUG; }
inline void Graphics::Finish (void){ glFinish(); GLDEBUG; }
inline void Graphics::Flush (void){ glFlush(); GLDEBUG; }
inline void Graphics::FramebufferRenderbuffer (VGenum target, VGenum attachment, VGenum renderbuffertarget, u32 renderbuffer){ glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer); GLDEBUG; }
inline void Graphics::FramebufferTexture2D (VGenum target, VGenum attachment, VGenum textarget, u32 texture, s32 level){ glFramebufferTexture2D(target, attachment, textarget, texture, level); GLDEBUG; }
inline void Graphics::FrontFace (VGenum mode){ glFrontFace(mode); GLDEBUG; }
inline void Graphics::GenBuffers (s32 n, u32* buffers){ glGenBuffers(n, buffers); GLDEBUG; }
inline void Graphics::GenerateMipmap (VGenum target){ glGenerateMipmap(target); GLDEBUG; }
inline void Graphics::GenFramebuffers (s32 n, u32* framebuffers){ glGenFramebuffers(n, framebuffers); GLDEBUG; }
inline void Graphics::GenRenderbuffers (s32 n, u32* renderbuffers){ glGenRenderbuffers(n, renderbuffers); GLDEBUG; }
inline void Graphics::GenTextures (s32 n, u32* textures){ glGenTextures(n, textures); GLDEBUG; }
inline void Graphics::GetActiveAttrib (u32 program, u32 index, s32 bufsize, s32* length, s32* size, VGenum* type, s8* name){ glGetActiveAttrib(program, index, bufsize, length, size, type, name); GLDEBUG; }
inline void Graphics::GetActiveUniform (u32 program, u32 index, s32 bufsize, s32* length, s32* size, VGenum* type, s8* name){ glGetActiveUniform(program, index, bufsize, length, size, type, name); GLDEBUG; }
inline void Graphics::GetAttachedShaders (u32 program, s32 maxcount, s32* count, u32* shaders){ glGetAttachedShaders(program, maxcount, count, shaders); GLDEBUG; }
inline s32  Graphics::GetAttribLocation (u32 program, const s8* name){ return glGetAttribLocation(program, name); GLDEBUG; }
inline void Graphics::GetBooleanv (VGenum pname, u8* params){ glGetBooleanv(pname, params); GLDEBUG; }
inline void Graphics::GetBufferParameteriv (VGenum target, VGenum pname, s32* params){ glGetBufferParameteriv(target, pname, params); GLDEBUG; }
inline VGenum Graphics::GetError (void){ return glGetError(); }
inline void Graphics::GetFloatv (VGenum pname, f32* params){ glGetFloatv(pname, params); GLDEBUG; }
inline void Graphics::GetFramebufferAttachmentParameteriv (VGenum target, VGenum attachment, VGenum pname, s32* params){ glGetFramebufferAttachmentParameteriv(target, attachment, pname, params); GLDEBUG; }
inline void Graphics::GetIntegerv (VGenum pname, s32* params){ glGetIntegerv(pname, params); GLDEBUG; }
inline void Graphics::GetProgramiv (u32 program, VGenum pname, s32* params){ glGetProgramiv(program, pname, params); GLDEBUG; }
inline void Graphics::GetProgramInfoLog (u32 program, s32 bufsize, s32* length, s8* infolog){ glGetProgramInfoLog(program, bufsize, length, infolog); GLDEBUG; }
inline void Graphics::GetRenderbufferParameteriv (VGenum target, VGenum pname, s32* params){ glGetRenderbufferParameteriv(target, pname, params); GLDEBUG; }
inline void Graphics::GetShaderiv (u32 shader, VGenum pname, s32* params){ glGetShaderiv(shader, pname, params); GLDEBUG; }
inline void Graphics::GetShaderInfoLog (u32 shader, s32 bufsize, s32* length, s8* infolog){ glGetShaderInfoLog(shader, bufsize, length, infolog); GLDEBUG; }
inline void Graphics::GetShaderPrecisionFormat (VGenum shadertype, VGenum precisiontype, s32* range, s32* precision){ assert(0); }//glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision); GLDEBUG; }
inline void Graphics::GetShaderSource (u32 shader, s32 bufsize, s32* length, s8* source){ glGetShaderSource(shader, bufsize, length, source); GLDEBUG; }
inline const u8* Graphics::GetString (VGenum name){ return glGetString(name); }
inline void Graphics::GetTexParameterfv (VGenum target, VGenum pname, f32* params){ glGetTexParameterfv(target, pname, params); GLDEBUG; }
inline void Graphics::GetTexParameteriv (VGenum target, VGenum pname, s32* params){ glGetTexParameteriv(target, pname, params); GLDEBUG; }
inline void Graphics::GetUniformfv (u32 program, s32 location, f32* params){ glGetUniformfv(program, location, params); GLDEBUG; }
inline void Graphics::GetUniformiv (u32 program, s32 location, s32* params){ glGetUniformiv(program, location, params); GLDEBUG; }
inline s32  Graphics::GetUniformLocation (u32 program, const s8* name){ return glGetUniformLocation(program, name); GLDEBUG; }
inline void Graphics::GetVertexAttribfv (u32 index, VGenum pname, f32* params){ glGetVertexAttribfv(index, pname, params); GLDEBUG; }
inline void Graphics::GetVertexAttribiv (u32 index, VGenum pname, s32* params){ glGetVertexAttribiv(index, pname, params); GLDEBUG; }
inline void Graphics::GetVertexAttribPointerv (u32 index, VGenum pname, void** pointer){ glGetVertexAttribPointerv(index, pname, pointer); GLDEBUG; }
inline void Graphics::Hint (VGenum target, VGenum mode){ glHint(target, mode); GLDEBUG; }
inline u8   Graphics::IsBuffer (u32 buffer){ return glIsBuffer(buffer); GLDEBUG; }
inline u8   Graphics::IsEnabled (VGenum cap){ return glIsEnabled(cap); GLDEBUG; }
inline u8   Graphics::IsFramebuffer (u32 framebuffer){ return glIsFramebuffer(framebuffer); GLDEBUG; }
inline u8   Graphics::IsProgram (u32 program){ return glIsProgram(program); GLDEBUG; }
inline u8   Graphics::IsRenderbuffer (u32 renderbuffer){ return glIsRenderbuffer(renderbuffer); GLDEBUG; }
inline u8   Graphics::IsShader (u32 shader){ return glIsShader(shader); GLDEBUG; }
inline u8   Graphics::IsTexture (u32 texture){ return glIsTexture(texture); GLDEBUG; }
inline void Graphics::LineWidth (f32 width){ glLineWidth(width); GLDEBUG; }
inline void Graphics::LinkProgram (u32 program){ glLinkProgram(program); GLDEBUG; }
inline void Graphics::PixelStorei (VGenum pname, s32 param){ glPixelStorei(pname, param); GLDEBUG; }
inline void Graphics::PolygonOffset (f32 factor, f32 units){ glPolygonOffset(factor, units); GLDEBUG; }
inline void Graphics::ReadPixels (s32 x, s32 y, s32 width, s32 height, VGenum format, VGenum type, void* pixels){ glReadPixels(x, y, width, height, format, type, pixels); GLDEBUG; }
inline void Graphics::ReleaseShaderCompiler (void){ assert(0); }//glReleaseShaderCompiler(); GLDEBUG; }
inline void Graphics::RenderbufferStorage (VGenum target, VGenum internalformat, s32 width, s32 height){ glRenderbufferStorage(target, internalformat, width, height); GLDEBUG; }
inline void Graphics::SampleCoverage (f32 value, u8 invert){ glSampleCoverage(value, invert); GLDEBUG; }
inline void Graphics::Scissor (s32 x, s32 y, s32 width, s32 height){ glScissor(x, y, width, height); GLDEBUG; }
inline void Graphics::ShaderBinary (s32 n, const u32* shaders, VGenum binaryformat, const void* binary, s32 length){ assert(0); }//glShaderBinary(n, shaders, binaryformat, binary, length); GLDEBUG; }
inline void Graphics::ShaderSource (u32 shader, s32 count, const s8** string, const s32* length){ glShaderSource(shader, count, string, length); GLDEBUG; }
inline void Graphics::StencilFunc (VGenum func, s32 ref, u32 mask){ glStencilFunc(func, ref, mask); GLDEBUG; }
inline void Graphics::StencilFuncSeparate (VGenum face, VGenum func, s32 ref, u32 mask){ glStencilFuncSeparate(face, func, ref, mask); GLDEBUG; }
inline void Graphics::StencilMask (u32 mask){ glStencilMask(mask); GLDEBUG; }
inline void Graphics::StencilMaskSeparate (VGenum face, u32 mask){ glStencilMaskSeparate(face, mask); GLDEBUG; }
inline void Graphics::StencilOp (VGenum fail, VGenum zfail, VGenum zpass){ glStencilOp(fail, zfail, zpass); GLDEBUG; }
inline void Graphics::StencilOpSeparate (VGenum face, VGenum fail, VGenum zfail, VGenum zpass){ glStencilOpSeparate(face, fail, zfail, zpass); GLDEBUG; }
inline void Graphics::TexImage2D (VGenum target, s32 level, s32 internalformat, s32 width, s32 height, s32 border, VGenum format, VGenum type, const void* pixels){ glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); GLDEBUG;
#ifdef ENABLE_PROFILER
	const u32 elementsize = getInternalFormatSize(internalformat);
	PROFILER.WriteTexture(m_bind_texture, width * height, elementsize);
#endif
}
inline void Graphics::TexParameterf  (VGenum target, VGenum pname, f32 param){ glTexParameterf(target, pname, param); GLDEBUG; }
inline void Graphics::TexParameterfv (VGenum target, VGenum pname, const f32* params){ glTexParameterfv(target, pname, params); GLDEBUG; }
inline void Graphics::TexParameteri  (VGenum target, VGenum pname, s32 param){ glTexParameteri(target, pname, param); GLDEBUG; }
inline void Graphics::TexParameteriv (VGenum target, VGenum pname, const s32* params){ glTexParameteriv(target, pname, params); GLDEBUG; }
inline void Graphics::TexSubImage2D  (VGenum target, s32 level, s32 xoffset, s32 yoffset, s32 width, s32 height, VGenum format, VGenum type, const void* pixels){ glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.WriteSubTexture(m_bind_texture, width * height);
#endif
}
inline void Graphics::Uniform1f  (s32 location, f32 x){ glUniform1f(location, x); GLDEBUG; }
inline void Graphics::Uniform1fv (s32 location, s32 count, const f32* v){ glUniform1fv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform1i  (s32 location, s32 x){ glUniform1i(location, x); GLDEBUG; }
inline void Graphics::Uniform1iv (s32 location, s32 count, const s32* v){ glUniform1iv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform2f  (s32 location, f32 x, f32 y){ glUniform2f(location, x, y); GLDEBUG; }
inline void Graphics::Uniform2fv (s32 location, s32 count, const f32* v){ glUniform2fv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform2i  (s32 location, s32 x, s32 y){ glUniform2i(location, x, y); GLDEBUG; }
inline void Graphics::Uniform2iv (s32 location, s32 count, const s32* v){ glUniform2iv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform3f  (s32 location, f32 x, f32 y, f32 z){ glUniform3f(location, x, y, z); GLDEBUG; }
inline void Graphics::Uniform3fv (s32 location, s32 count, const f32* v){ glUniform3fv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform3i  (s32 location, s32 x, s32 y, s32 z){ glUniform3i(location, x, y, z); GLDEBUG; }
inline void Graphics::Uniform3iv (s32 location, s32 count, const s32* v){ glUniform3iv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform4f  (s32 location, f32 x, f32 y, f32 z, f32 w){ glUniform4f(location, x, y, z, w); GLDEBUG; }
inline void Graphics::Uniform4fv (s32 location, s32 count, const f32* v){ glUniform4fv(location, count, v); GLDEBUG; }
inline void Graphics::Uniform4i  (s32 location, s32 x, s32 y, s32 z, s32 w){ glUniform4i(location, x, y, z, w); GLDEBUG; }
inline void Graphics::Uniform4iv (s32 location, s32 count, const s32* v){ glUniform4iv(location, count, v); GLDEBUG; }
inline void Graphics::UniformMatrix2fv (s32 location, s32 count, u8 transpose, const f32* value){ glUniformMatrix2fv(location, count, transpose, value); GLDEBUG; }
inline void Graphics::UniformMatrix3fv (s32 location, s32 count, u8 transpose, const f32* value){ glUniformMatrix3fv(location, count, transpose, value); GLDEBUG; }
inline void Graphics::UniformMatrix4fv (s32 location, s32 count, u8 transpose, const f32* value){ glUniformMatrix4fv(location, count, transpose, value); GLDEBUG; }
inline void Graphics::UseProgram (u32 program){ glUseProgram(program); GLDEBUG;
#ifdef ENABLE_PROFILER
	PROFILER.BindProgram(program);
#endif
}
inline void Graphics::ValidateProgram (u32 program){ glValidateProgram(program); GLDEBUG; }
inline void Graphics::VertexAttrib1f  (u32 indx, f32 x){ glVertexAttrib1f(indx, x); GLDEBUG; }
inline void Graphics::VertexAttrib1fv (u32 indx, const f32* values){ glVertexAttrib1fv(indx, values); GLDEBUG; }
inline void Graphics::VertexAttrib2f  (u32 indx, f32 x, f32 y){ glVertexAttrib2f(indx, x, y); GLDEBUG; }
inline void Graphics::VertexAttrib2fv (u32 indx, const f32* values){ glVertexAttrib2fv(indx, values); GLDEBUG; }
inline void Graphics::VertexAttrib3f  (u32 indx, f32 x, f32 y, f32 z){ glVertexAttrib3f(indx, x, y, z); GLDEBUG; }
inline void Graphics::VertexAttrib3fv (u32 indx, const f32* values){ glVertexAttrib3fv(indx, values); GLDEBUG; }
inline void Graphics::VertexAttrib4f  (u32 indx, f32 x, f32 y, f32 z, f32 w){ glVertexAttrib4f(indx, x, y, z, w); GLDEBUG; }
inline void Graphics::VertexAttrib4fv (u32 indx, const f32* values){ glVertexAttrib4fv(indx, values); GLDEBUG; }
inline void Graphics::VertexAttribPointer (u32 indx, s32 size, VGenum type, u8 normalized, s32 stride, const void* ptr){ glVertexAttribPointer(indx, size, type, normalized, stride, ptr); GLDEBUG; }
inline void Graphics::Viewport (s32 x, s32 y, s32 width, s32 height){ glViewport(x, y, width, height); GLDEBUG; }

// Extention
inline void Graphics::PolygonMode (VGenum face, VGenum mode) {
	PROFILER.PolygonMode(mode);
	glPolygonMode(face, mode); GLDEBUG; 
}
	
} // namespace MOE
#endif

