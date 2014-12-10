#include "Canvas.h"

void CVS_DeferredPipeline::LGHPass::SetUp()
{
	program = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("DeferredLight", "./Shaders/DeferredLight.vert", "./Shaders/DeferredLight.frag");
	//m_ShadowMapDir = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("ShadowMap", "./Shaders/ShadowMap2D.vert", "./Shaders/ShadowMap2D.frag");

	//Get CVS_Inputs
	V = program->getUniformHash("V");
	iP = program->getUniformHash("iP");
	MVP = program->getUniformHash("MVP");

	pos = program->getUniformHash("lgt.pos_ws");
	amb = program->getUniformHash("lgt.ambient");
	dif = program->getUniformHash("lgt.diffuse");
	spe = program->getUniformHash("lgt.specular");

	con = program->getUniformHash("lgt.constAtten");
	lin = program->getUniformHash("lgt.linearAtten");
	quad = program->getUniformHash("lgt.quadAtten");

	type = program->getUniformHash("lgt.type");

	spotDir = program->getUniformHash("lgt.spotDir");
	spotExp = program->getUniformHash("lgt.spotExp");
	spotCos = program->getUniformHash("lgt.spotCos");

	pos_map = program->getUniformHash("pos_map");
	color_map = program->getUniformHash("color_map");
	norm_map = program->getUniformHash("norm_map");
	uv_map = program->getUniformHash("uv_map");
	AO_map = program->getUniformHash("AO_map");

	rad = program->getUniformHash("rad");

	dcLoc = program->getUniformHash("dc");
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
}



void CVS_DeferredPipeline::LGHPass::pass(CVS_RenderScene* scene, CVS_View* view)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glCullFace(GL_FRONT);


	CVS_DeferredPipeline* pipeline = (CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline;

	program->setAsCurrentProgram();


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_Buffers[POS_TEX]);
	glUniform1i(pos_map, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_Buffers[DIF_TEX]);
	glUniform1i(color_map, 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_Buffers[NORM_TEX]);
	glUniform1i(norm_map, 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, pipeline->AO.m_AoTexture);
	glUniform1i(AO_map, 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_Buffers[UV_TEX]);
	glUniform1i(uv_map, 4);

	CVS_IRECT rec = ((CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline)->dc;
	glUniform2f(dcLoc, (float)rec.w, (float)rec.h);

	glUniformMatrix4fv(V, 1, GL_FALSE, glm::value_ptr(view->View));

	for (int i = 0, e = scene->lights.size(); i < e; ++i)
	{

		CVS_LightProperties prop = scene->lights[i]->properties;

	
		glUniform4fv(pos, 1, glm::value_ptr(prop.position));
		glUniform4fv(amb, 1, glm::value_ptr(prop.ambient));
		glUniform4fv(dif, 1, glm::value_ptr(prop.diffuse));
		glUniform4fv(spe, 1, glm::value_ptr(prop.specular));

		glUniform1f(con, prop.constAttenuation);
		glUniform1f(lin, prop.linearAttenuation);
		glUniform1f(quad, prop.quadraticAttenuation);

		glUniform1i(type, prop.type);

		glUniform1f(rad, prop.rad);

		if (prop.type == CVS_LightProperties::CVS_LGT_PT)
		{
			glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
			GLOBALSTATEMACHINE.m_RenderSub.primitives.Quad->Draw();
		}
		else if (prop.type == CVS_LightProperties::CVS_LGT_DIR)
		{
			glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
			GLOBALSTATEMACHINE.m_RenderSub.primitives.Quad->Draw();

		}
	}
	glCullFace(GL_BACK);

}

void CVS_DeferredPipeline::LGHPass::ShadowPass(CVS_RenderScene* scene, CVS_View* view)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ShadowFrameBuffer);

	for (int i = 0, e = scene->lights.size(); i < e; ++i)
	{
		
	}
}

void CVS_DeferredPipeline::AOBuffer::SetUp()
{
	AOShader = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("HBAO/SSAO", "./Shaders/HSAO.vert", "./Shaders/HSAO.frag");
	HBAOShader = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("HBAO", "./Shaders/HSAO.vert", "./Shaders/HBAO.frag");

	dep_map = AOShader->getUniformHash("depth_map");
	dcLoc = AOShader->getUniformHash("dc");
	iP = AOShader->getUniformHash("iP");

	HBAO_dep_map = HBAOShader->getUniformHash("depth_map");
	HBAO_noise_map = HBAOShader->getUniformHash("noise_map");
	HBAO_dc = HBAOShader->getUniformHash("dc");
	HBAO_iP = HBAOShader->getUniformHash("iP");
	HBAO_focal_length = HBAOShader->getUniformHash("FocalLen");

	CVS_DeferredPipeline* pipeline =(CVS_DeferredPipeline*) GLOBALSTATEMACHINE.m_RenderSub.pipeline;

	CVS_IRECT dc = pipeline->dc;

	glGenFramebuffers(1, &m_AoFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_AoFrameBuffer);

	glGenTextures(1, &m_AoTexture);

	m_DepthTexture;
	glGenTextures(1, &m_DepthTexture);

	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, dc.w ? dc.w : 100, dc.h ? dc.h : 100, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);


	glBindTexture(GL_TEXTURE_2D, m_AoTexture);

	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RGB32F, dc.w ? dc.w : 100, dc.h ? dc.h : 100,
		0, GL_RGB, GL_FLOAT, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_AoTexture, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, buffers);

	GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		MessageBox(NULL, "Error Framebuffer AO incomplete", "Error", MB_OK);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glGenTextures(1, &m_AoNoise);
	glBindTexture(GL_TEXTURE_2D, m_AoNoise);

	float* noise = new float[4 * 4 * 4];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			glm::vec2 xy = glm::circularRand(1.0f);
			float z = glm::linearRand(0.0f, 1.0f);
			float w = glm::linearRand(0.0f, 1.0f);

			int startPos = 4 * (i * 4 + j);
			noise[startPos + 0] = xy[0]; 
			noise[startPos + 1] = xy[1];
			noise[startPos + 2] = z;
			noise[startPos + 3] = w;
		}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, noise);
	
	glGenTextures(1, &m_PingPongBlur);
	glBindTexture(GL_TEXTURE_2D, m_PingPongBlur);
	
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RGB32F, dc.w ? dc.w : 100, dc.h ? dc.h : 100,
		0, GL_RGB, GL_FLOAT, NULL);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CVS_DeferredPipeline::AOBuffer::PassHBAO(CVS_RenderScene* scene, CVS_View* view)
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_AoFrameBuffer);

	HBAOShader->setAsCurrentProgram();
	CVS_DeferredPipeline* pipeline = (CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline;
	glUniformMatrix4fv(HBAO_iP, 1, GL_FALSE, glm::value_ptr(glm::inverse(view->Pers)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_DepthTexture);
	glUniform1i(HBAO_dep_map, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_AoNoise);
	glUniform1i(HBAO_noise_map, 1);

	glUniform2f(HBAO_dc, (float)pipeline->dc.w, (float)pipeline->dc.h);

	float Focal1 = view->Pers[1][1];
	float Focal2 = view->Pers[0][0];

	glUniform2f(HBAO_focal_length, Focal1, Focal2);

	GLOBALSTATEMACHINE.m_RenderSub.primitives.Quad->Draw();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
}

void CVS_DeferredPipeline::AOBuffer::PassAO(CVS_RenderScene* scene, CVS_View* view)
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_AoFrameBuffer);

	AOShader->setAsCurrentProgram();
	CVS_DeferredPipeline* pipeline = (CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline;
	glUniformMatrix4fv(iP, 1, GL_FALSE, glm::value_ptr(glm::inverse(view->Pers)));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_DepthTexture);
	glUniform1i(dep_map, 0);

	glUniform2f(dcLoc, (float)pipeline->dc.w, (float)pipeline->dc.h);

	GLOBALSTATEMACHINE.m_RenderSub.primitives.Quad->Draw();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
}

void CVS_DeferredPipeline::AOBuffer::UpdateSize()
{
	CVS_DeferredPipeline* pipeline = (CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline;

	CVS_IRECT dc = pipeline->dc;

	glBindTexture(GL_TEXTURE_2D, m_AoTexture);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RGB, dc.w ? dc.w : 100, dc.h ? dc.h : 100,
		0, GL_RGB, GL_FLOAT, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, dc.w, dc.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, m_PingPongBlur);

	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RGB32F, dc.w ? dc.w : 100, dc.h ? dc.h : 100,
		0, GL_RGB, GL_FLOAT, NULL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

bool CVS_DeferredPipeline::GBuffer::SetUp()
{
	glGenFramebuffers(1, &m_FrameBuffer);
	glGenTextures(ALL_TEX, m_Buffers);
	glGenTextures(1, &m_DepthTexture);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer);

	CVS_IRECT dc = ((CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline)->dc;

	for (int i = 0, e = ALL_TEX; i < e; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_Buffers[i]);

		glTexImage2D(GL_TEXTURE_2D, 
			0, GL_RGB32F,dc.w?dc.w : 100, dc.h?dc.h:100, 
			0, GL_RGB, GL_FLOAT, NULL);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 
			m_Buffers[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, dc.w ? dc.w : 100, dc.h ? dc.h : 100, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	
	GLenum dBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(ALL_TEX, dBuffers);

	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		MessageBox(NULL, "Error Framebuffer incomplete", "Error", MB_OK);
		return false;
	}

	glDepthRange(0.0f, 1.0f);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	return true;
}

void CVS_DeferredPipeline::GBuffer::UpdateSize()
{

	CVS_IRECT dc = ((CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline)->dc;

	for (int i = 0, e = ALL_TEX; i < e; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_Buffers[i]);

		glTexImage2D(GL_TEXTURE_2D,
			0, GL_RGB32F, dc.w, dc.h,
			0, GL_RGB, GL_FLOAT, NULL);

	}

	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, dc.w, dc.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	return;
}

void CVS_DeferredPipeline::GBuffer::WBind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FrameBuffer);
}

void CVS_DeferredPipeline::GBuffer::RBind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void CVS_DeferredPipeline::GBuffer::RBindPosBuffer()
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + POS_TEX);
}

void CVS_DeferredPipeline::SetUp()
{
	printf("Setup\n");
	GLOBALSTATEMACHINE.m_RenderSub.createNewShader("Default", "./Shaders/Deferred.vert", "./Shaders/Deferred.frag");

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	dc.x = viewport[0];
	dc.y = viewport[1];
	dc.w = viewport[2];
	dc.h = viewport[3]; 

	this->buffer.SetUp();
	this->light.SetUp();
	this->AO.SetUp();
}

void CVS_DeferredPipeline::Render(CVS_RenderScene* Scene, CVS_View view)
{
	//Geometry pass
	glDepthMask(GL_TRUE);
	buffer.WBind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	Scene->OptimizedDraw(&view);

	//Light pass
	buffer.RBind();
	glDisable(GL_DEPTH_TEST);
	this->AO.PassHBAO(Scene, &view);
	light.pass(Scene, &view);
}

void CVS_DeferredPipeline::SetSize(int w, int h)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	dc.x = viewport[0];
	dc.y = viewport[1];
	dc.w = viewport[2];
	dc.h = viewport[3];

	buffer.UpdateSize();
	AO.UpdateSize();
}

void CVS_DeferredPipeline::GenShadowMap(int type, GLuint* mapID)
{
	switch (type)
	{
	case CVS_LightProperties::CVS_LGT_DIR:
		glBindTexture(GL_TEXTURE_2D_ARRAY, *mapID);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, 2048, 2048, 3, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1024, 1024, 0, GL_DEPTH_COMPONENT24, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
}