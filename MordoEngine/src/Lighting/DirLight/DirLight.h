#pragma once
#include "DirLightData.h"

class DirLight
{
private:
	DirLightData m_Data;
	float m_TimeOfDay = 0.0f;
	float m_DaySpeed;

public:
	DirLight(float daySpeed);
	void Update(float deltaTime);
	const DirLightData& GetData() const { return m_Data; }
};
