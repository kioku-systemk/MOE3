#ifndef INCLUDE_TEXTUREOBJECT_H
#define INCLUDE_TEXTUREOBJECT_H

#include "../Core/Type.h"

namespace MOE {
class Graphics;

class TextureObject
{
public:
	TextureObject(Graphics* mg);
    // userTexFormat : user defined tex format MG_BGRA or ...
	TextureObject(Graphics* mg, s32 width, s32 height, s32 colorComponent, s32 colorBit = 32, s32 depthBit = -1);
	TextureObject(u32 texid);
	~TextureObject();

	void BindTexture();
	void UnbindTexture();
	u32 GetTexture() const;
    
	s32 Width() const;
	s32 Height() const;
	void Resize(s32 w, s32 h, int colorComponent, int colorBit = 32, int depthBit = -1);
	void Resize(s32 w, s32 h);
	b8 WriteImage(const f32* pImage);
	b8 WriteImage(const s32* pImage);
	b8 WriteImage(const f32* pImage, u32 width, u32 height);
	b8 WriteImage(const s32* pImage, u32 width, u32 height);
	b8 WriteImage(const void* pImage, u32 width, u32 height, s32 color_component, s32 color_bit);

private:
    u32         m_userTexInterFormat;
    u32         m_texInterFormat;
    u32         m_texFormat;
    u32         m_internalSize;
	b8          m_refmode;
	unsigned int m_texname;
	unsigned int m_savedTex;
	int m_width;
	int m_height;
	int m_color_component;
	int m_color_bit;
	int m_depth_bit;
	void release();
	void create();

	Graphics* g;
};
	
} // namespace MOE


#endif // INCLUDE_TEXTUREOBJECT_H

