
# Insallation

This project requires gcc, opengl, and glut to run.

```
> apt-get install libgl1-mesa-dev libx11-dev libxrandr-dev libglu-dev freeglut3
> make
> ./main
```

# Orignal Readme

James Wonsever && Ben Han
cs184-be && cs184-cn

See inst.eecs.berkeley.edu/~cs184-be/hw6.html

Our project is an attempt to recreate the popular video	game, Minecraft.  We extended our hw4 and used concepts from hw5 to do so.  If you don't know what minecraft is, try it out here: https://minecraft.net/  It's essentially legos for your computer, but better.  And cooler.

We added many extra credit features from hw4 (Particles, Advanced/Interesting Lighting effects, Texture Mipmaps, Object Interaction, Collision Detection, Displ\
ay Lists, Procedural World Generation, Portals, and a 2d HUD display.)

Below are all of the features we implemented, with descriptions.

Things we've done:

Everything from hw4
    -Display Lists
    -Re-Instantiated Objects
    -Transformations
    -Fullscreen/Windowed Modes
    -Wireframe mode
    -Toggle Textures/Lighting
    -FPS Style Camera Viewing
    -Frames Per Second Output Data
    -Zoom in/out
    -Taking Screenshots
    -Double Buffering
    -Perspective Projections
    -Hidden Surface Elimination
    -And on, and on...
    
Collision Detection
    We implemented collision detection between the Human Player (Represented by a 2 height tall object, with the 'eye' at height 1.6) and between all of the blocks in the scene.  There is also a coordinate frame forcing no two blocks to ever be in the same square.

Textures
    We implemented new features to our texture implementation from hw4, including:
    -Mipmapping, 3 stages of mipmaps to make textures look more pleasant.
    -Many more textures added, implemented a List + Enum pair representing all the textures.
    -Textures are loaded given only a filename.  Easy to add new textures.
    -GL_Clamp (Psuedo Cell Shading) This gives the black lines around the edge of each block, much faster then wireframe + fill using Opengl.

Animations
    We implemented animations, similar to hw4.
    -Sun moves around the world.
    -HUD highlights depends on which block you select to use.

Block Addition & Subtraction
    We implemented interactive changing of the world, through blocks.  This is in the spirit of the actual minecraft game.
    -Right clicking removes a block
    -Left clicking adds a block
    Both of these require selection of an object.  See below.

Object Selection
    -Using a raytracer.  All objects are listed in a vector of "block" objects, a class made for just this reason.  They have an ID, which is returned on click and used to add/remove the block.  We get the id through a modified raytracing algorithm.  This implements the spirit of hw5 into our game.  The block is recognized into triangles, then we use the ray-triangle intersection routine.  The ray is shot through the center (cursor) in the middle of the HUD.  If you are adding a block, the block is added at the adjacent square in the normal direction.  It also checks that you do no add a block where you are standing (collision detection.)

HUD Screen Overlay
    We implemented a 2d interactive HUD in front of the 3d scene.  This also uses the textures as seen in the video.  It consists of a cursor in the middle, and another bar below with 10 textures to choose from.  Selecting from 0-9 gives you options to place a block of that texture next time you place a block.  These active selections can also be used to place torches or portals.
  
Fog
    We use GL_Mix to mix the fog with distance from the camera.  This is a shader effect from the vertex and fragment shaders.  This makes it seem as if the world fade away in the distance.  Limits the distance of visible objects.

Shadowing
    We use a simple hueristic shadowing that any block that is not on the top level gets shadowed by an attenuation factor.  While this is not realistic, it looks reasonable and doing ray-shadowmapping at interactive rates would be very difficult.
 
Procedural World Generation
    Many objects in our scene are procedurally generated.  This can be a combination of algoritms and/or random placements/rotations/scales.
    -Trees
    -Terrains  (There are multiple terrain options that can be changed in the code.  Depending on the input to the initmap function.)
    -Underground Objects  (In the spirit of the original game, you can find rare mineral blocks underground.)
    -Torches smoke is procedurally generated.
	   
Block properties/differences
    Some blocks have special properties.
    -Unbreakable blocks at the bottom of the world.
    -Different textures for sides and top of a block. (Grass)

Tridirectional motions
    We improved some features of our camera from hw4.
    -Concurent XYZ Motions (You can move in x and y and the same time, you can jump, all simultaneously.)
    -Jumping (Give the camera an upward impluse, but only if it the human object is touching the ground.)

Time Interaction
    Some things in our scene change over time.
    -Day/Night.  The Sun is animated, and as the sun moves across the sky and passes the horizon, night begins and all colors are attenuated by a factor to make them darker.
    -Changing sun brightness.  As the sun approaches the horizon, the light from it gets dimmer and objects are less bright.

Point and Directional lighting
    The minecraft clone implements optional lighting effects.  These are implemented using glsl shaders.
    -lambertian + phong shading, as in HW4
    -Up to 12 active lights.  Could easily be increased.
    -Torches.  A point light source.  Torches light up the area depending on how close you are to them.  These can be added interactively to the scene.
    -Sun.  A directional light from above.  This is attenuated in nighttime mode.
    -attenuation on point lights, in torches.  Gives torches the feel of a "local" light source.  Not as powerful as the sunlight.
    -multiple lighting effects can be enabled/disabled.  (as in HW4)

Particle Smoke
    -Torches have smoke coming off them, in the form of many small triangles.  These are spewed in random directions and dissapear once they reach a certain height. (Randomly around a certain height actually, for realism.)

Accelerations
    Considering we have a huge amount of blocks in the world (in a size 50 world, you have 100 * 100 * 20 = 200000 * 12 triangles per block = 24,000,000 objects)  We had to implement a bunch of accelerations to make the speed reasonable.
    -First, in our raytracing algorithm.  We decided to make a maximum distance for selection of an object, so any object that is too far away is completely ignored.  This lowers the amount of blocks to a reasonable amount and makes the algorithm much faster.
    -We ignore all blocks outside the fog distance, as these would be hidden anyways.  This lets us focus on only the blocks nearby to the user.  These blocks are put in a Block buffer for faster calculations.
    -Our shader intentionally skips doing the look calculations on lights not in the scene, instead of running through the loop N times.
    -We avoid divides and sq. roots wherever possible.
    -Textures are preloaded before drawing the objects.
    -We use display-lists to accelerate the drawing of blocks, by not having to load it into the shader each time.

Portals
    What's there to ask??
    -Teleportation
    -Fucking awesome
    -Thinking with portals