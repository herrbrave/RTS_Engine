#include "Camera.h"

void translateToCamera(vector2f* vector, Camera* camera){
	*vector -= *camera->position.get();
}