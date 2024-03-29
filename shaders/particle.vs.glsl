#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec3 translation; 
layout(location = 2) in vec4 color;

// Output data ; will be interpolated for each fragment.
out vec4 particlecolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	particlecolor = color;
	vec3 new_pos = vec3(vertex_pos.xy + translation.xy, 1.0);
	gl_Position = vec4(projection * transform * new_pos, 1.0);
}