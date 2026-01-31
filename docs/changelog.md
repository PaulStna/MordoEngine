# Changelog

## 2026-01-24
- Initial project commit
- Initial project structure setup 
- Added basic OpenGL window and context creation using GLFW

## 2026-01-25
- Implemented input system handling for keyboard and mouse
- Added a basic Shader class for loading and compiling GLSL shaders into a shader program
- Added GLM for mathematical operations
- Integrated TriangleRenderer into the engine loop for testing the input system and shader class
- Added dynamic triangle creation using the space key in TriangleRenderer

## 2026-01-27
- Added a filesystem utility for loading shader source code from files
- Added stb_image for texture loading
- Implemented a Texture class for loading and binding textures using stb_image
- Added a texture manager for efficient texture loading and caching
- Updated TriangleRenderer to apply textures to triangles
- Updated triangle render implementation
- Updated shader class to set uniform variables  

## 2026-01-28
- Added Square class for rendering textured squares
- Implemented SquareRenderer to dynamically generate and render textured squares using indexed buffers
- Replaced TriangleRenderer and Triangle with SquareRenderer
- Refactored resource managers to use a generic template-based Manager<T> (e.g. textures, shaders)

## 2026-01-29
- Going 3D!
- Removed Square and SquareRenderer
- Added Cube and CubeRenderer for rendering textured 3D cubes with a coordinate system
- Random cube generation on space key press in CubeRenderer with random positions, rotations, scales

## 2026-01-30
- Implemented a Camera class for 3D navigation using quaternions
- Updated CubeRenderer to utilize the Camera for rendering cubes in 3D space
- Implemented WASD movement and mouse functionality in the cube renderer for camera control

## 2026-01-31
- Replace bricks texture with grass texture atlas
- Add color map for grass tinting
- Remove Cube class and dynamic cube generation
- Render NxN cube grids via instanced rendering with random heights
- Improve Texture class to auto-detect channel count
- Update fragment shader to tint top grass using color map
