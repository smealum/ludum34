#ifndef LIGHTING_H
#define LIGHTING_H

#include "gl.h"
#include "glm.h"
#include "shader.h"

#define NUM_LIGHTS (3)

class Lighting
{
	public:
		Lighting();
		
		glm::vec3 getLightPosition(int id);

		void setLightEnabled(int id, bool enabled);
		void setLightDirectional(int id, bool directional);
		void setLightPosition(int id, glm::vec3 position);
		void setLightADSS(int id, float ambient, float diffuse, float specular, float shininess);
		void setLightFresnel(int id, float fresnel, float fresnelBias, float fresnelScale, float fresnelPower);
		void setLightAmbient(int id, float v);
		void setLightDiffuse(int id, float v);
		void setLightSpecular(int id, float v);
		void setLightShininess(int id, float v);
		void setLightFresnel(int id, float v);
		void setLightFresnelBias(int id, float v);
		void setLightFresnelScale(int id, float v);
		void setLightFresnelPower(int id, float v);

		void update(ShaderProgram &prog);

	private:
		// LIGHTS (stored this way so they can be transfered to gpu quickly) (we could do better but this is good enough)
		glm::vec3 lights_position[NUM_LIGHTS];
		glm::vec3 lights_color[NUM_LIGHTS];
		float lights_ambient[NUM_LIGHTS], lights_diffuse[NUM_LIGHTS], lights_specular[NUM_LIGHTS], lights_fresnel[NUM_LIGHTS], lights_shininess[NUM_LIGHTS];
		float lights_fresnelBias[NUM_LIGHTS], lights_fresnelScale[NUM_LIGHTS], lights_fresnelPower[NUM_LIGHTS];
		int lights_enabled[NUM_LIGHTS];
		int lights_directional[NUM_LIGHTS];

};

#endif
