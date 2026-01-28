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
