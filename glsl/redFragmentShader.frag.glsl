#version 330 core

in vec2 vTexCoord; // Interpolated vertex coordinates
out vec4 FragColor;

void main() {
    vec3 color1 = vec3(1.0, 0.0, 0.0); // Red
    vec3 color2 = vec3(0.0, 0.0, 1.0); // Blue
    vec4 targetRedCoord = vec4()
    vec3 gradient = mix(color1, color2, vTexCoord.x*0.5 + vTexCoord.y*0.5);

    FragColor = vec4(gradient, 1.0);
}
