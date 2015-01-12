#pragma once

#include "CanvasCore.h"

#define CVS_RLAYER_ALL 0
#define CVS_RSCENE_MATERIALSHADERNODES 1
#define CVS_RSCENE_MATERIALNODES 2
#define CVS_RSCENE_RENDERNODES 3 
#define CVS_RSCENE_POINT_LIGHTS 4
#define CVS_RSCENE_SPOT_LIGHTS 5
#define CVS_RSCENE_DIR_LIGHTS 6
#define CVS_RSCENE_CAMERA 7
#define CVS_RSCENE_TOTAL 8

#define CVS_RRENDERER_PASSES 1
#define CVS_RRENDERER_FRAMEBUFFERS 2
#define CVS_RRENDERER_RENDERBUFFERS 3
#define CVS_RRENDERER_RSCENES 4
#define CVS_RRENDERER_TEXTURE 5
#define CVS_RRENDERER_SHADERS 6
#define CVS_RRENDERER_PROGRAMS 7
#define CVS_RRENDERER_TOTAL 8

#define CVS_RNODES_INPUT 1
#define CVS_RNODES_OUTPUT 2

#define CVS_RPROGRAM_INT_LAYER 1
#define CVS_RPROGRAM_FLOAT_LAYER 2
#define CVS_RPROGRAM_VEC3_LAYER 3
#define CVS_RPROGRAM_VEC4_LAYER 4
#define CVS_RPROGRAM_MAT4_LAYER 5

#define CVS_BOOL 700
#define CVS_INT 701 
#define CVS_FLOAT 702

#define CVS_VEC3 703
#define CVS_VEC4 704
#define CVS_MAT4 705

#define CVS_TEXTURE 706
#define CVS_TEXTURE_1D 707
#define CVS_TEXTURE_1D_ARRAY 708
#define CVS_TEXTURE_2D 709
#define CVS_TEXTURE_2D_ARRAY 710
#define CVS_TEXTURE_2D_MULTISAMPLE 711
#define CVS_TEXTURE_2D_MULTISAMPLE_ARRAY 712
#define CVS_TEXTURE_RECTANGLE 713
#define CVS_TEXTURE_BUFFER 714
#define CVS_TEXTURE_CUBE_MAP 715
#define CVS_TEXTURE_CUBE_MAP_ARRAY 716

#define CVS_TEXTURE_TYPE 734

//Reserve 1100 - 1150 for texture format definitions
#define CVS_COLOR_RGB 1100
#define CVS_COLOR_RGB16I 1101
#define CVS_COLOR_RGB32F 1102
#define CVS_COLOR_RGB8 1103
#define CVS_COLOR_RGBA 1104
#define CVS_COLOR_RGBA8 1105
#define CVS_COLOR_RGBA16 1106
#define CVS_COLOR_RGBA32F 1107

#define CVS_DEPTH16 1108
#define CVS_DEPTH24 1109
#define CVS_DEPTH32 1110
#define CVS_DEPTH32F 1111

#define CVS_DEPTH32_STENCIL8 1112
#define CVS_DEPTH32F_STENCIL8 1113

#define CVS_FRAMEBUFFER 717
#define CVS_RPASS 718
#define CVS_PROGRAM 719

#define CVS_NODE_LAYER 720

#define CVS_SHADER 721
#define CVS_VERT_SHADER 722
#define CVS_GEO_SHADER 723
#define CVS_TESS_SHADER 724
#define CVS_FRAG_SHADER 725
#define CVS_COMP_SHADER 726
 
#define CVS_FILTER_CLOSEST 727
#define CVS_FILTER_LINEAR 728

#define CVS_STRING 729
#define CVS_UNSIGNED_INT 730

#define CVS_SHADER_NODE 731
#define CVS_PROGRAM_NODE 732
#define CVS_MATERIAL_SHADER_NODE 733

namespace Canvas{

	class RRendererNetwork;
	class RScene;
	class RRenderer;
	class RendererNode;

	struct RNodeAttribute{
		void* data_ptr;
		uint32 type;
		uint32 data_size;
		std::vector<RNodeAttribute*> out_nodes;
		RNodeAttribute* in_node;
		bool owner;
		std::string m_name;
		void* GetDataPtr();
	};

	struct RNodeAttributeLayer{
		std::string m_name;
		std::vector<RNodeAttribute*> m_attributes;
		RNodeAttribute* Find(std::string);
	};

	struct RNodeConnection{
		RendererNode* _in_node;
		std::string _in_attrib;
		std::string _reciever_attrib;
	};

	struct RAttributes{
		std::vector<RNodeConnection> m_connections;
		std::vector<RNodeAttributeLayer*> m_layers;
		RAttributes();
		void CreateAttributeLayer(std::string _layer);
		RNodeAttributeLayer* FindLayer(std::string _name);
		RNodeAttributeLayer* FindLayer(uint32 _hash);
		RNodeAttribute* AddAttribute(std::string _name, uint32 _type, void* _data_ptr, std::string _layer_id);
		RNodeAttribute* AddAttribute(std::string _name, uint32 _type, void* _data_ptr, uint32 _layer_hash = 0);
		bool SetAttributeLayer(RNodeAttribute* _node, std::string _layer);
		bool SetAttributeLayer(RNodeAttribute* _node, uint32 _layer_hash);
		uint32 GetAttributeHash(std::string);
	};



	class RendererNode{
	public:
		std::string m_name;
		std::string m_type;
		uint32 m_all_layer;
		RAttributes m_attributes;
		RRenderer* m_renderer;

		RendererNode(std::string _name, std::string _type);

		RNodeAttribute* CreateAttribute(std::string _name, uint32 _type, void* _data_ptr);
		RNodeAttributeLayer* CreateNewLayer(std::string _name);

		std::string GetType();
		std::string GetName();
		std::string SetName();

		bool ConnectNode(RendererNode* _in_node, std::string _in_id, std::string _reciever_id);
		bool ConnectNode(RendererNode* _in_node, uint32 _in_hash, uint32 _reciever_hash);
		void SetRenderer(RRenderer* _renderer);

		uint32 QueryAttributeHash(std::string _hash_id);

		//Fectches the attribute data pointer.
		virtual const void* FetchDataPtr(uint32 _hash);
		//Link to the context. Done whenever 
		//Contexts changes
		virtual bool DataLink() = 0;
		//Gives nodes a chance to load the data.
		//Done whenever a node changes.
		virtual bool LoadLink() = 0; 
		//Link to respective objects. Done whenever the
		//overall network changes.
		virtual bool Link() = 0;
		//Last link, clean up unecessary data.
		//Done when there are no more editing.
		virtual bool CleanupLink() = 0;

		virtual RendererNode* Instance(std::string _name, RRenderer* _scene) = 0;
		virtual bool CreateConnection(RendererNode* _in_node, std::string _in_attrib, std::string _reciever_node);
		virtual void Unbind();
		virtual bool Rebind();
	};

	struct RRendererNodeLayer{
	private:
		uint32 m_first;
	public:
		std::string m_name;
		std::vector<RendererNode*> m_nodes;
		RRendererNodeLayer(std::string _name);
		uint32 AddNode(RendererNode* _node);	
		RendererNode* Find(uint32 _node_hash);
		RendererNode* Find(std::string _node_name);
		uint32 Hash(std::string);
		int Size();
	};
}