#ifndef INCLUDE_BUFFEROBJECT_H
#define INCLUDE_BUFFEROBJECT_H

#include "../Core/Type.h"
#include <string>

namespace MOE {
class Graphics;
class ProgramObject;
	
class VertexBufferInfo
{
public:
	static const s32 VERTEX_DESC_MAX = 8;
	
	VertexBufferInfo()
	{
		vertexSize = 0;
		vertexCount = 0;
		for (s32 i = 0; i < VERTEX_DESC_MAX; ++i) {
			desc[i].type      = 0;
			desc[i].normalize = 0;
			desc[i].num       = 0;
			desc[i]._offset   = 0;
			desc[i]._attrib   = 0;
		}
	}
	VertexBufferInfo(const VertexBufferInfo& t)
	{
		vertexSize  = t.vertexSize;
		vertexCount = t.vertexCount;
		for (s32 i = 0; i < VERTEX_DESC_MAX; ++i)
			desc[i] = t.desc[i];
	}
	struct VertexDesc {
        std::string name;
		VGenum type;
		VGenum normalize;
		s32    num;
		s32    _offset;
		s32    _attrib;
	};

	VertexDesc desc[VERTEX_DESC_MAX];
	s32 vertexSize;
	s32 vertexCount;
};

class VertexBuffer
{
public:
	VertexBuffer(Graphics* mg, ProgramObject* prg, const VertexBufferInfo& info, const void* buf = 0);
	~VertexBuffer();

	void Bind();
	void Unbind();
	u32 GetId() const;
	u32 GetStrideSize()  const;	
	u32 GetBufferSize()  const;
	u32 GetVertexSize()  const;
	u32 GetVertexCount() const;
	void Draw(VGenum primitive) const;
	void Update(const void* buffer);
	void ChangeProgramAttibute(ProgramObject* prg);

private:
	u32 m_id;
	VertexBufferInfo m_info;
	u32 m_bufsize;
	Graphics* g;
	ProgramObject* m_bindingpg;
};
	
class IndexBufferInfo
{
public:
	VGenum indexType;
	s32 indexCount;
};

class IndexBuffer
{
public:
	IndexBuffer(Graphics* mg, const IndexBufferInfo& info, const void* buf = 0);
	~IndexBuffer();
	
	void Bind();
	void Unbind();
	u32 GetId() const;
	u32 GetIndexCount() const;
	void Draw(VGenum primitive) const;
	void Update(const void* buffer);
	
	s32 GetIndexType() const;
private:
	u32 m_id;
	IndexBufferInfo m_info;
	u32 m_bufsize;
	
	Graphics* g;

};
	
} // namespace MOE

#endif // INCLUDE_BUFFEROBJECT_H

