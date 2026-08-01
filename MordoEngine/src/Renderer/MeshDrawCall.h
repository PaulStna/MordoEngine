#pragma once

/// One indexed draw: a slice of an index buffer plus the vertex it is relative
/// to.
///
/// The shape comes from glDrawElementsBaseVertex, not from any particular
/// technique. A technique only decides which slices to emit, so swapping
/// geomipmapping for CDLOD or clipmaps reuses the same renderer.
struct MeshDrawCall
{
	int indexOffset = 0;   // first index of the slice, in elements
	int indexCount = 0;    // how many indices to draw
	int baseVertex = 0;    // added to every index before it is looked up
};
