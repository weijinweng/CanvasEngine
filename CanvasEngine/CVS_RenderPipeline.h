
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

struct CVS_DeferredPipeline : public CVS_RenderPipeline{
	struct GBuffer{
		enum GTex{
			POS_TEX,
			UV_TEX,
			NORM_TEX,
			DI_TEX,
			ALL_TEX
		};
		GLuint m_FrameBuffer;
		GLuint m_Buffers[ALL_TEX];
		GLuint m_DepthTexture;
		bool SetUp();
		void WBind();
		void RBind();
		void RBindPosBuffer();
	};
	GBuffer buffer;

	void SetUp();
	void Render(CVS_RenderScene* scene, CVS_View view);
	void SetSize(int w, int h);
};