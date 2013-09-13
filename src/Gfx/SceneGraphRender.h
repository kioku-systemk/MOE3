//
//  SceneGraphRender.h
//

#ifndef INCLUDE_SCENEGRAPHRENDER_H
#define INCLUDE_SCENEGRAPHRENDER_H

#include "../Core/Type.h"

#include <string.h>
#include <map>
#include <vector>
#include "../Core/Math.h"

namespace MOE
{
	class Graphics;
	class TextureObject;
	class ProgramObject;
	class RenderCommand;
	
	namespace SceneGraph
	{
		class Node;
	}
		
	class SceneGraphRender
	{
	public:
		SceneGraphRender(Graphics* mg);
		~SceneGraphRender();

		void SetProjMatrix(const Math::matrix& mat);
		void SetViewMatrix(const Math::matrix& mat);
		void SetModerate(const Math::vec4& moderate);
		
		void UpdateBuffers(const SceneGraph::Node* node);
		void RemoveBuffers(const SceneGraph::Node* node);
		
		void Draw(const SceneGraph::Node* node);
		void Clear();
		
		void AddTexture(const s8* name, TextureObject* tex);
		void SetResourcePath(const s8* path);

	private:
		void recDraw(const SceneGraph::Node* node,const Math::matrix& world);
		
		class RenderBuffer;
        std::map<const SceneGraph::Node*, std::vector<RenderBuffer*> > m_buffers;
		
		// for ProgramObject
        std::map<std::string, ProgramObject*> m_prgcache;
        std::map<const SceneGraph::Node*, RenderBuffer*> m_prgtable;// don't need release
		
        std::map<std::string, TextureObject*> m_texturecache;
		TextureObject* m_blanktexture;
		
		Math::matrix m_proj;
		Math::matrix m_view;
		Math::vec4 m_moderate;
		
		ProgramObject* m_defprg;
		RenderCommand* m_rc;

		std::string m_respath;
		
		Graphics* g;
		
	};
	
} // namespace MOE

#endif // INCLUDE_SCENEGRAPHRENDER_H