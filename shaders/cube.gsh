#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform mat4 model, view, proj;
uniform float t;
uniform float floatiness;

in VertexAttrib
{
  vec2 texcoord;
  vec4 color;
  float fall_time;
} vin[];

out VertexAttrib
{
  vec2 texcoord;
  vec3 normal;
  vec3 position;
  vec4 color;
} vout;

#define outputVertex(v) tmp = v; \
	gl_Position = proj * vec4(tmp, 1.0); \
	vout.position = tmp; \
	EmitVertex();

void main() {
	mat4 viewmodel = view * model;
	
	vec3 x = 0.5 * viewmodel[0].xyz;
	vec3 y = 0.5 * viewmodel[1].xyz;
	vec3 z = 0.5 * viewmodel[2].xyz;

	vec3 pos = vec3(gl_in[0].gl_Position);
	pos += floatiness * sin(t + pos.z) * vec3(0.0, 0.1, 0.0);
	if(vin[0].fall_time > 0.0001)
	{
		float f = (t - vin[0].fall_time);
		vec3 fall_component = vec3(0.0, -1.0, 0.0) * f * f * 4.0;
		if(fall_component.y < -30.0f) return;
		pos += fall_component;
	}else if(vin[0].fall_time < -0.0001)
	{
		// intro
		float f = max(1.8 - (t - (-vin[0].fall_time - (pos.z + 10.0) * 0.15)), 0.0);
		vec3 intro_component = vec3(0.0, -1.0, 0.0) * f * f * 4.0;
		pos += intro_component;
	}
	vec3 tmp;

    vout.color = vin[0].color;

    {
    	vout.normal = vec3(0.0, 0.0, -1.0);
        outputVertex(pos - x - y - z);
        outputVertex(pos + x - y - z);
        outputVertex(pos - x + y - z);
        outputVertex(pos + x + y - z);
        EndPrimitive();
    
        vout.normal = vec3(0.0, 1.0, 0.0);
        outputVertex(pos - x + y - z);
        outputVertex(pos + x + y - z);
        outputVertex(pos - x + y + z);
        outputVertex(pos + x + y + z);
        EndPrimitive();
        
        vout.normal = vec3(0.0, 0.0, 1.0);
        outputVertex(pos - x + y + z);
        outputVertex(pos + x + y + z);
        outputVertex(pos - x - y + z);
        outputVertex(pos + x - y + z);
        EndPrimitive();
    }

    {
    	vout.normal = vec3(1.0, 0.0, 0.0);
        outputVertex(pos + x + y + z);
        outputVertex(pos + x + y - z);
        outputVertex(pos + x - y + z);
        outputVertex(pos + x - y - z);
        EndPrimitive();
    
        vout.normal = vec3(0.0, -1.0, 0.0);
        outputVertex(pos + x - y + z);
        outputVertex(pos + x - y - z);
        outputVertex(pos - x - y + z);
        outputVertex(pos - x - y - z);
        EndPrimitive();
    
        vout.normal = vec3(-1.0, 0.0, 0.0);
        outputVertex(pos - x - y + z);
        outputVertex(pos - x - y - z);
        outputVertex(pos - x + y + z);
        outputVertex(pos - x + y - z);
        EndPrimitive();
    }
}
