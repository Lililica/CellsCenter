#version 330 core

// Sorties du shader
in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords; // Coordonnées de texture du sommet
  
out vec4 FragColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = vec4(vNormal_vs, 1.0); // Output the normal as color
} 