#pragma once

class Scene
{
protected:
	Scene() = default;

public:
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void OnEntry() = 0;
	virtual ~Scene() = default;
};