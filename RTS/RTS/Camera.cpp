#include "Camera.h"

void translateToCamera(Vector2fPtr vector, CameraPtr camera) {
	*vector -= *camera->position;
}