#version 430 core

// Incoming frame data
uniform sampler2D tex;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  float contrast = 2.0; // Set the contrast intensity
	vec4 originalColor = texture(tex, tex_coord); //Sample texture 
  //Contrast calculation
	vec4 finalColor = vec4(((originalColor.rgb-vec3(0.5))*contrast)+vec3(0.5), 1.0);
	colour = clamp(finalColor, 0.0, 1.0); //Clamp calculated texture
}