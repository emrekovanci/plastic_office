### TODOs

- Shader uniform caching & hot reloading
- Uniform Buffer Objects for model & projection matrices
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

### Update & Sync Submodules

```bash
cd vendor/<submodule-name>
git fetch --tags
git describe --tags $(git rev-list --tags --max-count=1)
git checkout <tags>
cd ../..
git add vendor/<submodule-name>
git commit -m "pin <submodule-name> to <tag>"
```

```bash
git pull --recurse-submodules
```

### CMake Analysis

```
cmake --preset=msvc -DCMAKE_BUILD_TYPE=Debug --trace-expand 2> trace.log
```

### Frame Analysis

```
ffmpeg -i cap.mp4 -vf fps=60 frames/frame_%04d.png
```

### Building

```
cmake --preset=msvc -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Testing

```
cmake --build build --target tests
ctest --test-dir build -V
```
