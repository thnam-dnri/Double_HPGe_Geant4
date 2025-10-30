#!/bin/bash
# Configuration for G4CASCADE integration with DualHPGe simulation

# Point to CapGamData directory (absolute path)
export CAPGAM_DATA_DIR=/home/nam/geant4-install/Project/DualHPGe/CapGamData

# CASCADE settings (optional, for documentation)
# These would be used if integrating with modified neutron capture physics
export G4NEUTRONHP_USE_CASCADE=1
export G4NEUTRONHP_USE_RAW_EXCITATION=0
export G4NEUTRONHP_DO_UNPLACED=0

echo "CASCADE environment configured"
echo "CAPGAM_DATA_DIR = $CAPGAM_DATA_DIR"
