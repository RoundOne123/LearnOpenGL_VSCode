#version 330 core
out vec4 FragColor;

// 为冯氏光照模型的分量都定义一个颜色向量
struct Material {

    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
    // 使用纹理代替 固定的颜色值 来表示 各分量
    // 环境光颜色 和 漫反射颜色 一般都是相同的
    sampler2D diffuse;
    sampler2D specular;

    sampler2D highLightTex; // 练习中增加类似自发光的纹理

    float shininess;    // 镜面高光的散射半径
};

// 为光源对冯氏光照模型的各分量的强度（甚至是颜色值？）也是不一样的
// 这里同样创建一个结构体来表示
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;  // 包含物体颜色
uniform Light light;        // 包含光源（强度）颜色
uniform vec3 viewPos;       // 世界空间下的摄像机坐标

in vec3 FragPos;            // 世界空间的顶点位置
in vec3 Normal;             // 世界空间下的法向量
in vec2 TexCoord;           // 顶点的uv坐标

void main()
{
// 所有计算都是在世界空间下计算的

    // 环境光照
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // 漫反射光照
    vec3 norm = normalize(Normal);   // !!! 要归一化
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoord).rgb;

    // 镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * texture(material.specular, TexCoord).rgb;

    // 将三个分量相加
    vec3 result = ambient + diffuse + specular + texture(material.highLightTex, TexCoord).rgb;

    FragColor = vec4(result, 1.0);
}