#include "MOE.h"

#include "Image.h"
#include "Stream.h"

#include <stdio.h>
#include <string>
#include <libpng/png.h>
#include "jpgd.h"

namespace MOE {
    
class Image::Impl
{
private:
	unsigned char* m_img;
	int m_width;
	int m_height;
	
	typedef struct _bufferData
	{
		const char* dataPtr;
		size_t size;
		size_t offset;
	} bufferData;

	void freeImg()
	{
		m_width  = 0;
		m_height = 0;
		delete [] m_img;
		m_img = 0;
	}
	
public:
	Impl()
	{
		m_width  = 0;
		m_height = 0;
		m_img = 0;
	}
	
	~Impl()
	{
		freeImg();
	}

	int GetWidth() const
	{
		return m_width;
	}
	int GetHeight() const
	{
		return m_height;
	}
	void WriteImage(unsigned char* imgBuffer)
	{
		if (!m_img)
			return;
		
		//memcpy(imgBuffer, m_img, m_width * m_height * 4);
		
		for (int y = 0; y < m_height; y++)
		{
			memcpy(&imgBuffer[(m_height - y - 1) * m_width * 4], &m_img[y * m_width * 4], m_width * 4);
			/*
			 for(int x = 0; x < m_width; x++)
			 {
			 const unsigned int col = *(unsigned int*)&m_img[(x + y * m_width) * 4];
			 const unsigned int swapcol = ((0xFF000000 & col) | ((col&0xFF) << 16) | (col&0x0000FF00) | ((col&0x00FF0000)>>16));
			 *(unsigned int*)&imgBuffer[(x + (m_height - y - 1) * m_width) * 4] = swapcol;
			 //imgBuffer[(x + y * m_width) * 3 + 2] = ( col        & 0xFF);
			 //imgBuffer[(x + y * m_width) * 3 + 1] = ((col >>  8) & 0xFF);
			 //imgBuffer[(x + y * m_width) * 3 + 0] = ((col >> 16) & 0xFF);
			 }*/
		}
	}
    
    int* GetImage() const
    {
        return reinterpret_cast<int*>(m_img);
    }
	
    bool isExt(const char* filename, const char* ext)
    {
        size_t l = strlen(filename);
        return (strncasecmp(&filename[l-4], ext,strlen(ext)) == 0);
    }
    
    bool load(const char* filename, const void* buffer = 0, unsigned int size = 0)
    {
        const unsigned char pngheader[] = {0x89,0x50,0x4E,0x47};
        if ((buffer && memcmp(buffer, pngheader, 4)==0)
        ||  (filename && isExt(filename,".png"))){
            return loadPng(filename, buffer, size);
        } else {
            return loadJpg(filename, buffer, size);
        }
            
    }
    
    unsigned int BGRA2RGBA(unsigned int c)
    {
        return ((c&0x000000FF)<<16) | (c&0x0000FF00) | ((c&0x00FF0000) >> 16) | (c&0xFF000000);
    }
    bool loadJpg(const char* filename, const void* buffer, unsigned int size)
    {
        int width, height, comp, reqcomp = 4;
        if (filename){
            unsigned char* img = jpgd::decompress_jpeg_image_from_file(filename, &width, &height, &comp, reqcomp);
            if (!img)
                return false;
            m_width = width;
            m_height = height;
            const int bsize = width * height * 4;
            m_img = mnew unsigned char[bsize];
            for (int y = 0; y < height; ++y){
                for (int x = 0; x < width; ++x){
                    const unsigned int col = *reinterpret_cast<unsigned int*>(&img[(y * width + x) * 4]);
                    *reinterpret_cast<unsigned int*>(&m_img[((height - y - 1) * width + x) * 4]) = BGRA2RGBA(col);
                }
            }
            free(img);
        } else {
            unsigned char* img = jpgd::decompress_jpeg_image_from_memory(static_cast<const unsigned char*>(buffer), size, &width, &height, &comp, reqcomp);
            if (!img)
                return 0;
            m_width = width;
            m_height = height;
            const int bsize = width * height * 4;
            m_img = mnew unsigned char[bsize];
            for (int y = 0; y < height; ++y){
                for (int x = 0; x < width; ++x){
                    const unsigned int col = *reinterpret_cast<unsigned int*>(&img[(y * width + x) * 4]);
                    *reinterpret_cast<unsigned int*>(&m_img[((height - y - 1) * width + x) * 4]) = BGRA2RGBA(col);
                }
            }
            free(img);
        }
        return true;
    }
    
	bool loadPng(const char* filename, const void* buffer, unsigned int size)
	{
		FILE* fp = 0;
		if (!buffer)
		{
			fp = fopen(filename, "rb");
			if (!fp)
				return false;
		}
		
		freeImg();
		//fread(header, 1, number, fp);
		
		//int is_png = !png_sig_cmp(header, 0, number);
		//if (!is_png)
		//	return false;
		
		png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if( !png_ptr ){
			return false;
		}
		
		png_info *info_ptr = png_create_info_struct(png_ptr);
		if( !info_ptr ){
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}
		png_info *end_info = png_create_info_struct(png_ptr);
		if( !end_info ){
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return false;
		}
		
		bufferData buf = {static_cast<const char*>(buffer), size, 0};
		if (fp)
			png_set_read_fn(png_ptr, reinterpret_cast<unsigned char*>(fp), Image::Impl::pngReadProc);
		else
			png_set_read_fn(png_ptr, &buf, Image::Impl::pngMemReadProc);
		
		png_uint_32 nWidth, nHeight;
		int nPngDepth, nColorType;
		
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nPngDepth, &nColorType, NULL, NULL, NULL);
		
		// パレットPNGやグレースケールPNGをとりあえずフルカラーに展開
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
		{
			png_color_16p pBackground;
			
			/* it is not obvious from the libpng documentation, but this function
			 * takes a pointer to a pointer, and it always returns valid red,
			 * green and blue values, regardless of color_type: */
			png_get_bKGD(png_ptr, info_ptr, &pBackground);
			/* however, it always returns the raw bKGD data, regardless of any
			 * bit-depth transformations, so check depth and adjust if necessary */
			/*		if (bit_depth == 16) {
			 mainprog_ptr->bg_red   = pBackground->red   >> 8;
			 mainprog_ptr->bg_green = pBackground->green >> 8;
			 mainprog_ptr->bg_blue  = pBackground->blue  >> 8;
			 } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
			 if (bit_depth == 1)
			 mainprog_ptr->bg_red = mainprog_ptr->bg_green =
			 mainprog_ptr->bg_blue = pBackground->gray? 255 : 0;
			 else if (bit_depth == 2)
			 mainprog_ptr->bg_red = mainprog_ptr->bg_green =
			 mainprog_ptr->bg_blue = (255/3) * pBackground->gray;
			 else // bit_depth == 4
			 mainprog_ptr->bg_red = mainprog_ptr->bg_green =
			 mainprog_ptr->bg_blue = (255/15) * pBackground->gray;
			 } else {
			 mainprog_ptr->bg_red   = (uch)pBackground->red;
			 mainprog_ptr->bg_green = (uch)pBackground->green;
			 mainprog_ptr->bg_blue  = (uch)pBackground->blue;
			 }*/
		}
		
		
		/*
		 if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)){
		 png_set_expand(png_ptr);
		 }
		 if(nColorType == PNG_COLOR_TYPE_PALETTE){
		 if (nPngDepth <= 8)
		 png_set_palette_to_rgb(png_ptr);
		 else
		 //png_set_expand(png_ptr);
		 png_set_palette_to_rgb(png_ptr);
		 }
		 if(nColorType == PNG_COLOR_TYPE_GRAY && nPngDepth < 8){
		 png_set_expand(png_ptr);
		 }
		 if(nPngDepth > 8){
		 png_set_strip_16(png_ptr);
		 }
		 if(nColorType == PNG_COLOR_TYPE_GRAY){
		 png_set_gray_to_rgb(png_ptr);
		 }
		 */
		if (nColorType == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (nColorType == PNG_COLOR_TYPE_GRAY && nPngDepth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (nPngDepth == 16)
			png_set_strip_16(png_ptr);
		if (nColorType == PNG_COLOR_TYPE_GRAY || nColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		
		// 出力を1ピクセル4バイトに指定
		// RGBA の場合も無視されるっぽいのでとりあえず実行しておく
		png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);// BGRの場合はBGR1になる
		png_set_bgr(png_ptr);// BGR形式に変換
		
		// バイトデータ読み込み
		int nRowBytes = (nWidth * 32 + 31) / 32 * 4;
		int nImgBytes = nRowBytes * nHeight;
		m_img = mnew unsigned char[nImgBytes];
		unsigned char** ppLines = mnew unsigned char*[nHeight];
		for(int i = 0, y = nHeight-1; y >= 0; --y, ++i)
		{
			ppLines[i] = m_img + y * nRowBytes;
		}
		
		png_read_update_info(png_ptr, info_ptr);
		png_read_image(png_ptr, ppLines);
		png_read_end(png_ptr, end_info);
		
		delete [] ppLines;
		
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_info_struct(png_ptr, &end_info);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		
		m_width  = nWidth;
		m_height = nHeight;
		
		if (fp)	
			fclose(fp);
		return true;
	}
	
	static void pngReadProc(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		FILE* fp = reinterpret_cast<FILE*>( png_get_io_ptr(png_ptr) );
		fread(data, length, 1, fp);
	}
	static void pngMemReadProc(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		bufferData* buf = reinterpret_cast<bufferData*>(png_get_io_ptr(png_ptr));
		if (buf->offset + length <= buf->size)
		{
			memcpy(data, buf->dataPtr + buf->offset, length);
			buf->offset += length;
		}
		else
			png_error(png_ptr,"png_mem_read_func failed");
	}

};

#define png_cast(A)  const_cast<png_voidp>(A)

Image::Image(void) : m_imp(mnew Impl())
{
}

Image::~Image(void)
{
	delete m_imp;
}

bool Image::Load(const char* fname)
{
	return m_imp->load(fname);
}

bool Image::Load(const MOE::Stream* s)
{
	return m_imp->load(0, s->GetData(), s->GetSize());
}
    
int* Image::GetImage() const
{
	return m_imp->GetImage();
}

int Image::GetWidth() const
{
	return m_imp->GetWidth();
}

int Image::GetHeight() const
{
	return m_imp->GetHeight();
}


} // MOE
