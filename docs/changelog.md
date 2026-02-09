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
- Replaced bricks texture with grass texture atlas
- Added color map for grass tinting
- Removed Cube class and dynamic cube generation
- Rendered NxN cube grids via instanced rendering with Perlin noise
- Improved Texture class to auto-detect nrChannels count
- Updated fragment shader to tint top grass using color map

## 2026-02-02
- Removed CubeRenderer and Cube
- Removed atlas and color map textures
- Added GameScene class to manage terrain rendering and camera input controls
- Added uint16_t heightmap loading from RAW files
- Implemented heightmap-based terrain renderer (basic terrain rendering)

## 2026-02-03
- Implemented basic height-based texture splatting
- Added 3 terrain textures: grass, rock, dirt 
- Updated TriangleRenderer to manage terrain textures
- Updated fragment shader to blend terrain textures based on height

## 2026-02-04
- Implemented Fault Formation terrain generation and rendering
- Refactored GameScene to use the FaultFormation class for terrain generation and rendering
- Refactored Terrain to support multiple terrain generation techniques

## 2026-02-05
- Implemented midpoint displacement terrain generation and rendering

## 2026-02-07
- Implemented terrain normal calculation for lighting
- Updated shaders to apply basic directional lighting using computed normals
- Added a basic sun light that moves across the sky based on time of day, affecting terrain lighting

## 2026-02-08
- Implemented geomipmapping for terrain level-of-detail (LOD) rendering 
