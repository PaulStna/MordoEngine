#pragma once

// One indexed draw: a slice of an index buffer plus the vertex it is relative to. 
// The shape comes from glDrawElementsBaseVertex, not from any particular technique
// A technique only decides which slices to emit, so swapping
// geomipmapping for CDLOD or clipmaps reuses the same renderer.
struct MeshDrawCall
{
	int indexOffset = 0;  
	int indexCount = 0;
	int baseVertex = 0;
};
