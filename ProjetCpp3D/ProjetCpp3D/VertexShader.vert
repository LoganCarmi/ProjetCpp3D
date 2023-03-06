#version 330 core // Indique au compilateur que l'on va utiliser OpenGL 3

// Input vertex data, différent pour chaque exécution du shader
layout(location = 0) in vec3 vertexPosition_modelspace;
// Notez que le "1" ici correspond au "1" dans glVertexAttribPointer 
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal;

// Valeur qui reste constante pour tout le Mesh
uniform mat4 MVP;

// Données de sortie : sera interpolée pour chaque fragment 
out vec3 fragmentPosition;
out vec3 fragmentColor;
out vec2 fragmentUV;
out vec3 fragmentNormal;

void main()
{
    // Obtient la position du sommet, dans l'espace de découpe : MVP * position
    gl_Position = MVP * vec4(vertexPosition_modelspace,1);

    // La couleur de chaque sommet sera interpolée pour produire la couleur de chaque fragment
    fragmentPosition = vertexPosition_modelspace;
    fragmentColor = vertexColor;
    fragmentUV = vertexUV;
    fragmentNormal = vertexNormal;
}