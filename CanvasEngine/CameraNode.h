#pragma once

#include "RendererNode.h"
#include "ViewGL.h"

namespace Canvas{
	
	class RCameraNode{
		glm::vec3 m_pos;
		glm::fquat m_orientation;
		float FOV;
		float aspectRatio;
		float NearZ;
		float FarZ;
		View m_view;
	public:
		RCameraNode(glm::vec3 _pos, glm::vec3 _target, float _FOV = 45.0f, float _aspectRatio = 4.0f/3.0f, float _NearZ = 0.1f, float _FarZ = 100.0f);
		
		View* GetView();

		void SetPosition(glm::vec3 _pos);
		void SetTarget(glm::vec3 _target);
		void SetOrientation(glm::vec3 _orientation);
		void RotateAround(glm::vec3 _rot);
		void ShiftLocalPos(glm::vec3 _shift);
	};
}