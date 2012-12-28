/*
 *  Graphics Profiler
 *
 */
#ifndef INCLUDE_VGGRAPHICS_PROFILER_H
#define INCLUDE_VGGRAPHICS_PROFILER_H

#include "../Type.h"
#include <map>

namespace MOE {
	
class GraphicsProfiler
{
	
private:
	u32 m_vram_size;
	u32 m_vram_maxsize;
	u32 m_texture_size;
	u32 m_vertex_count;
	u32 m_vertex_size;
	u32 m_index_count;
	u32 m_index_size;
	u32 m_program_count;
	u32 m_draw_vertex_call_count;
	u32 m_draw_vertex_count;
	u32 m_draw_index_call_count;
	u32 m_draw_index_count;
	u32 m_draw_tri_call_count;
	u32 m_draw_tri_count;
	u32 m_draw_line_call_count;
	u32 m_draw_line_count;
	u32 m_draw_point_call_count;
	u32 m_draw_point_count;
	u32 m_bind_vertex_count;
	u32 m_bind_index_count;
	u32 m_bind_texture_count;
	u32 m_bind_program_count;
	u32 m_write_size;
	VGenum m_mode;
	
	class ElementInfo
	{
	public:
		ElementInfo()
		: count(0), size(0), elementsize(0) {}
		ElementInfo(const u32 count_, const u32 elementsize_)
		: count(count_), size(count_ * elementsize_), elementsize(elementsize_) {}
		u32 count;
		u32 size;
		u32 elementsize;
	};
	std::map<u32, u32> m_vertex;
	std::map<u32, u32> m_index;
	std::map<u32, ElementInfo> m_texture;

	GraphicsProfiler()
	{
		m_vram_size = 0;
		m_vram_maxsize = 0;
		m_texture_size = 0;
		m_vertex_count = 0;
		m_vertex_size = 0;
		m_index_count = 0;
		m_index_size = 0;
		m_program_count = 0;
		m_mode = VG_FILL;
		ClearDrawInfo();
	}
	
public:
	
	static GraphicsProfiler& GetInstance()
	{
		static GraphicsProfiler singleton;
		return singleton;
	}
	
	enum DrawType
	{
		TYPE_UNKOWN,
		TYPE_POINTS,
		TYPE_LINES,
		TYPE_LINE_LOOP,
		TYPE_LINE_STRIP,
		TYPE_TRIANGLES,
		TYPE_TRIANGLE_STRIP,
		TYPE_TRIANGLE_FAN
	};
	
	void ClearDrawInfo()
	{
		m_draw_vertex_call_count = 0;
		m_draw_vertex_count = 0;
		m_draw_index_call_count = 0;
		m_draw_index_count = 0;
		m_draw_tri_call_count = 0;
		m_draw_tri_count = 0;
		m_draw_line_call_count = 0;
		m_draw_line_count = 0;
		m_draw_point_call_count = 0;
		m_draw_point_count = 0;
		m_bind_vertex_count = 0;
		m_bind_index_count = 0;
		m_bind_texture_count = 0;
		m_bind_program_count = 0;
		m_write_size = 0;
	}
	
	u32 GetVramSize() const            { return m_vram_size; }
	u32 GetVramMaxSize() const         { return m_vram_maxsize; }
	u32 GetTexutreIdCount() const      { return static_cast<u32>(m_texture.size()); }
	u32 GetTextureVramSize() const     { return m_texture_size; }
	u32 GetVertexIdCount() const       { return static_cast<u32>(m_vertex.size()); }
	u32 GetVertexCount() const         { return m_vertex_count; }
	u32 GetVertexSize() const          { return m_vertex_size; }
	u32 GetIndexIdCount() const        { return static_cast<u32>(m_index.size()); }
	u32 GetIndexCount() const          { return m_index_count; }
	u32 GetIndexSize() const           { return m_index_size; }
	u32 GetDrawVertexCallCount() const { return m_draw_vertex_call_count; }
	u32 GetDrawVertexCount() const     { return m_draw_vertex_count; }
	u32 GetDrawIndexCallCount() const  { return m_draw_index_call_count; }
	u32 GetDrawIndexCount() const      { return m_draw_index_count; }
	u32 GetDrawTriCallCount() const    { return m_draw_tri_call_count; }
	u32 GetDrawTriCount() const        { return m_draw_tri_count; }
	u32 GetDrawLineCallCount() const   { return m_draw_line_call_count; }
	u32 GetDrawLineCount() const       { return m_draw_line_count; }
	u32 GetDrawPointCallCount() const  { return m_draw_point_call_count; }
	u32 GetDrawPointCount() const      { return m_draw_point_count; }
	u32 GetBindVertexCount() const     { return m_bind_vertex_count; }
	u32 GetBindIndexCount() const      { return m_bind_index_count; }
	u32 GetBindTextureCount() const    { return m_bind_texture_count; }
	u32 GetBindProgramCount() const    { return m_bind_program_count; }
	u32 GetWriteSize() const           { return m_write_size; }
	
	void WriteVertex(u32 vertexid, u32 size)
	{
		if (m_vertex.find(vertexid) != m_vertex.end())
		{
			m_vram_size -= m_vertex[vertexid];
			m_vertex_size -= m_vertex[vertexid];
		}
		
		m_vram_size += size;
		m_vertex_size += size;
		m_vertex[vertexid] = size;
		m_write_size += size;
	}
	void WriteIndex(u32 indexid, u32 size)
	{
		if (m_index.find(indexid) != m_index.end())
		{
			m_vram_size -= m_index[indexid];
			m_index_size -= m_index[indexid];
		}

		m_vram_size += size;
		m_index_size += size;
		m_index[indexid] = size;
		m_write_size += size;
	}
	void WriteTexture(u32 texid, u32 pixelnum, u32 elementsize)
	{
		const u32 size = pixelnum * elementsize;
		if (m_texture.find(texid) != m_texture.end())
		{
			m_vram_size -= m_texture[texid].size;
			m_texture_size -= m_texture[texid].size;
		}
		
		m_texture[texid] = ElementInfo(pixelnum, elementsize);
		m_vram_size += m_texture[texid].size;
		m_texture_size += m_texture[texid].size;
		
		m_write_size += size;
	}
	void WriteSubTexture(u32 texid, u32 pixelnum)
	{
		const u32 size = pixelnum * m_texture[texid].elementsize;
		m_write_size += size;
	}
	void CreateProgram(u32 num)
	{
		m_program_count += num;
	}
	void DeleteBuffer(u32 id)
	{
		if (m_index.find(id) != m_index.end())
		{
			DeleteIndex(id);
		}
		else if (m_vertex.find(id) != m_vertex.end())
		{
			DeleteVertex(id);
		}
	}
	void DeleteVertex(u32 vertexid)
	{
		const u32 vertexsize = m_vertex[vertexid];
		m_vram_size   -= vertexsize;
		m_vertex_size -= vertexsize;
		m_vertex.erase(vertexid);
	}
	void DeleteIndex(u32 indexid)
	{
		const u32 indexsize = m_index[indexid];
		m_vram_size  -= indexsize;
		m_index_size -= indexsize;
		m_index.erase(indexid);
	}
	void DeleteTexture(u32 texid)
	{
		const u32 texsize = m_texture[texid].size;
		m_vram_size    -= texsize;
		m_texture_size -= texsize;
		m_texture.erase(texid);
	}
	void DeleteProgram(u32 num)
	{
		m_program_count -= num;
	}
	void DrawVertex(u32 vertexid, u32 vertexcount, DrawType type)
	{
		m_draw_vertex_call_count++;
		m_draw_vertex_count += vertexcount;
		if (type == TYPE_TRIANGLES)
		{
			m_draw_tri_call_count++;
			m_draw_tri_count += vertexcount / 3;
		}
		else if (type == TYPE_TRIANGLE_STRIP || type == TYPE_TRIANGLE_FAN)
		{
			m_draw_tri_call_count++;
			m_draw_tri_count += (vertexcount - 2);
		}
		else if (type == TYPE_LINE_LOOP || type == TYPE_LINE_STRIP)
		{
			m_draw_line_call_count++;
			m_draw_line_count += vertexcount - (type - TYPE_LINE_LOOP);
		}
		else if (type == TYPE_POINTS)
		{
			m_draw_point_call_count++;
			m_draw_point_count += vertexcount;
		}
	}
	void DrawIndex(u32 indexid, u32 indexcount, DrawType type)
	{
		m_draw_index_call_count++;
		m_draw_index_count += indexcount;
		if (m_mode == VG_FILL)
		{
			if (type == TYPE_TRIANGLES)
			{
				m_draw_tri_call_count++;
				m_draw_tri_count += indexcount / 3;
			}
			else if (type == TYPE_TRIANGLE_STRIP || type == TYPE_TRIANGLE_FAN)
			{
				m_draw_tri_call_count++;
				m_draw_tri_count += (indexcount - 2);
			}
		}
		else if (m_mode == VG_LINE)
		{
			if (type == TYPE_TRIANGLES)
			{
				m_draw_line_call_count++;
				m_draw_line_count += indexcount / 3;
			}
			else if (type == TYPE_TRIANGLE_STRIP || type == TYPE_TRIANGLE_FAN)
			{
				m_draw_line_call_count++;
				m_draw_line_count += (indexcount - 2);
			}
		}
		else if (m_mode == VG_POINT)
		{
			if (type == TYPE_TRIANGLES)
			{
				m_draw_point_call_count++;
				m_draw_point_count += indexcount / 3;
			}
			else if (type == TYPE_TRIANGLE_STRIP || type == TYPE_TRIANGLE_FAN)
			{
				m_draw_point_call_count++;
				m_draw_point_count += (indexcount - 2);
			}
		}
		
		if (type == TYPE_LINE_LOOP || type == TYPE_LINE_STRIP)
		{
			m_draw_line_call_count++;
			m_draw_line_count += indexcount - (type - TYPE_LINE_LOOP);
		}
		else if (type == TYPE_POINTS)
		{
			m_draw_point_call_count++;
			m_draw_point_count += indexcount;
		}
	}
	void BindVertex(u32 vertexid)
	{
		if (vertexid)
			m_bind_vertex_count++;
	}
	void BindIndex(u32 indexid)
	{
		if (indexid)
			m_bind_index_count++;
	}
	void BindTexture(u32 texid)
	{
		if (texid)
			m_bind_texture_count++;
	}
	void BindProgram(u32 programid)
	{
		if (programid)
			m_bind_program_count++;
	}
	void PolygonMode(VGenum mode)
	{
		m_mode = mode;
	}
	 
};
	
} // namespace MOE
#endif

