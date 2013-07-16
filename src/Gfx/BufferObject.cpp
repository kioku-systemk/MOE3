#include "../Core/MOE.h"
#include "Graphics.h"
#include "BufferObject.h"
#include "ShaderProgramObject.h"

#include "../Core/Log.h"
#include <assert.h>

namespace {
	s32 typeSize(MOE::VGenum type)
	{
		static const s32 typesize[] = {
			1,//VG_BYTE                           0x1400
			1,//VG_UNSIGNED_BYTE                  0x1401
			2,//VG_SHORT                          0x1402
			2,//VG_UNSIGNED_SHORT                 0x1403
			4,//VG_INT                            0x1404
			4,//VG_UNSIGNED_INT                   0x1405
			4,//VG_FLOAT                          0x1406
		};
		return typesize[type - VG_BYTE];
	}
}

namespace MOE {
	
VertexBuffer::VertexBuffer(Graphics* mg, ProgramObject* prg, const VertexBufferInfo& info, const void* buf)
{
	g = mg;
	m_id = 0;
	m_bufsize = info.vertexSize * info.vertexCount;
	m_info = info;

	ChangeProgramAttibute(prg);
	
	g->GenBuffers(1, &m_id);
	Update(buf);
}
VertexBuffer::~VertexBuffer()
{
	g->DeleteBuffers(1, &m_id);
}

void VertexBuffer::Bind()
{
	g->BindBuffer(VG_ARRAY_BUFFER, m_id);
	for (s32 i = 0; i < VertexBufferInfo::VERTEX_DESC_MAX; ++i) {
		if (m_info.desc[i].num && m_info.desc[i]._attrib != -1) {
			g->EnableVertexAttribArray(m_info.desc[i]._attrib);
			g->VertexAttribPointer(m_info.desc[i]._attrib,
								   m_info.desc[i].num,
								   m_info.desc[i].type,
								   m_info.desc[i].normalize,
								   m_info.vertexSize, (const void*)m_info.desc[i]._offset);
		}
	}
	
}

void VertexBuffer::ChangeProgramAttibute(ProgramObject* prg)
{
	s32 offset = 0;
	for (s32 i = 0; i < VertexBufferInfo::VERTEX_DESC_MAX; ++i) {
		if (m_info.desc[i].type == 0)
			break;
		
		const s32 att = prg->GetAttribLocation(m_info.desc[i].name.c_str());
		m_info.desc[i]._attrib = att;
		m_info.desc[i]._offset = offset;
		offset += typeSize(m_info.desc[i].type) * m_info.desc[i].num;
	}
}
	
void VertexBuffer::Unbind()
{
	for (s32 i = 0; i < VertexBufferInfo::VERTEX_DESC_MAX; ++i) {
		if (m_info.desc[i].num && m_info.desc[i]._attrib != -1)
			g->DisableVertexAttribArray(m_info.desc[i]._attrib);
	}
	g->BindBuffer(VG_ARRAY_BUFFER, 0);
}

u32 VertexBuffer::GetId() const
{
	return m_id;
}
	
u32 VertexBuffer::GetVertexSize() const
{
	return m_info.vertexSize;
}
	
u32 VertexBuffer::GetVertexCount() const
{
	return m_info.vertexCount;
}

u32 VertexBuffer::GetBufferSize() const
{
	return m_bufsize;
}

void VertexBuffer::Draw(VGenum primitive) const
{
	g->DrawArrays(primitive, 0, m_info.vertexCount);
}

void VertexBuffer::Update(const void* buffer)
{
	if (buffer) {
		g->BindBuffer(VG_ARRAY_BUFFER, m_id);
		g->BufferData(VG_ARRAY_BUFFER, m_bufsize, buffer, VG_STATIC_DRAW);
		g->BindBuffer(VG_ARRAY_BUFFER, 0);
	}
}

IndexBuffer::IndexBuffer(Graphics* mg, const IndexBufferInfo& info, const void* buf)
{
	g = mg;
	m_id = 0;	
	m_bufsize = info.indexCount * typeSize(info.indexType);
	m_info = info;

	g->GenBuffers(1, &m_id);
	if (buf) {
		g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, m_id);
		g->BufferData(VG_ELEMENT_ARRAY_BUFFER, m_bufsize, buf, VG_STATIC_DRAW);
		g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);
	}
}

IndexBuffer::~IndexBuffer()
{
	g->DeleteBuffers(1, &m_id);
}

void IndexBuffer::Bind()
{
	g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::Unbind()
{
	g->BindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);
}

u32 IndexBuffer::GetId() const
{
	return m_id;
}
	
u32 IndexBuffer::GetIndexCount() const
{
	return m_info.indexCount;
}

s32 IndexBuffer::GetIndexType() const
{
	return m_info.indexType;
}
	
void IndexBuffer::Draw(VGenum primitive) const
{
	g->DrawElements(primitive, m_info.indexCount, m_info.indexType, 0);
}
	
void IndexBuffer::Update(const void* buffer)
{
	if (buffer) {
		g->BindBuffer(VG_ARRAY_BUFFER, m_id);
		g->BufferData(VG_ARRAY_BUFFER, m_bufsize, buffer, VG_STATIC_DRAW);
		g->BindBuffer(VG_ARRAY_BUFFER, 0);
	}
}

} // namespace MOE
