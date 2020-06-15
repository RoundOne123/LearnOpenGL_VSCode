#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

// 平行光
struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight
{
    vec3 position;

    // 衰减公式的常数项
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 聚光
struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 预处理指令定义点光源的数量
#define NR_POINT_LIGHT 4

uniform Material material;

// 光源
// 平行光
uniform DirLight dirLight;
// 创建一个PointLight结构体数组
uniform PointLight pointLights[NR_POINT_LIGHT];
// 创建 聚光
uniform SpotLight spotLight;

uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;


// 定义 计算平行光的函数原型
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

// 定义 计算点光源的函数原型
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// 定义 计算聚光的函数原型
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 定向光照
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // 点光源
    for(int i = 0; i < NR_POINT_LIGHT; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // 聚光
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);
}

// 计算平行光的光照
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // 合并结果
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

    return ambient + diffuse + specular;
}

// 计算点光源的光照
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 计算衰减
    float lightDis = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDis + light.quadratic * (lightDis * lightDis));

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // 合并结果
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

// 计算聚光的光照
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 计算衰减
    float lightDis = length(light.position - fragPos);
    float attenuation = 1 / (light.constant + light.linear * lightDis + light.quadratic * (lightDis * lightDis));

    // 计算 切角
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // 漫反射
    float diff = max(dot(lightDir, normal), 0.0);

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

    // 合并结果
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb * intensity; // attenuation
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb * attenuation * intensity;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb * attenuation * intensity;

    return ambient + diffuse + specular;
}