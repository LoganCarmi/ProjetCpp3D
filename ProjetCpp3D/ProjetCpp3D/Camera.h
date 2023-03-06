#pragma once
#include "GLFW/glfw3.h"
#include "../External/GLM/glm-master/glm/glm.hpp"
#include "../External/GLM/glm-master/glm/ext/matrix_clip_space.hpp"
#include "../External/GLM/glm-master/glm/ext/matrix_transform.hpp"

class Camera {
	private:
		glm::vec3 m_position, m_direction, m_right, m_up;
		float horizontalAngle;
		float verticalAngle;
		glm::mat4 view;
		double lastTime, currentTime;
		float speed, rotationSpeed;
		bool IsJumping, CanJump;
		float JumpTime;

	public:
		Camera(glm::vec3 position);
		~Camera();

		inline glm::mat4 getView() { return view; }
		void moveCamera(GLFWwindow* window);
		glm::vec3 getPosition();
};