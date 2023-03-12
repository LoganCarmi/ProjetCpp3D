#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdio>
#include "Shader.hpp"
#include "Camera.h"
#include <vector>
#include "Mesh.h"


GLFWwindow* window;
std::vector<Mesh*> meshes;
glm::mat4 projection, view;

void scroll_callback(GLFWwindow* window, double xoffste, double yoffset)
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i]->scaleMesh(yoffset, projection, view);
}

int main() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // antialiasing 4x (4 fragment dans un pixel)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Version OpenGL 3.3 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Pas de support de l'ancien OpenGL

	// Ouvre une fenêtre et crée son contexte OpenGL
	window = glfwCreateWindow(1024, 768, "C++ Jeux 3D - OpenGL", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialise GLEW
	glewExperimental = true; // Nécessaire pour le profil core
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	// Permet d'utiliser la touche Echap
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Fond d'�cran Bleu-Noir
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Active le test de profondeur
	glEnable(GL_DEPTH_TEST);
	// Accepte le fragment s'il est plus proche de la cam�ra que le pr�c�dent accept�
	glDepthFunc(GL_LESS);

	// Crée et compile notre programme GLSL à partir des shaders
	GLuint programID = LoadShaders("VertexShader.vert", "FragmentShader.frag");

	GLuint lightColorID = glGetUniformLocation(programID, "lightColor");
	glm::vec3 lightColor = glm::vec3(1.0f);
	GLuint lightPositionID = glGetUniformLocation(programID, "lightPosition");
	glm::vec3 lightPosition = glm::vec3(0, 0, 7);
	GLuint viewPositionID = glGetUniformLocation(programID, "viewPosition");

	Camera* cam = new Camera(glm::vec3(0, 0, 7));
	view = cam->getView();

	// Matrice de projection : Champ de vision de 45� , ration 4:3, distance d'affichage : 0.1 unit�s <-> 100 unit�s 
	projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Pour une cam�ra orthographique :
	//glm::mat4 projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Un tableau de trois vecteurs qui repr�sentent trois sommets
	GLfloat g_vertex_buffer_data_triangle[] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};
	GLfloat g_color_buffer_data_triangle[] = {
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
	};
	GLfloat g_uv_buffer_data_triangle[] = {
		 0.0f, 0.0f,
		 0.0f, 0.0f,
		 0.0f, 0.0f,
	};

	// Les sommets. Trois floats cons�cutifs donnent un sommet 3D ; trois sommets cons�cutifs donnent un triangle
	// Un cube poss�de six faces avec deux triangles pour chaque, donc cela fait 6*2=12 triangles et 12*3=36 sommets 
	GLfloat g_vertex_buffer_data_cube[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : d�but
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : fin 
	1.0f, 1.0f,-1.0f, // triangle 2 : d�but 
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : fin 
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
	};
	// Une couleur pour chaque sommet. Elles ont �t� g�n�r�es al�atoirement. 
	GLfloat g_color_buffer_data_cube[] = {
	0.583f,  0.771f,  0.014f,
	0.609f,  0.115f,  0.436f,
	0.327f,  0.483f,  0.844f,
	0.822f,  0.569f,  0.201f,
	0.435f,  0.602f,  0.223f,
	0.310f,  0.747f,  0.185f,
	0.597f,  0.770f,  0.761f,
	0.559f,  0.436f,  0.730f,
	0.359f,  0.583f,  0.152f,
	0.483f,  0.596f,  0.789f,
	0.559f,  0.861f,  0.639f,
	0.195f,  0.548f,  0.859f,
	0.014f,  0.184f,  0.576f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.116f,
	0.676f,  0.977f,  0.133f,
	0.971f,  0.572f,  0.833f,
	0.140f,  0.616f,  0.489f,
	0.997f,  0.513f,  0.064f,
	0.945f,  0.719f,  0.592f,
	0.543f,  0.021f,  0.978f,
	0.279f,  0.317f,  0.505f,
	0.167f,  0.620f,  0.077f,
	0.347f,  0.857f,  0.137f,
	0.055f,  0.953f,  0.042f,
	0.714f,  0.505f,  0.345f,
	0.783f,  0.290f,  0.734f,
	0.722f,  0.645f,  0.174f,
	0.302f,  0.455f,  0.848f,
	0.225f,  0.587f,  0.040f,
	0.517f,  0.713f,  0.338f,
	0.053f,  0.959f,  0.120f,
	0.393f,  0.621f,  0.362f,
	0.673f,  0.211f,  0.457f,
	0.820f,  0.883f,  0.371f,
	0.982f,  0.099f,  0.879f
	};
	// Deux coordonn�es UV pour chaque sommet. Les coordonn�es ont �t� cr��es avec Blender.
	GLfloat g_uv_buffer_data_cube[] = {
	0.000059f, 1.0f - 0.000004f,
	0.000103f, 1.0f - 0.336048f,
	0.335973f, 1.0f - 0.335903f,
	1.000023f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.336024f, 1.0f - 0.671877f,
	0.667969f, 1.0f - 0.671889f,
	1.000023f, 1.0f - 0.000013f,
	0.668104f, 1.0f - 0.000013f,
	0.667979f, 1.0f - 0.335851f,
	0.000059f, 1.0f - 0.000004f,
	0.335973f, 1.0f - 0.335903f,
	0.336098f, 1.0f - 0.000071f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.336024f, 1.0f - 0.671877f,
	1.000004f, 1.0f - 0.671847f,
	0.999958f, 1.0f - 0.336064f,
	0.667979f, 1.0f - 0.335851f,
	0.668104f, 1.0f - 0.000013f,
	0.335973f, 1.0f - 0.335903f,
	0.667979f, 1.0f - 0.335851f,
	0.335973f, 1.0f - 0.335903f,
	0.668104f, 1.0f - 0.000013f,
	0.336098f, 1.0f - 0.000071f,
	0.000103f, 1.0f - 0.336048f,
	0.000004f, 1.0f - 0.671870f,
	0.336024f, 1.0f - 0.671877f,
	0.000103f, 1.0f - 0.336048f,
	0.336024f, 1.0f - 0.671877f,
	0.335973f, 1.0f - 0.335903f,
	0.667969f, 1.0f - 0.671889f,
	1.000004f, 1.0f - 0.671847f,
	0.667979f, 1.0f - 0.335851f
	};



	// Déclaration de mes meshes

	Mesh* mesh = new Mesh(glm::vec3(0, 0, 0), programID);
	mesh->createMesh(programID, projection, view, "cube.obj", g_color_buffer_data_cube, sizeof(g_color_buffer_data_cube));
	meshes.push_back(mesh);

	Mesh* mesh2 = new Mesh(glm::vec3(3, 2, 3), programID);
	mesh2->createMesh(programID, projection, view, "cube.obj", g_color_buffer_data_cube, sizeof(g_color_buffer_data_cube));
	meshes.push_back(mesh2);

	Mesh* mesh3 = new Mesh(glm::vec3(-3, 0, -3), programID);
	mesh3->createMesh(programID, projection, view, "cube.obj", g_color_buffer_data_cube, sizeof(g_color_buffer_data_cube));
	meshes.push_back(mesh3);

	Mesh* mesh4 = new Mesh(glm::vec3(3, 0, -3), programID);
	mesh3->createMesh(programID, projection, view, "cube.obj", g_color_buffer_data_cube, sizeof(g_color_buffer_data_cube));
	meshes.push_back(mesh3);

	Mesh* mesh5 = new Mesh(glm::vec3(-3, 0, 3), programID);
	mesh3->createMesh(programID, projection, view, "cube.obj", g_color_buffer_data_cube, sizeof(g_color_buffer_data_cube));
	meshes.push_back(mesh3);

	glfwSetScrollCallback(window, scroll_callback);
	do
	{
		// Nettoie l'�cran
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Utilise notre shader
		glUseProgram(programID);

		glUniform3fv(lightColorID, 1, &lightColor[0]);
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glm::vec3 viewPosition = cam->getPosition();
		glUniform3fv(viewPositionID, 1, &viewPosition[0]);

		cam->moveCamera(window);
		view = cam->getView();
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->updateMVP(projection, view);
			meshes[i]->drawMesh();
		}

		// �change les tampons
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // V�rifie si la touche �chap a �t� appuy�e ou si la fen�tre a �t� ferm�e
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteProgram(programID);

	// Ferme OpenGL et GLFW
	glfwTerminate();

	return 0;
}