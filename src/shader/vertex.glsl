#version 330 core

// Attributs de sommet
layout(location = 0) in vec3 aVertexPosition; // Position du sommet
layout(location = 1) in vec3 aVertexNormal; // Normale du sommet
layout(location = 2) in vec2 aVertexTexCoords; // Coordonnées de texture du sommet



uniform vec3 uPosition; // Position de la caméra dans l'espace View
uniform float uScale; // Facteur d'échelle pour les transformations
uniform mat4 uCameraMatrix; // Matrice de la caméra pour la transformation de la vue

uniform mat4 uProjMatrix; // Matrice de projection pour la transformation de la projection

// Proj part

uniform float ufov; // Champ de vision (Field of View)
uniform float uaspect; // Rapport d'aspect (Aspect Ratio)
uniform float unear; // Plan de coupe proche (Near Plane)
uniform float ufar; // Plan de coupe éloigné (Far Plane)

// Sorties du shader
out vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
out vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
out vec2 vTexCoords; // Coordonnées de texture du sommet
   

float radians(float degrees) {
    float PI = 3.14159265358979323846;

    return degrees * (PI / 180.0);
}

float sin(float x) {
    // Approximation de sin(x) par la série de Taylor (précision suffisante pour GLSL)
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x3 * x2;
    float x7 = x5 * x2;
    return x - (x3 / 6.0) + (x5 / 120.0) - (x7 / 5040.0);
}

float cos(float x) {
    // Approximation de cos(x) par la série de Taylor (précision suffisante pour GLSL)
    float x2 = x * x;
    float x4 = x2 * x2;
    float x6 = x4 * x2;
    return 1.0 - (x2 / 2.0) + (x4 / 24.0) - (x6 / 720.0);
}

float tan(float x) {
    return sin(x) / cos(x);
}

mat4 projection(float fov, float aspect, float near, float far) {
    float tanHalfFov = tan(fov / 2.0);
    return mat4(
        1.0 / (aspect * tanHalfFov), 0.0, 0.0, 0.0,
        0.0, 1.0 / tanHalfFov, 0.0, 0.0,
        0.0, 0.0, -(far + near) / (far - near), -1.0,
        0.0, 0.0, -(2.0 * far * near) / (far - near), 0.0
    );
}

mat4 translate(mat4 matrix, vec3 translation) {
    mat4 translationMatrix = mat4(1.0);
    translationMatrix[3] = vec4(translation, 1.0);
    return matrix * translationMatrix;
}

mat4 scale(mat4 matrix, float scaleFactor) {
    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = scaleFactor;
    scaleMatrix[1][1] = scaleFactor;
    scaleMatrix[2][2] = scaleFactor;
    return matrix * scaleMatrix;
}

mat4 transpose(mat4 matrix) {
    return mat4(
        vec4(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]),
        vec4(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]),
        vec4(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]),
        vec4(matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3])
    );
}

float determinant(mat4 matrix) {
    // Calcul du déterminant d'une matrice 4x4
    float det = matrix[0][0] * (matrix[1][1] * (matrix[2][2] * matrix[3][3] - matrix[2][3] * matrix[3][2]) -
                                matrix[1][2] * (matrix[2][1] * matrix[3][3] - matrix[2][3] * matrix[3][1]) +
                                matrix[1][3] * (matrix[2][1] * matrix[3][2] - matrix[2][2] * matrix[3][1]))
                - matrix[0][1] * (matrix[1][0] * (matrix[2][2] * matrix[3][3] - matrix[2][3] * matrix[3][2]) -
                                matrix[1][2] * (matrix[2][0] * matrix[3][3] - matrix[2][3] * matrix[3][0]) +
                                matrix[1][3] * (matrix[2][0] * matrix[3][2] - matrix[2][2] * matrix[3][0]))
                + matrix[0][2] * (matrix[1][0] * (matrix[2][1] * matrix[3][3] - matrix[2][3] * matrix[3][1]) -
                                matrix[1][1] * (matrix[2][0] * matrix[3][3] - matrix[2][3] * matrix[3][0]) +
                                matrix[1][3] * (matrix[2][0] * matrix[3][1] - matrix[2][1] * matrix[3][0]))
                - matrix[0][3] * (matrix[1][0] * (matrix[2][1] * matrix[3][2] - matrix[2][2] * matrix[3][1]) -
                                matrix[1][1] * (matrix[2][0] * matrix[3][2] - matrix[2][2] * matrix[3][0]) +
                                matrix[1][2] * (matrix[2][0] * matrix[3][1] - matrix[2][1] * matrix[3][0]));
    return det;
}

mat4 myInverse(mat4 matrix) {
    // Algorithm de calcul de l'inverse d'une matrice 4x4
    float det = determinant(matrix);
    if (abs(det) < 1e-6) {
        // Return identity if not invertible
        return mat4(1.0);
    }

    // Compute the inverse using the classical adjugate/determinant method
    mat4 inv;
    inv[0][0] =  (matrix[1][1]*matrix[2][2]*matrix[3][3] + matrix[1][2]*matrix[2][3]*matrix[3][1] + matrix[1][3]*matrix[2][1]*matrix[3][2]
               - matrix[1][1]*matrix[2][3]*matrix[3][2] - matrix[1][2]*matrix[2][1]*matrix[3][3] - matrix[1][3]*matrix[2][2]*matrix[3][1]) / det;
    inv[0][1] = (-matrix[0][1]*matrix[2][2]*matrix[3][3] - matrix[0][2]*matrix[2][3]*matrix[3][1] - matrix[0][3]*matrix[2][1]*matrix[3][2]
               + matrix[0][1]*matrix[2][3]*matrix[3][2] + matrix[0][2]*matrix[2][1]*matrix[3][3] + matrix[0][3]*matrix[2][2]*matrix[3][1]) / det;
    inv[0][2] =  (matrix[0][1]*matrix[1][2]*matrix[3][3] + matrix[0][2]*matrix[1][3]*matrix[3][1] + matrix[0][3]*matrix[1][1]*matrix[3][2]
               - matrix[0][1]*matrix[1][3]*matrix[3][2] - matrix[0][2]*matrix[1][1]*matrix[3][3] - matrix[0][3]*matrix[1][2]*matrix[3][1]) / det;
    inv[0][3] = (-matrix[0][1]*matrix[1][2]*matrix[2][3] - matrix[0][2]*matrix[1][3]*matrix[2][1] - matrix[0][3]*matrix[1][1]*matrix[2][2]
               + matrix[0][1]*matrix[1][3]*matrix[2][2] + matrix[0][2]*matrix[1][1]*matrix[2][3] + matrix[0][3]*matrix[1][2]*matrix[2][1]) / det;

    inv[1][0] = (-matrix[1][0]*matrix[2][2]*matrix[3][3] - matrix[1][2]*matrix[2][3]*matrix[3][0] - matrix[1][3]*matrix[2][0]*matrix[3][2]
               + matrix[1][0]*matrix[2][3]*matrix[3][2] + matrix[1][2]*matrix[2][0]*matrix[3][3] + matrix[1][3]*matrix[2][2]*matrix[3][0]) / det;
    inv[1][1] =  (matrix[0][0]*matrix[2][2]*matrix[3][3] + matrix[0][2]*matrix[2][3]*matrix[3][0] + matrix[0][3]*matrix[2][0]*matrix[3][2]
               - matrix[0][0]*matrix[2][3]*matrix[3][2] - matrix[0][2]*matrix[2][0]*matrix[3][3] - matrix[0][3]*matrix[2][2]*matrix[3][0]) / det;
    inv[1][2] = (-matrix[0][0]*matrix[1][2]*matrix[3][3] - matrix[0][2]*matrix[1][3]*matrix[3][0] - matrix[0][3]*matrix[1][0]*matrix[3][2]
               + matrix[0][0]*matrix[1][3]*matrix[3][2] + matrix[0][2]*matrix[1][0]*matrix[3][3] + matrix[0][3]*matrix[1][2]*matrix[3][0]) / det;
    inv[1][3] =  (matrix[0][0]*matrix[1][2]*matrix[2][3] + matrix[0][2]*matrix[1][3]*matrix[2][0] + matrix[0][3]*matrix[1][0]*matrix[2][2]
               - matrix[0][0]*matrix[1][3]*matrix[2][2] - matrix[0][2]*matrix[1][0]*matrix[2][3] - matrix[0][3]*matrix[1][2]*matrix[2][0]) / det;

    inv[2][0] =  (matrix[1][0]*matrix[2][1]*matrix[3][3] + matrix[1][1]*matrix[2][3]*matrix[3][0] + matrix[1][3]*matrix[2][0]*matrix[3][1]
               - matrix[1][0]*matrix[2][3]*matrix[3][1] - matrix[1][1]*matrix[2][0]*matrix[3][3] - matrix[1][3]*matrix[2][1]*matrix[3][0]) / det;
    inv[2][1] = (-matrix[0][0]*matrix[2][1]*matrix[3][3] - matrix[0][1]*matrix[2][3]*matrix[3][0] - matrix[0][3]*matrix[2][0]*matrix[3][1]
               + matrix[0][0]*matrix[2][3]*matrix[3][1] + matrix[0][1]*matrix[2][0]*matrix[3][3] + matrix[0][3]*matrix[2][1]*matrix[3][0]) / det;
    inv[2][2] =  (matrix[0][0]*matrix[1][1]*matrix[3][3] + matrix[0][1]*matrix[1][3]*matrix[3][0] + matrix[0][3]*matrix[1][0]*matrix[3][1]
               - matrix[0][0]*matrix[1][3]*matrix[3][1] - matrix[0][1]*matrix[1][0]*matrix[3][3] - matrix[0][3]*matrix[1][1]*matrix[3][0]) / det;
    inv[2][3] = (-matrix[0][0]*matrix[1][1]*matrix[2][3] - matrix[0][1]*matrix[1][3]*matrix[2][0] - matrix[0][3]*matrix[1][0]*matrix[2][1]
               + matrix[0][0]*matrix[1][3]*matrix[2][1] + matrix[0][1]*matrix[1][0]*matrix[2][3] + matrix[0][3]*matrix[1][1]*matrix[2][0]) / det;

    inv[3][0] = (-matrix[1][0]*matrix[2][1]*matrix[3][2] - matrix[1][1]*matrix[2][2]*matrix[3][0] - matrix[1][2]*matrix[2][0]*matrix[3][1]
               + matrix[1][0]*matrix[2][2]*matrix[3][1] + matrix[1][1]*matrix[2][0]*matrix[3][2] + matrix[1][2]*matrix[2][1]*matrix[3][0]) / det;
    inv[3][1] =  (matrix[0][0]*matrix[2][1]*matrix[3][2] + matrix[0][1]*matrix[2][2]*matrix[3][0] + matrix[0][2]*matrix[2][0]*matrix[3][1]
               - matrix[0][0]*matrix[2][2]*matrix[3][1] - matrix[0][1]*matrix[2][0]*matrix[3][2] - matrix[0][2]*matrix[2][1]*matrix[3][0]) / det;
    inv[3][2] = (-matrix[0][0]*matrix[1][1]*matrix[3][2] - matrix[0][1]*matrix[1][2]*matrix[3][0] - matrix[0][2]*matrix[1][0]*matrix[3][1]
               + matrix[0][0]*matrix[1][2]*matrix[3][1] + matrix[0][1]*matrix[1][0]*matrix[3][2] + matrix[0][2]*matrix[1][1]*matrix[3][0]) / det;
    inv[3][3] =  (matrix[0][0]*matrix[1][1]*matrix[2][2] + matrix[0][1]*matrix[1][2]*matrix[2][0] + matrix[0][2]*matrix[1][0]*matrix[2][1]
               - matrix[0][0]*matrix[1][2]*matrix[2][1] - matrix[0][1]*matrix[1][0]*matrix[2][2] - matrix[0][2]*matrix[1][1]*matrix[2][0]) / det;

    return inv;
        
}

void main() {

    mat4 ProjMatrix = projection(ufov, uaspect, unear, ufar); // Calcul de la matrice de projection
    mat4 MVMatrix = mat4(1.0); // Initialisation de la matrice de vue
    MVMatrix = scale(MVMatrix, uScale); // Application de l'échelle
    MVMatrix = translate(MVMatrix, uPosition); // Application de la translation

    mat4 NormalMatrix = transpose(myInverse(MVMatrix)); // Calcul de la matrice normale
    mat4 MVPMatrix = ProjMatrix * uCameraMatrix * MVMatrix; // Calcul de la matrice MVP

    // Passage en coordonnées homogènes
    vec4 vertexPosition = vec4(aVertexPosition, 1);
    vec4 vertexNormal = vec4(aVertexNormal, 0);

    // Calcul des valeurs de sortie
    vPosition_vs = vec3(MVMatrix * vertexPosition);
    vNormal_vs = vec3(NormalMatrix * vertexNormal);
    vTexCoords = aVertexTexCoords;

    // Calcul de la position projetée
    gl_Position = MVPMatrix * vertexPosition;
}