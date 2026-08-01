#pragma once
#include <glm/glm.hpp>

/// Everything a renderer needs to draw one pass, carried as one value through
/// the whole render path.
///
/// This is why the water passes are cheap to express: a pass derives its own
/// context by copying this and overriding one field — the view for a mirrored
/// camera, the clip plane for the half being kept — instead of threading
/// nullable pointers and flags down through every renderer.
///
/// Built fresh each pass and never held onto. An object that owns its own
/// transform is free to ignore the model matrix and build one from its
/// transform instead.
struct RenderContext
{
	glm::mat4 projection{ 1.0f };
	glm::mat4 view{ 1.0f };

	// Per object, so this is the one field a caller normally overwrites between
	// draws. Identity means "already in world space".
	glm::mat4 model{ 1.0f };

	glm::vec3 cameraPos{ 0.0f };

	// xyz is the plane normal, w its distance from the origin. Geometry on the
	// negative side is clipped while GL_CLIP_DISTANCE0 is enabled, which is what
	// splits the world above and below the water for the reflection and
	// refraction passes. The default keeps everything.
	glm::vec4 clipPlane{ 0.0f, 1.0f, 0.0f, 0.0f };
};
