#ifndef CAMERA_H
#define CAMERA_H

#include "shader.h"
#include "glm.h"

class Camera
{
	public:
		Camera(float znear, float zfar);

		void update();
		void updateCamera(ShaderProgram &prog); // the shader needs to have 2 uniforms: view and proj
		void updateFrustum(void);
		void updateView(void);
		
		void setPosition(glm::dvec3 p);
		void movePositionDouble(glm::dvec3 v);
		
		glm::vec3 getPosition(glm::vec3 ref);
		glm::dvec3 getPositionDouble(glm::dvec3 ref);
		glm::vec3 getReference(void);
		glm::dvec3 getReferenceDouble(void);

		bool isPointInFrustum(glm::vec3 p);
		bool isBoxInFrustum(glm::vec3 o, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
		bool isBoxInFrustum(glm::vec3 p[], int n, glm::mat4 model);
		
		void moveReference(glm::dvec3 r);

		glm::mat4 view, proj, final;
		glm::mat3 view3;

	private:
		// view should be computed from view3 and pos
		// never modify view directly
		glm::dvec3 pos, refPos;
		glm::vec4 frustumPlane[6]; 
		float znear, zfar;
};

#endif
