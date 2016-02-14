#ifndef __CAMERA_H__
#define __CAMERA_H__

#include"vector2f.h"

struct Camera {
	p_vector2f position; 
	float width, height;
};

void translateToCamera(vector2f* vector, Camera* camera);

#endif // !__CAMERA_H__
