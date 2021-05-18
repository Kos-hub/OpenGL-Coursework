#version 410

// Model view projection matrix
uniform mat4 MVP;

layout (location = 0) in vec3 position;
layout (location = 10) in vec2 tex_coord_in;

layout (location = 0) out vec2 tex_coord;

void main()
{
	// Calculate screen position of vertex
	gl_Position = MVP * vec4(position, 1.0);
	// Output texture coordinate to fragement shader
	tex_coord = tex_coord_in;
}