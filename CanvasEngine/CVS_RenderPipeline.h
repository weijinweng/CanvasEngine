
#include "CVS_Precompiled.h"
#include "CVS_SubSystem.h"
#include "CVS_2DTools.h"
#include "CVS_GUI.h"
#include "CVS_RenderComponents.h"

struct CVS_RenderPipeline{
	virtual void SetUp();
	virtual void Render(CVS_RenderScene* scene, CVS_View view);
	virtual void SetSize(int w, int h);
};

enum GTex{
	POS_TEX,
	DIF_TEX,
	NORM_TEX,
	UV_TEX,
	ALL_TEX
};

struct CVS_DeferredPipeline : public CVS_RenderPipeline{
	struct VCT{

		void Voxelize();
		void Pass();
		void DrawVoxels();
	};

	struct AOBuffer{
		GLuint m_AoFrameBuffer;
		GLuint m_PingPongBlur;
		GLuint m_DepthTexture;
		GLuint m_AoTexture;
		GLuint m_AoNoise;
		CVS_RenderProgram* AOShader;
		CVS_RenderProgram* HBAOShader;
		CVS_RenderProgram* XBlurShader;
		CVS_RenderProgram* YBlurShader;

		int dep_map;
		int dcLoc;
		int iP;

		int HBAO_dep_map;
		int HBAO_noise_map;
		int HBAO_dc;
		int HBAO_iP;
		int HBAO_focal_length;

		void SetUp();
		void UpdateSize();
		void PassAO(CVS_RenderScene* scene, CVS_View* view);
		void PassHBAO(CVS_RenderScene* scene, CVS_View* view);
	};
	struct GBuffer{
		GLuint m_FrameBuffer;
		GLuint m_Buffers[ALL_TEX];
		GLuint m_DepthTexture;
		bool SetUp();
		void WBind();
		void RBind();
		void RBindPosBuffer();
		void UpdateSize();
	};
	struct LGHPass{
		
		int iP;
		int V;
		int MVP;

		int pos;
		int amb;
		int dif;
		int spe;

		int con;
		int lin;
		int quad;
		int type;

		int spotDir;
		int spotExp;
		int spotCos;

		int pos_map;
		int color_map;
		int norm_map;
		int uv_map;
		int AO_map;

		int cam;

		int rad;

		int dcLoc;
		//Light meshes
		CVS_Mesh* mPointSphere;
		CVS_Mesh* mQuad;
		CVS_Mesh* mSpotCone;

		CVS_RenderProgram* dirProgram;
		CVS_RenderProgram* spotProgram;
		CVS_RenderProgram* program;

		GLuint m_ShadowFrameBuffer;

		void ShadowPass(CVS_RenderScene*, CVS_View*);
		void SetUp();

		void pass(CVS_RenderScene* scene, CVS_View* view);
	};
	
	CVS_IRECT dc;

	GBuffer buffer;
	LGHPass light;
	AOBuffer AO;

	void SetUp();
	void Render(CVS_RenderScene* scene, CVS_View view);
	void SetSize(int w, int h);
};