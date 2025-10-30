# Cross-Section Visualization in Geant4

This guide shows how to view cross-sections of your detector geometry to inspect internal structures like the germanium crystal, dead layers, and lead shields.

## Method 1: Use the Cross-Section Macro (Recommended)

Run the pre-configured cross-section visualization:

```bash
./HPGeDual init_vis.mac
/control/execute vis_crosssection.mac
```

Or directly:
```bash
./HPGeDual vis_crosssection.mac
```

This will show a cutaway view along the X axis, revealing the internal structure.

## Method 2: Interactive Commands

After starting visualization with `init_vis.mac`, you can type these commands in the Geant4 session:

### Enable cutaway mode:
```
/vis/viewer/set/cutawayMode union
```

### Add cutaway planes:

**Cut along X axis (vertical cut, shows YZ plane):**
```
/vis/viewer/addCutawayPlane 0 0 0 m 1 0 0
```

**Cut along Y axis (horizontal cut, shows XZ plane):**
```
/vis/viewer/addCutawayPlane 0 0 0 m 0 1 0
```

**Cut along Z axis (beam direction cut, shows XY plane):**
```
/vis/viewer/addCutawayPlane 0 0 0 m 0 0 1
```

**Multiple cuts (quarter view):**
```
/vis/viewer/addCutawayPlane 0 0 0 m 1 0 0
/vis/viewer/addCutawayPlane 0 0 0 m 0 1 0
```

### Clear all cutaway planes:
```
/vis/viewer/clearCutawayPlanes
```

### Disable cutaway mode:
```
/vis/viewer/set/cutawayMode subtract
```

## Method 3: Section Plane (Alternative)

Instead of cutting away geometry, you can show only a thin slice:

```
/vis/viewer/set/sectionPlane on 0 0 0 m 1 0 0
```

To turn off:
```
/vis/viewer/set/sectionPlane off
```

## Cutaway Plane Syntax

The cutaway plane command syntax is:
```
/vis/viewer/addCutawayPlane x y z unit nx ny nz
```

Where:
- `x y z unit`: Point on the plane (e.g., `0 0 0 m` for origin)
- `nx ny nz`: Normal vector to the plane (e.g., `1 0 0` for YZ plane)

## Useful Tips

1. **Best view for detector internals**: Cut along X axis
   ```
   /vis/viewer/addCutawayPlane 0 0 0 m 1 0 0
   /vis/viewer/set/viewpointThetaPhi 90 0
   ```

2. **View down the beam axis**: Cut along Y axis and view from side
   ```
   /vis/viewer/addCutawayPlane 0 0 0 m 0 1 0
   /vis/viewer/set/viewpointThetaPhi 90 90
   ```

3. **Quarter section view**: Multiple cuts
   ```
   /vis/viewer/addCutawayPlane 0 0 0 m 1 0 0
   /vis/viewer/addCutawayPlane 0 0 0 m 0 1 0
   /vis/viewer/set/viewpointThetaPhi 45 45
   ```

4. **Offset cut** (to see specific detector):
   ```
   /vis/viewer/addCutawayPlane 0 0 5 cm 1 0 0
   ```

## Refresh the View

After adding/removing cutaway planes:
```
/vis/viewer/refresh
```
or
```
/vis/viewer/flush
```
