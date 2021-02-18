# trace3dV2: a simple raytracer

## about

trace3dV2 is a simple raytracer that at the time can load a stl file and trace the mesh that is contained in it

The end goal of the project is to create a fully fledged raytracer with support for textures, animated transform and video output and also the porting of the software to the Amiga 500.

## Why

* **Because:**
  - I **can**
  - I want to write software for the Amiga
  - I want to breag about writing a raytracer for the Amiga
  - I have no friends and am bored (maybe due the pandamic of human mallware as well)

## How is the project going

At this time this Github repo is mainly for sharing code between my laptop and desktop without using a flashdrive but here is the progress so far:

- [x] loading stl files
- [x] drawing 3d triangles
- [x] outputing the render to file
- [x] object and camera transformation
- [ ] "raytracing"
- [ ] texture mapping
- [ ] animation
- [ ] amiga port

**Amiga specifics**

- [ ] HAM-ify
- [ ] EHB-ify
- [ ] Offloading work to copper
- [ ] Switching to fixed point of float
- [ ] general optimisation for speed and memory usage

## Credits

I used code from these people:
- OBJ parser from '[Bly7](https://github.com/Bly7)' found [here](https://github.com/Bly7/OBJ-Loader)
- Math formula for raytracing addapted from java script [source](https://www.uninformativ.de/bin/
RaytracingSchnitttests-76a577a-CC-BY.pdf)
