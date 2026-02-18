#pragma once
#include <string>

class Scene
{
protected:
	std::string p_ShaderID;
	Scene(const std::string& shaderID)
	{
		p_ShaderID = shaderID;
	}
public:
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void OnEntry() = 0;
	virtual ~Scene() = default;
};