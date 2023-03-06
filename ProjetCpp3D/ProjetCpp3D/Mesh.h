#pragma once
// Inclure les ent�tes standard
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../External/GLM/glm-master/glm/glm.hpp"
#include "../External/GLM/glm-master/glm/ext/matrix_clip_space.hpp"  
#include "../External/GLM/glm-master/glm/ext/matrix_transform.hpp"
#include "objloader.hpp"
#include "vboindexer.hpp"

class Mesh
{
private:
	GLuint vertexArrayID;	// Vertex Array Object (VAO) / 1 par objet
	GLuint matrixID;		// Identifiant pour notre variable uniforme "MVP"
	GLuint texture;			// Texture OpenGL
	GLuint textureID;		// Identifiant pour notre variable uniforme "myTextureSampler"
	GLuint isTextureID;		// Identifiant pour notre variable uniform "isTexture"
	GLuint vertexBuffer;	// Permet d'identifier notre tampon de sommets / Vertex Buffer Object (VBO)
	GLuint colorBuffer;		// VBO de la couleur
	GLuint uvBuffer;		// VBO des UV
	GLuint normalBuffer;	// VBO des normales
	GLuint indiceBuffer;	// VBO des indices

	glm::mat4 MVP;			// MVP Matrix
	glm::mat4 model;		// Model Matrix
	bool m_texture, m_index;
	int m_vertexNumber;
	float scale;

	void loadTexture(GLuint programID, const char* pathTexture);

public:
	Mesh(glm::vec3 position, GLuint programID);
	~Mesh();

	void createMesh(GLuint programID, glm::mat4 Projection, glm::mat4 View, const char* pathObj, GLfloat* colorBufferData, int sizeColorBuffer, const char* pathTexture = "", bool isIndexed = true);
	void createMesh(GLuint programID, glm::mat4 Projection, glm::mat4 View, GLfloat* vertexBufferData, int sizeVertexBuffer, GLfloat* colorBufferData, int sizeColorBuffer, GLfloat* uvBufferData, int sizeUvBuffer, const char* pathTexture = "", bool isIndexed = true);

	void scaleMesh(double yoffset, glm::mat4 Projection, glm::mat4 View);
	void updateMVP(glm::mat4 Projection, glm::mat4 View);
	void drawMesh();
};