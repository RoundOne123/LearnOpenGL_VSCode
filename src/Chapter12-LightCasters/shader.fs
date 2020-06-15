#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct Light
{
    vec3 position;      // 使用vec4 来表示 根据w分量 区分光源类型
    vec3 direction;     // 聚光方向
    float cutOff;       // 聚光的内切光角
    float outerCutOff;  // 聚光的外切光角

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 实现衰减 -> 公式的几项的系数
    float constant;
    float linear;
    float quadratic;
};


uniform Material material;
uniform Light light;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

void main()
{

    vec3 result = vec3(0.0);
    vec3 norm = normalize(Normal);

    // 确定光线方向
    vec3 lightDir = vec3(0.0);
    // if (light.direction.w == 0.0)
    // {
         //定义的方向是从光源发出的 但是计算时 需要用到指向光源的方向
         //lightDir = normalize(-light.direction.xyz);
    // }
    // else if (light.direction.w == 1.0)
    // {
         lightDir = normalize(light.position - FragPos);
    // }
    
    // 求衰减
    float lightDistance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear * lightDistance + 
                light.quadratic * (lightDistance * lightDistance));



    // 计算 聚光指向的方向（spotDir） 和 光线方向的夹角（lightDir）
    float theta = dot(lightDir, normalize(-light.direction));
    // 平滑/软化边缘 -> 外切角
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    //ambient *= attenuation;  // 消除周围环境的衰减，否则在远距离处，由于else分支中的环境术语，内部的灯光将比聚光灯的外部更暗

    // if(theta > light.cutOff)
    // {
        // 执行光照
        // 漫反射光
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoord).rgb;
        diffuse *= attenuation;
        diffuse *= intensity;
 
        // 镜面高光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        vec3 specular = spec * light.specular * texture(material.specular, TexCoord).rgb;
        specular *= attenuation;  
        specular *= intensity;

        result = ambient + diffuse + specular;
    // }else
    // {
    //     // 使用环境光
    //     result = ambient;
    // }

    FragColor = vec4(result, 1.0);
}
