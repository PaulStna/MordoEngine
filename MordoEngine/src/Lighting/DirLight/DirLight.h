#pragma once
#include "DirLightData.h"

/// The sun: one directional light whose angle follows a running time of day.
///
/// It sweeps a circle in the YZ plane rather than modelling a real solar path,
/// which is enough to make the scene's lighting change over time without
/// anything having to know what time it is.
class DirLight
{
private:
	DirLightData m_Data;

	// Radians around the day circle, wrapped at 2*pi so it never grows large
	// enough to lose precision.
	float m_TimeOfDay = 0.0f;

	// Radians per second. This is what a day is worth.
	float m_DaySpeed;

public:
	/// @param daySpeed Radians per second the sun advances. Small values give a
	///                 slow day; 2*pi divided by this is its length in seconds.
	DirLight(float daySpeed);

	/// Advances the time of day and recomputes the light direction.
	void Update(float deltaTime);

	const DirLightData& GetData() const { return m_Data; }
};
