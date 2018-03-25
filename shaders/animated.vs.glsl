#version 330 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frame;
uniform int animation_dir;
uniform float animation_frame_w;
uniform float animation_frame_h;

void main()
{
	texcoord = in_texcoord;
	texcoord.x += animation_frame_w * frame;
	texcoord.y += animation_frame_h * animation_dir;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}