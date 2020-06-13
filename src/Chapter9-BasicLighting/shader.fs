#version 330 core
out vec4 FragColor;

in vec3 Normal;

in vec3 FragPos;            // 世界空间的顶点位置

uniform vec3 lightColor;    // 光照颜色
uniform vec3 objectColor;   // 物体颜色 -> 物体从一个颜色反射各个分量的大小

uniform vec3 lightPos;      // 世界空间下的灯光位置？？ 怎么赋值的？ 在CPU计算好赋值过来的？
uniform vec3 viewPos;     // 世界空间下的摄像机坐标

void main()
{
// 所有计算都是在世界空间下计算的

    // 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射光照
    vec3 norm = normalize(Normal);   // !!! 要归一化
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面光照
    float specularStrength = 0.5;       // 镜面强度
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}