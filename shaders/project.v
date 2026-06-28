#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat3 rotation;
uniform vec3 camPos;

void main() {
    vec3 newPos = aPos - camPos;
    
    vec3 rotatedPos = rotation * newPos;
    
    vec4(rotatedPos.xyz, -rotatedPos.z);
}