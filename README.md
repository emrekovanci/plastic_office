### TODOs

- Shader uniform caching
- Cross platform asset paths

### Docs

- Input Handling: https://www.reddit.com/r/Unity3D/comments/7267yi/player_inputs_update_or_fixedupdate/
- Movement & Collision: https://deepnight.net/tutorial/a-simple-platformer-engine-part-1-basics/
- SDL Quick Refeference: https://wiki.libsdl.org/SDL3/QuickReference
- OpenGL Wiki: https://wikis.khronos.org/opengl/Getting_Started
- OpenGL Tutorial: https://nicolbolas.github.io/oldtut/index.html
- Game Loop: https://dewitters.com/dewitters-gameloop/
- Game Loop: https://gameprogrammingpatterns.com/game-loop.html
- Game Loop: https://lotusspring.substack.com/p/upgrade-your-timestep
- Game Loop: https://gafferongames.com/post/fix_your_timestep/

### Issues

- https://github.com/libsdl-org/SDL/issues/12791
- https://gamedev.stackexchange.com/questions/49032/8-directional-movement-maintain-vertical-position

### Frame Analysis

```
ffmpeg -i cap.mp4 -vf fps=60 frames/frame_%04d.png
```

### Building

```
cmake --preset=msvc -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=OFF -DBUILD_CONSOLE=ON
cmake --build build
```

### Testing

```
cmake --build build --target tests
ctest --test-dir build -V
```
