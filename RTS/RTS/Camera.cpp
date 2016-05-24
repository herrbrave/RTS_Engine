#include "Camera.h"

void translateToCamera(Vector2f& vector, CameraPtr camera) {
	vector -= *camera->position;
}