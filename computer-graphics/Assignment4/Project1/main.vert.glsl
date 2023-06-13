#version 330 core
// <vec2 position, vec2 texCoords>
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 model;
uniform mat4 projection;
uniform vec4 color;

void main(){
    float scale = 5.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * model * vec4((vertex.xy * scale), 0.0, 1.0);
}

