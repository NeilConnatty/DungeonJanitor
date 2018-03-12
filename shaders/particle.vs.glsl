#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec4 particlecolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	particlecolor = color;
	vec3 pos = projection * transform * vec3(xyzs.xy * xyzs.w, 1.0);
	gl_Position = vec4(pos.xy, xyzs.z, 1.0);
}