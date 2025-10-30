#!/usr/bin/env python3
"""
Cr54 Level Data Parser - Enhanced for ENSDF Format
Converts Cr54 level scheme data from ENSDF format to JSON for GEANT4 integration
"""

import json
import re
from typing import Dict, List, Any


def parse_cr54_level_data(file_path: str) -> Dict[int, Dict[str, Any]]:
    """
    Parse Cr54 level data from ENSDF format file

    Args:
        file_path: Path to the Cr54 level data file

    Returns:
        Dictionary mapping level numbers to level data
    """
    levels = {}
    current_level = None

    with open(file_path, 'r') as file:
        lines = file.readlines()

    for line_num, line in enumerate(lines, 1):
        line = line.rstrip()

        # Skip empty lines
        if not line.strip():
            continue

        # Check if this is a level header line (starts with level number)
        # Level header format: NL EL[MeV] S/P F T1/2[s] Ng s unc s-info nd m p mode
        level_match = re.match(r'^\s*(\d+)\s+([\d\.]+)\s+([\d\.\-]+)\s+([\d\-]+)\s+([\d\.E\-\+]+)', line)

        if level_match:
            # Parse level header
            nl = int(level_match.group(1))
            energy = float(level_match.group(2))
            spin = level_match.group(3)
            parity_field = level_match.group(4)
            half_life = level_match.group(5)

            # Parse spin and parity
            spin_value, parity = parse_spin_parity(spin, parity_field)

            # Create new level entry
            current_level = {
                'levelNumber': nl,
                'energy': energy,
                'spin': spin_value,
                'parity': parity,
                'halfLife': parse_half_life(half_life),
                'transitions': [],
                'branchingRatios': [],
                'levelInfo': line.strip()
            }

            levels[nl] = current_level

        elif current_level is not None and line.strip() and not line[0:3].strip().isdigit():
            # This is a gamma transition line (not a level header)
            # Format: Nf Eg[MeV] Pg Pe Icc
            gamma_match = re.match(r'^\s+(\d+)\s+([\d\.]+)\s+([\d\.E\-\+]+)\s+([\d\.E\-\+]+)\s+([\d\.E\-\+]+)', line)

            if gamma_match:
                target_level = int(gamma_match.group(1))
                gamma_energy = float(gamma_match.group(2))
                gamma_intensity = float(gamma_match.group(3))
                electron_intensity = float(gamma_match.group(4))
                internal_conversion = float(gamma_match.group(5))

                # Add transition to current level
                current_level['transitions'].append({
                    'targetLevel': target_level,
                    'gammaEnergy': gamma_energy,
                    'gammaIntensity': gamma_intensity,
                    'electronIntensity': electron_intensity,
                    'internalConversion': internal_conversion
                })

                # Calculate branching ratio (normalized intensity)
                current_level['branchingRatios'].append(gamma_intensity)

    # Normalize branching ratios
    for level_num, level_data in levels.items():
        if level_data['branchingRatios']:
            total_intensity = sum(level_data['branchingRatios'])
            if total_intensity > 0:
                level_data['branchingRatios'] = [br / total_intensity for br in level_data['branchingRatios']]

    return levels


def parse_spin_parity(spin_field: str, parity_field: str) -> tuple:
    """
    Parse spin and parity from ENSDF format fields

    Args:
        spin_field: Spin field from level header
        parity_field: Parity field from level header

    Returns:
        Tuple of (spin_value, parity)
    """
    # Parse spin
    try:
        spin_value = float(spin_field)
    except ValueError:
        # Handle special spin values
        if spin_field == '0.0':
            spin_value = 0.0
        else:
            spin_value = 0.0  # Default for unknown

    # Parse parity
    try:
        parity_int = int(parity_field)
        if parity_int > 0:
            parity = 1  # Positive parity
        elif parity_int < 0:
            parity = -1  # Negative parity
        else:
            parity = 1  # Default to positive
    except ValueError:
        parity = 1  # Default to positive

    return spin_value, parity


def parse_half_life(half_life_str: str) -> float:
    """
    Parse half-life from ENSDF format

    Args:
        half_life_str: Half-life string in scientific notation

    Returns:
        Half-life in seconds as float
    """
    try:
        # Handle scientific notation
        if 'E' in half_life_str or 'e' in half_life_str:
            return float(half_life_str)
        else:
            return float(half_life_str)
    except ValueError:
        return -1.0  # Stable or unknown


def convert_to_geant4_format(levels: Dict[int, Dict[str, Any]]) -> Dict[str, Any]:
    """
    Convert parsed levels to GEANT4-compatible format

    Args:
        levels: Parsed level data

    Returns:
        GEANT4-compatible level structure
    """
    geant4_levels = {}

    for level_num, level_data in levels.items():
        geant4_level = {
            'levelNumber': level_data['levelNumber'],
            'energy': level_data['energy'],
            'spin': level_data['spin'],
            'parity': level_data['parity'],
            'halfLife': level_data['halfLife']
        }

        # Convert transitions to GEANT4 format
        transitions = []
        branching_ratios = []

        for i, transition in enumerate(level_data['transitions']):
            transitions.append({
                'targetLevel': transition['targetLevel'],
                'gammaEnergy': transition['gammaEnergy']
            })

            # Use normalized branching ratio if available
            if i < len(level_data['branchingRatios']):
                branching_ratios.append(level_data['branchingRatios'][i])
            else:
                # Fallback to gamma intensity
                branching_ratios.append(transition['gammaIntensity'])

        geant4_level['transitions'] = transitions
        geant4_level['branchingRatios'] = branching_ratios

        geant4_levels[level_num] = geant4_level

    return geant4_levels


def main():
    """Main function to parse Cr54 level data and generate JSON output"""
    input_file = "/home/nam/geant4-install/Project/DualHPGe/cr54_level.txt"
    output_file = "/home/nam/geant4-install/Project/DualHPGe/cr54_levels.json"

    print(f"Parsing Cr54 level data from: {input_file}")

    try:
        # Parse the level data
        levels = parse_cr54_level_data(input_file)

        print(f"Successfully parsed {len(levels)} levels")

        # Convert to GEANT4 format
        geant4_levels = convert_to_geant4_format(levels)

        # Write JSON output
        with open(output_file, 'w') as f:
            json.dump(geant4_levels, f, indent=2)

        print(f"JSON output written to: {output_file}")

        # Print summary
        print("\nLevel Summary:")
        for level_num in sorted(geant4_levels.keys())[:10]:  # Show first 10 levels
            level = geant4_levels[level_num]
            print(f"  Level {level_num}: E={level['energy']} MeV, spin={level['spin']}, "
                  f"parity={level['parity']}, transitions={len(level['transitions'])}")

        if len(geant4_levels) > 10:
            print(f"  ... and {len(geant4_levels) - 10} more levels")

    except Exception as e:
        print(f"Error parsing Cr54 level data: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()