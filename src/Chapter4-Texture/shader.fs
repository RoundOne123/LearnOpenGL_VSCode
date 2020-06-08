#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// 传递纹理
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // 这种vec4 * vec4 是怎么相乘的？
    //FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2); // 0.2 代表 参数二的纹理占比 0.2
}