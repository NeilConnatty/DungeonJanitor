#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec2 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec4 particlecolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	particlecolor = vec4(0.0, 0.0, 0.0, 1.0);
	//vec3 pos = projection * transform * vec3(squareVertices.xy, 1.0);
	gl_Position = vec4(squareVertices.xy, 0.5, 1.0);
}