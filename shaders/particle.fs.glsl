#version 330 core

// Interpolated values from the vertex shaders
in vec4 particlecolor;

// Ouput data
out vec4 color;

void main(){
	color = vec4(1.0, 0.0, 0.0, 1.0);
}