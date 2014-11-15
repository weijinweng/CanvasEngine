#pragma once
#include "Canvas.h"

namespace Canvas
{
	struct cameraControls
	{
		bool w, a, s, d, z, x, shift;
		cameraControls(bool all) :w(all), a(all), s(all), d(all), z(all), x(all), shift(all)
		{}
	};

	class Camera
	{
	public:
		float FOV;
		float aspectRatio;
		glm::mat4 Perspective;
		glm::vec3 position;
		Camera();
		void manageInput();
		virtual glm::mat4 getView();
		virtual glm::mat4 getPerspective();
	};

	class targetCamera :public Camera
	{
	protected:
		glm::vec3 target;
		glm::vec3 position;
	public:
		targetCamera(glm::vec3 target);
		targetCamera();
		targetCamera(glm::vec3  target, glm::vec3 position);
		//Camera manipulation
		void lookAt(glm::vec3 target);
		void move(glm::vec3 offset);
		void moveTo(glm::vec3 destination);
		//Camera data generation
		glm::mat4 getView();
	};

	class fpsCamera :public Camera
	{
	public:
		cameraControls controls;
		float verticleAngle, horizontalAngle;
		fpsCamera(float FOV, float aspectRatio, glm::vec3 position);
		void getEvents(SDL_Event e);
		void processEvents();
		glm::mat4 getView();
	};
}

