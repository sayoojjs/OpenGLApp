#version 330


in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;

uniform sampler2D roughnessMap;
uniform bool useRoughnessMap;
uniform float roughnessIntensity;

uniform sampler2D normalMap;    
uniform bool useNormalMap;
uniform float normalStrength; 


uniform vec3 eyePosition;

vec3 GetNormal()
{
     if (useNormalMap)
    {
        vec3 n = texture(normalMap, TexCoord).rgb;
        n = normalize(n * 2.0 - 1.0);
		n.y = -n.y;
        n = normalize(mix(vec3(0.0, 0.0, 1.0), n, normalStrength));
        return normalize(TBN * n);
    }
    return normalize(TBN[2]);
}

vec4 CalcLightByDirection(Light light, vec3 direction)
{
    vec3 normal = GetNormal();

    vec4 ambientColour = vec4(light.colour, 1.0) * light.ambientIntensity;

    float diffuseFactor = max(dot(normal, normalize(direction)), 0.0);
    vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0);

    vec4 specularColour = vec4(0.0);
    if (diffuseFactor > 0.0)
    {
	
    float effectiveShininess = material.shininess;
    if (useRoughnessMap)
    {

        float roughness = texture(roughnessMap, TexCoord).r;
		roughness = clamp(roughness * roughnessIntensity, 0.0, 1.0);
        float glossiness = 1.0 - roughness;
        effectiveShininess = max(material.shininess * glossiness * glossiness, 1.0);
    }

		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(-direction, normal));
		float specularFactor = max(dot(fragToEye, reflectedVertex), 0.0);
		if (specularFactor > 0.0)
		    {
				specularFactor = pow(specularFactor, effectiveShininess);
				specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0);
			}
    }

    return ambientColour + diffuseColour + specularColour;
}

vec4 CalcDirectionalLight()
{
    return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLights()
{
    vec4 totalColour = vec4(0.0);
    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 direction = pointLights[i].position - FragPos;
        float distance = length(direction);
        direction = normalize(direction);

        vec4 c = CalcLightByDirection(pointLights[i].base, direction);
        float attenuation = pointLights[i].exponent * distance * distance
                          + pointLights[i].linear * distance
                          + pointLights[i].constant;

        totalColour += c / attenuation;
    }
    return totalColour;
}

void main()
{
    vec4 finalColour = CalcDirectionalLight();
    finalColour += CalcPointLights();
    colour = texture(theTexture, TexCoord) * finalColour;
}