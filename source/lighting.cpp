#include "lighting.h"
#include "shader.h"

Lighting::Lighting()
{
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		// default settings
		lights_ambient[i] = 0.2f;
		lights_diffuse[i] = 0.5f;
		lights_specular[i] = 0.7f;
		lights_fresnel[i] = 0.7f;
		lights_shininess[i] = 1.0f;

		lights_fresnelBias[i] = 0.1f;
		lights_fresnelScale[i] = 0.7f;
		lights_fresnelPower[i] = 2.0f;

		lights_position[i] = glm::vec3(0.0f);

		lights_enabled[i] = false;
		lights_directional[i] = false;
	}
}

void Lighting::update(ShaderProgram &prog)
{
	prog.setUniform("lights_ambient", lights_ambient, NUM_LIGHTS);
	prog.setUniform("lights_diffuse", lights_diffuse, NUM_LIGHTS);
	prog.setUniform("lights_specular", lights_specular, NUM_LIGHTS);
	prog.setUniform("lights_fresnel", lights_fresnel, NUM_LIGHTS);
	prog.setUniform("lights_shininess", lights_shininess, NUM_LIGHTS);

	prog.setUniform("lights_fresnelBias", lights_fresnelBias, NUM_LIGHTS);
	prog.setUniform("lights_fresnelScale", lights_fresnelScale, NUM_LIGHTS);
	prog.setUniform("lights_fresnelPower", lights_fresnelPower, NUM_LIGHTS);

	prog.setUniform("lights_position", lights_position, NUM_LIGHTS);

	prog.setUniform("lights_directional", lights_directional, NUM_LIGHTS);
	prog.setUniform("lights_enabled", lights_enabled, NUM_LIGHTS);
}

glm::vec3 Lighting::getLightPosition(int id)
{
	if(id < 0 || id >= NUM_LIGHTS) return glm::vec3(0.0f);

	return lights_position[id];
}

void Lighting::setLightEnabled(int id, bool enabled)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_enabled[id] = enabled;
}

void Lighting::setLightDirectional(int id, bool directional)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_directional[id] = directional;
}

void Lighting::setLightPosition(int id, glm::vec3 position)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_position[id] = position;
}

void Lighting::setLightADSS(int id, float ambient, float diffuse, float specular, float shininess)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_ambient[id] = ambient;
	lights_diffuse[id] = diffuse;
	lights_specular[id] = specular;
	lights_shininess[id] = shininess;
}

void Lighting::setLightFresnel(int id, float fresnel, float fresnelBias, float fresnelScale, float fresnelPower)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_fresnel[id] = fresnel;
	lights_fresnelBias[id] = fresnelBias;
	lights_fresnelScale[id] = fresnelScale;
	lights_fresnelPower[id] = fresnelPower;
}

void Lighting::setLightAmbient(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_ambient[id] = v;
}

void Lighting::setLightDiffuse(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_diffuse[id] = v;
}

void Lighting::setLightSpecular(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_specular[id] = v;
}

void Lighting::setLightShininess(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_shininess[id] = v;
}

void Lighting::setLightFresnel(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_fresnel[id] = v;
}

void Lighting::setLightFresnelBias(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_fresnelBias[id] = v;
}

void Lighting::setLightFresnelScale(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_fresnelScale[id] = v;
}

void Lighting::setLightFresnelPower(int id, float v)
{
	if(id < 0 || id >= NUM_LIGHTS) return;

	lights_fresnelPower[id] = v;
}
