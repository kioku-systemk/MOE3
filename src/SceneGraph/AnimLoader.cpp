//
//  AnimLoader.cpp
//

#include "../Core/MOE.h"
#include "AnimLoader.h"
#include "../Core/Stream.h"

#include <string>
#include "../Core/Log.h"

#include "SceneGraph.h"

#include "../Core/Math.h"

#define MOELogDX(...)
//#define MOELogDX MOELogD

namespace  {
	class KeyFrame
	{
	public:
		/*
		MOE::Math::vec3 trans;
		MOE::Math::vec3 rot;
		MOE::Math::vec3 scale;*/
		MOE::Math::matrix mat;
		f32 visible;
		/*
		u32 time;
		f32 value;
		s32 inType;
		s32 outType;*/
	};
		
	class AnimData
	{
	public:
		enum AnimType {
			ANIMTYPE_UNKNOWN = 0,
			ANIMTYPE_TRANSLATION_X,
			ANIMTYPE_TRANSLATION_Y,
			ANIMTYPE_TRANSLATION_Z,
			ANIMTYPE_ROTATE_X,
			ANIMTYPE_ROTATE_Y,
			ANIMTYPE_ROTATE_Z,
			ANIMTYPE_SCALE_X,
			ANIMTYPE_SCALE_Y,
			ANIMTYPE_SCALE_Z,
			ANIMTYPE_VISIBLE
		};

        std::string m_objname;
		AnimType type;
        std::vector<KeyFrame> m_key;
	};
	
} // namespace

namespace MOE {
class Animation::internalAnimation
{
public:
	internalAnimation(s32 nodenum){
		m_data.resize(nodenum);
	}
	~internalAnimation()
	{
		
	}
	
	void SetAnimData(s32 i, AnimData anim)
	{
		if (i < static_cast<s32>(m_data.size())) {
			m_data[i] = anim;
			m_animmap[anim.m_objname] = &m_data[i];
		}
	}
	
	f64 GetMaxAnimTime()
	{
        std::map<std::string, AnimData*>::iterator it = m_animmap.begin();
		if (it != m_animmap.end())
			return it->second->m_key.size() / 24.0;
		else
			return 0.0;
	}

	
	KeyFrame interpolateKeyframe(const KeyFrame& a, const KeyFrame& b, const double f)
	{
		KeyFrame k;
/*		k.trans = (b.trans - a.trans) * f + a.trans;
		k.rot   = (b.rot - a.rot) * f + a.rot;
		k.scale = (b.scale - a.scale) * f + a.scale;*/
		for (int i = 0; i < 16; ++i)
			k.mat.f[i] = static_cast<float>(static_cast<double>(b.mat.f[i] - a.mat.f[i]) * f + static_cast<double>(a.mat.f[i]));
		k.visible = (b.visible - a.visible) * f + a.visible;
		return k;
	}
	MOE::Math::matrix getParentMatrix(const MOE::SceneGraph::Node* node)
	{
		MOE::Math::matrix parentMat = MOE::Math::Identity();
		using namespace MOE::SceneGraph;
		Group* pg = node->GetParent();
		while (pg) {
			if (pg->GetType() == NODETYPE_TRANSFORM
			||  pg->GetType() == NODETYPE_JOINT) {
				parentMat = static_cast<const Transform*>(pg)->GetMatrix() * parentMat;
			}
			pg = pg->GetParent();
		}
		return parentMat;
	}
	MOE::Math::matrix getRot(const MOE::Math::matrix& m)
	{
		return MOE::Math::matrix(
								  m.f[0],m.f[1],m.f[2],0,
								  m.f[4],m.f[5],m.f[6],0,
							  	  m.f[8],m.f[9],m.f[10],0,
								  0,0,0,1);
	}
	
	void updateSkin(MOE::SceneGraph::GeometryCache* node, const MOE::Math::matrix* mat)
	{
		using namespace MOE::SceneGraph;
		using namespace MOE::Math;

		Geometry* geo = node->GetTargetGeometry();
		const u32 vn = geo->GetVertexCount();
		const GeometryCache::VertexWeightFormat* vw = node->GetVertexWeight();
		assert(vn == node->GetVertexCount());
		Geometry::VertexFormat* tv = geo->GetVertex();
		Geometry::VertexFormat* ov = node->GetVertex();
		for (u32 i = 0; i < vn; ++i){
			const vec4& opos = vec4(ov[i].pos, 1.0f);
			const vec4& onrm = vec4(ov[i].normal, 0.0f);
			vec4 tpos(0,0,0,0);
			vec4 normal(0,0,0,0);
			for (s32 w = 0; w < 8; ++w){
				const f32 wei = vw[i].weight[w];
				if (wei < 0.001)
					break;
				tpos   += wei * (mat[vw[i].wid[w]] * opos).xyz();
				normal += wei * (mat[vw[i].wid[w]] * onrm).xyz();
			}
			
			tv[i] = ov[i];
			tv[i].pos = tpos.xyz();
			tv[i].normal = normal.xyz();
		}
		geo->EnableNeedUpdate();
	}

	void calcMatrixTable(MOE::SceneGraph::GeometryCache* node, MOE::Math::matrix* mat)
	{
		using namespace MOE::SceneGraph;
		using namespace MOE::Math;
		
		matrix meshparentMat = getParentMatrix(node);
		matrix imesh = Inverse(meshparentMat);
		s32 n = node->GetNodeMatrixTableCount();
		for (s32 i = 0; i < n; ++i){
			const Joint* t = node->GetNodeMatrixTable(i);
			if (!t)
				continue;
			matrix boneInvpos = t->GetOrigInverseMatrix();
			mat[i] = t->GetMatrix();
			const Group* g = t->GetParent();
			while(g && (g->GetType() == NODETYPE_TRANSFORM ||
						g->GetType() == NODETYPE_JOINT)){
				const Transform* pt = static_cast<const Transform*>(g);
				mat[i] = pt->GetMatrix() * mat[i];
				g = pt->GetParent();
			}
			mat[i] = imesh * mat[i] * boneInvpos * meshparentMat;
		}

	}
	void skinAnimate(MOE::SceneGraph::GeometryCache* node)
	{
		// Calc matrix table
		MOE::Math::matrix mat[256];
		calcMatrixTable(node, mat);
		updateSkin(node, mat);
	}
	
	void recAnimate(MOE::SceneGraph::Node* node, double time)
	{
		using namespace MOE::SceneGraph;
		using namespace MOE::Math;
		if (!node)
			return;
		
		const NODETYPE ntype = node->GetType();
		if (ntype == NODETYPE_GROUP
		||  ntype == NODETYPE_TRANSFORM
		||	ntype == NODETYPE_JOINT) {
			Group* g = static_cast<Group*>(node);
			const s32 cn = g->GetChildCount();
			for (s32 c = 0; c < cn; ++c) {
				recAnimate(g->GetChild(c), time);
			}
		} else if (ntype != NODETYPE_CAMERA){
			if (ntype == NODETYPE_GEOMETRYCACHE) {
				// skin animation
				skinAnimate(static_cast<MOE::SceneGraph::GeometryCache*>(node));
			}
			return;
		}
		
		// Transform node
        std::map<std::string, AnimData*>::iterator it = m_animmap.find(node->GetName());
		if (it != m_animmap.end()) {
			const s32 maxk = static_cast<s32>(it->second->m_key.size());
			double tm = time * 24.0;
			s32 timek = static_cast<s32>(MOE::Math::Floor(tm));
			double fl = tm - timek;
			if (timek >= maxk - 1) {
				timek = maxk - 2;
				fl    = 1.0;
			}
			
			const KeyFrame& kf = interpolateKeyframe(it->second->m_key[timek],it->second->m_key[timek+1], fl);
			if (ntype == NODETYPE_TRANSFORM || ntype == NODETYPE_JOINT) {
				Transform* Tr = static_cast<Transform*>(node);
				Tr->SetMatrix(kf.mat);
                Tr->SetVisible(kf.visible > .5);
			} else if (ntype == NODETYPE_CAMERA) {
                Camera* ca = static_cast<Camera*>(node);
                ca->SetFov(kf.visible);
            }
		}
	
	}
	void Animate(MOE::SceneGraph::Node* node, double time)
	{
		recAnimate(node, time);
	}

private:
    std::vector<AnimData> m_data;
    std::map<std::string, AnimData*> m_animmap;
};

// ---------------------------------
	
Animation::Animation(internalAnimation* internalAnim){
	anim = internalAnim;
}
Animation::~Animation(){
	delete anim;
}
void Animation::Animate(MOE::SceneGraph::Node* node, double time){
	anim->Animate(node, time);
}

// ---------------------------------
	
AnimLoader::AnimLoader(){
}
AnimLoader::~AnimLoader(){
}	

Animation* AnimLoader::Load(const Stream* st)
{
	MOELogDX("AnimLoader");
	if (!st || !st->IsOpened())
		return 0;

	const s8* data = static_cast<const s8*>(st->GetData());
	
	// header
	if (data[0] != 'A'
	||  data[1] != 'N'
	||  data[2] != 'M'
	)
	{
		MOELogE("Invalid header");
		return 0;
	}
	const u8 ver = data[3];
	data += 4;
	
	/*const s32 fpsmode = *reinterpret_cast<const u32*>(&data[0]);*/ data += sizeof(s32);
	
	const s32 nodenum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(s32);
	
	Animation::internalAnimation* internalAnim = mnew Animation::internalAnimation(nodenum);
	Animation* anim = mnew Animation(internalAnim);
	
	for (s32 i = 0; i < nodenum; ++i) {
		AnimData ad;
		ad.m_objname = data; data += ad.m_objname.length() + 1;
		//ad.type = static_cast<AnimData::AnimType>(*reinterpret_cast<const u8*>(&data[0])); data += sizeof(u8);
		MOELogDX("Name = %s", ad.m_objname.c_str());
		const s32 keynum = *reinterpret_cast<const s32*>(&data[0]); data += sizeof(s32);
		MOE::Math::vec3 av;
		ad.m_key.reserve(keynum);
		MOELogDX(" Keynum = %d", keynum);
		if (ver >= 1) {
			for (s32 j = 0; j < keynum; ++j) {
				using namespace MOE::Math;
				KeyFrame ke;
				memcpy(&ke.mat, &data[0], sizeof(MOE::Math::matrix));
				data += sizeof(MOE::Math::matrix);
				memcpy(&ke.visible, &data[0], sizeof(float));
				data += sizeof(float);
				ad.m_key.push_back(ke);
			}
		} else { // ver==0
			for (s32 j = 0; j < keynum; ++j) {
				using namespace MOE::Math;
				KeyFrame ke;
				memcpy(&ke.mat, &data[0], sizeof(MOE::Math::matrix));
				ke.visible = 1.0f;
				data += sizeof(MOE::Math::matrix);
				ad.m_key.push_back(ke);
			}
		}
		internalAnim->SetAnimData(i, ad);
	}
	

	
	return anim;
}

f64 Animation::GetMaxAnimTime()
{
    if (!anim)
        return 0.0f;
	return anim->GetMaxAnimTime();
}
	
} // namespace MOE

