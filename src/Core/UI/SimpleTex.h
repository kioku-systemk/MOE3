/*
 *  SimpleTex.h
 *
 *  Created by kioku on 2012/08/20.
 *
 */

#ifndef INCLUDE_SIMPLETEX_H
#define INCLUDE_SIMPLETEX_H

namespace skGUI {
	
class SimpleTex
{
public:
	enum COLORTYPE {
		COLOR_RGBA8888,
		COLOR_R8
	};

	SimpleTex(MOE::Graphics* sg, int w, int h, COLORTYPE col = COLOR_RGBA8888)
	{
		g = sg;
		m_tex    = -1;
		m_width  = w;
		m_height = h;
		m_color  = col;
		const int colsize = (col == COLOR_RGBA8888 ? 4 : 1);
		m_buffer = new unsigned char[colsize * w * h];
	}
	~SimpleTex()
	{
		delete [] m_buffer;
		g->DeleteTextures(1, &m_tex);
	}
	int GetWidth () const          { return m_width;  }
	int GetHeight() const          { return m_height; }
	COLORTYPE GetColorType() const { return m_color; }
	void Resize(int w, int h)
	{
		delete [] m_buffer;
		const int colsize = (m_color == COLOR_RGBA8888 ? 4 : 1);
		m_buffer = new unsigned char[colsize * w * h];
		m_width  = w;
		m_height = h;
	}
	unsigned char* Map()
	{
		return m_buffer;
	}
	void Unmap()
	{
		if (m_tex == -1){
			g->GenTextures(1, &m_tex);
			g->BindTexture(VG_TEXTURE_2D, m_tex);
			g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MIN_FILTER, VG_LINEAR);
			g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_MAG_FILTER, VG_LINEAR);
			g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_S, VG_CLAMP_TO_EDGE);
			g->TexParameteri(VG_TEXTURE_2D, VG_TEXTURE_WRAP_T, VG_CLAMP_TO_EDGE);
		}
		// Update
		g->BindTexture(VG_TEXTURE_2D, m_tex);
		if (m_color == COLOR_RGBA8888)
			g->TexImage2D(VG_TEXTURE_2D, 0, VG_RGBA, m_width, m_height, 0, VG_RGBA, VG_UNSIGNED_BYTE, m_buffer);
		else
			g->TexImage2D(VG_TEXTURE_2D, 0, VG_LUMINANCE, m_width, m_height, 0, VG_LUMINANCE, VG_UNSIGNED_BYTE, m_buffer);
	}
	unsigned int GetID()
	{
		return m_tex;
	}
private:
	COLORTYPE m_color;
	
	MOE::Graphics* g;
	unsigned int m_tex;
	int m_width;
	int m_height;
	unsigned char* m_buffer;
};
	
} // namespace skGUI

#endif // INCLUDE_SIMPLETEX_H

