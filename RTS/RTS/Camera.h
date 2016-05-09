#ifndef __CAMERA_H__
#define __CAMERA_H__

#include"Constants.h"
#include"Vector2f.h"

typedef struct {
	Vector2fPtr position; 
	float width, height;
} Camera;
typedef shared_ptr<Camera> CameraPtr;
typedef weak_ptr<Camera> WeakCameraPtr;

void translateToCamera(Vector2fPtr vector, CameraPtr camera);

#endif // !__CAMERA_H__
