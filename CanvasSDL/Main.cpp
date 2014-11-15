#include "Externals.h"
#include "RenderSystem.h"
#include "Scene.h"
#include <cmath>

const float PI = 3.14159265;

using namespace Canvas;

bool quit = false;

bool Up = false,
	Down = false,
	Left = false,
	Right  = false,
	accelerate = false,
	ShiftUp = false,
	ShiftDown = false;

float horAngle = 0, vertAngle = 0;

glm::vec3 position(0);

float fclamp(float value, float max, float min)
{
	if(value > max)
		return max;
	if(value < min)
		return min;
	return value;
}

void computeMatrix(fpsCamera* cam)
{
	static int lastTime = 0;
	int deltaTime = SDL_GetTicks() - lastTime;
	lastTime = SDL_GetTicks();
	float multiplier = 1.0f;

	int x,y;
	SDL_GetMouseState(&x,&y);
	cam->verticleAngle += (y-900/2)*(0.01);
	cam->horizontalAngle -= (x - 1200/2)*(0.01);

	cam->verticleAngle = fclamp(cam->verticleAngle, 3.14/2 + 3.14, -3.14/2 + 3.14);

	glm::vec3 direction(std::cos(cam->verticleAngle)*std::sin(cam->horizontalAngle),
						std::sin(cam->verticleAngle),
						std::cos(cam->verticleAngle)*std::cos(cam->horizontalAngle));

	glm::vec3 directionRight(std::sin(cam->horizontalAngle-3.14f/2.0f),
							 0,
							 std::cos(cam->horizontalAngle-3.14f/2.0f));
	float timeSeconds = (float)deltaTime/1000.0f;
	if(accelerate)
		multiplier = 3.0f;
	if(Up)
	{
		cam->position += direction*timeSeconds*multiplier;
	}
	if(Down)
	{
		cam->position -= direction*timeSeconds*multiplier;
	}
	if (Left)
	{
		cam->position += directionRight*timeSeconds*multiplier;
	}
	if (Right)
	{
		cam->position -= directionRight*timeSeconds*multiplier;
	}
	if (ShiftUp)
	{
		cam->position.y += 2*timeSeconds*multiplier;
	}
	if (ShiftDown)
	{
		cam->position.y -= 2*timeSeconds*multiplier;
	}

}

void computeMatrix(glm::mat4* view)
{
	static int lastTime = 0;
	int deltaTime = SDL_GetTicks() - lastTime;
	lastTime = SDL_GetTicks();
	float multiplier = 1.0f;

	int x,y;
	SDL_GetMouseState(&x,&y);


	vertAngle += (y - 900/2)*(0.01);
	horAngle -= (x - 1200/2)*(0.01);

	vertAngle = fclamp(vertAngle, 3.14/2 + 3.14, -3.14/2 + 3.14);

	glm::vec3 direction(std::cos(vertAngle)*std::sin(horAngle),
						std::sin(vertAngle),
						std::cos(vertAngle)*std::cos(horAngle));

	glm::vec3 directionRight(std::sin(horAngle-3.14f/2.0f),
							 0,
							 std::cos(horAngle-3.14f/2.0f));

	float timeSeconds = (float)deltaTime/1000.0f;
	if(accelerate)
		multiplier = 3.0f;
	if(Up)
	{
		position += direction*timeSeconds*multiplier;
	}
	if(Down)
	{
		position -= direction*timeSeconds*multiplier;
	}
	if (Left)
	{
		position += directionRight*timeSeconds*multiplier;
	}
	if (Right)
	{
		position -= directionRight*timeSeconds*multiplier;
	}
	if (ShiftUp)
	{
		position.y += 2*timeSeconds*multiplier;
	}
	if (ShiftDown)
	{
		position.y -= 2*timeSeconds*multiplier;
	}

	glm::vec3 directionUp(glm::cross(direction,directionRight));
	*view = glm::lookAt(position,direction + position, directionUp);


}

int main(int argc, char* args[])
{
	CanvasWindow* window = CanvasWindow::CVS_CreateWindow(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 900, CVS_WDW_WINDOW);
	Scene* myScene = window->renderer.createNewScene();

	std::vector<CVS_Mesh*> meshes = myScene->loadFromFile("dk.FBX");
	fpsCamera mycam(45.0f, 4.0f/3.0f, glm::vec3(0,1.12,2.14));
	mycam.horizontalAngle = 5095.77246f;
	mycam.verticleAngle = 2.77999949f;
	myScene->cameras.push_back(&mycam);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*
	auto bodyTexture = window->renderer.createNewTexture("Body");

	bodyTexture->loadFile("dragon_knight.jpg");

	renderNode* node = myScene->getNode("g knight_knight_");
	node->setTexture("myTextureSampler", "Body");

	Texture* weaponTexture = window->renderer.createNewTexture("Weapon");
	weaponTexture->loadFile("knight_weapon_color.jpg");

	renderNode* weaponsNode = myScene->getNode("g weapon_weapon_");
	weaponsNode->setTexture("myTextureSampler", "Weapon");

	Texture* skirtsTexture = window->renderer.createNewTexture("Skirts");
	skirtsTexture->loadFile("skirt_color.jpg");

	renderNode* skirtsNode = myScene->getNode("g skirt1_skirt_");
	skirtsNode->setTexture("skirts1:ski", "Skirts");
	
	Texture* shoulderTexture = window->renderer.createNewTexture("Shoulder");
	shoulderTexture->loadFile("shoulders_color.jpg");
	
	renderNode* shoulderNode = myScene->getNode("g shoulders_shoulders_");
	shoulderNode->setTexture("myTextureSampler", "Shoulder");

	window->renderer.createNewTexture("helmet")->loadFile("helmet_color.jpg");

	myScene->getNode("g helmet1_helmet_")->setTexture("myTextureSampler", "helmet");

	window->renderer.createNewTexture("shield")->loadFile("shield_color.jpg");

	myScene->getNode("g shield_shield_")->setTexture("myTextureSampler", "shield");
	
	Light* lol = myScene->addDirectionalLight("Light1", glm::vec3(1,0.5,0));
	myScene->activateShadow("Light1");

	Texture* text = window->renderer.createNewTexture("Hello");
	*/
	

	
	//Shadow map code
	/*GLuint frameBuffer = 0;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	printf("Framebuffer %d\n", frameBuffer);

	texturelol->textureID = 0;

	glGenTextures(1, &texturelol->textureID);
	glBindTexture(GL_TEXTURE_2D, texturelol->textureID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT, 1024,1024,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
	glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texturelol->textureID, 0);



	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer error status = 0x%x\n", status);
	}


	GLuint colorframe = 0;
	glGenFramebuffers(1, &colorframe);
	glBindFramebuffer(GL_FRAMEBUFFER, colorframe);

	glGenTextures(1, &texturelol->textureID);
	glBindTexture(GL_TEXTURE_2D, texturelol->textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1200, 900, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texturelol->textureID, 0);
	
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

	glDrawBuffer(GL_NONE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}




	myScene->generateLightBlock();

	


	texture->loadFile("dragon_knight.jpg");

	myScene->root.scale = glm::vec3(0.02,0.02,0.02);

	renderProgram* program = window->renderer.createNewProgram("image", "./shaders/shadow.vert", "./shaders/shadow.frag");
	
	glUseProgram(program->programID);

	GLuint mvpLocation = glGetUniformLocation(program->programID, "depthMVP");

	if(mvpLocation == -1)
	{
		printf("error");
	}

	glm::mat4 Model(1.0f);

	Model = glm::scale(Model, glm::vec3(0.02,0.02,0.02));

	glm::mat4 Perspective = glm::ortho<float>(-10, 10, -10, 10, -10, 20 );

	glm::mat4 View = glm::lookAt(glm::vec3(3,4,3), glm::vec3(0,0,0), glm::vec3(0,1,0));


	glBindVertexArray(window->renderer.meshes[0].VAO);
	
	*/
	myScene->generateLightBlock();
	bool turnoff = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	myScene->root.scale *= 1.0f;
	turnoff = true;
	while(!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		SDL_Event e;
		while(SDL_PollEvent(&e) > 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
			}
			if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						Up = true;
						break;
					case SDLK_s:
						Down = true;
						break;
					case SDLK_a:
						Left = true;
						break;
					case SDLK_d:
						Right = true;
						break;
					case SDLK_q:
						quit = true;
						break;
					case SDLK_x:
						ShiftUp = true;
						break;
					case SDLK_z:
						ShiftDown = true;
						break;
					case SDLK_LSHIFT:
						accelerate = true;
						break;
					case SDLK_e:
						turnoff = !turnoff;
				}
			}
			if(e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						Up = false;
						break;
					case SDLK_s:
						Down = false;
						break;
					case SDLK_a:
						Left = false;
						break;
					case SDLK_d:
						Right = false;
						break;
					case SDLK_x:
						ShiftUp = false;
						break;
					case SDLK_z:
						ShiftDown = false;
						break;
					case SDLK_LSHIFT:
						accelerate = false;
				}
			}
		}



		if(!turnoff)
		{
			computeMatrix(&mycam);
			SDL_WarpMouseInWindow(window->windowHandler, 1200/2, 900/2);
		}
		myScene->Render();

		Texture text;
		//text.textureID = lol->shadowTexture;

		/*
		glm::mat4 MVP = Perspective * View * Model;

		glBindFramebuffer(GL_FRAMEBUFFER, colorframe);
		glUseProgram(program->programID);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(MVP));
		for(int i = 0; i < window->renderer.meshes.size(); ++i)
		{
			glBindVertexArray(window->renderer.meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, window->renderer.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
		}
		//myScene->Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window->renderer.DrawTextureRect(texturelol);

		*/
		/*
		glm::mat4 MVP = Perspect  * View * Model;
		glUniformMatrix4fv(MVPID, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(VID,1,GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(MID,1,GL_FALSE, glm::value_ptr(Model));
		
		glDrawElements(GL_TRIANGLES, newMesh.indices.size(), GL_UNSIGNED_INT, 0);
		*/

		SDL_GL_SwapWindow(window->windowHandler);




	}
}

	//renderProgram firstProgram("first_program", "./shaders/first.vert", "./shaders/first.frag", &window->renderer);
	//glUseProgram(firstProgram.programID);
	/*
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	SDL_GetWindowWMInfo(window->windowHandler, &wmInfo);
	
	HWND hwnd = wmInfo.info.win.window;

	SetWindowLong(hwnd, GWL_EXSTYLE,
				GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, (255*70)/100, LWA_ALPHA);
	
    // Extend the frame into the client area.
	MARGINS margins = {0,0,0,25};

	HRESULT hr = S_OK;
	BOOL Stuff = 0;
	hr = DwmIsCompositionEnabled(&Stuff);
	BOOL enabled = FALSE;

	    // Create and populate the blur-behind structure.
    DWM_BLURBEHIND bb = {0};

    // Specify blur-behind and blur region.
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = true;
    bb.hRgnBlur = NULL;

	HRESULT hr2 = DwmEnableBlurBehindWindow(hwnd, &bb);

    if (SUCCEEDED(hr))
    {
        printf("the fuck\n");
    }
	
	SDL_Rect somerect = {-10, -10, 200, 200};
	*/

	/*Scene scene(&window->renderer);

	scene.loadFromFile("dk.3ds");

	scene.root.print();

	Texture texture1;
	texture1.loadFile("dragon_knight.jpg");
	GLint texture2Dloc = glGetUniformLocation(firstProgram.programID, "myTexture");
	glUniform1i(texture2Dloc,0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture1.textureID);

	Mesh newMesh;
	newMesh.loadMeshFromFile("dk.3ds");
	printf("number of indices is %d", newMesh.indices.size());
	GLuint MVPID = glGetUniformLocation(firstProgram.programID, "MVP");
	GLuint VID = glGetUniformLocation(firstProgram.programID,"V");
	GLuint MID = glGetUniformLocation(firstProgram.programID, "M");

	GLuint LightPosition_worldspace = glGetUniformLocation(firstProgram.programID, "LightPosition_worldspace");

	glUniform3f(LightPosition_worldspace, 3, 1, 3);

	glm::mat4 Perspect = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);

	glm::mat4 Model(1.0);

	Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.02));

	glm::mat4 View = glm::lookAt(glm::vec3(3,4,3), glm::vec3(0,0,0), glm::vec3(0,1,0));

	glm::mat4 MVP = Perspect  * View * Model;

	glBindVertexArray(newMesh.VAO);*/