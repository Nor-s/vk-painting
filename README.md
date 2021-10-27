# vk-painting

### 220 x 220 1min ~ 3min

<img src = "/note/lena.gif" width="30%" height="30%" title="n=8" alt="n=8"></img>

<img src = "/note/lena.png" width="30%" height="30%" title="n=8" alt="n=8"></img>

### 512 x 512 3min ~ 10min

<img src = "/note/sample.gif" width="30%" height="30%" title="n=8" alt="n=8"></img>

<img src = "/note/sample.png" width="30%" height="30%" title="n=8" alt="n=8"></img>

# ToDo

offscreen - command pool

separation: object - pipeline (for renderpass compatibility)

key input 

camera movement

command buffers arrangement

Fix multithread

Fix hardcoding -> supportsBlit = false

Add library VMA: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

Add imgui: https://github.com/ocornut/imgui


# Algorithm

random algorithm (basic genetic algorithm)

1. init population
2. draw brush (dna : scale, position, rotation, color .. => transformation)
3. Similarity measurement (cosine similarity:  https://en.wikipedia.org/wiki/Cosine_similarity)
4. sort by fitness
5. top dna : offscreen rendering and copy to texture
6. top 3 dna : cross over, mutate 
7. repeat 2~6



# Dependency

vulkan: https://www.vulkan.org/

glm: https://github.com/g-truc/glm

glfw: https://github.com/glfw/glfw

stb: https://github.com/nothings/stb


# Reference

Vulkan tutorial: https://vulkan-tutorial.com/

Khronos group vulkan example: https://github.com/KhronosGroup/Vulkan-Samples

SaschWillems vulkan example: https://github.com/SaschaWillems/Vulkan

Little vulkan enginge: https://github.com/blurrypiano/littleVulkanEngine

Scarp engine: https://github.com/ScrappyCocco/ScrapEngine

Acid: https://github.com/EQMG/Acid

My repo: https://github.com/Nor-s/genetic-painting
