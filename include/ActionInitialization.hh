// ==============================================================================
// ActionInitialization.hh - Action Initialization for Multi-Threading
// ==============================================================================

#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "globals.hh"
#include <string>

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization(const std::string& rainierFile = "",
                        bool generateCascades = true,
                        SourceMode sourceMode = (SourceMode)0,
                        G4int cascadeZ = 17,
                        G4int cascadeA = 36,
                        G4double cascadeSn = 8.579,
                        bool twoGammaOnly = false);
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

private:
    std::string fRAINIERFile;
    bool fGenerateCascades;
    SourceMode fSourceMode;
    G4int fCascadeZ;
    G4int fCascadeA;
    G4double fCascadeSn;
    bool fTwoGammaOnly;
};

#endif
