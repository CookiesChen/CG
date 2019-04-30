#version 330 core
out vec4 FragColor;
in vec3 ourColor;

uniform vec3 objectColor;

void main()
{
    FragColor = vec4(ourColor * objectColor, 1.0);
}