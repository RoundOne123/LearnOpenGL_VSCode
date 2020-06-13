#version 330 core
out vec4 FragColor;

// 为冯氏光照模型的分量都定义一个颜色向量
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;    // 镜面高光的散射半径
};

uniform Material material;

// 为光源对冯氏光照模型的各分量的强度（甚至是颜色值？）也是不一样的
// 这里同样创建一个结构体来表示
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;


in vec3 Normal;
in vec3 FragPos;            // 世界空间的顶点位置

//uniform vec3 lightColor;    // 光照颜色
//uniform vec3 objectColor;   // 物体颜色 -> 物体从一个颜色反射各个分量的大小

//uniform vec3 lightPos;      // 世界空间下的灯光位置
uniform vec3 viewPos;     // 世界空间下的摄像机坐标

void main()
{
// 所有计算都是在世界空间下计算的

    // 环境光照
    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor;
    //vec3 ambient = material.ambient * lightColor;
    vec3 ambient = material.ambient * light.ambient;

    // 漫反射光照
    vec3 norm = normalize(Normal);   // !!! 要归一化
    //vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    //vec3 diffuse = diff * material.diffuse * lightColor;
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    // 镜面光照
    //float specularStrength = 0.5;       // 镜面强度
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    //vec3 specular = specularStrength * spec * lightColor;
    //vec3 specular = spec * material.specular * lightColor;
    vec3 specular = spec * material.specular * light.specular;

    // 将三个分量相加
    //vec3 result = (ambient + diffuse + specular) * objectColor;
    // 这里不在给冯氏光照模型的各分量使用统一的objectColor了，
    // 而是再Material中 分别提供 所以这里就不用再乘 objectColor
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}