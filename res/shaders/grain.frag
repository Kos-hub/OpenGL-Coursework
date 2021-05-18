#version 430 core

// Captured render
uniform sampler2D tex;
// Time 
uniform float time;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
    // Calculate noise and sample texture
    float mdf = 0.1; // increase for noise amount 
    float noise = (fract(sin(dot(tex_coord, vec2(12.9898,78.233)*2.0)) * 43758.5453));
    vec4 finalTex = texture(tex, tex_coord);
    
    mdf *= sin(time) + 1.0; // animate the effect's strength
    
    vec4 col = finalTex - noise * mdf;

    // Output to screen
    colour = col;
}