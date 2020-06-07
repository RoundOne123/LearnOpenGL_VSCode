#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// 传递纹理
uniform sampler2D ourTexture;

void main()
{
    // 这种vec4 * vec4 是怎么相乘的？
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}