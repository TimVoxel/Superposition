# Superposition

Superposition is a minimal particle system with the capability to distribute particles according to a 2d probability distribution. This makes the system suitable for making clear (and cool looking) demonstrations of basic quantum systems, such as the hydrogen orbitals.
<br>
The project also features an implementation of the discrete (Crank-Nicolson) non-stationary Schrödinger equation. Unfortunately, it is currently impossible to visualize the probability changes due to their complex nature.

## Building

The project uses CMake (3.20+) with `FetchContent` to pull GLFW and nlohmann/json automatically — no manual dependency setup needed. It builds three targets:

- **Superposition** — the main renderer/player (`src/main.cpp`).
- **AnalyticalHydrogen** (`tools/AnalyticalHydrogen`) — generates a hydrogen orbital `.bin` file from an analytical solution for a given `(n, l, m)`.
- **EigenstateSolver** (`tools/EigenstateSolver`) — a numerical eigenstate solver, also producing `.bin` orbital files (used for systems without a clean analytical solution).

```
cmake --build build/debug --target Superposition
```

## How to run

The application can run in 2 modes - display and render.  
`-d` flag is used to run in display mode (default). In this mode a window will open, rendering the scene in realtime.  
`-r` flag is used to run in render mode. In this mode, an ffmpeg pipe will be opened, rendering into a video file according to the configuration in `resources/config.json` (this path is currently hardcoded, not configurable via CLI).  
`-s <path>` flag is used to specify which scene to display/render. A scene file is a json representation of what should happen in the scene. Defaults to `resources/scene1.json` if omitted.  
`-o <dir>` flag is used to specify the output directory (when render mode is used). Defaults to `output/`.

```
build\debug\Superposition.exe -d -s "resources/final/scene1.json"          # display mode
build\debug\Superposition.exe -r -s "resources/final/scene1.json" -o out   # render mode
```

In render mode, the output file is written to `<outputDir>/<scene name>.<config.video.fileExtension>` — the scene's own `"name"` field becomes the filename, so scene names should avoid characters that aren't valid in a filename (`: * ? " < > | / \`).

Orbitals are generated separately, ahead of time, with the `AnalyticalHydrogen` tool (see [Generating orbitals](#generating-orbitals-analyticalhydrogen) below) — scenes just reference the resulting `.bin` file path.

## Render config (`resources/config.json`)

Loaded once at startup (path is hardcoded to `resources/config.json`), independent of which scene is loaded. Controls the video output format and the OpenGL clear color.

```jsonc
{
    "video": {
        "width": 1920,
        "height": 1080,
        "fps": 60,
        "codec": "prores_ks",           // optional, defaults to "libx264"
        "pixelFormat": "yuva444p10le",  // optional, defaults to "yuv420p"
        "fileExtension": "mov",         // optional, defaults to "mp4"
        "extraArgs": [                  // optional, defaults to none
            { "flag": "profile:v", "value": "4444" }
        ]
    },
    "rendering": {
        "clearColor": [0.0, 0.0, 0.0, 0.0]   // RGBA, background/clear color
    }
}
```

`extraArgs` are passed straight through to ffmpeg as `-<flag> <value>` pairs, inserted between `-c:v <codec>` and `-pix_fmt <pixelFormat>`.
Note: `width`/`height` also set the display-mode window size, and (in render mode) the resolution particle point sizes are computed against — see [Coordinate system](#coordinate-system) below.

## Scene file format

A scene is a JSON file describing a name, a duration, and a tree of objects.

```jsonc
{
    "name": "Scene 1 - wave function",
    "durationSeconds": 34.0,
    "objects": [ /* SceneObject[] , see below */ ]
}
```

- `name` — used in render mode as the output filename.
- `durationSeconds` — in render mode, `totalFrames = durationSeconds * config.video.fps`; the scene runs exactly that many frames then stops. Doesn't limit anything in display mode (the window just stays open).
- `objects` — top-level list of `SceneObject`s (see below). Scene time starts at 0 for every object at once.

### SceneObject (shared fields)

Every object in `objects` (and every nested `children`/`first`/`second`) is one of three types, chosen by a `"type"` field: `"particleSystem"`, `"particleSystemLink"`, or `"container"`. Regardless of type, every object shares these fields:

| field | required | description |
|---|---|---|
| `type` | yes | `"particleSystem"` \| `"particleSystemLink"` \| `"container"` |
| `transform` | yes | `{ "pos": [x, y], "scale": [x, y], "opacity": 1.0 }` — `opacity` is optional, defaults to `1.0`. See [Transform](#transform--worldtransform) below. |
| `children` | no | array of nested `SceneObject`s, defaults to none. |
| `animations` | no | array of `Animation`s applied to this object's own transform, defaults to none. See [Animations](#animations) below. |

#### Transform & worldTransform

`transform.pos`/`scale` are plain `[x, y]` arrays (world units, not pixels — see [Coordinate system](#coordinate-system)). An object's final on-screen transform is its own (animated) transform composed down through its parent chain:

```
world.position = parent.world.position + own.position * parent.world.scale
world.scale    = parent.world.scale * own.scale
world.opacity  = parent.world.opacity * own.opacity
```

So a child's `pos` is relative to its parent, scaled by the parent's scale; opacity multiplies down the chain (a parent at `opacity: 0.5` halves everything under it, regardless of what the children set).

#### Animations

Each entry in `animations` has a `type` and a `target`. `target` is either a single string or an array of strings from `"position"`, `"scale"`, `"opacity"` — multiple targets receive the same computed delta each frame (for `opacity`, only the X component of a 2D animation is used, since opacity is scalar).

All animation types are **additive**: they add a computed delta on top of the object's base `transform` value (and on top of whatever other animations already contributed), they never overwrite it outright. `time` passed to every animation is elapsed time since *that object's own* animations first started ticking (not raw wall-clock time), so `startTime`/`phase`/etc. are reproducible regardless of when the scene actually starts.

**`sin`** — continuous periodic oscillation.
```jsonc
{ "type": "sin", "target": "position", "amplitudeX": 0.2, "amplitudeY": 0.0, "frequency": 1.0, "phase": 0.0 }
```
`value = sin(time * frequency + phase) * amplitude`, added every frame. `frequency` is angular (rad/s) — period is `2π / frequency`, not `1 / frequency`.

**`ease`** — one-shot ramp. Ramps from `0` up to `(deltaX, deltaY)` over `[startTime, startTime + duration]` using a smoothstep curve, then holds at the full delta forever after (contributes nothing before `startTime`).
```jsonc
{ "type": "ease", "target": "opacity", "deltaX": -1.0, "deltaY": 0.0, "startTime": 10.0, "duration": 1.5 }
```
To fade an object from value A to value B, set its base `transform` value to A and give this a delta of `B - A`. Several `ease` animations on the same target compose correctly in sequence (e.g. a fade-in early, a fade-out later), since each only contributes within its own window.

**`pulse`** — repeating on/off toggle, the periodic counterpart to `ease`. Swings between "off" (contributes 0) and "on" (contributes the full delta) every `period` seconds, snapping quickly between the two rather than smoothly breathing like `sin`.
```jsonc
{ "type": "pulse", "target": "opacity", "deltaX": 1.0, "deltaY": 0.0, "period": 3.14159265, "phase": -1.57079633, "edge": 0.32 }
```
Internally a sine wave at rate `2π/period` is thresholded through a smoothstep across `[-edge, edge]` around each zero-crossing, so it sits flat at 0 or 1 most of the cycle and transitions over roughly `edge` seconds around each crossing (`edge` is clamped to `(0.001, 0.999)`). `phase` uses the same convention as `sin`, so a `pulse` can be phase-locked to a `sin` animation driving some other object (e.g. make a collapse coincide with a `sin`-driven position hitting its extreme).

### `particleSystem`

A single emitter that spawns particles according to a `waveFunction`'s probability distribution.

```jsonc
{
    "type": "particleSystem",
    "transform": { "pos": [0.0, 0.0], "scale": [1.5, 1.5] },
    "config": { /* ParticleSystemConfig, see below */ },
    "waveFunction": { /* WaveFunction, see below — optional */ }
}
```

`waveFunction` is optional at the schema level, but required in practice — a `particleSystem` without one can't spawn particles randomly (it would need to be driven externally, e.g. as a `particleSystemLink` child).

`config`
| field | type | description |
|---|---|---|
| `growSeconds` | float | duration of the initial ramp-up: spawn rate climbs linearly from 0 to `maxSpawnRatePS` over this many seconds. |
| `sustainSeconds` | float | after `growSeconds`, spawn rate holds flat at `maxSpawnRatePS` for this many more seconds. |
| `maxSpawnRatePS` | float | particles spawned per second once fully ramped up. |
| `maxParticles` | int | hard cap on simultaneously-alive particles for this system; spawning silently no-ops once hit. |
| `lifetime` | float | seconds a particle lives after spawning before it's removed. |
| `pointSize` | float | base rendered point size, in world units (see [Coordinate system](#coordinate-system) for how this maps to pixels). |
| `sizeIncrease` | float | how much a particle's rendered size grows over its lifetime (added over `[0, lifetime]`, see `Compositor`). |
| `amplitude` | float | magnitude of a small per-particle sinusoidal jitter applied continuously after spawn (independent of the spawn-time position). |
| `frequency` | float | jitter oscillation rate, used by the same per-particle wobble as `amplitude`. |
| `color` | `[r, g, b, a]` | base RGBA color (0-1 range), multiplied by the object's world opacity at render time. |

**Spawn lifecycle, precisely**: once `elapsedTime >= growSeconds + sustainSeconds`, spawning stops completely — for good, not just paused — and never resumes. Existing particles keep updating (jittering, aging) and disappear individually once their own `lifetime` elapses, producing a visual "fade out".

### `particleSystemLink`

Two particle systems ("entangled" pair) driven by **one shared** `waveFunction`. Every time a sample is drawn, it's mirrored: `first` receives `(x, y)`, `second` receives `(-x, -y)` — same instant, same underlying random draw, negated. This is how the project's "quantum nonlocality" scenes get two visually-separated clouds/points whose positions are always exact mirror images of each other in real time, rather than independently random.

```jsonc
{
    "type": "particleSystemLink",
    "transform": { "pos": [0.0, -0.6], "scale": [1.0, 1.0] },
    "first":  { "type": "particleSystem", "transform": {...}, "config": {...}, "animations": [...] },
    "second": { "type": "particleSystem", "transform": {...}, "config": {...}, "animations": [...] },
    "waveFunction": { /* shared WaveFunction */ }
}
```

- `first` and `second` are each a full `particleSystem` object (own `transform`, `config`, optional `animations`) — this is where per-side anchor position, color, size etc. differ. If specified, their own `waveFunction` gets ignored.
- The **grow/sustain timing for the whole pair is driven by `first`'s config only** (`second`'s `growSeconds`/`sustainSeconds`/`maxSpawnRatePS` are parsed but not used for spawn timing — keep them consistent with `first`'s to avoid confusion, even though only `first`'s actually matter). `maxParticles`, `lifetime`, `pointSize`, `color`, etc. are read independently per side as usual.
- Each of `first`/`second`'s own `animations` (e.g. a `pulse` fading it in/out) applies normally and independently, since each gets its own `applyAnimation()` call.
- This system is not real quantum enganglement, rather it was used to create a simple and intuitive analogy.

### `container`

A plain grouping node with no rendering/spawning behavior of its own.

```jsonc
{
    "type": "container",
    "transform": { "pos": [-2.0, 0.0], "scale": [1.0, 1.0] },
    "animations": [ { "type": "sin", "target": "position", "amplitudeX": 0.0, "amplitudeY": 0.15, "frequency": 0.5, "phase": 3.14159265 } ],
    "children": [ /* SceneObject[] */ ]
}
```

### `waveFunction`

Determines where a `particleSystem` (or `particleSystemLink`) draws random spawn positions from. Chosen by `"type"`.

**`gaussian`** — a simple 2D normal distribution (not a real wave function — used for classical/non-quantum effects).
```jsonc
{ "type": "gaussian", "sigma": 0.25 }
```

**`hydrogen`** — samples a real hydrogen orbital's `|ψ|²` probability density, precomputed in a `.bin` file:
```jsonc
{ "type": "hydrogen", "orbital": "resources/orbitals/hydrogen_n2_l1_m0.bin", "timeStep": 0.01 }
```
`timeStep` is optional in both forms (defaults to `0.0001`) and controls the internal PDE integration step for the (currently real-amplitude-only) time evolution — it does not affect a static precomputed orbital's spatial distribution, only how it's allowed to evolve afterward.

## Generating orbitals (`AnalyticalHydrogen`)

`resources/orbitals/*.bin` files are generated ahead of time by the `AnalyticalHydrogen` tool from a closed-form hydrogen wavefunction solution (valid `l` is `0..n-1`, valid `m` is `-l..l`):

```
build\debug\AnalyticalHydrogen.exe -n 2 -l 1 -m 0 -r 256 -o resources/orbitals/hydrogen_n2_l1_m0.bin
```

| flag | default | meaning |
|---|---|---|
| `-n` | 2 | principal quantum number |
| `-l` | 1 | azimuthal quantum number |
| `-m` | 0 | magnetic quantum number |
| `-r` | 64 | grid resolution (produces an `r × r` grid) |
| `-a0` | 1.0 | Bohr radius, in simulation units |
| `-o` | `orbital.bin` | output path |

The tool auto-selects a physical extent (`size`) wide enough that the wavefunction's tail has genuinely decayed (`< 0.1%` of peak amplitude at the edge) before evaluating the grid, so you don't need to guess a bounding box yourself. `run-ah.bat <n> <l> <m> <outputPrefix>` wraps this (builds first, then runs with `-r 256`), matching the naming convention `<prefix>_n<n>_l<l>_m<m>.bin`.

The **`.bin` file format** (`OrbitalFile`, `include/wavefunction/OrbitalFile.hpp`):

```
uint32   magic          (0x48594F33)
int32    resolution
float32  size
float32  a0
float32  energy
int32    n
int32    l
int32    m
int32    radialLevel
[resolution * resolution] complex64 psi   (interleaved re, im; row-major, index = y * resolution + x)
```

For systems without a clean closed-form solution, `EigenstateSolver` (`tools/EigenstateSolver`) numerically solves for eigenstates instead, writing the same `.bin` format.

## Coordinate system

World units, not pixels: `transform.pos`/`scale` are in an abstract unit where the *full screen height* spans `2` units (`y` from `-1` to `1`), regardless of render resolution. Horizontally, the vertex shader divides by aspect ratio (`gl_Position.x = position.x / aspectRatio`), so `x` spans `±aspectRatio` (e.g. `±1.778` at 16:9) — meaning **both axes scale by the same number of pixels per world unit** (at 1920×1080: `540 px` per world unit, since `1080 / 2 = 540` and `(1920/1080)⁻¹ · ... = 540` work out identically by construction).

`pointSize` in a particle config is also in world units, converted to a screen-space point size as `resolution.x * pointSize * transform.scale.x` at render time — using `config.video` resolution, not the actual display window size, so render-mode output scales correctly regardless of window state.

## Examples
The project contains a multitude of sample scenes under ```resources/```