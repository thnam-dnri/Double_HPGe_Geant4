// ==============================================================================
// HPGeDual.cc - Dual HPGe Detector Simulation (Minimal Verbosity Version)
// Geant4 gamma cascade simulation using RAINIER data with dual detectors
// ==============================================================================

#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#include "G4SystemOfUnits.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

// Headers for verbosity control
#include "G4NuclearLevelData.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ProcessTable.hh"
#include "G4HadronicProcessStore.hh"
#include "G4CrossSectionDataSetRegistry.hh"
#include "G4ios.hh"

// Global flag for quiet mode
bool g_quietMode = false;

void PrintUsage() {
    G4cout << "\nUsage: " << G4endl;
    G4cout << "  ./HPGeDual [options] [RAINIER_file] [macro_file]" << G4endl;
    G4cout << "\nOptions:" << G4endl;
    G4cout << "  -angle <degrees>    : Angle for second detector (default: 180.0)" << G4endl;
    G4cout << "  -coin               : Generate Co-60 coincidences (2 gammas per event)" << G4endl;
    G4cout << "  -single             : Generate single gammas (1 gamma per event)" << G4endl;
    G4cout << "  -cascade [Z A Sn]   : Generate CASCADE gamma cascades (realistic neutron capture)" << G4endl;
    G4cout << "                        Z  = Atomic number (default: 17 for Cl)" << G4endl;
    G4cout << "                        A  = Mass number (default: 36)" << G4endl;
    G4cout << "                        Sn = Neutron separation energy in MeV (default: 8.579)" << G4endl;
    G4cout << "  -RAINIER <file>     : Use RAINIER ROOT file as cascade source" << G4endl;
    G4cout << "                        File should be RAINIER simulation output (Run####.root)" << G4endl;
    G4cout << "  -two-gamma-only     : For RAINIER mode, only use cascades with exactly 2 gammas" << G4endl;
    G4cout << "                        AND total energy > 5.4 MeV" << G4endl;
    G4cout << "                        Default: allow all cascade multiplicities" << G4endl;
    G4cout << "  -threads <N>        : Number of threads for parallel execution (default: 1)" << G4endl;
    G4cout << "                        Use 'auto' or 0 to use all available CPU cores" << G4endl;
    G4cout << "  -quiet              : Suppress all non-essential output" << G4endl;
    G4cout << "  -h, --help          : Show this help message" << G4endl;
    G4cout << "\nArguments:" << G4endl;
    G4cout << "  RAINIER_file        : Optional RAINIER data file (uses Co-60 test data if not provided)" << G4endl;
    G4cout << "  macro_file          : Optional Geant4 macro file (.mac extension)" << G4endl;
    G4cout << "\nExamples:" << G4endl;
    G4cout << "  ./HPGeDual -quiet                    # Silent mode with Co-60 test data" << G4endl;
    G4cout << "  ./HPGeDual -angle 45 -quiet          # Silent mode, 45° detector angle" << G4endl;
    G4cout << "  ./HPGeDual -quiet rainier_data.txt   # Silent mode with RAINIER file" << G4endl;
    G4cout << "  ./HPGeDual -coin -quiet              # Silent mode with Co-60 coincidences" << G4endl;
    G4cout << "  ./HPGeDual -single -quiet            # Silent mode with single gammas" << G4endl;
    G4cout << "  ./HPGeDual -cascade                  # CASCADE mode with Cl-36 (default)" << G4endl;
    G4cout << "  ./HPGeDual -cascade 24 54 9.719      # CASCADE mode with Cr-54" << G4endl;
    G4cout << "  ./HPGeDual -cascade 64 158 7.937     # CASCADE mode with Gd-158" << G4endl;
    G4cout << "  ./HPGeDual -cascade -threads 4       # CASCADE with 4 CPU cores" << G4endl;
    G4cout << "  ./HPGeDual -cascade -threads auto    # CASCADE using all CPU cores" << G4endl;
    G4cout << "  ./HPGeDual -RAINIER Run0001.root     # Use RAINIER cascades from file" << G4endl;
    G4cout << "  ./HPGeDual -RAINIER Run0001.root -threads 4  # RAINIER with 4 cores" << G4endl;
    G4cout << "  ./HPGeDual -RAINIER Run0001.root -two-gamma-only  # Only 2-gamma, E>5.4 MeV" << G4endl;
    G4cout << "\n" << G4endl;
}

int main(int argc, char** argv)
{
    // Parse command line arguments first to check for quiet mode
    bool quietMode = false;
    bool cascadeMode = true;  // Default to cascade mode for coincidence analysis
    SourceMode sourceMode = CO60_CASCADE;  // Default source mode
    std::string rainierFile = "";
    G4double detector2Angle = 180.0;
    std::string macroFile = "";

    // CASCADE isotope parameters (defaults: Cl-36)
    G4int cascadeZ = 17;
    G4int cascadeA = 36;
    G4double cascadeSn = 8.579;  // MeV

    // Multi-threading parameter
    G4int nThreads = 1;  // Default: single-threaded

    // RAINIER filter parameter
    bool twoGammaOnly = false;  // Default: allow all cascade multiplicities

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return 0;
        }
        else if (arg == "-quiet" || arg == "-q") {
            quietMode = true;
        }
        else if (arg == "-coin") {
            cascadeMode = true;
            sourceMode = CO60_CASCADE;
        }
        else if (arg == "-single") {
            cascadeMode = false;
            sourceMode = SINGLE_GAMMA;
        }
        else if (arg == "-cascade") {
            sourceMode = CASCADE_DIRECT;

            // Check for optional Z A Sn arguments
            if (i + 3 < argc) {
                // Try to parse next 3 arguments as numbers
                std::stringstream ssZ(argv[i + 1]);
                std::stringstream ssA(argv[i + 2]);
                std::stringstream ssSn(argv[i + 3]);

                G4int tempZ, tempA;
                G4double tempSn;

                if ((ssZ >> tempZ) && (ssA >> tempA) && (ssSn >> tempSn)) {
                    // Successfully parsed Z A Sn
                    cascadeZ = tempZ;
                    cascadeA = tempA;
                    cascadeSn = tempSn;
                    i += 3;  // Skip the 3 arguments we just consumed
                }
            }
        }
        else if (arg == "-RAINIER") {
            if (i + 1 < argc) {
                rainierFile = argv[i + 1];
                sourceMode = CASCADE_RAINIER;
                i++;
                if (!quietMode) {
                    G4cout << "RAINIER ROOT file specified: " << rainierFile << G4endl;
                }
            } else {
                if (!quietMode) {
                    G4cout << "Error: -RAINIER requires a ROOT file path" << G4endl;
                }
                return 1;
            }
        }
        else if (arg == "-angle") {
            if (i + 1 < argc) {
                std::stringstream ss(argv[i + 1]);
                if (!(ss >> detector2Angle)) {
                    if (!quietMode) G4cout << "Error: Invalid angle value '" << argv[i + 1] << "'" << G4endl;
                    detector2Angle = 180.0;
                }
                i++;
            }
        }
        else if (arg == "-threads") {
            if (i + 1 < argc) {
                std::string threadArg = argv[i + 1];
                if (threadArg == "auto" || threadArg == "0") {
                    // Use all available CPU cores
                    nThreads = std::thread::hardware_concurrency();
                } else {
                    std::stringstream ss(threadArg);
                    if (!(ss >> nThreads) || nThreads < 1) {
                        if (!quietMode) G4cout << "Error: Invalid threads value '" << threadArg << "'" << G4endl;
                        nThreads = 1;
                    }
                }
                i++;
            }
        }
        else if (arg == "-two-gamma-only") {
            twoGammaOnly = true;
            if (!quietMode) {
                G4cout << "RAINIER filter: Only 2-gamma cascades with E_total > 5.4 MeV will be used" << G4endl;
            }
        }
        else if (arg.find(".mac") != std::string::npos) {
            macroFile = arg;
        }
        else if (arg.find(".txt") != std::string::npos || arg.find(".dat") != std::string::npos) {
            rainierFile = arg;
        }
        else if (rainierFile.empty() && macroFile.empty()) {
            rainierFile = arg;
        }
        else if (macroFile.empty()) {
            macroFile = arg;
        }
        else {
            if (!quietMode) G4cout << "Warning: Ignoring unrecognized argument: " << arg << G4endl;
        }
    }

    // Set the global quiet mode flag
    g_quietMode = quietMode;

    // Print startup info only if not in quiet mode
    if (!quietMode) {
        G4cout << "\n========================================" << G4endl;
        G4cout << "  Dual HPGe Detector Simulation" << G4endl;
        G4cout << "  RAINIER + Geant4 Integration" << G4endl;
        G4cout << "========================================\n" << G4endl;

        if (rainierFile == "" || rainierFile == "\"\"") {
            G4cout << "No RAINIER file specified. Using test Co-60 cascade." << G4endl;
            rainierFile = "";
        } else {
            G4cout << "Using RAINIER input file: " << rainierFile << G4endl;
        }

        G4cout << "Configuration:" << G4endl;
        G4cout << "  Detector 2 angle: " << detector2Angle << " degrees" << G4endl;
        G4cout << "  RAINIER file: " << (rainierFile.empty() ? "Co-60 test data" : rainierFile) << G4endl;

        std::string modeStr;
        if (sourceMode == CASCADE_RAINIER) {
            modeStr = "RAINIER (theoretical cascades from ROOT file)";
        } else if (sourceMode == CASCADE_DIRECT) {
            modeStr = "CASCADE (realistic neutron capture cascades)";
        } else if (sourceMode == CO60_CASCADE) {
            modeStr = "Co-60 Cascade (2 gammas/event)";
        } else {
            modeStr = "Single gamma (1 gamma/event)";
        }
        G4cout << "  Generation mode: " << modeStr << G4endl;
        if (!macroFile.empty()) {
            G4cout << "  Macro file: " << macroFile << G4endl;
        }
        G4cout << G4endl;
    }

    // Create run manager (MT or ST depending on nThreads)
    G4RunManager* runManager = nullptr;
    if (nThreads > 1) {
        G4MTRunManager* mtRunManager = new G4MTRunManager();
        mtRunManager->SetNumberOfThreads(nThreads);
        runManager = mtRunManager;
        if (!quietMode) {
            G4cout << "Multi-threading enabled with " << nThreads << " threads" << G4endl;
        }
    } else {
        runManager = new G4RunManager();
        if (!quietMode) {
            G4cout << "Single-threaded mode" << G4endl;
        }
    }

    // COMPREHENSIVE VERBOSITY SUPPRESSION
    // ===================================
    
    // 1. Nuclear de-excitation parameters
    G4DeexPrecoParameters* deex = G4NuclearLevelData::GetInstance()->GetParameters();
    deex->SetVerbose(0);
    
    // 2. Production cuts table
    G4ProductionCutsTable::GetProductionCutsTable()->SetVerboseLevel(0);
    
    // 3. Physics list helper
    G4PhysicsListHelper::GetPhysicsListHelper()->SetVerboseLevel(0);
    
    // 4. Hadronic process store
    G4HadronicProcessStore::Instance()->SetVerbose(0);
    
    // 5. Cross section registry (method may not exist in all Geant4 versions)
    // G4CrossSectionDataSetRegistry::Instance()->SetVerboseLevel(0);
    
    // 6. Process table
    G4ProcessTable::GetProcessTable()->SetVerboseLevel(0);

    // Set mandatory initialization classes
    DetectorConstruction* detConstruction = new DetectorConstruction(detector2Angle);
    runManager->SetUserInitialization(detConstruction);

    PhysicsList* physicsList = new PhysicsList();
    runManager->SetUserInitialization(physicsList);

    // Use ActionInitialization for MT-safe action setup
    ActionInitialization* actionInitialization =
        new ActionInitialization(rainierFile, cascadeMode, sourceMode,
                                cascadeZ, cascadeA, cascadeSn, twoGammaOnly);
    runManager->SetUserInitialization(actionInitialization);

    // Initialize visualization (only if not quiet mode)
    G4VisManager* visManager = nullptr;
    if (!quietMode) {
        visManager = new G4VisExecutive("Quiet"); // Use quiet visualization
        visManager->Initialize();
    }

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // COMPREHENSIVE UI COMMAND SUPPRESSION
    // ===================================
    UImanager->ApplyCommand("/process/verbose 0");
    UImanager->ApplyCommand("/geometry/navigator/verbose 0");
    UImanager->ApplyCommand("/particle/verbose 0");
    UImanager->ApplyCommand("/run/verbose 0");
    UImanager->ApplyCommand("/event/verbose 0");
    UImanager->ApplyCommand("/tracking/verbose 0");
    UImanager->ApplyCommand("/process/em/verbose 0");
    UImanager->ApplyCommand("/process/had/verbose 0");
    UImanager->ApplyCommand("/cuts/verbose 0");
    UImanager->ApplyCommand("/material/verbose 0");
    UImanager->ApplyCommand("/physics_lists/verbose 0");
    
    // Additional quiet mode suppressions
    if (quietMode) {
        UImanager->ApplyCommand("/run/printProgress 0");
        UImanager->ApplyCommand("/control/verbose 0");
        UImanager->ApplyCommand("/vis/verbose 0");
        UImanager->ApplyCommand("/vis/scene/verbose 0");
        UImanager->ApplyCommand("/vis/sceneHandler/verbose 0");
        UImanager->ApplyCommand("/vis/viewer/verbose 0");
    }

    // Batch vs interactive
    bool batchMode = !macroFile.empty();

    if (batchMode) {
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + macroFile);
    } else {
        if (!quietMode) {
            G4UIExecutive* ui = new G4UIExecutive(argc, argv);
            UImanager->ApplyCommand("/control/execute init_vis.mac");
            if (ui->IsGUI()) {
                UImanager->ApplyCommand("/control/execute gui.mac");
            }
            ui->SessionStart();
            delete ui;
        } else {
            // In quiet mode without macro, just show final message
            if (!quietMode) {
                G4cout << "Quiet mode: Use -h for help or provide a macro file for batch execution." << G4endl;
            }
        }
    }

    // Clean up
    if (visManager) delete visManager;
    delete runManager;

    // Final message (always shown unless completely silent)
    if (!quietMode) {
        G4cout << "\nDual detector simulation completed successfully!" << G4endl;
        G4cout << "Output files:" << G4endl;
        G4cout << "  - gamma_spectrum_det1.dat (Detector 1 at +Z axis)" << G4endl;
        G4cout << "  - gamma_spectrum_det2.dat (Detector 2 at " << detector2Angle << "°)" << G4endl;
    }
    
    return 0;
}