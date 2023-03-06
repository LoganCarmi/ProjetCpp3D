#include "Mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../External/STB/stb-master/stb_image.h"

Mesh::Mesh(glm::vec3 position, GLuint programID) : m_texture(false), scale(1.0f), m_index(false)
{
	// Identifiant pou notre variable uniforme "MVP"
	// Seulement au moment de l'initialisation.
	matrixID = glGetUniformLocation(programID, "MVP");
	isTextureID = glGetUniformLocation(programID, "isTexture");

	// Initialise le VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Matrice de mod�le
	model = glm::mat4(1.0f); // Matrice Identit� (le mod�le sera � l'origine)
	model = glm::translate(model, position);
}

Mesh::~Mesh()
{
	// Nettoie VBO, VAO, texture
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &indiceBuffer);
	glDeleteBuffers(1, &isTextureID);
	glDeleteBuffers(1, &matrixID);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vertexArrayID);
}

void Mesh::loadTexture(GLuint programID, const char* pathTexture) {
	// Cr�e une texture OpenGL
	glGenTextures(1, &texture);
	// "Lie" la nouvelle texture cr��e : tous les appels suivants aux fonctions de texture vont modifier cette texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// R�p�te si ne prend pas toute la face.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Lorsque l'on r�tr�cit l'image, utiliser un fondu lin�aire entre deux MIP maps, chacune �tant aussi filtr�e lin�airement
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Lorsque l'on agrandit l'image (aucune MIP map plus grande n'est disponible), utiliser le filtrage LIN�AIRE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Lit le fichier, appel glTexImage2D avec les bons param�tres
	int width, height, nrChannels;
	unsigned char* data = stbi_load(pathTexture, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		// G�n�rer les MIP maps. 
		glGenerateMipmap(GL_TEXTURE_2D);

		// Initialise "myTextureSampler"
		textureID = glGetUniformLocation(programID, "myTextureSampler");
	}
	else
	{
		printf("Failed to load texture");
	}
	stbi_image_free(data);
}

void Mesh::createMesh(GLuint programID, glm::mat4 Projection, glm::mat4 View, const char* pathObj, GLfloat* colorBufferData, int sizeColorBuffer, const char* pathTexture, bool isIndexed)
{
	m_index = isIndexed;

	// ModelViewProjection : multiplication des trois  matrices
	MVP = Projection * View * model; // Rappel : la multiplication de matrice fonctionne dans l'autre sens

	if (pathTexture != "") {
		loadTexture(programID, pathTexture);
		m_texture = 1;
	}

	// Lire le fichier .obj
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(pathObj, vertices, uvs, normals);

	std::vector<glm::vec3> verticesIndex;
	std::vector<glm::vec2> uvsIndex;
	std::vector<glm::vec3> normalsIndex;
	std::vector<unsigned short> indices;
	if (m_index) {
		indexVBO(vertices, uvs, normals, indices, verticesIndex,
			uvsIndex, normalsIndex);
		m_vertexNumber = verticesIndex.size();
	}
	else
		m_vertexNumber = vertices.size();

	glGenBuffers(1, &vertexBuffer); // G�n�re un tampon et place l'identifiant dans 'vertexbuffer'
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Les commandes suivantes vont parler de notre tampon 'vertexbuffer'
	// Fournit les sommets � OpenGL.
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, verticesIndex.size() * sizeof(glm::vec3), &verticesIndex[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeColorBuffer, colorBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, uvsIndex.size() * sizeof(glm::vec2), &uvsIndex[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, normalsIndex.size() * sizeof(glm::vec3), &normalsIndex[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	if (m_index) {
		glGenBuffers(1, &indiceBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
	}
}

void Mesh::createMesh(GLuint programID, glm::mat4 Projection, glm::mat4 View, GLfloat* vertexBufferData, int sizeVertexBuffer, GLfloat* colorBufferData, int sizeColorBuffer, GLfloat* uvBufferData, int sizeUvBuffer, const char* pathTexture, bool isIndexed)
{
	m_index = isIndexed;

	// ModelViewProjection : multiplication des trois  matrices
	MVP = Projection * View * model; // Rappel : la multiplication de matrice fonctionne dans l'autre sens

	if (pathTexture != "") {
		loadTexture(programID, pathTexture);
		m_texture = 1;
	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	// Cr�ation des vertices et des normales
	int i = 0;
	while (i < sizeVertexBuffer)
	{
		// R�cup�ration des points
		glm::vec3 p1 = glm::vec3(vertexBufferData[i], vertexBufferData[i + 1], vertexBufferData[i + 2]);
		vertices.push_back(p1);
		glm::vec3 p2 = glm::vec3(vertexBufferData[i + 3], vertexBufferData[i + 4], vertexBufferData[i + 5]);
		vertices.push_back(p2);
		glm::vec3 p3 = glm::vec3(vertexBufferData[i + 6], vertexBufferData[i + 7], vertexBufferData[i + 8]);
		vertices.push_back(p3);

		// Cr�ation des faces
		glm::vec3 v1 = p2 - p1;
		glm::vec3 v2 = p3 - p1;

		// Calcul de la normale du triangle
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
		// On le met 3 fois pour le nombre de points
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);

		i += 9;
	}
	// Cr�ation des UVs
	i = 0;
	while (i < sizeUvBuffer)
	{
		// R�cup�ration des points
		glm::vec2 uv = glm::vec2(vertexBufferData[i], vertexBufferData[i + 1]);
		uvs.push_back(uv);

		i += 2;
	}

	std::vector<glm::vec3> verticesIndex;
	std::vector<glm::vec2> uvsIndex;
	std::vector<glm::vec3> normalsIndex;
	std::vector<unsigned short> indices;
	if (m_index) {
		indexVBO(vertices, uvs, normals, indices, verticesIndex,
			uvsIndex, normalsIndex);
		m_vertexNumber = verticesIndex.size();
	}
	else
		m_vertexNumber = vertices.size();

	glGenBuffers(1, &vertexBuffer); // G�n�re un tampon et place l'identifiant dans 'vertexbuffer'
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Les commandes suivantes vont parler de notre tampon 'vertexbuffer'
	// Fournit les sommets � OpenGL.
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeVertexBuffer, vertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeColorBuffer, colorBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeUvBuffer, uvBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	if (m_index)
		glBufferData(GL_ARRAY_BUFFER, normalsIndex.size() * sizeof(glm::vec3), &normalsIndex[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	if (m_index) {
		glGenBuffers(1, &indiceBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
	}
}

void Mesh::scaleMesh(double yoffset, glm::mat4 Projection, glm::mat4 View)
{
	scale += yoffset * 0.1f;
	model = glm::scale(model, glm::vec3(scale));
	scale = 1.0f;
	MVP = Projection * View * model;
}

void Mesh::updateMVP(glm::mat4 Projection, glm::mat4 View)
{
	MVP = Projection * View * model;
}

void Mesh::drawMesh()
{
	// Envoie notre transformation au shader dans la variable uniforme "MVP"
		// Pour chaque mod�le, la MVP sera diff�rente (au moins la partie M)
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	// Lie texture � GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Lie "myTextureSampler" � GL_TEXTURE0
	glUniform1i(textureID, 0);

	if (m_texture)
		glUniform1i(isTextureID, 1);
	else
		glUniform1i(isTextureID, 0);

	// Premier tampon d'attributs : les sommets
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribut 0. Aucune raison particuli�re pour 0, mais cela doit correspondre au � layout � dans le shader 
		3,                  // taille
		GL_FLOAT,           // type 
		GL_FALSE,           // normalis�
		0,                  // nombre d'octets s�parant deux sommets dans le tampon
		(void*)0            // d�calage du tableau de tampon
	);

	// deuxi�me tampon d'attributs : Couleurs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1,
		3,                                // taille
		GL_FLOAT,                         // type
		GL_FALSE,
		0,                                // nombre d'octets s�parant deux sommets dans le tampon
		(void*)0                          // d�calage du tableau de tampons
	);

	// troisi�me tampon d'attributs : UVs
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(
		2,
		2,                                // taille : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,
		0,                                // nombre d'octets s�parant deux sommets dans le tampon
		(void*)0                          // d�calage du tableau de tampons
	);

	// quatri�me tampon d'attributs : Normales
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(
		3,
		3,                                // taille
		GL_FLOAT,                         // type
		GL_FALSE,
		0,                                // nombre d'octets s�parant deux sommets dans le tampon
		(void*)0                          // d�calage du tableau de tampons
	);


	if (m_index) {
		// Index buffer 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);

		// Dessine les triangles  
		glDrawElements(
			GL_TRIANGLES,      // mode 
			m_vertexNumber,    // nombre
			GL_UNSIGNED_SHORT,   // type 
			(void*)0           // d�calage  du tableau de tampons
		);
	}
	else {
		// Dessine le Mesh
		glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber); // D�marre � partir du sommet 0
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}