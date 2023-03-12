#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position) : m_position(position), speed(3.0f), rotationSpeed(1.0f)
{
	CanJump = true;
	IsJumping = false;
	JumpTime = 0;
	// Angle horizontal : vers -Z 
	horizontalAngle = 3.14f;
	// Angle vertical : 0, regarde l'horizon 
	verticalAngle = 0.0f;
	// Direction : une conversion de coordonn�es sph�riques vers coordonn�es cart�siennes
	m_direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	// Vecteur � vers la droite �
	m_right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
	// Vecteur � haut � : perpendiculaire � la direction et le vecteur vers la droite
	m_up = glm::cross(m_right, m_direction);
	// Matrice de la cam�ra
	view = glm::lookAt(
		m_position,				// Position de la cam�ra (4,3,3) dans l'espace monde
		m_position + m_direction,			// Direction de la cam�ra (ce qu'elle regarde)
		m_up							// Vecteur Up (utilisez 0,-1,0 pour regarder � l'envers)
	);

	lastTime = glfwGetTime();
}

Camera::~Camera() {
}

void Camera::moveCamera(GLFWwindow* window)
{
	// R�cup�re le temps pour le d�placement
	currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;
	// Aller vers l'avant
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		m_position += m_direction * deltaTime * speed;
	// Aller vers l'arri�re
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_position -= m_direction * deltaTime * speed;
	// Pas � droite
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_position += m_right * deltaTime * speed;
	// Pas � gauche
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_position -= m_right * deltaTime * speed;
	// Tourne � gauche
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		horizontalAngle += rotationSpeed * deltaTime;
	// Tourne � droite
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		horizontalAngle -= rotationSpeed * deltaTime;


	// Permet la détection du jump et le jump en lui même.
	// Ici ma variable CanJump détecte s'il a la possibilité d'activer le jump, donc si le joueur a les conditions requises pour jump
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && CanJump == true) {
		IsJumping = true;                                                          
		m_position += m_up * deltaTime * speed;
		JumpTime += deltaTime;
		std::cout << JumpTime << "\n";
		if (JumpTime > 2.0f) { // Ici j'ai set 2.0s en temps maximal pour rester appuyé sur la touche pour sauter
			CanJump = false;
		}
	}

	// Cette fonction permet d'empêcher la réactivation du jump lorsque le joueur a relaché la touche
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && IsJumping == true) {
		CanJump = false;
		m_position -= m_up * deltaTime * speed;
		JumpTime = 0;
		if (m_position.y <= 0) {
			IsJumping = false;
			CanJump = true;
		}
	}

	// Permet la "gravité", donc de faire redescendre le joueur à la hauteur y = 0 
	else if (CanJump == false && IsJumping == true) {
		m_position -= m_up * deltaTime * speed;
		if (m_position.y <= 0) {
			IsJumping = false;
			CanJump = true;
			JumpTime = 0;
		}
	}



	m_direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	m_right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
	view = glm::lookAt(
		m_position,		// Position de la cam�ra (4,3,3) dans l'espace monde
		m_position + m_direction,	// Direction de la cam�ra (ce qu'elle regarde)
		m_up			// Vecteur Up (utilisez 0,-1,0 pour regarder � l'envers)
	);
}

glm::vec3 Camera::getPosition() { return m_position; }