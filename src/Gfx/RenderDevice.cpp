
#include "../Core/MOE.h"
#include "Graphics.h"
#include "../SceneGraph/SceneGraph.h"
#include "../Core/Math.h"
#include "ShaderProgramObject.h"

#include "RenderDevice.h"
#include <algorithm>
#include <vector>

using namespace MOE;
using namespace MOE::Math;

namespace  {
	
enum DRAWMODE
{
	MODE_POLYGON = 0,
	MODE_POLYGONLINE,
	MODE_LINES,
	MODE_POINTS,
};

//#define CMD_DEBUG
	
static void CommandDrawCustom(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject* prg, DRAWMODE mode)
{
	assert(prg);
	if (mode == MODE_POLYGONLINE)
		g->PolygonMode(VG_FRONT_AND_BACK, VG_LINE);

	const u32 buf[] = {arg->triarg.m_vb, arg->triarg.m_ib};
	g->BindBuffer(VG_ARRAY_BUFFER, buf[0]);
	g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, buf[1]);
	
	const s32 pos_att = prg->GetAttribLocation("pos");
	const s32 col_att = prg->GetAttribLocation("col");
	const s32 nor_att = prg->GetAttribLocation("nor");
	//const s32 idx_att = prg->GetAttribLocation("idx");
	g->EnableVertexAttribArray(pos_att);
	g->VertexAttribPointer(pos_att, 3, VG_FLOAT, VG_FALSE, sizeof(SceneGraph::Geometry::VertexFormat), 0);
	if (col_att >= 0)
	{
		g->EnableVertexAttribArray(col_att);
		g->VertexAttribPointer(col_att, 4, VG_UNSIGNED_BYTE, VG_TRUE, sizeof(SceneGraph::Geometry::VertexFormat), (const void*)(sizeof(f32)*3));
	}
	if (nor_att >= 0)
	{
		g->EnableVertexAttribArray(nor_att);
		g->VertexAttribPointer(nor_att, 3, VG_FLOAT, VG_FALSE, sizeof(SceneGraph::Geometry::VertexFormat), (const void*)(sizeof(f32)*4));
	}
	/*if (idx_att >= 0)
	{
		g->EnableVertexAttribArray(idx_att);
		g->VertexAttribPointer(idx_att, 1, VG_UNSIGNED_INT, VG_FALSE, sizeof(SceneGraph::Geometry::VertexFormat), (const void*)(sizeof(f32)*7));
	}*/

	const u32 indexcount = arg->triarg.m_indexCount;
	static const u32 ElementMode[] = {
		VG_TRIANGLES,
		VG_TRIANGLES,
		VG_LINES,
		VG_POINTS
	};
	g->DrawElements(ElementMode[mode], indexcount, VG_UNSIGNED_INT, 0);
	g->BindBuffer(VG_ARRAY_BUFFER, 0);
	g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);

	if (mode == MODE_POLYGONLINE)
		g->PolygonMode(VG_FRONT_AND_BACK, GL_FILL);
}
		
static void CommandDrawTriangle(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandDrawTriangle(prg = %p)\n",prg);
#endif
	CommandDrawCustom(g, arg, prg, MODE_POLYGON);
}
static void CommandDrawTriangleLine(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandDrawTriangleLine(prg = %p)\n",prg);
#endif
	CommandDrawCustom(g, arg, prg, MODE_POLYGONLINE);
}

static void CommandDrawLines(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandDrawLines(prg = %p)\n",prg);
#endif
	CommandDrawCustom(g, arg, prg, MODE_LINES);
}

static void CommandDrawPoints(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandDrawPoints(prg = %p)\n",prg);
#endif
	CommandDrawCustom(g, arg, prg, MODE_POINTS);
}

static void CommandSetMatrix(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandSetMatrix(prg = %p)\n",prg);
#endif
	assert(prg);
	prg->SetUniformMatrix4x4(arg->m_name.c_str(), 1, VG_FALSE, &arg->m_mat.f[0]);
}
	
static void CommandSetFloat(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandSetFloat(prg = %p)\n",prg);
#endif
	assert(prg);
	prg->SetUniform(arg->m_name.c_str(), arg->m_vec.x);
}
	
static void CommandSetVec4(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandSetVec4(prg = %p)\n",prg);
#endif
	assert(prg);
	prg->SetUniform(arg->m_name.c_str(), arg->m_vec.x, arg->m_vec.y, arg->m_vec.z, arg->m_vec.w);
}

static void CommandSetVec4Array(Graphics* g, RenderDevice::CommandArg* arg, ProgramObject*& prg)
{
#ifdef CMD_DEBUG
	MOELogD("CommandSetVec4Array(prg = %p)\n",prg);
#endif
	assert(prg);
//	prg->SetUniformArray4f(arg->m_name.c_str(), arg->m_vecnum, reinterpret_cast<const float*>(arg->m_vec4array));
}

} // namespace

namespace MOE {
	
	RenderDevice::RenderDevice(MOE::Graphics* mg)
	{
		g = mg;
		m_bufferID = 0;
		m_bindingShader = 0;
		
		m_tempVB = 0;
		m_tempIB = 0;
		
		m_cmdcounter = 0;
		m_cmdarg.resize(MAX_COMMANDARG_SIZE);
		m_cmdargCnt = 0;
	}
	RenderDevice::~RenderDevice()
	{
		mdeleteArray(m_tempVB);
		mdeleteArray(m_tempIB);
	}
	
	RenderDevice::CommandArg* RenderDevice::GetCommandArg()
	{
		if (m_cmdargCnt >= MAX_COMMANDARG_SIZE)
		{
			assert(0); // many args error !!
			return 0;
		}

		return &m_cmdarg[m_cmdargCnt++];
	}

	u32 RenderDevice::WriteBuffer(const void* vertex, u32 vertex_num, const void* index, u32 index_num, const u32* idBuffer, u32 bufferID)
	{
		u32 tbufferID = bufferID ? bufferID : ++m_bufferID; // TODO:new ID
		
		const u32 maxIndexCount = 600000;
		if (!m_tempVB)
			m_tempVB = mnew SceneGraph::Geometry::VertexFormat[maxIndexCount];
		if (!m_tempIB)
			m_tempIB = mnew SceneGraph::Index[maxIndexCount];
		
		if (index_num < maxIndexCount)
		{
			u32 buf[2];
			if (!bufferID)
			{
				g->GenBuffers(2, buf);
			}
			else // update
			{
				const u32 bufNum = static_cast<u32>(m_buffers[tbufferID].size());
				if (bufNum != 1) // resize
				{
					for (u32 b = 1; b < bufNum; ++b)
						g->DeleteBuffers(2, m_buffers[tbufferID][b].vbib);
					m_buffers[tbufferID].resize(1);
				}

				tbufferID = bufferID;
				buf[0] = m_buffers[tbufferID][0].vbib[0];
				buf[1] = m_buffers[tbufferID][0].vbib[1];
				m_buffers[bufferID][0].indexCount = index_num;
			}
			
			if (idBuffer) {
				// set id/selection
				for (u32 i = 0; i < index_num; ++i)
				{
					const MOE::SceneGraph::Index* idx = static_cast<const MOE::SceneGraph::Index*>(index);
					m_tempVB[i] = static_cast<const SceneGraph::Geometry::VertexFormat*>(vertex)[idx[i]];
					const u32 bufidx = i/3;
					m_tempVB[i].col = idBuffer[bufidx];
					m_tempIB[i] = i;
				}
				g->BindBuffer(VG_ARRAY_BUFFER, buf[0]);
				g->BufferData(VG_ARRAY_BUFFER, index_num * sizeof(SceneGraph::Geometry::VertexFormat), m_tempVB, VG_STATIC_DRAW);
				g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, buf[1]);
				g->BufferData(VG_ELEMENT_ARRAY_BUFFER, index_num * sizeof(u32), m_tempIB, VG_STATIC_DRAW);
			} else {
				g->BindBuffer(VG_ARRAY_BUFFER, buf[0]);
				g->BufferData(VG_ARRAY_BUFFER, vertex_num * sizeof(SceneGraph::Geometry::VertexFormat), vertex, VG_STATIC_DRAW);
				g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, buf[1]);
				g->BufferData(VG_ELEMENT_ARRAY_BUFFER, index_num * sizeof(u32), index, VG_STATIC_DRAW);			
			}
			g->BindBuffer(VG_ARRAY_BUFFER, 0);
			g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);			

			if (!bufferID)
				m_buffers[tbufferID].push_back(VBIB(buf[0],buf[1],index_num));

			//printf("Created Buffer(%d)=(VB:%d IB:%d)\n", tbufferID, vertex_num, index_num);
			return tbufferID;
		}
		
		// ---------------------------------
		// over maxIndexCount
		std::vector<u32> idxnums;
		u32 maxnum = index_num/maxIndexCount;
				
		if (maxnum)
			idxnums.assign(maxnum, maxIndexCount);
		u32 modnum = index_num % maxIndexCount;
		if (modnum)
			idxnums.push_back(modnum);
	
		if (bufferID)
		{
			u32 newBufnum = maxnum + 1;
			u32 oldBufnum = static_cast<u32>(m_buffers[bufferID].size());
			if (oldBufnum != newBufnum) // Reindexing buffer
			{
				if (oldBufnum > newBufnum)
				{
					for (u32 b = oldBufnum - 1; b >= newBufnum; --b)
						g->DeleteBuffers(2, m_buffers[bufferID][b].vbib);
					m_buffers[bufferID].resize(newBufnum);
					
				}
				else
				{
					m_buffers[bufferID].resize(newBufnum);
					for (u32 b = oldBufnum; b < newBufnum; ++b)
						g->GenBuffers(2, m_buffers[bufferID][b].vbib);
					
					// new indexnum
					for (u32 b = 0; b < newBufnum - 1; ++b)
						m_buffers[bufferID][b].indexCount = maxnum;
				}	
			}
			// last new indexnum
			m_buffers[bufferID][newBufnum - 1].indexCount = modnum;
		}
		
		const u32 cacheIndexSize = 64; // reindexing vertex cache size
		std::pair<u32,u32> vbib_cache[cacheIndexSize]={};
		u32 vbib_cache_ptr = 0;
		u32 idxmaxnum = static_cast<u32>(idxnums.size());
		u32 baseIndex = 0;
		for (u32 idx = 0; idx < idxmaxnum; idx++)
		{
			// --- ReIndexing vertex/index buffer ---
			for (u32 ch = 0; ch < cacheIndexSize; ch++)
				vbib_cache[ch] = std::pair<u32,u32>(-1, -1);
			
			// copy temp buffer
			u32 vcount = 0;
			const u32 inum = idxnums[idx];
			for (u32 i = 0; i < inum; i++)
			{
				const SceneGraph::Index realIndex = (static_cast<const SceneGraph::Index*>(index))[i + baseIndex];
				u32 triid;
				if (idBuffer)
					triid = idBuffer[(baseIndex + i)/3];
				else
					triid = static_cast<const SceneGraph::Geometry::VertexFormat*>(vertex)[realIndex].col;
				
				// cache search
				u32 c;
				for (c = 0; c < cacheIndexSize; c++) {
					if (vbib_cache[c].first == realIndex
					&&  m_tempVB[vbib_cache[c].second].col == triid)
						break;
				}
				
				if (c == cacheIndexSize) // not found
				{
					m_tempIB[i] = vcount;
					m_tempVB[vcount] = static_cast<const SceneGraph::Geometry::VertexFormat*>(vertex)[realIndex];
					m_tempVB[vcount].col = triid;
					
					// cacheing
					const std::pair<u32,u32> cache(realIndex, vcount);
					vbib_cache[vbib_cache_ptr] = cache;
					vbib_cache_ptr++; vbib_cache_ptr &= (cacheIndexSize - 1);
					vcount++;
				}
				else // found
				{
					m_tempIB[i] = vbib_cache[c].second;
				}
			}
			baseIndex += inum;
			// --------------
			
			// create buffer
			u32 buf[2];
			if (!bufferID){
				g->GenBuffers(2, buf);
			} else {
				std::vector<VBIB>& bbb = m_buffers[tbufferID];
				buf[0] = bbb[idx].vbib[0];
				buf[1] = bbb[idx].vbib[1];
			}
			g->BindBuffer(VG_ARRAY_BUFFER, buf[0]);
			g->BufferData(VG_ARRAY_BUFFER, vcount * sizeof(SceneGraph::Geometry::VertexFormat), m_tempVB, VG_STATIC_DRAW);
			g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, buf[1]);
			g->BufferData(VG_ELEMENT_ARRAY_BUFFER, inum * sizeof(u32), m_tempIB, VG_STATIC_DRAW);
			if (!bufferID)
				m_buffers[tbufferID].push_back(VBIB(buf[0],buf[1],inum));
		}
		g->BindBuffer(VG_ARRAY_BUFFER, 0);
		g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);
		
		//printf("Created Buffer(%d)=(VB:%d IB:%d)\n", tbufferID, vertex_num, index_num);
		return tbufferID;
	}

	void RenderDevice::DeleteBuffer(u32 bufferId)
	{
		//printf("delete buffer = %d\n", m_buffers.size());
		std::map<s32, std::vector<VBIB> >::iterator it = m_buffers.find(bufferId);
		if (it != m_buffers.end())
		{
			u32 n = static_cast<u32>(it->second.size());
			for (u32 i = 0; i < n; i++)
				g->DeleteBuffers(2, it->second[i].vbib);
			m_buffers.erase(it);
		}
		//printf("deleted buffer = %d\n", m_buffers.size());
	}
	void RenderDevice::QRenderTriangle(u32 bufferId)
	{
		std::map<s32, std::vector<VBIB> >::iterator it = m_buffers.find(bufferId);
		if (it != m_buffers.end())
		{
			u32 n = static_cast<u32>(it->second.size());
			for (u32 i = 0; i < n; i++)
			{
				const u32 drawIndexNum = it->second[i].indexCount;
				CommandArg* arg = GetCommandArg();
				arg->TriangleLineArg(it->second[i].vbib[0],it->second[i].vbib[1],drawIndexNum);
				m_cmd.push_back(RenderCommand(RenderCommand::CMD_RENDER_TRIANGLE, arg, m_bindingShader, m_cmdcounter++));
			}
		}
	}
	void RenderDevice::QRenderTriangleLine(u32 bufferId)
	{
		std::map<s32, std::vector<VBIB> >::iterator it = m_buffers.find(bufferId);
		if (it != m_buffers.end())
		{
			u32 n = static_cast<u32>(it->second.size());
			for (u32 i = 0; i < n; i++)
			{
				const u32 drawIndexNum = it->second[i].indexCount;
				CommandArg* arg = GetCommandArg();
				arg->TriangleLineArg(it->second[i].vbib[0],it->second[i].vbib[1],drawIndexNum);
				m_cmd.push_back(RenderCommand(RenderCommand::CMD_RENDER_TRIANGLE_LINE, arg, m_bindingShader, m_cmdcounter++));
			}
		}
	}
	void RenderDevice::QRenderLines(u32 bufferId)
	{
		std::map<s32, std::vector<VBIB> >::iterator it = m_buffers.find(bufferId);
		if (it != m_buffers.end())
		{
			u32 n = static_cast<u32>(it->second.size());
			for (u32 i = 0; i < n; i++)
			{
				const u32 drawIndexNum = it->second[i].indexCount;
				CommandArg* arg = GetCommandArg();
				arg->TriangleLineArg(it->second[i].vbib[0],it->second[i].vbib[1],drawIndexNum);
				m_cmd.push_back(RenderCommand(RenderCommand::CMD_RENDER_LINES, arg, m_bindingShader, m_cmdcounter++));
			}
		}
	}
	void RenderDevice::QRenderPoints(u32 bufferId)
	{
		assert(0); // TODO
	}

	
	void RenderDevice::QMatrix(const s8* name, const MOE::Math::matrix4x4& mat)
	{
		CommandArg* arg = GetCommandArg();
		arg->Matrix4Arg(name, mat);
		m_cmd.push_back(RenderCommand(RenderCommand::CMD_MATRIX, arg, m_bindingShader, m_cmdcounter++));
	}

	void RenderDevice::QFloat(const s8* name, const float f)
	{
		CommandArg* arg = GetCommandArg();
		arg->FloatArg(name, f);
		m_cmd.push_back(RenderCommand(RenderCommand::CMD_FLOAT, arg, m_bindingShader, m_cmdcounter++));
	}

	void RenderDevice::QVec4(const s8* name, const MOE::Math::vec4& v)
	{
		CommandArg* arg = GetCommandArg();
		arg->Vec4Arg(name, v);
		m_cmd.push_back(RenderCommand(RenderCommand::CMD_VEC4, arg, m_bindingShader, m_cmdcounter++));
	}
	
	void RenderDevice::QVec4Array(const s8* name, const MOE::Math::vec4* v, s32 vecnum)
	{
		CommandArg* arg = GetCommandArg();
		arg->Vec4ArrayArg(name, v, vecnum);
		m_cmd.push_back(RenderCommand(RenderCommand::CMD_VEC4ARRAY, arg, m_bindingShader, m_cmdcounter++));
	}

	
	void RenderDevice::QBindShader(MOE::ProgramObject* prg)
	{
		m_bindingShader = prg;
	}
	void RenderDevice::QUnbindShader()
	{
		m_bindingShader = 0;
	}

	
	void RenderDevice::FlashCommands()
	{
		std::sort(m_cmd.begin(), m_cmd.end());
		
		static void (*commands[])(Graphics*,CommandArg*,ProgramObject*&) = {
			0,
			CommandDrawTriangle,      // CMD_RENDER_TRIANGLE
			CommandDrawTriangleLine,  // CMD_RENDER_TRIANGLE_LINE
			CommandDrawLines,         // CMD_RENDER_LINES
			CommandDrawPoints,        // CMD_RENDER_POINTS
			CommandSetMatrix,         // CMD_MATRIX
			CommandSetFloat,          // CMD_FLOAT
			CommandSetVec4,           // CMD_VEC4
			CommandSetVec4Array       // CMD_VEC4ARRAY
		};
#ifdef CMD_DEBUG
		MOELogD("-----FlashCommands-----\n");
#endif
		g->Enable(VG_BLEND);
		g->BlendFunc(VG_SRC_ALPHA, VG_ONE_MINUS_SRC_ALPHA);
		
		std::vector<RenderCommand>::const_iterator it, eit = m_cmd.end();
		MOE::ProgramObject* bindingprg = 0;
		for (it = m_cmd.begin(); it != eit; ++it)
		{
			if (it->prg != bindingprg)
			{
				if (bindingprg)
					bindingprg->Unbind();
				bindingprg = it->prg;
				bindingprg->Bind();
#ifdef CMD_DEBUG
				MOELogD("-- BindNewPrg\n");
#endif
			}
			(*commands[it->cmd])(g, it->arg, bindingprg);
		}
		if (bindingprg)
			bindingprg->Unbind();
		m_cmd.clear();
		m_cmdargCnt = 0;
	}
	
} // namespace MOE

