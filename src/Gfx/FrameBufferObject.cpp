#include "../Core/MOE.h"
#include "FrameBufferObject.h"
#include "../Gfx/Graphics.h"

#include "../Core/Log.h"

#include <string.h>
#include <stdio.h>

#if MOE_PLATFORM_WINDOWS | MOE_PLATFORM_MACOSX
#define USE_RGBA32F_FORMAT
#define USE_L32_FORMAT
#define USE_DEPTH_EXT
//#define USE_PBO
#endif

#define RGBA2BGRA(c) ((((c)&0x00FF0000)>>16)|(((c)&0x000000FF)<<16)|((c)&0xFF00FF00))

namespace {
	void getFormat(int color_bit, int color_component,
				   unsigned int& gltexformat, unsigned int& internalformat, unsigned int& internalsize)
	{
		gltexformat    = VG_RGBA;
		internalformat = VG_RGB;
		internalsize   = VG_UNSIGNED_BYTE;
#ifdef USE_RGBA32F_FORMAT
		if (color_bit == 128)
		{
			gltexformat    = VG_RGBA32F_ARB;
			internalformat = VG_RGBA;
			internalsize   = VG_FLOAT;
		}
		else
#endif
        if (color_bit == 32)
		{
#ifdef USE_L32_FORMAT
			if (color_component == 1)
			{
				gltexformat = VG_LUMINANCE32F_ARB;
				internalformat = VG_RED;
				internalsize   = VG_FLOAT;
			}
			else
#endif
			{
				gltexformat = VG_RGBA;
				internalformat = VG_RGBA;
				internalsize   = VG_UNSIGNED_BYTE;
			}
		}
		else if (color_bit == 24)
		{
			gltexformat = VG_RGB;
			internalformat = VG_RGB;
			internalsize   = VG_UNSIGNED_BYTE;
		}
	}
	
} // namespace

namespace MOE {
	
FrameBufferObject::FrameBufferObject(Graphics* mg) :
	g(mg), m_fboname(0), m_texname(0),
#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
	m_rbufname(0),
#else
	m_depthname(0),
#endif
	m_save_fboname(0), m_err(0),
	m_width(0), m_height(0), m_color_component(3),
	m_color_bit(32), m_depth_bit(-1)
{
}

FrameBufferObject::FrameBufferObject(Graphics* mg, s32 width, s32 height, s32 colorComponent, s32 colorBit, s32 depthBit) :
	g(mg), m_fboname(0), m_texname(0),
#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
	m_rbufname(0),
#else
	m_depthname(0),
#endif
	m_save_fboname(0), m_err(0),
	m_width(width), m_height(height), m_color_component(colorComponent),
	m_color_bit(colorBit), m_depth_bit(depthBit)
{
	create();
}

FrameBufferObject::~FrameBufferObject()
{
	release();
}

bool FrameBufferObject::create()
{
	if (m_fboname)
		release();

	if (m_fboname)
		return false;

	GLuint save_framebuffer;
	g->GetIntegerv(VG_FRAMEBUFFER_BINDING, &(GLint &)save_framebuffer);
	
	g->GenFramebuffers(1, &m_fboname);
	g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
	
	if (!m_texname)
	{
		GLuint save_texname;
		g->GetIntegerv(VG_TEXTURE_BINDING_2D, &(GLint &)save_texname);
		g->GenTextures(1, &m_texname);
		g->BindTexture(VG_TEXTURE_2D, m_texname);
		
		unsigned int gltexformat, internalformat, internalsize;
		getFormat(m_color_bit, m_color_component,
				  gltexformat, internalformat, internalsize);
		
		g->TexImage2D(
			VG_TEXTURE_2D, 0,
			gltexformat,
			m_width, m_height, 0,
			internalformat,
			internalsize, 0
		);
		GLenum err = glGetError();
		if (err != VG_NO_ERROR)
			printf("[%s]CreateTexture error=%x\n",__FILE__, err);
		
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MIN_FILTER, VG_LINEAR);
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_S, VG_CLAMP_TO_EDGE);
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_T, VG_CLAMP_TO_EDGE);
		g->BindTexture(VG_TEXTURE_2D, save_texname);
		if (err != VG_NO_ERROR)
			return false;
	}
	
#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
	if (!m_rbufname)
	{
		GLuint save_rbufname;
		g->GetIntegerv(VG_RENDERBUFFER_BINDING, &(GLint &)save_rbufname);
		g->GenRenderbuffers(1, &m_rbufname);
		g->BindRenderbuffer(VG_RENDERBUFFER, m_rbufname);
		
		unsigned int gldepthsize = GL_DEPTH_COMPONENT16;
#ifdef USE_DEPTH_EXT       
		if      (m_depth_bit == 32)
			gldepthsize = VG_DEPTH_COMPONENT32;
		else if (m_depth_bit == 24)
			gldepthsize = VG_DEPTH_COMPONENT24;
		else if (m_depth_bit == 16)
			gldepthsize = VG_DEPTH_COMPONENT16;
#endif	
		g->RenderbufferStorage(
			VG_RENDERBUFFER,
			gldepthsize,
			m_width, m_height
		);
	
		g->BindRenderbuffer(VG_RENDERBUFFER, save_rbufname);
			
		GLenum err = glGetError();
		if (err != VG_NO_ERROR)
		MOELogD("[%s]CreateTexture error=%d\n",__FILE__, err);

	}
#else
	if (!m_depthname)
	{
		GLuint save_texname;
		g->GetIntegerv(VG_TEXTURE_BINDING_2D, &(GLint &)save_texname);
		g->GenTextures(1, &m_depthname);
		g->BindTexture(VG_TEXTURE_2D, m_depthname);
		
		unsigned int gldepthsize = VG_DEPTH_COMPONENT;
		if      (m_depth_bit == 32)
			gldepthsize = VG_DEPTH_COMPONENT32;
		else if (m_depth_bit == 24)
			gldepthsize = VG_DEPTH_COMPONENT24;
		else if (m_depth_bit == 16)
			gldepthsize = VG_DEPTH_COMPONENT16;
		
		g->TexImage2D(
			VG_TEXTURE_2D, 0,
			gldepthsize,
			m_width, m_height, 0,
			VG_DEPTH_COMPONENT,
			VG_FLOAT,
			0
		);
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MIN_FILTER, VG_LINEAR);
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_S, VG_CLAMP_TO_EDGE);
		g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_T, VG_CLAMP_TO_EDGE);
		g->BindTexture(VG_TEXTURE_2D, save_texname);
	}
#endif
	g->FramebufferTexture2D(VG_FRAMEBUFFER, VG_COLOR_ATTACHMENT0, VG_TEXTURE_2D, m_texname, 0);
	
#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
	g->FramebufferRenderbuffer(VG_FRAMEBUFFER, VG_DEPTH_ATTACHMENT, VG_RENDERBUFFER, m_rbufname);
#else
	g->FramebufferTexture2D(VG_FRAMEBUFFER, VG_DEPTH_ATTACHMENT, VG_TEXTURE_2D, m_depthname, 0);
#endif

	m_err = g->CheckFramebufferStatus(VG_FRAMEBUFFER);
	g->BindFramebuffer(VG_FRAMEBUFFER, save_framebuffer);
	
	if (m_err != VG_FRAMEBUFFER_COMPLETE) {
		MOELogD("FrameBuffer error");
		return false;
	} else {
		return true;
	}
}

void FrameBufferObject::Begin()
{
	if (m_width <= 0 || m_height <= 0)
		return;
	
	if (m_err != VG_FRAMEBUFFER_COMPLETE)
	{
		printf("[%s] CreateFrameBuffer error\n", __FILE__);
		return;
	}
	
	g->GetIntegerv(VG_FRAMEBUFFER_BINDING, &(GLint &)m_save_fboname);
	if (m_fboname)
		g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
}

void FrameBufferObject::End()
{
	g->BindFramebuffer(VG_FRAMEBUFFER, m_save_fboname);
	m_save_fboname = 0;

}

void FrameBufferObject::BindTexture()
{
	g->BindTexture(VG_TEXTURE_2D, m_texname);
}

void FrameBufferObject::release()
{
	if (m_fboname)
	{
		g->DeleteFramebuffers(1, &m_fboname);
		m_fboname = 0;
	}
#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
	if (m_rbufname) {
		g->DeleteRenderbuffers(1, &m_rbufname);
		m_rbufname = 0;
	}
#else
	if (m_depthname) {
		g->DeleteTextures(1, &m_depthname);
		m_depthname = 0;
	}
#endif
	if (m_texname) {
		g->DeleteTextures(1, &m_texname);
		m_texname = 0;
	}
}

int FrameBufferObject::Width() const
{
	return m_width;
}

int FrameBufferObject::Height() const
{
	return m_height;
}

bool FrameBufferObject::Resize(int w, int h, int colorComponent, int colorBit, int depthBit)
{
	if (w != m_width || h != m_height)
	{
		release();
		m_width           = w;
		m_height          = h;
		m_color_component = colorComponent;
		m_color_bit       = colorBit;
		m_depth_bit       = depthBit;
		return create();
	}
	return true;
}
bool FrameBufferObject::Resize(int w, int h)
{
	return Resize(w, h, m_color_component, m_color_bit, m_depth_bit);
}


unsigned int FrameBufferObject::GetColorTexture() const
{
	return m_texname;
}

#ifdef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE

unsigned int FrameBufferObject::GetDepthTexture() const
{
	return m_depthname;
}

#endif // VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE

bool FrameBufferObject::GetImage(void* imageBuf, int colorBit, int compNum)
{
	if (m_fboname == 0)
		return false;

#ifdef USE_PBO
	unsigned int pboId;
	g->GenBuffers(1, &pboId);
	g->BindBuffer(VG_PIXEL_PACK_BUFFER, pboId);
	int width = Width();
	int height = Height();
	int widthByte = width * (colorBit / 8);
	unsigned int dataSize = widthByte * height;
	g->BufferData(VG_PIXEL_PACK_BUFFER, dataSize, 0, VG_STREAM_READ);
    
	g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
	g->PixelStorei(VG_PACK_ALIGNMENT ,1);
	g->Finish();

	if (compNum == 1 && colorBit == 32)
		g->ReadPixels(0, 0, width, height, VG_RED, VG_FLOAT, 0);
	else if (compNum == 4 && colorBit == 32)
		g->ReadPixels(0, 0, width, height, VG_BGRA, VG_UNSIGNED_BYTE, 0);
	else
		g->ReadPixels(0, 0, width, height, VG_BGRA, VG_UNSIGNED_BYTE, 0);

	g->BindFramebuffer(VG_FRAMEBUFFER, 0);
	g->Finish();

	unsigned char* color = (unsigned char*)g->MapBuffer(VG_PIXEL_PACK_BUFFER, VG_READ_ONLY);
	unsigned char* image = static_cast<unsigned char*>(imageBuf);
	if (color)
	{
		for (int y = 0; y < height; y++)
		{
			memcpy(&image[(height - 1 - y) * widthByte], &color[y * widthByte], widthByte); 
	//		for (int x = 0; x < width; x++)
	//		{
	//			int dy = height - 1 - y;
	//			image[x + dy * width] = color[x + y * width];
	//		}
		}
		g->UnmapBuffer(VG_PIXEL_PACK_BUFFER);
	}
	g->BindBuffer(VG_PIXEL_PACK_BUFFER, 0);
	g->DeleteBuffers(1, &pboId);
#else
	g->GetIntegerv(VG_FRAMEBUFFER_BINDING, &(GLint &)m_save_fboname);
	
    g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
	g->PixelStorei(VG_PACK_ALIGNMENT ,1);
    
    // TODO: other options
    g->ReadPixels(0, 0, Width(), Height(), VG_RGBA, VG_UNSIGNED_BYTE, imageBuf);
	g->Finish();
	
	// upside down and RGBA -> BGRA
	const int halfh = (Height() + 1)/2;
	for (int y = 0; y < halfh; y++) {
		for (int x = 0; x < Width(); x++) {
			u32* a = &static_cast<u32*>(imageBuf)[y * Width() + x];
			u32* b = &static_cast<u32*>(imageBuf)[(Height() - 1 - y) * Width() + x];
			u32  t = RGBA2BGRA((*a));
			*a = RGBA2BGRA((*b));
			*b = t;
		}
	}
			   
    GLenum err = glGetError();
    if (err != VG_NO_ERROR)
        printf("[%s]ReadPixels Error=%d\n",__FILE__, err);

    g->BindFramebuffer(VG_FRAMEBUFFER, m_save_fboname);
#endif
	return true;
}

bool FrameBufferObject::GetDepth(void* depthBuf, int depthBit, int compNum)
{
	if (m_fboname == 0)
		return false;

#ifdef USE_PBO
	unsigned int pboId;
	g->GenBuffers(1, &pboId);
	g->BindBuffer(VG_PIXEL_PACK_BUFFER, pboId);
	int width = Width();
	int height = Height();
	int widthByte = width * (depthBit / 8);
	unsigned int dataSize = widthByte * height;
	g->BufferData(VG_PIXEL_PACK_BUFFER, dataSize, 0, VG_STREAM_READ);
	
	g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
	g->PixelStorei(VG_PACK_ALIGNMENT ,1);
	g->Finish();
	if (compNum == 1 && depthBit == 32)
		glReadPixels(0, 0, width, height, VG_DEPTH_COMPONENT, VG_FLOAT, 0);
	else
	{
		printf("[%s]Non supported format for depth readback\n",__FILE__);
		return false;
	}
	
	g->BindFramebuffer(VG_FRAMEBUFFER, 0);
	
	g->Finish();
	unsigned char* color = (unsigned char*)g->MapBuffer(VG_PIXEL_PACK_BUFFER, VG_READ_ONLY);
	unsigned char* image = static_cast<unsigned char*>(depthBuf);
	if (color)
	{
		for (int y = 0; y < height; y++)
		{
			memcpy(&image[(height - 1 - y) * widthByte], &color[y * widthByte], widthByte); 
			//		for (int x = 0; x < width; x++)
			//		{
			//			int dy = height - 1 - y;
			//			image[x + dy * width] = color[x + y * width];
			//		}
		}
		g->UnmapBuffer(VG_PIXEL_PACK_BUFFER);
	}
	g->BindBuffer(VG_PIXEL_PACK_BUFFER, 0);
	g->DeleteBuffers(1, &pboId);
#else
	g->GetIntegerv(VG_FRAMEBUFFER_BINDING, &(GLint &)m_save_fboname);
    g->BindFramebuffer(VG_FRAMEBUFFER, m_fboname);
	g->PixelStorei(VG_PACK_ALIGNMENT ,1);
    
    // TODO: other options
    g->ReadPixels(0, 0, Width(), Height(), VG_DEPTH_COMPONENT, VG_FLOAT, depthBuf);
	g->Finish();
	
    g->BindFramebuffer(VG_FRAMEBUFFER, m_save_fboname);
#endif
	return true;

}
	
} // namespace MOE

