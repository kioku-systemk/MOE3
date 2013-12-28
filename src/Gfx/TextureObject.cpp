#include "../Core/MOE.h"
#include "TextureObject.h"

#include "Graphics.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

//#define USE_TEXSUBIMAGE

namespace {
	
	void getFormat(s32 color_bit, s32 color_component,
				   u32& gltexformat, u32& internalformat, u32& internalsize)
	{
		//
		//  gltexformat    is VRAM format.
		//  internalformat is system memory format.
		//  internalsize   is system memory component size.
        gltexformat    = VG_RGBA;          // VRAM format
	   	internalsize   = VG_UNSIGNED_BYTE;
		/*if (color_bit == 128)
		{
			gltexformat    = VG_RGBA32F;
			internalformat = VG_RGBA;
			internalsize   = VG_FLOAT;
		}
		else*/ 
        if (color_bit == 32)
		{
			/*if (color_component == 1)
			{
				gltexformat = VG_LUMINANCE32F;
				internalformat = VG_LUMINANCE;
				internalsize   = VG_FLOAT;
			}
			else
			{*/
                if (internalformat == VG_NONE) {
                    internalformat = VG_BGRA;// default sysmem format is BGRA
                }
			//}
		}
		else if (color_bit == 24)
		{
            gltexformat = VG_RGB; // (-_-)oO(will not support VG_BGR)
			if (internalformat == VG_NONE)
                internalformat = VG_RGB;
		}
		else if(color_bit == 16 && color_component == 4)
		{
		   	internalsize = VG_UNSIGNED_SHORT_4_4_4_4;
            gltexformat  = VG_RGBA;
            if (internalformat == VG_NONE)
                internalformat = VG_RGBA;
		}
		else if(color_bit == 16 && color_component == 3)
		{
			internalsize = GL_UNSIGNED_SHORT_5_6_5;
            gltexformat  = VG_RGB;
            if (internalformat == VG_NONE)
                internalformat = VG_RGB;
		}
		else if(color_bit == 8)
		{
            gltexformat = VG_LUMINANCE;
            if (internalformat == VG_NONE)
                internalformat = VG_LUMINANCE;
		}
		assert(color_component != 32);// Fuck you!! color_component maybe 4.
	}
	
} // namespace

namespace MOE {
	
TextureObject::TextureObject(Graphics* mg) :
	m_texInterFormat(VG_NONE), m_texname(0), m_width(0), m_height(0), m_color_component(4), m_color_bit(32), m_depth_bit(-1)
{
	g = mg;
	m_refmode = false;
}

TextureObject::TextureObject(Graphics* mg, s32 width, s32 height, s32 colorComponent, s32 colorBit, s32 depthBit) :
	m_texInterFormat(VG_NONE),
	m_texname(0), m_width(width), m_height(height), m_color_component(colorComponent), m_color_bit(colorBit), m_depth_bit(depthBit)
{
	g = mg;
	m_refmode = false;
	create();
}

TextureObject::TextureObject(u32 texid)
{
	m_refmode = true;
	m_texname = texid;
	m_width  = -1;
	m_height = -1;
}

TextureObject::~TextureObject()
{
	release();
}

void TextureObject::create()
{
	if (!m_texname)
	{
		unsigned int save_texname;
		g->GetIntegerv(VG_TEXTURE_BINDING_2D, &(int &)save_texname);
		g->GenTextures(1, &m_texname);
		glBindTexture(VG_TEXTURE_2D, m_texname);
		        
		getFormat(m_color_bit, m_color_component,
				  m_texFormat, m_texInterFormat, m_internalSize);
		
		g->TexImage2D(
			VG_TEXTURE_2D, 0,
			m_texFormat,     // vram format
			m_width, m_height, 0,
			m_texInterFormat,// sysmem format
			m_internalSize, 0
		);
	//g->GenerateMipmap(VG_TEXTURE_2D);
	//g->TexParameterf(VG_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MIN_FILTER, VG_LINEAR);
	g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MAG_FILTER, VG_LINEAR);
        g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_S, VG_CLAMP_TO_EDGE);
        g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_T, VG_CLAMP_TO_EDGE);
		g->BindTexture(VG_TEXTURE_2D, save_texname);
	}
}

void TextureObject::BindTexture()
{
	g->GetIntegerv(VG_TEXTURE_BINDING_2D, &(int &)m_savedTex);
	g->BindTexture(VG_TEXTURE_2D, m_texname);
}

void TextureObject::UnbindTexture()
{
	g->BindTexture(VG_TEXTURE_2D, m_savedTex);
}

u32 TextureObject::GetTexture() const
{
	return m_texname;
}

void TextureObject::release()
{
	if (m_texname && !m_refmode) {
		//printf("TextureObject::release texture %d\n", m_texname);
		g->DeleteTextures(1, &m_texname);
		m_texname = 0;
	}
}

s32 TextureObject::Width() const
{
	return m_width;
}

s32 TextureObject::Height() const
{
	return m_height;
}

void TextureObject::Resize(s32 w, s32 h, s32 colorComponent, s32 colorBit, s32 depthBit)
{
	if (m_refmode)
		return;
	
	if (w != m_width || h != m_height || colorComponent != m_color_component || colorBit != m_color_bit || depthBit != m_depth_bit)
	{
		release();
		m_width      = w;
		m_height     = h;
		m_color_component = colorComponent;
		m_color_bit  = colorBit;
		m_depth_bit  = depthBit;
		create();
	}
}
void TextureObject::Resize(s32 w, s32 h)
{
	Resize(w, h, m_color_component, m_color_bit, m_depth_bit);
}

b8 TextureObject::WriteImage(const f32* pImage)
{
	return WriteImage(pImage, m_width, m_height);
}

b8 TextureObject::WriteImage(const f32* pImage, u32 width, u32 height)
{
	return WriteImage(pImage, width, height, 4, 128);
}

//
// pImage's order must be BGRA.
//
b8 TextureObject::WriteImage(const s32* pImage)
{
	return WriteImage(pImage, m_width, m_height);
}

b8 TextureObject::WriteImage(const s32* pImage, u32 width, u32 height)
{
	return WriteImage(pImage, width, height, 4, 32);
}

b8 TextureObject::WriteImage(const void* pImage, u32 width, u32 height, s32 color_component, s32 color_bit)
{
	if (m_refmode || !m_texname)
	{
        Resize(width, height, color_component, color_bit);
    }
	
	unsigned int save_texname;
	g->GetIntegerv(VG_TEXTURE_BINDING_2D, &(int &)save_texname);
	g->BindTexture(VG_TEXTURE_2D, m_texname);
	if (m_color_bit == color_bit && m_color_component == color_component)
	{		
#ifdef USE_TEXSUBIMAGE
		if (width == 0 && height == 0) {
#endif
			g->TexImage2D(VG_TEXTURE_2D, 0,
						  m_texFormat,
						  m_width, m_height, 0,
						  m_texInterFormat,
						  m_internalSize, pImage
						  );
#ifdef USE_TEXSUBIMAGE
		} else {
			g->TexSubImage2D(VG_TEXTURE_2D, 0, 0, 0, width, height, m_texInterFormat, m_internalSize, pImage);
		}
#endif
		VGenum err = g->GetError();
		err = err;
	}
	else
	{
		return false;
	}
	g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MIN_FILTER, VG_LINEAR);
	g->BindTexture(VG_TEXTURE_2D, save_texname);
	
	return true;
}

} // namespace MOE

