#version 330 core

in vec2 vTexCoord; // Interpolated vertex coordinates
out vec4 FragColor;

void main() {
    vec3 color1 = vec3(1.0, 0.0, 0.0); // Red
    vec3 color2 = vec3(1.0, 1.0 , 1.0); // Black
    vec3 blue = vec3(0.0, 0.0 , 1.0); // Blue
    vec3 color3 = vec3(0.0, 1.0, 0.0);
    vec3 gradientTmp2 = mix(blue, color3, vTexCoord.x );
    vec3 gradientTmp1 = mix(gradientTmp2, color2, vTexCoord.y);
    vec3 finalGradient = mix(gradientTmp1, color1, vTexCoord.y);

    FragColor = vec4(finalGradient, 1.0);
}
