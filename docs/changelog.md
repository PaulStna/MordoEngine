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

## 2026-03-01
- Added Framebuffer class to manage OpenGL framebuffer objects (FBOs) for off-screen rendering
- Added PlaneRenderer to render a fullscreen quad for post-processing effects

## 2026-03-07
- Implemented a WaterSystem to manage water rendering and related functionality 
- Implemented water reflection rendering using a framebuffer and clipping planes to render the scene from the water's perspective in WaterSystem
- Implemented water refraction rendering using a framebuffer and clipping planes to render the scene from below the water's perspective in WaterSystem
- Added water vertex and fragment shaders for plane water rendering 
- Implemented clipping planes in terrain shader for water reflection and refraction rendering

## 2026-03-08
- Implemented refraction and reflection texture blending in the water shader for realistic water surface rendering
- WaterSystem now handle the reflection and refraction water rendering 
- Added dudv map texture for animated water surface distortion in the water shader
- Updated water shader to use the dudv map for animated water surface and some distortions

## 2026-03-22
- Added fesnel reflectance calculation to the water shader for more realistic water reflections based on view angle

## 2026-07-13
(Long time, huh?)
- Encapsulated all matrix (mat4) and vector (vec3) data required for rendering into a single RenderContext structure, reducing the number of parameters passed to renderers.
- Created a World class to manage all data shared between scenes, including the RenderContext and engine systems such as terrain, lighting, sky, and water.
- Added an EngineContext class that owns the World and serves as the foundation for future global resources and input systems.
- Refactored GameCameraController and EditorCameraController to access the Camera through the RenderContext instead of holding weak_ptr references.
- Cleaned up GameScene and EditorScene to use the new World and EngineContext classes, eliminating the need for shared_ptr references to Terrain, LightSystem, SkySystem, WaterSystem, and other shared systems.
- Reorganized the Engine initialization and shutdown flow with explicit handling of the EngineContext, SceneManager, and the main loop, ensuring proper cleanup of scene resources (FBOs, VAOs, etc.) and preventing memory leaks.

## 2026-07-15
- Removed the old Manager classes and introduced a generic ResourceLibrary<T> template to handle resource management (textures, shaders, etc.) in a more reusable and extensible way.
- Updated EngineContext to own a ResourceLibrary instance for each resource type, centralizing resource storage and providing shared access across the engine.
- Refactored the engine's resource loading and access flow to fully adopt the new ResourceLibrary system, removing dependencies on the previous Manager-based architecture.

## 2026-07-20
- Implemented a new water rendering system that includes reflection and refraction with distortion, improving the visual quality of water surfaces in the engine.

## 2026-07-22
- Propagated RenderContext through every system and renderer, removing the nullable matrix pointers that were passed around as separate parameters.
- Reduced the Renderer base class to a single pure virtual Render(const RenderContext&), dropping the empty virtual overloads and the UpdateBuffers/SetHeights methods that most subclasses never implemented.
- Removed the duplicated Render implementation left in Geomipmapping and turned UpdateBuffers into a regular member function.
- Refactored LightSystem, PointLight, SkySystem, WaterSystem, EditorSystem and AreaSelectorRenderer to take the RenderContext instead of individual matrix, camera and clip plane parameters.
- Changed the WaterSystem capture callback to receive a RenderContext, so the system now derives the reflection and refraction pass contexts itself instead of leaving that logic to World.
- Made RenderReflection and RenderRefraction private, as they are internal steps of CaptureReflectionRefraction.
- Removed the Camera dependency from WaterSystem::RenderSurface, which now reads the view matrix and camera position from the RenderContext.
- Changed EditorSystem to hold an AreaSelectorRenderer directly instead of a base Renderer pointer, removing the static_cast needed to reach the derived interface.
- Restored terrain rendering in EditorScene and updated it to build the selector transform by copying the pass context and overriding its model matrix.

## 2026-07-23
- Split Geomipmapping into a meshing technique and a GL renderer, separating the level-of-detail calculations from the OpenGL calls that draw their result.
- Moved Geomipmapping and LodManager to Terrain/Mesh, where the technique now holds no GL state and issues no draw calls, so its index and LOD arithmetic can be exercised without a rendering context.
- Added TerrainMeshRenderer under Renderer/Terrain to own the terrain VAO, VBO and EBO, the vertex layout, and the full, partial and draw operations on those buffers.
- Extracted the terrain vertex out of Terrain.h into its own TerrainVertex header, and moved the InitVertex logic into Geomipmapping so the vertex no longer depends on the Terrain class.
- Added a technique-neutral MeshDrawCall struct in Renderer, describing a single indexed draw, so replacing geomipmapping with another technique reuses the same renderer untouched.
- Reworked TerrainSystem to drive the terrain data, the meshing technique and the mesh renderer together, and to own the terrain material uniforms.
- Preserved the incremental edit path: the technique recomputes only the vertices affected by a terrain modification and returns their indices, and the renderer re-uploads just those.
- Removed six dead members from Geomipmapping (the three texture id strings, the texture scale and the two height thresholds) along with their setters, as the values actually in use always lived in TerrainSystem.
- Removed the shader library parameter from the TerrainSystem constructor, since neither the technique nor the mesh renderer owns a shader anymore.
- Cleaned up AreaSelectorRenderer, which was including Terrain and Camera headers it no longer used.
- Removed the terrain namespace, which was the only namespace left in the engine and wrapped a single class hierarchy that was already unambiguous on its own.
- Nested HeightData inside Terrain as a protected type and RAW_HEIGHT_MAX as a public static constant, so both left the global scope with the namespace instead of leaking into it.
- Updated every terrain reference across the engine, mapping terrain::Terrain to Terrain, terrain::Vertex to TerrainVertex, terrain::HeightData to Terrain::HeightData and terrain::RAW_HEIGHT_MAX to Terrain::RAW_HEIGHT_MAX, and dropped the using directives that were pulling the namespace in.
- Dropped the glm includes from Terrain.h, which were only needed by the vertex struct that had already moved out, and added the glm/vec2 include to FaultFormationTerrain.h, which had been relying on Terrain.h to provide it.
- Moved the underwater post-process out of World and into WaterSystem, which now owns the fullscreen pass that resolves the offscreen scene through the underwater shader.
- Made the underwater effect conditional on submergence: the scene is only captured into the offscreen buffer and run through the post-process when the camera is below the surface, and is otherwise drawn straight to the screen.
- Tracked the submerged state inside WaterSystem, computed once per frame from the camera position in Update and exposed through GetIsUnderwater, so callers no longer recompute it against the water level themselves.
- Removed all raw OpenGL from World: the viewport and clear moved into Framebuffer, and the above-water case now relies on the per-frame screen clear the backend already performs.
- Gave Framebuffer its own width and height instead of the hardcoded 800x800, and folded the viewport and clear into BindBuffer so binding a framebuffer also prepares it to be drawn into.
- Changed World::Render to take no parameters and build the frame RenderContext itself, since World owns the camera, and updated GameScene accordingly.
- Extracted the shared opaque-scene-plus-water-surface pass into World::RenderSceneAndWater, reused by both the submerged and above-water paths.

## 2026-07-27
- Vendored Assimp v6.0.5 under MordoEngine/vendor/assimp, following the same include and lib layout already used by GLFW, glad, glm, perlin and stb.
- Built it as a shared library and wired it into Debug|x64 and Release|x64, with the ASSIMP_DLL define and a post-build step that copies the DLL next to the executable.

## 2026-07-28
- Added a model loading pipeline following the same split as the terrain: a loader that produces plain CPU data with no GL calls, a renderer that owns the GPU buffers, and a system that drives them.
- Implemented LoadModel on top of Assimp, resolving texture paths against the model file's own folder so the working directory stops mattering, and reading the PBR base colour slot with the legacy diffuse one as a fallback. Deliberately without aiProcess_FlipUVs, since Texture already flips through stb and applying both cancels out.
- Added ModelRenderer, which packs every submesh into a single VBO and EBO and draws each one as a slice through glDrawElementsBaseVertex.
- Added the model shaders, using the terrain's clip plane so models show up in the water reflection and refraction passes, and made LightSystem::ApplyUniforms public: it was private and only ever applied to the terrain shader, which left any model rendering completely black.
- Registered the model shader in ResourceLoader; model textures stay out of LoadTextures and are loaded on demand instead, keyed by the path Assimp returns.

## 2026-07-29
- Extracted a Model class out of ModelSystem: a Model owns one model's buffers, the draw call and texture of each submesh and its place in the world, while ModelSystem became the director of a std::vector<Model>. Model holds its renderer through a unique_ptr because ModelRenderer is neither copyable nor movable, and that indirection is the only thing making Model storable in a vector at all.
- Split the shader uniforms by scope: ModelSystem sets projection, view, plane and texture1 once for the whole pass, and each Model sets only its own model matrix. World's optional prop unique_ptr became a plain ModelSystem member, since a system holding zero models is a valid state and needs no null checks.
- Added a Transform class under Core/Transform holding position, rotation in degrees and scale, with SetHeight, a uniform SetScale and SetYaw, and a dirty-flag matrix cache because the water passes ask each object for its matrix several times per frame. Model now holds one instead of a raw glm::mat4.
- Added skeletal and rigid-node animation: bone ids and weights in the vertex layout, and the node hierarchy, the bones with their inverse bind matrices and the keyframe channels in ModelData.
- Made LoadModel choose its post-processing flags per file, reading once with no post-processing to check for animation first: PreTransformVertices and OptimizeMeshes for static models, LimitBoneWeights for animated ones. The first two bake away the node hierarchy the animation drives, so the same set cannot serve both.
- Added an Animator under Core/Model/Animation that samples the playing clip, resolves the node globals in a single forward pass -- the loader flattens the hierarchy depth first, so a parent always precedes its children -- and produces one final matrix per bone. It supports named clip lookup, looping and non-looping playback, and falls back to the rest pose so a file with no clips draws its bind pose instead of collapsing.
- Added the skinning path to the model vertex shader, blending up to four bone matrices per vertex and normalising by the total weight; the bone ids go through glVertexAttribIPointer so they arrive as integers rather than floats. Added Shader::SetMat4Array to upload a whole skeleton in one call.
- Added the Khronos Lantern and Fox sample models under res/models to exercise the pipeline end to end. The Lantern is CC0; the Fox is CC0 for the model but CC BY 4.0 for its rigging, animation and glTF conversion, so its README and LICENSE are kept alongside it.
- Split the documentation into loading-blender-models.md and model-animation.md, both in English with an index, explaining how each piece works and linking to the source instead of pasting it. Morph target animation is documented as not implemented: it needs per-vertex blending rather than node transforms, so it would be a separate path.

## 2026-07-31
- Added an actor layer under src/Actor: Actor owns the transform, ActorSystem updates every actor once per frame, and ActorContext hands it the terrain, the camera and the system itself. Actors never draw, which keeps gameplay out of World::RenderOpaque, since that runs three times per frame for the reflection and refraction passes.
- Made ModelSystem hold its models by unique_ptr, so the reference Add returns survives later calls and an actor can keep pointing at its model.
- Added PlayerActor, which reads the camera each frame and turns it into a position on the ground and a facing the rest of the world can query. The camera itself is untouched: the dependency runs camera to player until the player has to move on its own.
- Added LightActor and AnimalActor and moved the scene onto them, the two bare lights, the lantern as a light with a body and the fox as a creature that switches clips. LightActor runs a model space offset through the transform, so the lantern lights from its head and follows the actor's scale and yaw. LightSystem now hands back the index of a light and can move it, and PointLight can skip its debug cube.