#version 330 core

// Valeurs interpolées à partir du vertex shader
in vec3 fragmentPosition;
in vec3 fragmentColor;
in vec2 fragmentUV;
in vec3 fragmentNormal;

// Données de sortie
out vec3 color;

// Valeurs qui sont constantes pour l'ensemble du modèle.
uniform sampler2D myTextureSampler;
uniform bool isTexture;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
	// Lumière Ambiante
	float ambiantFactor = 0.1f;
	vec3 ambiant = lightColor * ambiantFactor;

	// Lumière Diffuse
	vec3 normal = normalize(fragmentNormal);
	//vec3 lightDir = normalize(lightPosition - fragmentPosition);
	vec3 lightDir = normalize(viewPosition - fragmentPosition);
	float NDotL = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = lightColor * NDotL;

	// Lumière Spéculaire
	float specularFactor = 0.8f;
	float shininess = 32.0f;
	vec3 viewDir = normalize(viewPosition - fragmentPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = lightColor * specularFactor * pow(NDotH, shininess);

	vec3 result = ambiant + diffuse + specular;
	
	if(!isTexture) {
		// Output color = couleur spécifié dans le vertex shader interpolé entre les trois sommets alentours
		color = result * fragmentColor;
	}
	else {
		// Output color = couleur de la texture pour les coordonnées UV spécifiées
		vec3 texel = texture(myTextureSampler, fragmentUV).rgb;
		color = result * texel;
	}
}