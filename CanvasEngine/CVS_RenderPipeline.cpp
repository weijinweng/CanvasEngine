#include "Canvas.h"

void CVS_DeferredPipeline::LGHPass::SetUp()
{
	program = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("DeferredLight", "./Shaders/DeferredLight.vert", "./Shaders/DeferredLight.frag");
	
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
	dep_map = program->getUniformHash("depth_map");
	AO_map = program->getUniformHash("AO_map");

	cam = program->getUniformHash("cam");

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
	glBindTexture(GL_TEXTURE_2D, pipeline->buffer.m_DepthTexture);
	glUniform1i(dep_map, 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, pipeline->AO.m_AoTexture);
	glUniform1i(AO_map, 4);

	CVS_IRECT rec = ((CVS_DeferredPipeline*)GLOBALSTATEMACHINE.m_RenderSub.pipeline)->dc;
	glUniform2f(dcLoc, (float)rec.w, (float)rec.h);



	cvec4 campos = view->View * cvec4(0, 0, 0, 1);
	campos *= -1.0f;

	glUniformMatrix4fv(V, 1, GL_FALSE, glm::value_ptr(view->View));
	glUniformMatrix4fv(iP, 1, GL_FALSE, glm::value_ptr(glm::inverse(view->Pers)));

	for (int i = 0, e = scene->lights.size(); i < e; ++i)
	{



		CVS_LightProperties prop = scene->lights[i]->properties;

		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(view->Pers * view->View * prop.transform.transform));

		glUniform4fv(pos, 1, glm::value_ptr(prop.position));
		glUniform4fv(amb, 1, glm::value_ptr(prop.ambient));
		glUniform4fv(dif, 1, glm::value_ptr(prop.diffuse));
		glUniform4fv(spe, 1, glm::value_ptr(prop.specular));

		glUniform3f(cam, campos.x, campos.y, campos.z);

		glUniform1f(con, prop.constAttenuation);
		glUniform1f(lin, prop.linearAttenuation);
		glUniform1f(quad, prop.quadraticAttenuation);

		glUniform1i(type, prop.type);

		glUniform1f(rad, prop.rad);

		switch (prop.type)
		{
			case CVS_LightProperties::CVS_LGT_PT:
			{
												GLOBALSTATEMACHINE.m_RenderSub.primitives.Sphere->Draw();
			}
			case CVS_LightProperties::CVS_LGT_DIR:
			{
											glDisable(GL_CULL_FACE);
											GLOBALSTATEMACHINE.m_RenderSub.primitives.Sphere->Draw();
											glEnable(GL_CULL_FACE);
			}
		}

	}
	glCullFace(GL_BACK);

}

void CVS_DeferredPipeline::AOBuffer::SetUp()
{
	AOShader = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("HBAO/SSAO", "./Shaders/HSAO.vert", "./Shaders/HSAO.frag");

	dep_map = AOShader->getUniformHash("depth_map");
	dcLoc = AOShader->getUniformHash("dc");
	iP = AOShader->getUniformHash("iP");

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
	Scene->Draw(&view);



	//Light pass
	buffer.RBind();
	glDisable(GL_DEPTH_TEST);
	this->AO.PassAO(Scene, &view);
	glClear(GL_COLOR_BUFFER_BIT);
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