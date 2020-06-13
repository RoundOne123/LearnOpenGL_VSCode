#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // 原始数据是定义的在模型空间的法线

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));    // 计算世界空间顶点坐标
    //Normal = aNormal;   // 我们把光照计算放在世界空间 + Cube的在世界空间的位置是（0，0，0）（和模型空间位置重合，我们设置的） 所以 aNormal可以直接传递
    // 如果cube的位置 不再是（0， 0， 0），则必须使用法线矩阵将法线从模型空间转换到世界空间
    // 法线 不能直接使用 模型矩阵进行变换，因为不等比例的缩放，会改变法线方向
    // 法线矩阵 = 模型矩阵左上角的逆矩阵的转置矩阵
    Normal = mat3(transpose(inverse(model))) * aNormal;
}   