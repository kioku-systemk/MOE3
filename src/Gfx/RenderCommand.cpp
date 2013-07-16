#include "../Core/MOE.h"
#include "../Core/Math.h"
#include "Graphics.h"
#include "BufferObject.h"
#include "RenderCommand.h"
#include "TextureObject.h"
#include "ShaderProgramObject.h"

#include <vector>
#include <string>

#include <algorithm>
#include <assert.h>

//#define DEBUG_RCMD

#ifdef DEBUG_RCMD
#include "../Core/Log.h"
#endif

namespace {
		
class RCommand
{
public:
		
	enum RCommandType
	{
		CMD_UNKNOWN = 0,
		CMD_CLEARCOLOR,
		CMD_CLEAR,
		CMD_BIND_PROGRAM,
		CMD_BIND_VERTEXBUFFER,
		CMD_BIND_INDEXBUFFER,
		CMD_BIND_TEXTURE,
		CMD_UNBIND_VERTEXBUFFER,
		CMD_UNBIND_INDEXBUFFER,
		CMD_DRAWARRAYS,
		CMD_DRAWELEMENTS,
		CMD_UNIFORM_MATRIX,
		CMD_UNIFORM_VEC4
	};
	RCommand(RCommandType type, void* arg, MOE::ProgramObject* prg, u32 counter)
	{
		m_type = type;
		m_arg = arg;
		m_prg = prg;
		m_counter = counter;
	}
	
	b8 operator<(const RCommand& r) const
	{
		const s32 lprio = m_prg   ? m_prg->GetPriority()   : 0;
		const u32 lid   = m_prg   ? m_prg->GetID()         : 0;
		const s32 rprio = r.m_prg ? r.m_prg->GetPriority() : 0;
		const u32 rid   = r.m_prg ? r.m_prg->GetID()       : 0;
#if 1
		if (lprio != rprio)           return lprio     < rprio;
		if (lid   != rid)             return lid       < rid;
		return m_counter < r.m_counter;
#elif 0
		if ((!m_prg) || (!r.m_prg)) {
			return m_counter < r.m_counter;
		} else {
			const b8 bprio = lprio < rprio;
			return (bprio)
			||     (!bprio && lid < rid)
			||     (!bprio && lid >= rid && m_counter < r.m_counter);						
		}
#else
		const b8 bprio = lprio < rprio;
		return (bprio)
		||     (!bprio && lid < rid)
		||     (!bprio && lid >= rid && m_counter < r.m_counter);
#endif
	}
	
	RCommandType m_type;
	void* m_arg;
	MOE::ProgramObject* m_prg;
	u32 m_counter;

};
	
} // namespace

namespace MOE {
union RenderCommand::CommandArg{
	enum { UNIFORM_NAME_MAX = 32 };
	struct {
		f32 red, green, blue, alpha;
	} ClearColor;
	struct {
		VGenum primitive;
		s32 vertexCount;
		s32 start;
	} DrawArrays;
	struct {
		VGenum primitive;
		s32 indexCount;
	} DrawElements;
	struct {
		char name[UNIFORM_NAME_MAX];
		const MOE::TextureObject* tex;
	} BindTexture;
	struct {
		char name[UNIFORM_NAME_MAX];
		f32 matrix[16];
	} UniformMatrix;
	struct {
		char name[UNIFORM_NAME_MAX];
		f32 vec[4];
	} UniformVec4;
};

class RenderCommand::CommandQ
{
private:
	std::vector<RCommand> q;
public:
	CommandQ() {}
	~CommandQ(){}
	
	void Push(const RCommand& cmd){ q.push_back(cmd); }
	void Clear()                  { q.clear();        }
	u32 Count()                   { return static_cast<u32>(q.size());  }
	const RCommand& Get(s32 i)    { return q[i];      }
	void Sort()
	{
		std::sort(q.begin(), q.end());
	}
#ifdef DEBUG_RCMD
	void Dump()
	{
		static const char* cmdname[] =
		{
			"RCMD:UNKNOWN",
			"RCMD:CLEARCOLOR",
			"RCMD:CLEAR",
			"RCMD:BIND_PROGRAM",
			"RCMD:BIND_VERTEXBUFFER",
			"RCMD:BIND_INDEXBUFFER",
			"RCMD:BIND_TEXTURE",
			"RCMD:UNBIND_VERTEXBUFFER",
			"RCMD:UNBIND_INDEXBUFFER",
			"RCMD:DRAWARRAYS",
			"RCMD:DRAWELEMENTS",
			"RCMD:UNIFORM_MATRIX",
			"RCMD:UNIFORM_VEC4"
		};
		MOEVector<RCommand>::const_iterator it,eit = q.end();
		for (it = q.begin(); it != eit; ++it)
		{
			MOELogD("%30s - PRG=%d - CNT=%d", cmdname[it->m_type], it->m_prg, it->m_counter);
		}
	}
#endif
};
	
class RenderCommand::CommandArgPool
{
private:
	std::vector<CommandArg*> pools;
	s32 m_cnt;
	s32 m_argnum;
	enum{
		def_argnum = 4096
	};
public:
	CommandArgPool(s32 default_argnum = def_argnum) {
		m_cnt = 0;
		m_argnum = default_argnum;
		pools.push_back(mnew CommandArg[m_argnum]);
	}
	~CommandArgPool()
	{
		const size_t n = pools.size();
		for (size_t i = 0; i < n; ++i)
			delete [] pools[i];
		pools.clear();
	}
	void Clear()     { m_cnt = 0; }
	CommandArg* Get(){
		if (m_cnt >= static_cast<s32>(pools.size())*def_argnum)
			pools.push_back(mnew CommandArg[m_argnum]);
		CommandArg* carg = &pools[m_cnt/m_argnum][m_cnt%m_argnum];
		m_cnt++;
		return carg;
	}
};
	
RenderCommand::RenderCommand(Graphics* mg)
{
	g = mg;
	m_q = mnew CommandQ();
	m_argpool = mnew CommandArgPool();
	m_qCounter = 0;
	m_q_prg = 0;
	
	m_binding_prg = 0;
	m_binding_vb  = 0;
	m_binding_ib  = 0;
	m_binding_tex = 0;
}
	
RenderCommand::~RenderCommand()
{
	delete m_q;
	delete m_argpool;
}
	
void RenderCommand::Clear(s32 cf)
{
	RCommand cmd(RCommand::CMD_CLEAR, reinterpret_cast<void*>(cf), m_q_prg, m_qCounter++);
	m_q->Push(cmd);
}

void RenderCommand::ClearColor(f32 r, f32 g, f32 b, f32 a)
{
	CommandArg* arg = m_argpool->Get();
	arg->ClearColor.red   = r;
	arg->ClearColor.green = g;
	arg->ClearColor.blue  = b;
	arg->ClearColor.alpha = a;
	const RCommand cmd(RCommand::CMD_CLEARCOLOR, arg, m_q_prg, m_qCounter++);
	m_q->Push(cmd);
}

void RenderCommand::BindProgram(ProgramObject* pg)
{
	const RCommand cmd(RCommand::CMD_BIND_PROGRAM, reinterpret_cast<CommandArg*>(pg), pg, m_qCounter++);
	m_q->Push(cmd);
	m_q_prg = pg;
}

void RenderCommand::BindTex(const s8* name, TextureObject* tex)
{
	CommandArg* arg = m_argpool->Get();
	strncpy(arg->UniformVec4.name, name, CommandArg::UNIFORM_NAME_MAX);
	arg->BindTexture.tex  = tex;
	const RCommand cmd(RCommand::CMD_BIND_TEXTURE, reinterpret_cast<CommandArg*>(arg), m_q_prg, m_qCounter++);
	m_q->Push(cmd);	
}
	
void RenderCommand::BindVB(VertexBuffer* vb)
{
	const RCommand cmd(RCommand::CMD_BIND_VERTEXBUFFER, reinterpret_cast<CommandArg*>(vb), m_q_prg, m_qCounter++);
	m_q->Push(cmd);
}

void RenderCommand::BindIB(IndexBuffer* ib)
{
	const RCommand cmd(RCommand::CMD_BIND_INDEXBUFFER, reinterpret_cast<CommandArg*>(ib), m_q_prg, m_qCounter++);
	m_q->Push(cmd);	
}
	
void RenderCommand::UnbindVB(VertexBuffer* vb)
{
	const RCommand cmd(RCommand::CMD_UNBIND_VERTEXBUFFER, reinterpret_cast<CommandArg*>(vb), m_q_prg, m_qCounter++);
	m_q->Push(cmd);
}

void RenderCommand::UnbindIB(IndexBuffer* ib)
{
	const RCommand cmd(RCommand::CMD_UNBIND_INDEXBUFFER, reinterpret_cast<CommandArg*>(ib), m_q_prg, m_qCounter++);
	m_q->Push(cmd);
}

void RenderCommand::SetUniform(const char* name, const Math::matrix& mat)
{
	CommandArg* arg = m_argpool->Get();
	strncpy(arg->UniformVec4.name, name, CommandArg::UNIFORM_NAME_MAX);
	memcpy(arg->UniformMatrix.matrix, &mat, sizeof(arg->UniformMatrix.matrix));
	const RCommand cmd(RCommand::CMD_UNIFORM_MATRIX, arg, m_q_prg, m_qCounter++);
	m_q->Push(cmd);	
}

void RenderCommand::SetUniform(const char* name, const Math::vec4& vec)
{
	CommandArg* arg = m_argpool->Get();
	strncpy(arg->UniformVec4.name, name, CommandArg::UNIFORM_NAME_MAX);
	memcpy(arg->UniformVec4.vec, &vec, sizeof(arg->UniformVec4.vec));
	const RCommand cmd(RCommand::CMD_UNIFORM_VEC4, arg, m_q_prg, m_qCounter++);
	m_q->Push(cmd);		
}
	
void RenderCommand::DrawArrays(PrimitiveType mode, s32 first, s32 vertexCount)
{
	CommandArg* arg = m_argpool->Get();
	arg->DrawArrays.primitive = mode;
	arg->DrawArrays.start = first;
	arg->DrawArrays.vertexCount = vertexCount;
	const RCommand cmd(RCommand::CMD_DRAWARRAYS, arg, m_q_prg, m_qCounter++);
	m_q->Push(cmd);		
}

void RenderCommand::DrawElements(PrimitiveType mode, s32 indexCount)
{
	CommandArg* arg = m_argpool->Get();
	arg->DrawElements.primitive = mode;
	arg->DrawElements.indexCount = indexCount;
	const RCommand cmd(RCommand::CMD_DRAWELEMENTS, arg, m_q_prg, m_qCounter++);
	m_q->Push(cmd);		
	
}

void RenderCommand::CmdUnknown(CommandArg* arg)
{
	assert("Unknown Command");
}
	
void RenderCommand::CmdClear(CommandArg* arg)
{
	g->Clear(static_cast<u32>(reinterpret_cast<size_t>(arg)));
}

void RenderCommand::CmdClearColor(CommandArg* arg)
{
	g->ClearColor(arg->ClearColor.red, arg->ClearColor.green, arg->ClearColor.blue, arg->ClearColor.alpha);
}

void RenderCommand::CmdBindProgram(CommandArg* arg)
{
	ProgramObject* p = reinterpret_cast<ProgramObject*>(arg);
	if (m_binding_prg != p) {
#ifdef DEBUG_RCMD
		if (!p)
			p = p;
		MOELogD("RCMD:->Changed Program->0x%X",p);
#endif
		if (p)
			p->Bind();
		else
			g->UseProgram(0);

		for (s32 i = 0; i < 8; ++i)
			g->DisableVertexAttribArray(i);

		m_binding_prg = p;
		m_binding_tex = 0;
	}
}
void RenderCommand::CmdBindVertex(CommandArg* arg)
{
	VertexBuffer* vb = reinterpret_cast<VertexBuffer*>(arg);
	if (m_binding_vb != vb) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Changed VB");
#endif
		vb->Bind();
		m_binding_vb = vb;
	}
}

void RenderCommand::CmdBindIndex(CommandArg* arg)
{
	IndexBuffer* ib = reinterpret_cast<IndexBuffer*>(arg);
	if (m_binding_ib != ib) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Changed IB->0x%X",ib);
#endif
		ib->Bind();
		m_binding_ib = ib;
	}
}

void RenderCommand::CmdUnbindVertex(CommandArg* arg)
{
	assert(arg);
	VertexBuffer* vb = reinterpret_cast<VertexBuffer*>(arg);
	if (m_binding_vb != vb) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Unbind VB->0x%X",vb);
#endif
		vb->Unbind();
		m_binding_vb = 0;
	}
}

void RenderCommand::CmdUnbindIndex(CommandArg* arg)
{
	assert(arg);
	IndexBuffer* ib = reinterpret_cast<IndexBuffer*>(arg);
	if (m_binding_ib != ib) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Unbind IB->0x%X",ib);
#endif
		ib->Unbind();
		m_binding_ib = 0;
	}
}

void RenderCommand::CmdBindTex(CommandArg* arg)
{
	const MOE::TextureObject* t = arg->BindTexture.tex;
	if (m_binding_tex != t) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Changed TEX");
#endif
		if (t)
			g->BindTexture(VG_TEXTURE_2D, t->GetTexture());
		else
			g->BindTexture(VG_TEXTURE_2D, 0);
		if (m_binding_prg)
			m_binding_prg->SetUniform(arg->BindTexture.name, 0);
		m_binding_tex = t;
	}
}
void RenderCommand::CmdDrawArrays(CommandArg* arg)
{
	g->DrawArrays(arg->DrawArrays.primitive, arg->DrawArrays.start, arg->DrawArrays.vertexCount);
}
void RenderCommand::CmdDrawElements(CommandArg* arg)
{
	if (m_binding_ib)
		g->DrawElements(arg->DrawElements.primitive, arg->DrawElements.indexCount, m_binding_ib->GetIndexType(), 0);
}

void RenderCommand::CmdUniformMatrix(CommandArg* arg)
{
	if(m_binding_prg) {
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Unifrom MATRIX(%s)", arg->UniformMatrix.name);
#endif
		m_binding_prg->SetUniformMatrix4x4(arg->UniformMatrix.name, 1, VG_FALSE, arg->UniformMatrix.matrix);
	}
}

void RenderCommand::CmdUniformVec4(CommandArg* arg)
{
	if(m_binding_prg) {
		const MOE::Math::vec4* v = reinterpret_cast<const MOE::Math::vec4*>(arg->UniformVec4.vec);
#ifdef DEBUG_RCMD
		MOELogD("RCMD:->Unifrom VEC4(%s)", arg->UniformVec4.name);
#endif
		m_binding_prg->SetUniform(arg->UniformVec4.name, v->x, v->y, v->z, v->w);
	}
}

void RenderCommand::CommandExecute()
{
	// sort
	m_q->Sort();
	//m_q->Dump();
	
	// execute
	static void (RenderCommand::*const funcs[])(CommandArg*) = {
		0,
		&RenderCommand::CmdClearColor,
		&RenderCommand::CmdClear,
		&RenderCommand::CmdBindProgram,
		&RenderCommand::CmdBindVertex,
		&RenderCommand::CmdBindIndex,
		&RenderCommand::CmdBindTex,
		&RenderCommand::CmdUnbindVertex,
		&RenderCommand::CmdUnbindIndex,
		&RenderCommand::CmdDrawArrays,
		&RenderCommand::CmdDrawElements,
		&RenderCommand::CmdUniformMatrix,
		&RenderCommand::CmdUniformVec4
	};
#ifdef DEBUG_RCMD
	static const char* cmdname[] = 
	{
		"RCMD:UNKNOWN",
		"RCMD:CLEARCOLOR",
		"RCMD:CLEAR",
		"RCMD:BIND_PROGRAM",
		"RCMD:BIND_VERTEXBUFFER",
		"RCMD:BIND_INDEXBUFFER",
		"RCMD:BIND_TEXTURE",
		"RCMD:UNBIND_VERTEXBUFFER",
		"RCMD:UNBIND_INDEXBUFFER",
		"RCMD:DRAWARRAYS",
		"RCMD:DRAWELEMENTS",
		"RCMD:UNIFORM_MATRIX",
		"RCMD:UNIFORM_VEC4"
	};
	MOELogD("----------------CommandExecute-------------------");
#endif
	s32 n = m_q->Count();
	for (s32 i = 0; i < n; ++i)
	{
		const RCommand& cmd = m_q->Get(i);
#ifdef DEBUG_RCMD
		MOELogD("%s",cmdname[cmd.m_type]);
		if (cmd.m_type != RCommand::CMD_BIND_PROGRAM)
			assert(cmd.m_prg == m_binding_prg); // If here, sort has bug!!!
#endif
		(this->*funcs[cmd.m_type])(static_cast<RenderCommand::CommandArg*>(cmd.m_arg));
	}
	clearQ();
	m_argpool->Clear();
	// force unbind
	m_binding_prg = 0;
	m_binding_ib  = 0;
	m_binding_vb  = 0;
	m_binding_tex = 0;
}
	
void RenderCommand::clearQ()
{
	m_q->Clear();
	m_qCounter = 0;
	m_q_prg = 0;
}

ProgramObject* RenderCommand::GetProgram() const
{
	return m_q_prg;
}

} // namespace MOE
