#ifndef INCLUDE_MOE_FRAMEBUFFEROBJECT_H
#define INCLUDE_MOE_FRAMEBUFFEROBJECT_H

#include "../Core/Type.h"

#define VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
#define USE_RGBA32F_FORMAT
#define USE_L32_FORMAT

namespace MOE {

class Graphics;

class FrameBufferObject
{
public:
	FrameBufferObject(Graphics* mg);
	FrameBufferObject(Graphics* mg, s32 width, s32 height, s32 colorComponent, s32 colorBit = 32, s32 depthBit = -1);
	~FrameBufferObject();

	void Begin();

	void End();
    void BindTexture();
    s32 Width() const;
	s32 Height() const;
	
	b8 Resize(s32 w, s32 h, s32 colorComponent, s32 colorBit = 32, s32 depthBit = -1);
    b8 Resize(s32 w, s32 h);
    
	u32 GetColorTexture() const;
	#ifdef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
		u32 GetDepthTexture() const;
	#endif

	//b8 Save(const s8* path, s32 saveColorBit = 32, s32 saveComponent = 4) const;

	b8 GetImage(void* imageBuf, s32 colorBit, s32 compNum);
	b8 GetDepth(void* depthBuf, s32 colorBit, s32 compNum);

private:
    Graphics* g;
	u32 m_fboname;
	u32 m_texname;
	#ifndef VG_FRAMEBUFFEROBJECT_USE_DEPTH_TEXTURE
		u32 m_rbufname;
	#else
		u32 m_depthname;
	#endif
	u32 m_save_fboname;
	u32 m_err;
	s32 m_width;
	s32 m_height;
	s32 m_color_component;
	s32 m_color_bit;
	s32 m_depth_bit;
	
	s32 m_msaa;
	void release();
	b8 create();
};

} // namespace MOE

#endif // INCLUDE_MOE_FRAMEBUFFEROBJECT_H

