# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a GEANT4 simulation project for dual HPGe (High-Purity Germanium) detector systems that integrates with RAINIER nuclear physics data. The project simulates gamma-ray cascades and detector responses for nuclear physics experiments with sophisticated coincidence analysis and ROOT data output.

### Development Workflow

1. First think through the problem, read the codebase for relevant files, and write a plan to todo.md
2. The plan should have a list of todo items that you can check off as you complete them
3. Before you begin working, check in with me and I will verify the plan
4. Then, begin working on the todo items, marking them as complete as you go
5. Please every step of the way just give me a high level explanation of what changes you made
6. Make every task and code change you do as simple as possible. We want to avoid making any massive or complex changes. Every change should impact as little code as possible. Everything is about simplicity
7. Finally, add a review section to the todo.md file with a summary of the changes you made and any other relevant information


## Build System

This project uses CMake for building. The typical build process is:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the executable
make
```

## Project Structure

- `src/` - Source files containing GEANT4 user action classes
- `include/` - Header files for the user action classes
- `build/` - Build directory (created during compilation)
- `*.mac` - GEANT4 macro files for visualization and batch execution

## Key Source Files

- `HPGeDual.cc` - Main application file with command-line argument parsing and verbosity control
- `DetectorConstruction.cc` - Defines the dual HPGe detector geometry with configurable angle
- `PrimaryGeneratorAction.cc` - Generates gamma cascades from RAINIER data with cascade/single mode
- `Run.cc`/`RunAction.cc` - Run management with coincidence analysis and ROOT output
- `EventAction.cc` - Event processing with angular correlation and cascade tracking
- `SteppingAction.cc` - Step-level tracking for energy deposition
- `PhysicsList.cc` - Defines the physics processes
- `RootOutput.cc` - ROOT file output for coincidence data

## Common Development Tasks

### Building and Running

```bash
# Build the project
cd build && cmake .. && make

# Run with default settings
./HPGeDual

# Run in quiet mode with specific detector angle
./HPGeDual -quiet -angle 45

# Run with RAINIER data file
./HPGeDual -quiet rainier_data.txt

# Run with macro file
./HPGeDual run.mac
```

### Command Line Options

- `-angle <degrees>` - Set angle for second detector (default: 180.0)
- `-cascade` - Generate Co-60 cascades (2 gammas per event)
- `-single` - Generate single gammas (1 gamma per event)
- `-quiet` - Suppress all non-essential output
- `-h, --help` - Show help message

### Output Files

The simulation generates several output files:
- `gamma_spectrum_det1.dat` - Energy spectrum from detector 1 (+Z axis)
- `gamma_spectrum_det2.dat` - Energy spectrum from detector 2 (specified angle)
- `coincidence_matrix.dat` - Coincidence data between detectors
- `angular_distribution.dat` - Angular distribution data
- `timing_spectrum.dat` - Timing spectrum data
- `coincidences.root` - ROOT file with detailed coincidence event data

## Architecture Notes

- The project follows standard GEANT4 application structure with user action classes
- Integration with RAINIER nuclear physics data for realistic gamma cascade simulation
- Dual detector setup allows for coincidence measurements and angular correlation studies
- Verbosity control implemented throughout for both interactive and batch execution
- Physics list includes electromagnetic processes relevant for gamma-ray detection
- **Coincidence Analysis**: Sophisticated coincidence detection with timing windows and angular correlation
- **Cascade Tracking**: Full cascade sequence tracking with parent-child relationships
- **ROOT Integration**: Automatic ROOT file output for detailed event analysis
- **Configurable Geometry**: Second detector angle can be set via command line
- **Multiple Modes**: Support for both cascade (multiple gammas per event) and single gamma modes

## Dependencies

- GEANT4 (with visualization and UI components)
- ROOT (for data analysis and output)
- CMake (version 3.8 or higher)
- C++11 compatible compiler
- Optional: RAINIER nuclear physics data files