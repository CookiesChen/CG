#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D shadowMap;

uniform float ambientStrength;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform int Shininess;
uniform float specularStrength;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 归一化
    projCoords = projCoords * 0.5 + 0.5;
    // 获取最近点深度
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 当前片元深度
    float currentDepth = projCoords.z;
    // 计算阴影
	vec3 lightDir = normalize(lightPos - FragPos);
	float bias = max(0.05 * (1.0 - dot(normalize(Normal), lightDir)), 0.02);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
	// ambient
	vec3 ambient = ambientStrength * lightColor;
    // diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = specularStrength * spec * lightColor;  
	// shadow
	float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor; 

	FragColor = vec4(result, 1.0);
}