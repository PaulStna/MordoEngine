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

## 2026-02-10
- Implemented GeoMipMapping with dynamic camera distance-based LOD selection
- Added world size parameter to control terrain scale and LOD levels

## 2026-02-12
- Added a terrain editor scene to handle terrain editing functionality
- Implemented a basic terrain picking system using ray casting to select terrain points with the mouse
- Added visual feedback for selected terrain area points (yellow circle)
- Added terrain selector fragment and vertex shaders for rendering the selection indicator on the terrain
- Added a SceneManager to manage switching between the main game scene and the terrain editor scene

## 2026-02-13
- Refactored SceneManager to handle the shared terrain data between the GameScene and TerrainEditorScene
- Refactored GameScene and EditorScene to use the shared terrain data from the SceneManager
- Added editor camera controller to manage input in terrain editor scene
- Refactored scene update methods to delegate input and camera updates to editor camera controller class

## 2026-02-14
- Added EditorSystem class to manage editor-specific functionality and tools
- Refactored AreaSelectorRenderer to use EditorSystem for area selection state management
- Refactored AreaSelectorRenderer to inherit from the base Renderer class
- Refactored Render methods in Redenderer class to be more flexible and reusable across different renderers
- Refactored Render methods in all Renderer subclasses

## 2026-02-15
- Implemented terrain editing in TerrainEditorScene using EditorSystem
- Added terrain height increment on left mouse click (unoptimized)
- Refactored terrain vertex and fragment shaders 

## 2026-02-16
- Added terrain height decrement on right mouse click (still unoptimized)
- Added EditorInputHandlerController for selector size and brush strength input handling

## 2026-02-17
- Added GameCameraController to manage camera input in the main game scene
- Added an OnEntry method for Scenes to handle scene initialization logic or switching scenes
- GameCameraController handles WASD movement and mouse look with terrain collision detection

## 2026-02-18
- Added a cubemap-loading constructor to the Texture class
- Added skyBox vertex and fragment shaders for skybox rendering
- Added cubemap textures for the skybox
- Added SkyboxRenderer to render the skybox using the cubemap textures
- Updated GameScene to include the SkyboxRenderer for rendering the skybox in the background

## 2026-02-22
- Replaced test Sun class light with a DirLight class that calculates sun direction based on time of day
- Added a CubeRenderer class for rendering point light sources as small cubes in the scene.
- Added vertex and fragment shaders for rendering white point light cubes (used by CubeRenderer).
- Added a PointLight class that handles point light data and properties.
- Implemented light system to handle multiple light types (directional, point) and pass light data to shaders

## 2026-02-28
- Implemented a TerrainSystem to manage terrain rendering and related functionality.
- Refactored LightSystem to handle multiple point lights
- Refactored SkyBoxRenderer  
- Added a SkySystem to manage skybox rendering and related functionality