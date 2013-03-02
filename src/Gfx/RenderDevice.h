
#include "../Core/Type.h"
#include "../Core/Math.h"

#include <vector>

namespace MOE {
class Graphics;
class ProgramObject;
	
class RenderDevice
{
public:
	
	class CommandArg
	{
	public:
		CommandArg(){}
		std::string m_name;
		//union {
			MOE::Math::vec4   m_vec;
			MOE::Math::matrix m_mat;
			struct {
				u32 m_vb;
				u32 m_ib;
				u32 m_indexCount;
			} triarg;
			const MOE::Math::vec4* m_vec4array;
			s32 m_vecnum;
		//};
		void TriangleLineArg(u32 vb, u32 ib, u32 indexcount)
		{
			triarg.m_vb = vb;
			triarg.m_ib = ib;
			triarg.m_indexCount = indexcount;
		}
		void FloatArg(const s8* name, const float f)
		{
			m_name  = name;
			m_vec.x = f;
		}
		void Vec4Arg(const s8* name, const MOE::Math::vec4& v)
		{
			m_name = name;
			m_vec  = v;
		}
		void Matrix4Arg(const s8* name, const MOE::Math::matrix& m)
		{
			m_name = name;
			m_mat  = m;
		}
		void Vec4ArrayArg(const s8* name, const MOE::Math::vec4* v, s32 vecnum)
		{
			m_name = name;
			m_vec4array = v;
			m_vecnum = vecnum;
		}

	};

	
	class RenderCommand
	{
	public:
		enum TYPE_COMMAND
		{
			CMD_RENDER_TRIANGLE = 1,
			CMD_RENDER_TRIANGLE_LINE,
			CMD_RENDER_LINES,
			CMD_RENDER_POINTS,
			CMD_MATRIX,
			CMD_FLOAT,
			CMD_VEC4,
			CMD_VEC4ARRAY,
		};
		RenderCommand(TYPE_COMMAND cmd_, CommandArg* arg_, ProgramObject* prg_, u32 cid_)
		{
			cmd = cmd_;
			arg = arg_;
			prg = prg_;
			cid = cid_;
		}
		
		b8 operator<(const RenderCommand& t) const
		{
			return prg < t.prg || (prg <= t.prg && cid < t.cid);
		}
		TYPE_COMMAND cmd;
		CommandArg*  arg;
		ProgramObject* prg;
		u32 cid;
	};
	
	
	RenderDevice(MOE::Graphics* mg);
	~RenderDevice();
	
	// ----- Create Commands ------
	u32  WriteBuffer(const void* vertex, u32 vertex_num, const void* index, u32 index_num, const u32* idBuffer, u32 bufferID = 0);
	void DeleteBuffer(u32 bufferId);
	
	// ----- Queue commands ---------
	void QRenderTriangle(u32 bufferId);
	void QRenderTriangleLine(u32 bufferId);
	void QRenderLines(u32 bufferId);
	void QRenderPoints(u32 bufferId);
	void FlashCommands();
	
	//void SetViewMat(const MOE::Math::matrix4x4& mat);
	//void SetProjMat(const MOE::Math::matrix4x4& mat);
	
	void QMatrix(const s8* name, const MOE::Math::matrix4x4& mat);
	void QFloat(const s8* name, float f);
	void QVec4(const s8* name, const MOE::Math::vec4& v);
	void QVec4Array(const s8* name, const MOE::Math::vec4* v, s32 vecnum);
	void QBindShader  (MOE::ProgramObject* prg);
	void QUnbindShader();
	
private:
		
	static const u32 batchVertexSize = 65000;
	static const u32 MAX_COMMANDARG_SIZE = 30000;
	
	MOE::Graphics* g;
	ProgramObject* m_bindingShader;
//	ProgramObject* m_triprg;
//	ProgramObject* m_lineprg;
	
//	MOE::Math::matrix4x4 m_proj;
//	MOE::Math::matrix4x4 m_view;

	class VBIB
	{
	public:
		VBIB()
		{
			vbib[0] = 0;
			vbib[1] = 0;
			indexCount = 0;
		}
		VBIB(u32 vb, u32 ib, u32 indexcount)
		{
			vbib[0] = vb;
			vbib[1] = ib;
			indexCount = indexcount;
		}
		u32 vbib[2];
		u32 indexCount;
	};
	std::map<s32, std::vector<VBIB> > m_buffers;
	u32 m_bufferID; // !!! Limitation : max recreate buffer is u32 size !!!
	
	std::vector<RenderCommand> m_cmd;
	u32 m_cmdcounter;
	std::vector<CommandArg> m_cmdarg;
	u32 m_cmdargCnt;
	CommandArg* GetCommandArg();
	
	MOE::SceneGraph::Geometry::VertexFormat* m_tempVB;
	MOE::SceneGraph::Index* m_tempIB;
};

} // namespace MOE

