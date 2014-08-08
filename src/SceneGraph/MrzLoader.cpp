//
//  MrzLoader.cpp
//
//  Created by kioku on 12/06/07.
//

#include "../Core/MOE.h"
#include "MrzLoader.h"
#include "../Core/Stream.h"

#include <string>
#include "../Core/Log.h"

#include "SceneGraph.h"

#define MOELogDX(...)
//#define MOELogDX MOELogE

namespace {
	b8 isMesh(const s8*& data)
	{
		if (data[0] == 'M' && data[1] == 'S' && data[2] == 'H')
			return true;
		else
			return false;
	}
    b8 isInstance(const s8*& data)
	{
		if (data[0] == 'I' && data[1] == 'N' && data[2] == 'S')
			return true;
		else
			return false;
	}
	b8 isCachedMesh(const s8*& data)
	{
		if (data[0] == 'M' && data[1] == 'S' && data[2] == 'C')
			return true;
		else
			return false;
	}
	b8 isGroup(const s8*& data)
	{
		if (data[0] == 'G' && data[1] == 'R' && data[2] == 'P')
			return true;
		else
			return false;
	}
	b8 isTransform(const s8*& data)
	{
		if (data[0] == 'T' && data[1] == 'S' && data[2] == 'F')
			return true;
		else
			return false;
	}
	b8 isJoint(const s8*& data)
	{
		if (data[0] == 'J' && data[1] == 'N' && data[2] == 'T')
			return true;
		else
			return false;
	}
	b8 isMaterial(const s8*& data)
	{
		if (data[0] == 'M' && data[1] == 'A' && data[2] == 'T')
			return true;
		else
			return false;
	}
    b8 isCamera(const s8*& data)
	{
		if (data[0] == 'C' && data[1] == 'A' && data[2] == 'M')
			return true;
		else
			return false;
	}

	int getLevel(const s8*& data)
	{
		//return static_cast<int>(*reinterpret_cast<const u8*>(&data[0]));
        return *(u8*)(&data[0]);
	}

	
	MOE::SceneGraph::Node* readMesh(const s8*& data, std::map<const std::string, MOE::SceneGraph::Material*>& mattable)
	{
		using namespace MOE::SceneGraph;
		Geometry* geo = mnew Geometry();
		
		// Geometry name
		std::string name = &data[0];
		data += (name.size() + 1);
		geo->SetName(name);
		
		// vertex/index num
		//u32 vtxnum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(u32);
		//u32 idxnum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(u32);
        //printf("0x%X,0x%X,0x%X,0x%X\n",*(u8*)(&data[0]),*(u8*)(&data[1]),*(u8*)(&data[2]), *(u8*)(&data[3]));
		u32 vtxnum = *(u8*)(&data[0]) | (*(u8*)(&data[1])<<8) | (*(u8*)(&data[2])<<16) | (*(u8*)(&data[3])<<24) ;
        data += sizeof(u32);
        u32 idxnum = *(u8*)(&data[0]) | (*(u8*)(&data[1])<<8) | (*(u8*)(&data[2])<<16) | (*(u8*)(&data[3])<<24) ;
        data += sizeof(u32);

        
		MOELogDX("read mesh = %s", name.c_str());
		MOELogDX("vertexCount=%d", vtxnum);
		MOELogDX("indexCount=%d", idxnum);
		
		geo->Alloc(vtxnum, idxnum);
		
		// material id
		std::string matname = &data[0];
		data += (matname.size() + 1);
		std::map<const std::string, MOE::SceneGraph::Material*>::const_iterator it = mattable.find(matname);
		if (it != mattable.end())
			geo->SetMaterial(it->second);
		
		// vertex buffer
		const u32 vtxbufsize = sizeof(Geometry::VertexFormat) * vtxnum;
		memcpy(geo->GetVertex(), data, vtxbufsize);
		data += vtxbufsize;
		
		// index buffer
		const u32 idxbufsize = geo->GetIndexTypeSize() * idxnum;
		memcpy(geo->GetIndex(), data, idxbufsize);
		data += idxbufsize;

		geo->CalcBounds();
		geo->EnableNeedUpdate();
		return geo;
	}
	
	MOE::SceneGraph::Node* findNode_rec(MOE::SceneGraph::Node* node, MOE::SceneGraph::NODETYPE ntype, const std::string& nodename)
	{
		using namespace MOE::SceneGraph;
		const NODETYPE nodetype = node->GetType();
		if (nodetype == NODETYPE_GROUP
		||  nodetype == NODETYPE_TRANSFORM
		||  nodetype == NODETYPE_JOINT) {
			Group* g = static_cast<Group*>(node);
			const int n = g->GetChildCount();
			for (int i = 0; i < n; ++i)
			{
				if (nodetype == ntype && node->GetName() == nodename)
					return static_cast<Transform*>(node);
				
				Node* r = findNode_rec(g->GetChild(i), ntype, nodename);
				if (r)
					return r;
			}
		}
		if (nodetype == ntype && node->GetName() == nodename)
			return node;
		return 0;
	}
	
	MOE::SceneGraph::Node* findNode(MOE::SceneGraph::Node* root, MOE::SceneGraph::NODETYPE ntype, const std::string& nodename)
	{
		return findNode_rec(root, ntype, nodename);
	}

    MOE::SceneGraph::Node* readInstance(const s8*& data, MOE::SceneGraph::Node* root)
	{
        using namespace MOE::SceneGraph;
		Instance* inst = mnew Instance();
        // Instance target name
		std::string name = &data[0];
		data += (name.size() + 1);
		inst->SetInstanceName(name);
		return inst;
    }
    
	MOE::SceneGraph::Node* readCachedMesh(const s8*& data, MOE::SceneGraph::Node* root)
	{
		using namespace MOE::SceneGraph;
		GeometryCache* geo = mnew GeometryCache();
		
		// Geometry name
		std::string name = &data[0];
		data += (name.size() + 1);
		geo->SetName(name);
		
        printf("readCachedMesh\n");
		// vertex/index num
		//u32 vtxnum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(u32);
		//u32 idxnum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(u32);
        u32 vtxnum = *(u8*)(&data[0]) | (*(u8*)(&data[1])<<8) | (*(u8*)(&data[2])<<16) | (*(u8*)(&data[3])<<24) ;
        data += sizeof(u32);
        u32 idxnum = *(u8*)(&data[0]) | (*(u8*)(&data[1])<<8) | (*(u8*)(&data[2])<<16) | (*(u8*)(&data[3])<<24) ;
        data += sizeof(u32);

		
		MOELogDX("read cached mesh = %s", name.c_str());
		MOELogDX("vertexCount=%d", vtxnum);
		MOELogDX("indexCount=%d", idxnum);
		
		geo->Alloc(vtxnum, idxnum);
		
		// material id
		std::string matname = &data[0];
		data += (matname.size() + 1);
/*		std::map<const std::string, MOE::SceneGraph::Material*>::const_iterator it = mattable.find(matname);
		if (it != mattable.end())
			geo->SetMaterial(it->second);
*/
		// vertex buffer
		const u32 vtxbufsize = sizeof(Geometry::VertexFormat) * vtxnum;
		memcpy(geo->GetVertex(), data, vtxbufsize);
		data += vtxbufsize;
		
		// index buffer
		const u32 idxbufsize = geo->GetIndexTypeSize() * idxnum;
		memcpy(geo->GetIndex(), data, idxbufsize);
		data += idxbufsize;
		
		geo->CalcBounds();
		
		// Read original mesh name
		std::string orgname = &data[0];
		data += (orgname.size() + 1);
		Node* geonode = findNode(root, NODETYPE_GEOMETRY, orgname);
		geo->SetTargetGeometry(static_cast<Geometry*>(geonode));
		assert(geonode);
		
		// Read matrix num
		u32 mnum = 0;
		memcpy(&mnum, data, sizeof(u32));
		geo->AllocNodeMatrixTable(mnum);
		data += sizeof(u32);
		
		// Read matrix node name
		for (u32 i = 0; i < mnum; ++i){
			std::string nodename = &data[0];
			data += (nodename.size() + 1);
			
			Joint* nd = static_cast<Joint*>(findNode(root, NODETYPE_JOINT, nodename));
			//assert(nd);
			if (!nd) {
				MOELogE("Not found Joint:%s", nodename.c_str());
			} else {
				geo->SetNodeMatrixTable(i, nd);
				
				MOE::Math::matrix rotmat;
				memcpy(&rotmat, data, sizeof(rotmat));
				data += sizeof(rotmat);
				
				nd->SetOrigInverseMatrix(rotmat);
			}
		}
		
		// Read weights
		GeometryCache::VertexWeightFormat* wei = geo->GetVertexWeight();
		const u32 weightbufsize = geo->GetVertexCount() * sizeof(GeometryCache::VertexWeightFormat);
		memcpy(wei, data, weightbufsize);
		data += weightbufsize;
		/*for (s32 x = 0; x < 100; ++x){
			printf("weight(%d) = [%.3f,%.3f,%.3f]-[%d,%d,%d]\n", x, wei[x].weight[0], wei[x].weight[1], wei[x].weight[2]
				   ,wei[x].wid[0],wei[x].wid[1],wei[x].wid[2]);
		}*/
		
		return geo;
	}
	
	MOE::SceneGraph::Node* readTransform(const s8*& data)
	{
		MOE::SceneGraph::Transform* t = mnew MOE::SceneGraph::Transform();
		const std::string name = data;
		data += (name.size() + 1);
		t->SetName(name);
		MOE::Math::matrix4x4 mat;
		memcpy(&mat, data, sizeof(mat));
		data += sizeof(mat);
		MOELogDX("Transform: %s\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n",
				 name.c_str(), mat.f[0],mat.f[1],mat.f[2],mat.f[3],mat.f[4],mat.f[5],mat.f[6],mat.f[7],
				 mat.f[8],mat.f[9],mat.f[10],mat.f[11],mat.f[12],mat.f[13],mat.f[14],mat.f[15]);
		t->SetMatrix(mat);
		return t;
	}
	
	MOE::SceneGraph::Node* readJoint(const s8*& data)
	{
		MOE::SceneGraph::Joint* t = mnew MOE::SceneGraph::Joint();
		const std::string name = data;
		data += (name.size() + 1);
		t->SetName(name);
		MOE::Math::matrix4x4 mat;
		memcpy(&mat, data, sizeof(mat));
		data += sizeof(mat);
		MOELogDX("Joint: %s\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f,%.3f\n",
				 name.c_str(), mat.f[0],mat.f[1],mat.f[2],mat.f[3],mat.f[4],mat.f[5],mat.f[6],mat.f[7],
				 mat.f[8],mat.f[9],mat.f[10],mat.f[11],mat.f[12],mat.f[13],mat.f[14],mat.f[15]);
		t->SetMatrix(mat);
		return t;
	}
	
	MOE::SceneGraph::Node* readMaterial(const s8*& data)
	{
		MOE::SceneGraph::Material* m = mnew MOE::SceneGraph::Material();
		
		const std::string matname = data;
		data += (matname.length() + 1);
		m->SetName(matname);
		
		s32 num;
		// vec4
		memcpy(&num, data, sizeof(num)); data+= sizeof(num);
		for (s32 i = 0; i < num; ++i) {
			MOE::Math::vec4 vec;
			const std::string name = data;
			data += (name.length() + 1);
			memcpy(&vec, data, sizeof(vec)); data += sizeof(vec);
			m->Set(name, vec);
		}
		// matrix
		memcpy(&num, data, sizeof(num)); data+= sizeof(num);
		for (s32 i = 0; i < num; ++i) {
			MOE::Math::matrix mat;
			const std::string name = data;
			data += (name.length() + 1);
			memcpy(&mat, data, sizeof(mat)); data += sizeof(mat);
			m->Set(name, mat);
		}
		
		// string
		memcpy(&num, data, sizeof(num)); data+= sizeof(num);
		for (s32 i = 0; i < num; ++i) {
			const std::string name = data;
			data += (name.length() + 1);
			const std::string str = data;
			data += str.length() + 1;
			m->Set(name, str);
		}

		return m;
	}
    
    MOE::SceneGraph::Node* readCamera(const s8*& data)
	{
		MOE::SceneGraph::Camera* t = mnew MOE::SceneGraph::Camera();
		const std::string name = data;
		data += (name.size() + 1);
		t->SetName(name);
        float holfov;
        memcpy(&holfov, data, sizeof(float));
        t->SetFov(holfov);
        data += sizeof(float);
		return t;
	}
	
	MOE::SceneGraph::Node* readGroup(const s8*& data)
	{
		MOE::SceneGraph::Group* g = mnew MOE::SceneGraph::Group();
		const std::string name = data;
		data += (name.size() + 1);
		g->SetName(name);
		return g;
	}
	
	bool isGroupNode(MOE::SceneGraph::Node* node)
	{
		if (node->GetType() == MOE::SceneGraph::NODETYPE_GROUP
		||  node->GetType() == MOE::SceneGraph::NODETYPE_TRANSFORM
		||  node->GetType() == MOE::SceneGraph::NODETYPE_JOINT)
			return true;
		else
			return false;
	}
    
    
    void setIntances_rec(MOE::SceneGraph::Node* node, MOE::SceneGraph::Node* root)
    {
        using namespace MOE::SceneGraph;
		const NODETYPE nodetype = node->GetType();
		if (nodetype == NODETYPE_GROUP
        ||  nodetype == NODETYPE_TRANSFORM
        ||  nodetype == NODETYPE_JOINT) {
			Group* g = static_cast<Group*>(node);
			const int n = g->GetChildCount();
			for (int i = 0; i < n; ++i)
				setIntances_rec(g->GetChild(i), root);
		}
		if (node->GetType() == NODETYPE_INSTANCE) {
            Instance* inst = static_cast<Instance*>(node);
		    const std::string& instname = inst->GetInstanceName();
            Node* node = findNode(root, NODETYPE_TRANSFORM, instname);
            inst->SetInstanceNode(node);
        }
    }
    void setIntances(MOE::SceneGraph::Node* root)
    {
        using namespace MOE::SceneGraph;
        setIntances_rec(root, root);
    }
}

namespace MOE {
	
MrzLoader::MrzLoader(){
}
MrzLoader::~MrzLoader(){
}	

SceneGraph::Node* MrzLoader::Load(const Stream* st){
	using namespace MOE::SceneGraph;

	if (!st || !st->IsOpened())
		return 0;

	MOELogDX("MrzLoader");
	const s8* data = static_cast<const s8*>(st->GetData());
	
	// header
	if (data[0] != 'M'
	||  data[1] != 'R'
	||  data[2] != 'Z'
	||  data[3] != 0
	)
	{
		MOELogE("Invalid header");
		return 0;
	}
	data += 4;
	
	//u32 nodenum = *reinterpret_cast<const u32*>(&data[0]); data += sizeof(u32);
    u32 nodenum = *(u8*)(&data[0]) | (*(u8*)(&data[1])<<8) | (*(u8*)(&data[2])<<16) | (*(u8*)(&data[3])<<24) ;
    data += sizeof(u32);

	
	Node* root = 0;
	int currentLevel = -1;
	Group* currentNode = 0;
	std::map<const std::string, MOE::SceneGraph::Material*> mattable;
	// geometry
	for (u32 m = 0; m < nodenum; ++m) {
		Node* node = 0;
		int level = 0;
		if (isMesh(data)) { data += 3;
			level = getLevel(data); data++;
			node = readMesh(data, mattable);
        } else if (isInstance(data)) { data += 3;
			level = getLevel(data); data++;
			node = readInstance(data, root);
		} else if (isCachedMesh(data)) { data += 3;
			level = getLevel(data); data++;
			node = readCachedMesh(data, root);
		} else if (isGroup(data)) { data += 3;
			level = getLevel(data); data++;
			node = readGroup(data);
		} else if (isTransform(data)) { data += 3;
			level = getLevel(data); data++;
			node = readTransform(data);
		} else if (isJoint(data)) { data += 3;
			level = getLevel(data); data++;
			node = readJoint(data);
		} else if (isMaterial(data)) { data += 3;
			level = getLevel(data); data++;
			node = readMaterial(data);
			mattable[node->GetName()] = static_cast<MOE::SceneGraph::Material*>(node);
        } else if (isCamera(data)) { data += 3;
			level = getLevel(data); data++;
			node = readCamera(data);
        }

		if (!node)
			continue;
		
		MOELogDX("%s", node->GetName().c_str());
		if (currentLevel == -1 && level == 0) { // root
			root = node;
			if (isGroupNode(node)) {
				currentNode = static_cast<Group*>(node);
				currentLevel = 0;
			}
		} else if (currentLevel >= level) { // sbling
			int levelup = currentLevel - level;
			Group* parent = currentNode->GetParent();
			for (int i = 0; i < levelup; ++i) 
				parent = parent->GetParent();
			
			if (!parent) {
				MOELogE("Invalid hierarchy");
				delete root;
				return 0;
			}
			parent->AddChild(node);
			currentNode = static_cast<Group*>(node);
			currentLevel = level;
		} else if (currentLevel + 1 == level) { // child
			if (isGroupNode(currentNode)) {
				static_cast<Group*>(currentNode)->AddChild(node);
			} else {
				MOELogE("Invalid hierarchy");
				delete root;
				return 0;
			}
			currentNode = static_cast<Group*>(node);
			currentLevel++;
		} else {
			MOELogE("Invalid hierarchy or broken format.");
			delete root;
			return 0;
		}
	}

	setIntances(root);
	return root;
}

	
} // namespace MOE

