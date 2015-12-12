#version 330

#define NUM_LIGHTS 3

in VertexAttrib
{
  vec2 texcoord;
  vec3 normal;
  vec3 position;
  vec4 color;
} vin;

uniform mat4 model, view, proj;
uniform bool bTexture;
uniform sampler2D texture;

uniform vec3 lights_position[NUM_LIGHTS];
uniform vec3 lights_color[NUM_LIGHTS];
uniform float lights_ambient[NUM_LIGHTS], lights_diffuse[NUM_LIGHTS], lights_specular[NUM_LIGHTS], lights_fresnel[NUM_LIGHTS], lights_shininess[NUM_LIGHTS];
uniform float lights_fresnelBias[NUM_LIGHTS], lights_fresnelScale[NUM_LIGHTS], lights_fresnelPower[NUM_LIGHTS];
uniform bool lights_enabled[NUM_LIGHTS];
uniform bool lights_directional[NUM_LIGHTS];

uniform bool light_objectcolor;

out vec4 out_color;

void main()
{
	vec3 Ilight = vec3(0.0);

	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		if(!lights_enabled[i]) continue;

		vec3 light_pos = lights_position[i];
		if(!lights_directional[i]) light_pos = vec3(view * vec4(light_pos, 1.0));

		vec3 N = mat3(model) * vin.normal;
		vec3 VN = mat3(model) * vin.normal;

		vec3 L;
		if(!lights_directional[i]) L = normalize(light_pos - vin.position);
		else L = normalize(light_pos);
		vec3 E = normalize(-vin.position);
		vec3 R = normalize(-reflect(L, VN));

		float fresnelBias = lights_fresnelBias[i];
		float fresnelScale = lights_fresnelScale[i];
		float fresnelPower = lights_fresnelPower[i];
		vec3 ambient = vec3(lights_ambient[i]);
		vec3 diffuse = vec3(lights_diffuse[i]);
		vec3 specular = vec3(lights_specular[i]);
		vec3 fresnel = vec3(lights_fresnel[i]);
		float shininess = lights_shininess[i];

		//calculate Ambient Term:  
		vec3 Iamb = ambient;

		//calculate Diffuse Term:  
		vec3 Idiff = diffuse * max(dot(VN, L), 0.0);
		Idiff = clamp(Idiff, 0.0, 1.0);

		// calculate Specular Term:
		vec3 Ispec = specular * pow(max(dot(R, E), 0.0), 0.3 * shininess);
		Ispec = clamp(Ispec, 0.0, 1.0);

		// fresnel
		vec3 Ifresnel = fresnel * (fresnelBias + fresnelScale * pow(1.0 + dot(vin.position, N), fresnelPower));
		Ifresnel = clamp(Ifresnel, 0.0, 1.0);

		Ilight += Iamb + Idiff + Ispec + Ifresnel;
		// Ilight += Ifresnel;
	}

	if(bTexture) out_color = texture2D( texture, vec2(vin.texcoord.x, 1.0 - vin.texcoord.y) ).zyxw * vec4(Ilight, 1.0);
	else if(light_objectcolor) out_color = clamp(vin.color * vec4(Ilight, 1.0), 0.0, 1.0);
	else out_color = clamp(vec4(Ilight, vin.color.w), 0.0, 1.0);
}
