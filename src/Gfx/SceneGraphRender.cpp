//
//  SceneGraphRender.cpp
//

#include "../Core/MOE.h"
#include "SceneGraphRender.h"
#include "Graphics.h"
#include "TextureObject.h"
#include "ShaderProgramObject.h"
#include "BufferObject.h"
#include "../Core/Log.h"
#include "RenderCommand.h"
#include "../SceneGraph/SceneGraph.h"
#include "../Core/Stream.h"

#define STRINGIFY(A) #A

namespace  {
	
static const char* vsShader = STRINGIFY(
										attribute vec3 pos;
										attribute vec4 col;
										attribute vec3 nor;
										attribute vec2 uv;
										uniform mat4 proj;
										uniform mat4 view;
										uniform mat4 world;
										varying HIGHP vec2 vtx_uv;
										varying LOWP vec3    vtx_nor;
										varying LOWP vec4    vtx_col;
										void main(void)
{
	vtx_uv = uv;
	vtx_nor = normalize((view * vec4(nor, 0.0)).xyz);
	vtx_col = col;
	gl_Position = proj * view * world * vec4(pos,1);
}
										);

static const char* fsShader = STRINGIFY(
										vec3 lerp(vec3 a, vec3 b, float r) {
											return (b - a) * r + a;
										}
										uniform sampler2D color_tex; 
										uniform vec4 color;
										uniform vec4 mode;
										varying HIGHP vec2 vtx_uv;
										varying LOWP vec3    vtx_nor;
										varying LOWP vec4    vtx_col;
										void main(void)
{
	vec4 lcol = vtx_col;
	vec3 lvec = normalize(lerp(vec3(0.0,1.0,1.0),vec3(0.0,-1.0,-1.0),mode.r));
	gl_FragColor = lcol * vec4(color.rgb * (0.6 * max(0.0,dot(lvec, normalize(vtx_nor.xyz))) + 0.4), color.a) * texture2D(color_tex,vtx_uv);
}
										);
	
} // namespace

namespace MOE {
	
class SceneGraphRender::RenderBuffer {
public:
	RenderBuffer() : vb(0), ib(0), pg(0) {}
	RenderBuffer(VertexBuffer* vb_, IndexBuffer* ib_, ProgramObject* pg_):vb(vb_),ib(ib_),pg(pg_) {}
	~RenderBuffer() {
		delete vb;
		delete ib;
	}
	VertexBuffer* vb;
	IndexBuffer*  ib;
	ProgramObject* pg;
};
	

SceneGraphRender::SceneGraphRender(Graphics* mg)
{
	g = mg;
    
	// ---- Shader ----
	MOE::ShaderObject vs(g), fs(g);
	vs.LoadFromMemory(vsShader, MOE::ShaderObject::VERTEX_SHADER);
	fs.LoadFromMemory(fsShader, MOE::ShaderObject::FRAGMENT_SHADER);
	m_defprg = mnew MOE::ProgramObject(g);
	b8 r = m_defprg->Link(vs, fs);
	if (!r) {
		MOELogE("error link shader\n");
		exit(1);
	}
	m_blanktexture = mnew MOE::TextureObject(g,4,4, 4,32);
	s32* white = mnew s32[4*4];
	memset(white, 0xFF, 4*4*4);
	m_blanktexture->WriteImage(white, 4, 4);
//	delete [] white;
	
	m_rc = mnew MOE::RenderCommand(g);
}
SceneGraphRender::~SceneGraphRender()
{
	Clear();
	delete m_blanktexture;
	delete m_rc;
	delete m_defprg;
}
	
void SceneGraphRender::ClearPrograms()
{
	// ProgramObject
    std::map<std::string, MOE::ProgramObject*>::iterator pit, peit = m_prgcache.end();
	for (pit = m_prgcache.begin(); pit != peit; ++pit) {
		if (pit->second != m_defprg)
			delete pit->second;
	}
	m_prgcache.clear();
	m_prgtable.clear();	
}

void SceneGraphRender::Clear()
{
	// Buffer
    std::map<const MOE::SceneGraph::Node*, std::vector<RenderBuffer*> >::iterator it, eit = m_buffers.end();
	for (it = m_buffers.begin(); it != eit; ++it) {
        std::vector<RenderBuffer*>::iterator vit, veit = it->second.end();
		for (vit = it->second.begin(); vit != veit; ++vit) {
			delete (*vit);
		}
	}
	m_buffers.clear();
	
	// Texture
    std::map<std::string, MOE::TextureObject*>::iterator tit, teit = m_texturecache.end();
	for (tit = m_texturecache.begin(); tit != teit; ++tit) {
		delete tit->second;
	}
	m_texturecache.clear();

	ClearPrograms();
}
void SceneGraphRender::SetProjMatrix(const MOE::Math::matrix& mat)
{
	m_matrixs["proj"] = mat;
}

void SceneGraphRender::SetViewMatrix(const MOE::Math::matrix& mat)
{
	m_matrixs["view"] = mat;
}
	
void SceneGraphRender::SetUniform(const s8* name, const Math::vec4& val)
{
    m_vec4s[name] = val;
}

void SceneGraphRender::SetUniform(const s8* name, const Math::matrix& val)
{
    m_matrixs[name] = val;
}    

void SceneGraphRender::UpdateBuffers(const MOE::SceneGraph::Node* node)
{
	if (!node)
		return;
	
	using namespace MOE::SceneGraph;
	const NODETYPE type = node->GetType();
	if (type == NODETYPE_GROUP
	||  type == NODETYPE_TRANSFORM
	||  type == NODETYPE_JOINT) {
		const Group* g = static_cast<const Group*>(node);
		const s32 n = g->GetChildCount();
		for (s32 i = 0; i < n; ++i)
			UpdateBuffers(g->GetChild(i));
	}
	else if (type == NODETYPE_GEOMETRY) {
		const Geometry* geo = static_cast<const Geometry*>(node);
		if (!geo->GetNeedUpdate())
			return;
		
		geo->DisableNeedUpdate();
		
        std::map<const MOE::SceneGraph::Node*, std::vector<RenderBuffer*> >::iterator it = m_buffers.find(node);
		if (it != m_buffers.end()) { // find
            std::vector<RenderBuffer*>::iterator vit,veit = it->second.end();
			for (vit = it->second.begin(); vit != veit; ++vit) {
				// need to update buffer.
				b8 upd = false;
				if ((*vit)->vb->GetVertexCount() == geo->GetVertexCount()){
					(*vit)->vb->Update(geo->GetVertex());
					upd = true;
				}
				if ((*vit)->ib && (*vit)->ib->GetIndexCount() == geo->GetIndexCount()){
					(*vit)->ib->Update(geo->GetIndex());
					upd = true;
				}
				if (!upd) {
					// changed VB/IB count, we will recreate Buffer.
					delete (*vit);
				}
			}
		}
	
		// find Shader
		Material* mat = geo->GetMaterial();
		//MOELogD("Geo = %s Mat = %s",geo->GetName().c_str(), mat->GetName().c_str());
		ProgramObject* prg = m_defprg;
		size_t np;
		if (mat && (np = mat->GetName().rfind("_")) != std::string::npos) {
			const std::string& mname = mat->GetName();
			const std::string fname = mname.substr(np+1, mname.length());
            std::map<std::string, ProgramObject*>::const_iterator pit = m_prgcache.find(fname);
			if (pit != m_prgcache.end())
				prg = pit->second;
		}

		MOE::VertexBufferInfo info;
		info.vertexSize = sizeof(MOE::SceneGraph::Geometry::VertexFormat);
		info.vertexCount = geo->GetVertexCount();
		info.desc[0].name = "pos";
		info.desc[0].num  = 3;
		info.desc[0].type = VG_FLOAT;
		info.desc[0].normalize = VG_FALSE;
		info.desc[1].name = "col";
		info.desc[1].num  = 4;
		info.desc[1].type = VG_UNSIGNED_BYTE;
		info.desc[1].normalize = VG_TRUE;
		info.desc[2].name = "nor";
		info.desc[2].num  = 3;
		info.desc[2].type = VG_FLOAT;
		info.desc[2].normalize = VG_FALSE;
		info.desc[3].name = "uv";
		info.desc[3].num  = 2;
		info.desc[3].type = VG_FLOAT;
		info.desc[3].normalize = VG_FALSE;
	
#ifdef ENABLE_VERTEX_SPLIT
		const s32 maxVertexCount = 65535;
		if ( info.vertexCount >= maxVertexCount+1) {
			if (geo->GetIndexCount() > 0){
				MOELogE("max vertex/index error");// TODO
				assert(0);
			}
            std::vector<RenderBuffer*> rbufs;
			const s32 vpn = geo->GetVertexCount() / maxVertexCount + 1;
			s32 vtxPtr = 0;
			s32 vtxNum = geo->GetVertexCount();
			for (s32 vp = 0; vp < vpn; ++vp)
			{
				info.vertexSize = sizeof(MOE::SceneGraph::Geometry::VertexFormat);
				if (vtxNum - maxVertexCount < 0) info.vertexCount = vtxNum;
				else                             info.vertexCount = maxVertexCount;
				vtxNum -= maxVertexCount;
				
				MOE::VertexBuffer* vb = minew MOE::VertexBuffer(g, prg, info, &(geo->GetVertex()[vtxPtr]));
				MOE::IndexBuffer*  ib = 0;
				rbufs.push_back(minew RenderBuffer(vb, ib, prg));
				vtxPtr += maxVertexCount;
			}
			m_buffers[node] = rbufs;

		} else {
#endif
			MOE::IndexBufferInfo ibinfo;
			ibinfo.indexCount = geo->GetIndexCount();
			if (geo->GetIndexTypeSize() == 2)
				ibinfo.indexType  = VG_UNSIGNED_SHORT;
			else
				ibinfo.indexType  = VG_UNSIGNED_INT;
			MOE::VertexBuffer* vb = mnew MOE::VertexBuffer(g, prg, info, geo->GetVertex());
			MOE::IndexBuffer*  ib = 0;
			if (ibinfo.indexCount)
				ib = mnew MOE::IndexBuffer (g, ibinfo, geo->GetIndex());
            std::vector<RenderBuffer*> rbufs;
			rbufs.push_back(mnew RenderBuffer(vb, ib, prg));
			m_buffers[node] = rbufs;
#ifdef ENABLE_VERTEX_SPLIT
		}
#endif
											 
	
	} else if (type == NODETYPE_MATERIAL) {
		const std::string name = node->GetName();
		const Material* mat = static_cast<const Material*>(node);
/*
		// Load Texture
		int tn = mat->GetNumTexture();
		for (int i = 0; i < tn; ++i) {
			std::string name,tex;
			mat->GetTexture(i, name, tex);
			
            std::map<std::string, MOE::TextureObject*>::const_iterator it = m_texturecache.find(tex);
			if (it != m_texturecache.end())
				continue;  // already caching
			
			// Load texture
            MOE::Image img;
			MOE::Stream* st = mnew MOE::Stream(m_respath + tex.c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
			b8 r = img.Load(st);
			delete st;
			if (!r) {
				MOELogE("Load failed texture: %s", tex.c_str());
				continue;
			}
			MOELogD("Load texture: %s", tex.c_str());
			MOE::TextureObject* tobj = mnew MOE::TextureObject(g);
			tobj->WriteImage(&img);
			
			m_texturecache[tex] = tobj;
		}*/
		// Load Shader
		size_t np;
		if (mat && (np = mat->GetName().rfind("_")) != std::string::npos) {
			const std::string& mname = mat->GetName();
			const std::string fname = mname.substr(np+1, mname.length());
			
			// TODO: fx
            std::map<std::string, ProgramObject*>::const_iterator pit = m_prgcache.find(fname);
			if (pit == m_prgcache.end()) {
                
#if 1
                const std::string fxname = fname + ".fx";
                MOE::Stream fxfile((m_respath + fxname).c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
                if (fxfile.IsOpened())
                {
                    ProgramObject* prg = mnew MOE::ProgramObject(g);
                    if (!prg->LoadFromMemory(std::string(static_cast<const s8*>(fxfile.GetData()))))
                    {
                        MOELogE("error link shader > %s.fx", fname.c_str());
                        delete prg;
                        prg = m_defprg;
                    }
                    MOELogI("Material = %s - Loaded Shader: %s.fx",mname.c_str(), fname.c_str());
                    m_prgcache[fname] = prg;
                }
#else
				const std::string vsname = fname + ".vs";
				const std::string fsname = fname + ".fs";
                MOE::Stream vshader_file((m_respath + vsname).c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
                MOE::Stream fshader_file((m_respath + fsname).c_str(), MOE::Stream::MODE_INPUT_BINARY_ONMEMORY);
                s8* vshader_src = mnew s8[vshader_file.GetSize() + 1];
                s8* fshader_src = mnew s8[fshader_file.GetSize() + 1];
                memcpy(vshader_src, vshader_file.GetData(), vshader_file.GetSize()); vshader_src[vshader_file.GetSize()] = 0;
                memcpy(fshader_src, fshader_file.GetData(), fshader_file.GetSize()); fshader_src[fshader_file.GetSize()] = 0;
                
                MOE::ShaderObject vs(g), fs(g);
                b8 vsr = vs.LoadFromMemory(vshader_src, MOE::ShaderObject::VERTEX_SHADER);
                b8 fsr = fs.LoadFromMemory(fshader_src, MOE::ShaderObject::FRAGMENT_SHADER);
                if (vsr && fsr) {
                    ProgramObject* prg = mnew MOE::ProgramObject(g);
                    b8 r = prg->Link(vs, fs);
                    if (!r) {
                        MOELogE("error link shader > %s.vs/fs", fname.c_str());
                        delete prg;
                        prg = m_defprg;
                    }
                    MOELogI("Material = %s - Loaded Shader: %s.vs/fs",mname.c_str(), fname.c_str());
                    m_prgcache[fname] = prg;
                }
                delete [] vshader_src;
                delete [] fshader_src;
#endif
			}
		}
	}
}

void SceneGraphRender::SetResourcePath(const s8* fpath)
{
	m_respath = std::string(fpath);
}

void SceneGraphRender::Draw(const MOE::SceneGraph::Node* node, ProgramObject* prg)
{
	MOE::Math::matrix world = MOE::Math::Identity();
	
	recDraw(node, world, prg);
	m_rc->BindProgram(0); // unbind program
	m_rc->CommandExecute();
}

void SceneGraphRender::recDraw(const MOE::SceneGraph::Node* node, const MOE::Math::matrix& world, ProgramObject* ovprg) {
	if (!node)
		return;
	
	using namespace MOE::SceneGraph;
	const NODETYPE type = node->GetType();
	if (type == NODETYPE_GROUP) {
		const Group* g = static_cast<const Group*>(node);
		const s32 n = g->GetChildCount();
		for (s32 i = 0; i < n; ++i)
			recDraw(g->GetChild(i), world, ovprg);
	}
	else if (type == NODETYPE_TRANSFORM||type == NODETYPE_JOINT) {
		const Transform* g = static_cast<const Transform*>(node);
		const s32 n = g->GetChildCount();
		const MOE::Math::matrix m = g->GetMatrix();
		const MOE::Math::matrix lworld = world * m;
		for (s32 i = 0; i < n; ++i)
			recDraw(g->GetChild(i), lworld, ovprg);
	}
	else if (type == NODETYPE_GEOMETRY) {
		const Geometry* gg = static_cast<const Geometry*>(node);
		//MOELogD("geo=%s / mat=%s", node->GetName().c_str(), gg->GetMaterial()->GetName().c_str() );
        std::map<const MOE::SceneGraph::Node*, std::vector<RenderBuffer*> >::iterator it = m_buffers.find(node);
		if (it == m_buffers.end())
			return;
		
        if (ovprg)
            m_rc->BindProgram(ovprg);
        else
            m_rc->BindProgram(it->second[0]->pg);
        
        std::map<std::string,Math::vec4>::iterator vit,veit = m_vec4s.end();
        for (vit = m_vec4s.begin(); vit != veit; ++vit)
            m_rc->SetUniform(vit->first.c_str(), vit->second);
        std::map<std::string,Math::matrix>::iterator mit,meit = m_matrixs.end();
        for (mit = m_matrixs.begin(); mit != meit; ++mit)
            m_rc->SetUniform(mit->first.c_str(), mit->second);
        
		m_rc->SetUniform("world", world);

		MOE::RenderCommand::PrimitiveType ptype = MOE::RenderCommand::TRIANGLES;
		Material* mat = gg->GetMaterial();
		if (mat) {
			// Vec4
			const s32 vn = mat->GetNumVec4();
			for (s32 i = 0; i < vn; ++i) {
				std::string name;
				MOE::Math::vec4 v;
				mat->GetVec4(i, name, v);
				m_rc->SetUniform(name.c_str(), v);
			}
			// Matrix
			const s32 mn = mat->GetNumMatrix();
			for (s32 i = 0; i < mn; ++i) {
				std::string name;
				MOE::Math::matrix4x4 m;
				mat->GetMatrix(i, name, m);
				m_rc->SetUniform(name.c_str(), m);
			}

			// PrimitiveType
			ptype = static_cast<MOE::RenderCommand::PrimitiveType>(mat->GetDrawMode());
				
			// Texture
			b8 hasColortex = false;
			const s32 tn = mat->GetNumTexture();
			for (s32 i = 0; i < tn; ++i) {
				std::string name, texname;
				mat->GetTexture(i, name, texname);
				
                std::map<std::string, MOE::TextureObject*>::iterator it = m_texturecache.find(texname);
				if (it != m_texturecache.end()) {
					m_rc->BindTex(name.c_str(), it->second);
				}else {
					m_rc->BindTex(name.c_str(), m_blanktexture);
				}

				if (name == "color_tex")
					hasColortex = true;
			}
			// default texture
			if (!hasColortex)
				m_rc->BindTex("color_tex", m_blanktexture);
		}
		

		if (it->second.size() == 2)
			it = it;
        std::vector<RenderBuffer*>::const_iterator bit,beit = it->second.end();
		for (bit = it->second.begin(); bit != beit; ++bit)
		{
			//MOELogD("NODE=0x%X : VB=0x%X / IB=0x%X / PG=0x%X",node,(*vit)->vb, (*vit)->ib, (*vit)->pg);
			MOE::VertexBuffer* vb = (*bit)->vb;
			MOE::IndexBuffer* ib = (*bit)->ib;
			
			if (ib) {
				m_rc->BindVB(vb);
				m_rc->BindIB(ib);
				m_rc->DrawElements(ptype, ib->GetIndexCount());
				m_rc->UnbindVB(vb);
				m_rc->UnbindIB(ib);
			} else {
				m_rc->BindVB(vb);
				m_rc->DrawArrays(ptype, 0, vb->GetVertexCount());
				m_rc->UnbindVB(vb);
			}
		}
	}
}
	
	
void SceneGraphRender::AddTexture(const s8* name, TextureObject* tex)
{
	m_texturecache[name] = tex;
}
	
	
} // namespace MOE


