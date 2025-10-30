// ==============================================================================
// PrimaryGeneratorAction.hh - Enhanced for Cascade Generation
// ==============================================================================

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4CASCADE.hh"
#include "G4Fragment.hh"
#include <string>
#include <vector>
#include <random>

// ROOT forward declarations and types
#include "Rtypes.h"  // For Double_t, Long64_t types

class G4ParticleGun;
class G4Event;

// Enhanced structure for gamma cascade data
struct GammaData {
    double energy;      // MeV
    double intensity;   // relative intensity
    double time;        // ns (cascade timing)
    int cascadeIndex;   // Which cascade this gamma belongs to
    int sequenceOrder;  // Order in cascade (1st, 2nd, etc.)
};

// Structure for complete cascade sequences
struct CascadeData {
    std::vector<GammaData> gammas;
    double totalIntensity;
    std::string cascadeName;
};

// Structure for two-gamma pairs (Sn -> intermediate -> ground)
struct TwoGammaPair {
    double gamma1;              // First gamma energy (MeV): Sn - E_intermediate
    double gamma2;              // Second gamma energy (MeV): E_intermediate
    int intermediateLevel;      // Intermediate level number
    double intermediateEnergy;  // Intermediate level energy (MeV)
    double spin;                // Intermediate level spin
    int parity;                 // Intermediate level parity
};

// Source mode enumeration
enum SourceMode {
    CO60_CASCADE,    // Co-60 cascade (2 gammas: 1.173 + 1.332 MeV)
    SINGLE_GAMMA,    // Single gamma mode (random Co-60 gamma)
    CASCADE_DIRECT,  // Direct cascade generation using G4CASCADE
    CASCADE_RAINIER  // RAINIER cascade from ROOT file
};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(const std::string& rainierFile = "", bool generateCascades = true,
                           SourceMode initialMode = CO60_CASCADE);
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    // CASCADE mode configuration
    void SetSourceMode(SourceMode mode);
    void SetIsotope(G4int Z, G4int A) { fIsotopeZ = Z; fIsotopeA = A; }
    void SetExcitationEnergy(G4double E) { fExcitationEnergy = E; }
    void SetCascadePosition(G4ThreeVector pos) { fCascadePosition = pos; }
    void SetTwoGammaOnly(bool flag) { fTwoGammaOnly = flag; }

private:
    G4ParticleGun* fParticleGun;
    std::string fRAINIERFile;
    std::vector<GammaData> fGammaData;        // Individual gammas (legacy)
    std::vector<CascadeData> fCascadeData;    // Complete cascades
    std::mt19937 fRandomGenerator;
    bool fGenerateCascades;                   // Flag for cascade mode

    // CASCADE mode members
    SourceMode fSourceMode;
    G4int fIsotopeZ;                          // Atomic number for cascade
    G4int fIsotopeA;                          // Mass number for cascade
    G4double fExcitationEnergy;               // Excitation energy (MeV)
    G4CASCADE* fCascadeGenerator;
    G4ThreeVector fCascadePosition;           // Where cascade occurs

    // RAINIER ROOT file members
    class TFile* fRAINIERRootFile;            // ROOT file handle
    class TTree* fRAINIERTree;                // ROOT tree
    std::vector<Double_t>* fRAINIEREgs;       // Gamma energies from RAINIER
    std::vector<Double_t>* fRAINIERExfs;      // Final excitation energies
    Long64_t fRAINIERCurrentEntry;            // Current entry in ROOT tree
    Long64_t fRAINIERTotalEntries;            // Total entries in tree
    Long64_t fRAINIEREmptyCount;              // Count of empty cascades skipped
    bool fTwoGammaOnly;                       // Only allow 2-gamma cascades

    // Methods for cascade handling
    GammaData SampleGamma();                  // Sample individual gamma (legacy)
    void InitializeRAINIERFile();             // Open and setup RAINIER ROOT file
    bool GetNextRAINIERCascade();             // Read next valid cascade from file

    // Position and direction sampling
    G4ThreeVector SampleSourcePosition();
    G4ThreeVector SampleDirection();

    // Cascade generation methods
    void GenerateSingleGammaEvent(G4Event* anEvent);
    void GenerateCo60Cascade(G4Event* anEvent);
    void GenerateCascadeGammas(G4Event* anEvent);
    void GenerateRAINIERCascade(G4Event* anEvent);

    const char* SourceModeToString(SourceMode mode) const;
};

#endif
