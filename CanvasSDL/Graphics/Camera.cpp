#include "Camera.h"

namespace Canvas
{
	Camera::Camera()
	{

	}

	glm::mat4 Camera::getView()
	{
		return glm::mat4(1.0f);
	}

	glm::mat4 Camera::getPerspective()
	{
		Perspective = glm::perspective(FOV, aspectRatio, 0.1f, 1000.0f);
		return Perspective;
	}

	fpsCamera::fpsCamera(float FOV, float aspectRatio, glm::vec3 position) :verticleAngle(0.0f), horizontalAngle(0.0f), controls(false)
	{
		this->FOV = FOV;
		this->aspectRatio = aspectRatio;
		this->position = position;
		this->getPerspective();
	}

	void fpsCamera::getEvents(SDL_Event e)
	{
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				controls.w = true;
				break;
			case SDLK_s:
				controls.s = true;
				break;
			case SDLK_a:
				controls.a = true;
				break;
			case SDLK_d:
				controls.d = true;
				break;
			case SDLK_x:
				controls.x = true;
				break;
			case SDLK_z:
				controls.z = true;
				break;
			case SDLK_LSHIFT:
				controls.shift = true;
			}
		}
		if (e.type == SDL_KEYUP)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_w:
				controls.w = false;
				break;
			case SDLK_s:
				controls.s = false;
				break;
			case SDLK_a:
				controls.a = false;
				break;
			case SDLK_d:
				controls.d = false;
				break;
			case SDLK_x:
				controls.x = false;
				break;
			case SDLK_z:
				controls.z = false;
				break;
			case SDLK_LSHIFT:
				controls.shift = false;
			}
		}
	}

	glm::mat4 fpsCamera::getView()
	{
		glm::vec3 direction(std::cos(verticleAngle)*std::sin(horizontalAngle),
			std::sin(verticleAngle),
			std::cos(verticleAngle)*std::cos(horizontalAngle));

		glm::vec3 directionRight(std::sin(horizontalAngle - 3.14f / 2.0f),
			0,
			std::cos(horizontalAngle - 3.14f / 2.0f));

		glm::vec3 up = glm::cross(direction, directionRight);

		glm::mat4 View = glm::lookAt(position, direction + position, up);

		return View;
	}

}