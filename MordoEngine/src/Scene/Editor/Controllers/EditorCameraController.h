#pragma once
#include "../../../Camera/Camera.h"

class EditorCameraController
{
private:
    Camera& m_Camera;

public:
    EditorCameraController(Camera& camera);
    void Update(float deltaTime, float velocity);
};