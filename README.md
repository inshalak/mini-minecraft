# Mini Minecraft
## Milestone 1
[Video](https://youtu.be/f-kt_XCjlXg)
### Procedural Terrain (Insha)
All of the procedural terrain functions are implemented within the Terrain class. They are consolidated within the fillBlock function. I used implementations of Perlin and Voronoi (Worley) noise we had covered in lecture to experiment with different types of implementations for the terrain. I also interpolated between the two biomes using the same perlin Noise implementation with a frequency of 128 to ensure that both terrains were present with adequate frequency. I experimented with different offsets for the Mountain range on the GPU before implementing the exact configurations on the CPU. Most of my trouble involved trial and error during the process of figuring out the right shape and size for the mountain range and figuring out the right noise function for the grasslands. 

### Efficient Terrain Rendering & Chunking (Jonah)

- The VBOs for each chunk are created by iterating through each block in the chunk, checking if the block has any faces next to empty blocks, if so, vertex data is added to the VBO. After iterating through every block in the chunk, the vertex data and index data vectors are sent to the GPU with glBufferData.
  - To iterate through each face, I implemented the adjacentFaces hashMap as demonstrated in class.
- The shader program needed a new method in order to draw from interleaved vertex data.
- The terrain stores a set with all of its chunks

### Game Engine Tick Function & Player Physics (Fiona)
- PlayerPhysics: I used standard velocity, acceleration, gravity, friction, and drag functions. I chose values of 0.7 and 0.4 for friction and drag, respectively. 
- Overall structures: I tried to keep purely key, mouse, and button presses in MyGL and call functions in my Player Class
- Colliding with Terrain: I initially started with my terrain collision as 1 ray / each of the corners and then transitioned to 3 rays (x, y, z) rays / each of the corners. However, my 3 ray implementation did not perfectly work yet and I have gone back to the 1 ray / each corner implementation for now.
- Adding & Removing Blocks: I used the look vector to figure out what direction I was looking in to add/remove a block. 

## Milestone 2
[Video](https://youtu.be/FK23mczUOP8)
### Cave Systems (Fiona)
Creating caves was quite straight forward as I used Perlin3d from the slides and then in our SetBlock function, changed it to create lava or bedrock at certain levels. I altered player physics by altering the GridMarch to also return what type of block it was. In the case that it was water, I would set a WaterMode which would then allow players to jump. Creating caves lead to merge conflicts with Multithreading. 

The post-process rendering pipeling was confusing at first especially understanding that I had to add m_quad from HW4. Both me and textures had to use uv coordinates and it was confusing trying to figure out who is what. Using and looking at HW4 was quite helpful and I used it as a reference. Afterwards though, it ended up working and making more sense. 

### Texturing and Texture Animation (Insha)
Texturing was a methodoligical proccess that took some time to get right. The key was using lecture notes and HW4 as a reference. Overall, merging with mulitithreading made the process of understanding Opaque vs Transparent blocks a lot easier. 

### Multithreading (Jonah)
I used Qt's multithreading support for this feature. To implement multithreading, I created two new "worker" classes, which are subclasses of QRunnable. The worker classes mostly reuse code from the efficient terrain rendering feature, where FBMWorker generates chunks using noise functions and VBOWorker prepares VBO data from the chunk data. The big change is that the VBO worker does not directly send data to the GPU, the data must be sent by the main thread. To tie everything together, the main thread spawns FBMWorkers when a new chunk needs to be generated, spawns VBOWorkers when the chunk data is ready, and then collects the data from the VBOWorkers and sends it to the GPU.

## Milestone 3

### Redstone (Jonah)
To implement a redstone system, I added a new "RedstoneItem" abstract class that represents any redstone block. The "RedstoneItem" class holds pointers to the other RedstoneItems that it is adjacent to, as well as an update() function, in which the RedstoneItem updates it's current state based on the external redstone state. The update() function is implemented using a recursive algorithm similar to a graph traversal, which allows for signals to travel down wires. The terrain class has two collections, one that owns unique pointers to each redstone item, and one that holds every redstone item which is a "source". To update the state of the redstone blocks, the terrain object calls update() on all of its source blocks.

### Inventory + Sound + Partial Crafting (Fiona)
To implement Inventory, I added a new GUI that pops up and down with the button I. I added a new variable to editing and adding blocks that allowed me to pass in what block type is being used at a given time. I just used the types of blocks that we currently had that our inventory could store. 
For sound, it was a little difficult at first because the names of the class used has changed to QSoundEffect as opposed to QSound and it took a while for me to figure that out. I added two sounds, one for underwater - bubbles and then a thunking noise for when I added and removed blocks. However, recording the video was tough because it was hard for the sounds to sync up on video even though they did in real life. 
For partial crafting, I started implementing the GUI for crafting, including the four blocks that the user can choose from and having them populate on PushButtons. Finding out how to use the menu of buttons was a little difficult as there isn't a default dropdown for QT. With more time, I would expand the capabilities of inventory and crafting for more items and so more items could be crafted.

### Additional Biomes (Desert, Swamp, Plateau ) + Smooth interpolation based on temperature + (Kinda) Procedural Sky ( Day-Night Cycle) + Procedural Placement of Trees + Cactus + Water Displacement 

I experiemnted a lot with different noise functions to get the biomes to work right. The badlands plateau was one of the hardest to implement but it was fun playing around with differrent textures and elements. I also implemented procedural sky from scratch, to create my own sun-like structure. To ensure the water displacement worked well, I played around with different noise functions that allowed me to mimic a wave like effect on the surface, and also interpolated colour so that when the wave surfaced outside of the water it appeared as an opaque blue colour. 
