#pragma once

#include "CanvasCore.h"

namespace Canvas{

	struct ShaderVariableContainerM4{
		int loc;
		glm::mat4 value;
		std::string name;
	};

	struct ShaderVariableContainerF4{
		int loc;
		glm::vec4 value;
		std::string name;
	};

	struct ShaderVariableContainerF3{
		int loc;
		glm::vec3 value;
		std::string name;
	};

	struct ShaderVariableContainerF{
		int loc;
		float value;
		std::string name;
	};

	struct ShaderVariableContainerI{
		int loc;
		int value;
		std::string name;
	};

	struct ShaderVariableContainerT2D{
		int loc;
		Texture* tex;
		std::string name;
	};

	struct ShaderVariableTemp{
		std::map<std::string, ShaderVariableContainerF> m_floats;
		std::map<std::string, ShaderVariableContainerF4> m_vec4;
		std::map<std::string, ShaderVariableContainerF3> m_vec3;
		std::map<std::string, ShaderVariableContainerI> m_ints;
		std::map<std::string, ShaderVariableContainerT2D> m_tex2ds;
		std::map<std::string, ShaderVariableContainerM4> m_mat4s;

		void AddValue(std::string, int);
		void AddValue(std::string, float);
		void AddValue(std::string, glm::vec4);
		void AddValue(std::string, glm::vec3);
		void AddValue(std::string, glm::mat4);

		glm::mat4 FindMat4(std::string);
		glm::vec3 FindVec3(std::string);
		glm::vec4 FindVec4(std::string);
		float FindFloat(std::string);
		int FindInt(std::string);
		Texture2DNode* FindTexture(std::string);
	};

}