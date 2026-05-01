### TODOs

- Collision
- Better input management & states
- Cross Platform asset paths

### Movement Docs

- https://deepnight.net/tutorial/a-simple-platformer-engine-part-1-basics/

### SDL

- https://wiki.libsdl.org/SDL3/QuickReference

### OpenGL Docs

- https://wikis.khronos.org/opengl/Getting_Started
- https://nicolbolas.github.io/oldtut/index.html

### Issues

- https://gamedev.stackexchange.com/questions/49032/8-directional-movement-maintain-vertical-position
- https://github.com/libsdl-org/SDL/issues/12791

### Frame Analysis

```
ffmpeg -i cap.mp4 -vf fps=60 frames/frame_%04d.png
```

### Building & Testing

```
cmake --preset=msvc -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build -V
```
