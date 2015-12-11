#version 330

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

out vec4 out_color;

void main()
{
	vec3 light_pos = vec3(view * vec4(2.0, 2.0, 2.0, 1.0));

	vec3 N = mat3(model) * vin.normal;
	vec3 VN = mat3(model) * vin.normal;

	vec3 L = normalize(light_pos - vin.position);   
	vec3 E = normalize(-vin.position);
	vec3 R = normalize(-reflect(L, VN));

	float fresnelBias = 0.1;
	float fresnelScale = 0.7;
	float fresnelPower = 3.0;

	vec3 ambient = vec3(0.2);
	vec3 diffuse = vec3(0.5);
	vec3 specular = vec3(0.7);
	vec3 fresnel = vec3(1.0);
	float shininess = 0.5;

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

	vec3 Ilight = Iamb + Idiff + Ispec + Ifresnel;
	// vec3 Ilight = Ifresnel;

	if(bTexture) out_color = texture2D( texture, vec2(vin.texcoord.x, 1.0 - vin.texcoord.y) ).zyxw * vec4(Ilight, 1.0);
	else out_color = vin.color * vec4(Ilight, 1.0);
}
