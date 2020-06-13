#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;    // 灯光颜色
uniform vec3 objectColor;     // 物体颜色

void main()
{
    // 物体颜色 * 光源颜色 = 反射的颜色 （感知的颜色）
    // 物体颜色 => 物体从一个光源反射各个颜色分量的大小
    FragColor = vec4(lightColor * objectColor, 1.0f);
}