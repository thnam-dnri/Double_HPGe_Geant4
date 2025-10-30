// ==============================================================================
// PrimaryGeneratorAction.cc - Enhanced for Cascade Generation
// ==============================================================================

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "G4NuclideTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"
#include "G4Gamma.hh"
#include "G4ReactionProduct.hh"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>
#include <sstream>

// ROOT configuration must see std::string_view support before including TFile/TTree
#include "RConfigure.h"
#ifndef R__HAS_STD_STRING_VIEW
#define R__HAS_STD_STRING_VIEW 1
#endif

// ROOT headers for RAINIER file reading
#include "TFile.h"
#include "TTree.h"

// External global variable for quiet mode
extern bool g_quietMode;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& rainierFile,
                                               bool generateCascades,
                                               SourceMode initialMode)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
  fRAINIERFile(rainierFile),
  fRandomGenerator(std::random_device{}()),
  fGenerateCascades(generateCascades),
  fSourceMode(initialMode),
  fIsotopeZ(17),              // Default: Cl-36 (from Cl-35 + n)
  fIsotopeA(36),
  fExcitationEnergy(8.579),   // Cl-36 neutron separation energy (MeV)
  fCascadePosition(0, 0, 0),  // Default cascade position at origin
  fCascadeGenerator(nullptr),
  fRAINIERRootFile(nullptr),
  fRAINIERTree(nullptr),
  fRAINIEREgs(nullptr),
  fRAINIERExfs(nullptr),
  fRAINIERCurrentEntry(0),
  fRAINIERTotalEntries(0),
  fRAINIEREmptyCount(0),
  fTwoGammaOnly(false)
{
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    // Default particle type and properties - set to gamma
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "gamma";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    fParticleGun->SetParticleEnergy(1.*MeV);

    // Initialize CASCADE generator
    fCascadeGenerator = new G4CASCADE();

    // Debug output to verify constructor parameters
    if (!g_quietMode) {
        G4cout << "PrimaryGeneratorAction constructor called with:" << G4endl;
        G4cout << "  generateCascades = " << fGenerateCascades << G4endl;
        G4cout << "  rainierFile = " << fRAINIERFile << G4endl;
        G4cout << "  sourceMode = " << SourceModeToString(fSourceMode) << G4endl;
    }

    // Verify CASCADE has data for default isotope
    if (fCascadeGenerator->HasData(fIsotopeZ, fIsotopeA)) {
        if (!g_quietMode) {
            G4cout << "CASCADE: Data available for Z=" << fIsotopeZ
                   << " A=" << fIsotopeA << " (Cl-36)" << G4endl;
        }
    } else {
        if (!g_quietMode) {
            G4cout << "CASCADE: WARNING - No data for default isotope Z="
                   << fIsotopeZ << " A=" << fIsotopeA << G4endl;
        }
    }

    if (!g_quietMode) {
        switch (fSourceMode) {
            case CO60_CASCADE:
                G4cout << "Using Co-60 cascade source (1.173 and 1.332 MeV)" << G4endl;
                break;
            case SINGLE_GAMMA:
                G4cout << "Using single gamma mode (randomized Co-60 gamma)" << G4endl;
                break;
            case CASCADE_DIRECT:
                G4cout << "Using CASCADE-generated neutron capture cascades" << G4endl;
                break;
            case CASCADE_RAINIER:
                G4cout << "RAINIER cascade mode enabled; reading cascades from ROOT file"
                       << G4endl;
                break;
        }
    }

    // Initialize RAINIER ROOT file if specified
    if (!fRAINIERFile.empty()) {
        InitializeRAINIERFile();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fCascadeGenerator;

    // Clean up RAINIER ROOT file resources
    if (fRAINIERRootFile) {
        if (!g_quietMode && fRAINIEREmptyCount > 0) {
            G4cout << "RAINIER file statistics: Skipped " << fRAINIEREmptyCount
                   << " empty cascades" << G4endl;
        }
        fRAINIERRootFile->Close();
        delete fRAINIERRootFile;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SetSourceMode(SourceMode mode)
{
    if (fSourceMode == mode) {
        if (mode == CASCADE_RAINIER && !fRAINIERFile.empty() && fRAINIERRootFile == nullptr) {
            InitializeRAINIERFile();
        }
        return;
    }

    fSourceMode = mode;

    if (!g_quietMode) {
        G4cout << "PrimaryGeneratorAction: Switching source mode to "
               << SourceModeToString(mode) << G4endl;
    }

    if (mode == CASCADE_RAINIER && !fRAINIERFile.empty() && fRAINIERRootFile == nullptr) {
        InitializeRAINIERFile();
    }
}

const char* PrimaryGeneratorAction::SourceModeToString(SourceMode mode) const
{
    switch (mode) {
        case CO60_CASCADE:
            return "Co-60 cascade";
        case SINGLE_GAMMA:
            return "single gamma";
        case CASCADE_DIRECT:
            return "CASCADE neutron capture";
        case CASCADE_RAINIER:
            return "RAINIER cascade";
    }
    return "unknown";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    switch(fSourceMode) {
        case CO60_CASCADE:
            GenerateCo60Cascade(anEvent);
            break;
        case SINGLE_GAMMA:
            GenerateSingleGammaEvent(anEvent);
            break;
        case CASCADE_DIRECT:
            GenerateCascadeGammas(anEvent);
            break;
        case CASCADE_RAINIER:
            GenerateRAINIERCascade(anEvent);
            break;
    }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateSingleGammaEvent(G4Event* anEvent)
{
    // Original single gamma generation (for compatibility)
    GammaData gamma = SampleGamma();
    
    fParticleGun->SetParticleEnergy(gamma.energy * MeV);
    
    G4ThreeVector sourcePos = SampleSourcePosition();
    fParticleGun->SetParticlePosition(sourcePos);
    
    G4ThreeVector direction = SampleDirection();
    fParticleGun->SetParticleMomentumDirection(direction);
    
    fParticleGun->SetParticleTime(0.0);
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateCo60Cascade(G4Event* anEvent)
{
    // Generate Co-60 cascade (2 gammas per event)
    G4ThreeVector sourcePos = SampleSourcePosition();

    // First gamma: 1.173 MeV
    fParticleGun->SetParticleEnergy(1.173 * MeV);
    fParticleGun->SetParticlePosition(sourcePos);
    fParticleGun->SetParticleMomentumDirection(SampleDirection());
    fParticleGun->SetParticleTime(0.0);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    // Second gamma: 1.332 MeV
    fParticleGun->SetParticleEnergy(1.332 * MeV);
    fParticleGun->SetParticlePosition(sourcePos);
    fParticleGun->SetParticleMomentumDirection(SampleDirection());
    fParticleGun->SetParticleTime(0.0);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateCascadeGammas(G4Event* anEvent)
{
    // Create G4LorentzVector with mass and excitation energy
    // At rest at the cascade position
    G4ParticleDefinition* ion = G4IonTable::GetIonTable()
        ->GetIon(fIsotopeZ, fIsotopeA, 0.0);  // Ground state
    G4double mass = ion->GetPDGMass();
    G4double totalEnergy = mass + fExcitationEnergy * MeV;
    G4LorentzVector momentum(0., 0., 0., totalEnergy);

    // Create excited nucleus fragment
    G4Fragment* excitedNucleus = new G4Fragment(
        fIsotopeA,           // Mass number
        fIsotopeZ,           // Atomic number
        momentum             // 4-momentum (at rest + excitation)
    );

    // Generate cascade gammas using G4CASCADE
    G4ReactionProductVector* cascadeProducts = nullptr;

    if(fCascadeGenerator->HasData(fIsotopeZ, fIsotopeA)) {
        cascadeProducts = fCascadeGenerator->GetGammas(
            *excitedNucleus,
            false,  // UseRawExcitation = false (fixed excitation)
            false   // doUnplaced = false (ignore unplaced gammas)
        );
    } else {
        G4cerr << "ERROR: No CASCADE data for Z=" << fIsotopeZ
               << " A=" << fIsotopeA << G4endl;
        delete excitedNucleus;
        return;
    }

    // Add all cascade gammas to the event
    if(cascadeProducts) {
        for(size_t i = 0; i < cascadeProducts->size(); i++) {
            G4ReactionProduct* product = (*cascadeProducts)[i];

            // Only add gammas (skip electrons from internal conversion)
            if(product->GetDefinition() == G4Gamma::Gamma()) {
                fParticleGun->SetParticleDefinition(
                    const_cast<G4ParticleDefinition*>(product->GetDefinition()));

                // Get momentum vector
                G4ThreeVector momentum = product->GetMomentum();
                G4double energy = momentum.mag();
                G4ThreeVector direction = momentum.unit();

                fParticleGun->SetParticleEnergy(energy);
                fParticleGun->SetParticleMomentumDirection(direction);
                fParticleGun->SetParticlePosition(fCascadePosition);

                fParticleGun->GeneratePrimaryVertex(anEvent);
            }
        }

        // Clean up
        for(auto* product : *cascadeProducts) {
            delete product;
        }
        delete cascadeProducts;
    }

    delete excitedNucleus;

    // Debug output every 1000 events
    if(anEvent->GetEventID() % 50000 == 0 && !g_quietMode) {
        G4cout << "Event " << anEvent->GetEventID()
               << ": Generated CASCADE from Z=" << fIsotopeZ
               << " A=" << fIsotopeA << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GammaData PrimaryGeneratorAction::SampleGamma()
{
    // Co-60 gamma source: randomly select between 1.173 and 1.332 MeV
    GammaData gamma;

    std::uniform_real_distribution<> dist(0.0, 1.0);
    if (dist(fRandomGenerator) < 0.5) {
        gamma.energy = 1.173;  // 1.173 MeV
    } else {
        gamma.energy = 1.332;  // 1.332 MeV
    }

    gamma.intensity = 100.0;
    gamma.time = 0.0;
    gamma.cascadeIndex = -1;
    gamma.sequenceOrder = 1;

    return gamma;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector PrimaryGeneratorAction::SampleSourcePosition()
{
    // Point source at origin
    return G4ThreeVector(0., 0., 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector PrimaryGeneratorAction::SampleDirection()
{
    // Isotropic emission direction
    G4double cosTheta = 2.*G4UniformRand() - 1.;
    G4double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
    G4double phi = twopi*G4UniformRand();

    return G4ThreeVector(sinTheta*std::cos(phi),
                        sinTheta*std::sin(phi),
                        cosTheta);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::InitializeRAINIERFile()
{
    if (fRAINIERFile.empty()) {
        G4cerr << "ERROR: RAINIER file path is empty!" << G4endl;
        return;
    }

    // Open ROOT file
    fRAINIERRootFile = TFile::Open(fRAINIERFile.c_str(), "READ");
    if (!fRAINIERRootFile || fRAINIERRootFile->IsZombie()) {
        G4cerr << "ERROR: Cannot open RAINIER ROOT file: " << fRAINIERFile << G4endl;
        fRAINIERRootFile = nullptr;
        return;
    }

    // Get tree
    fRAINIERTree = (TTree*)fRAINIERRootFile->Get("tree");
    if (!fRAINIERTree) {
        G4cerr << "ERROR: Cannot find 'tree' in RAINIER file: " << fRAINIERFile << G4endl;
        fRAINIERRootFile->Close();
        delete fRAINIERRootFile;
        fRAINIERRootFile = nullptr;
        return;
    }

    fRAINIERTotalEntries = fRAINIERTree->GetEntries();

    // Set up branch addresses
    fRAINIEREgs = nullptr;
    fRAINIERExfs = nullptr;
    fRAINIERTree->SetBranchAddress("Egs", &fRAINIEREgs);
    fRAINIERTree->SetBranchAddress("Exfs", &fRAINIERExfs);

    if (!g_quietMode) {
        G4cout << "\n========================================" << G4endl;
        G4cout << "  RAINIER ROOT File Initialized" << G4endl;
        G4cout << "========================================" << G4endl;
        G4cout << "File: " << fRAINIERFile << G4endl;
        G4cout << "Total cascade events: " << fRAINIERTotalEntries << G4endl;
        G4cout << "========================================\n" << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool PrimaryGeneratorAction::GetNextRAINIERCascade()
{
    if (!fRAINIERTree) {
        G4cerr << "ERROR: RAINIER tree not initialized!" << G4endl;
        return false;
    }

    // Loop through entries to find next valid cascade
    // (with at least 1 gamma, or exactly 2 gammas with E_total > 5.4 MeV if filter enabled)
    while (fRAINIERCurrentEntry < fRAINIERTotalEntries) {
        fRAINIERTree->GetEntry(fRAINIERCurrentEntry++);

        // Check if this cascade meets the filter criteria
        if (fRAINIEREgs && fRAINIEREgs->size() > 0) {
            // Apply 2-gamma filter if enabled
            if (fTwoGammaOnly) {
                if (fRAINIEREgs->size() == 2) {
                    // Check total energy: sum of both gammas must be > 5.4 MeV
                    G4double totalEnergy = (*fRAINIEREgs)[0] + (*fRAINIEREgs)[1];
                    if (totalEnergy > 5.4) {
                        return true;  // Found valid 2-gamma cascade with E_total > 5.4 MeV
                    } else {
                        fRAINIEREmptyCount++;  // Count filtered cascades (low energy)
                    }
                } else {
                    fRAINIEREmptyCount++;  // Count filtered cascades
                }
            } else {
                return true;  // Found valid cascade (any multiplicity)
            }
        } else {
            fRAINIEREmptyCount++;  // Count empty cascades
        }
    }

    // Reached end of file - wrap around to beginning
    if (!g_quietMode) {
        G4cout << "Reached end of RAINIER file. Wrapping to beginning..." << G4endl;
    }
    fRAINIERCurrentEntry = 0;
    return GetNextRAINIERCascade();  // Recursive call to get first valid entry
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateRAINIERCascade(G4Event* anEvent)
{
    // Get next valid cascade from RAINIER file
    if (!GetNextRAINIERCascade()) {
        G4cerr << "ERROR: Failed to read RAINIER cascade!" << G4endl;
        return;
    }

    // Get source position (same for all gammas in cascade)
    G4ThreeVector sourcePos = SampleSourcePosition();

    // Generate all gamma rays from this cascade
    for (size_t i = 0; i < fRAINIEREgs->size(); i++) {
        G4double gammaEnergy = (*fRAINIEREgs)[i] * MeV;

        // Set particle properties
        fParticleGun->SetParticleEnergy(gammaEnergy);
        fParticleGun->SetParticlePosition(sourcePos);
        fParticleGun->SetParticleMomentumDirection(SampleDirection());
        fParticleGun->SetParticleTime(0.0);  // All gammas at t=0 (instant cascade)

        // Add gamma to event
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    // Debug output every 1000 events
    if (anEvent->GetEventID() % 50000 == 0 && !g_quietMode) {
        G4cout << "Event " << anEvent->GetEventID()
               << ": Generated " << fRAINIEREgs->size()
               << " gammas from RAINIER cascade" << G4endl;
    }
}
