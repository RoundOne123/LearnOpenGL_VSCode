#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));    // 计算世界空间顶点坐标
    // 法线 不能直接使用 模型矩阵进行变换，因为不等比例的缩放，会改变法线方向
    // 法线矩阵 = 模型矩阵左上角的逆矩阵的转置矩阵
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}