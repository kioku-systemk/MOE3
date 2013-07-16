//
//  RenderCommand.h
//

#ifndef INCLUDE_RENDERCOMMAND_H
#define INCLUDE_RENDERCOMMAND_H

#include "../Core/Type.h"
#include "../Core/Math.h"

namespace MOE
{		
	class Graphics;
	class VertexBuffer;
	class IndexBuffer;
	class ProgramObject;
	class TextureObject;
	
	class RenderCommand
	{
	public:
		RenderCommand(Graphics* mg);
		~RenderCommand();

		// DrawCommands
		enum ClearFlag{
			CLEAR_COLOR = VG_COLOR_BUFFER_BIT,
			CLEAR_DEPTH = VG_DEPTH_BUFFER_BIT
		};
		enum PrimitiveType{
			POINTS         = VG_POINTS,
			LINES          = VG_LINES,
			LINE_LOOP      = VG_LINE_LOOP,
			LINE_STRIP     = VG_LINE_STRIP,
			TRIANGLES      = VG_TRIANGLES,
			TRIANGLE_STRIP = VG_TRIANGLE_STRIP,
			TRIANGLE_FAN   = VG_TRIANGLE_FAN
		};
		void Clear(s32 cf);
		void ClearColor(f32 r, f32 g, f32 b, f32 a);
		
		void BindProgram(ProgramObject* pg);
		void BindVB(VertexBuffer* vb);
		void BindIB(IndexBuffer* ib);
		void BindTex(const s8* name, TextureObject* tex);

		void UnbindVB(VertexBuffer* vb);
		void UnbindIB(IndexBuffer* ib);

		void SetUniform(const char* name, const Math::matrix& mat);
		void SetUniform(const char* name, const Math::vec4& vec);
		void DrawArrays(PrimitiveType mode, s32 first, s32 vertexCount);
		void DrawElements(PrimitiveType mode, s32 indexCount);
		
		void CommandExecute();

		
		ProgramObject* GetProgram() const;
	private:
		Graphics* g;
		
		class CommandQ;
		CommandQ* m_q;
		u32 m_qCounter;
		ProgramObject* m_q_prg;
		void clearQ();
		
		class CommandArgPool;
		CommandArgPool* m_argpool;

		union CommandArg;
		typedef void CommandFunc(CommandArg* arg);
		
		// binding state
		ProgramObject* m_binding_prg;
		const VertexBuffer*  m_binding_vb;
		const IndexBuffer*   m_binding_ib;
		const TextureObject* m_binding_tex;
		
		
		// Commands func
		CommandFunc CmdUnknown;
		CommandFunc CmdClearColor;
		CommandFunc CmdClear;
		CommandFunc CmdBindProgram;
		CommandFunc CmdBindVertex;
		CommandFunc CmdBindIndex;
		CommandFunc CmdBindTex;
		CommandFunc CmdUnbindVertex;
		CommandFunc CmdUnbindIndex;
		CommandFunc CmdDrawArrays;
		CommandFunc CmdDrawElements;
		CommandFunc CmdUniformMatrix;
		CommandFunc CmdUniformVec4;

	};
	
} // namespace MOE

#endif // INCLUDE_RENDERCOMMAND_H

