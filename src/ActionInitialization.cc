// ==============================================================================
// ActionInitialization.cc - Action Initialization for Multi-Threading
// ==============================================================================

#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

// External global variable for quiet mode
extern bool g_quietMode;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization(const std::string& rainierFile,
                                         bool generateCascades,
                                         SourceMode sourceMode,
                                         G4int cascadeZ,
                                         G4int cascadeA,
                                         G4double cascadeSn,
                                         bool twoGammaOnly)
: G4VUserActionInitialization(),
  fRAINIERFile(rainierFile),
  fGenerateCascades(generateCascades),
  fSourceMode(sourceMode),
  fCascadeZ(cascadeZ),
  fCascadeA(cascadeA),
  fCascadeSn(cascadeSn),
  fTwoGammaOnly(twoGammaOnly)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
    // Master thread only creates RunAction for global run accumulation
    SetUserAction(new RunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
    // Primary generator
    PrimaryGeneratorAction* primaryGenerator =
        new PrimaryGeneratorAction(fRAINIERFile, fGenerateCascades);
    primaryGenerator->SetSourceMode(fSourceMode);
    primaryGenerator->SetTwoGammaOnly(fTwoGammaOnly);

    // Configure CASCADE isotope if in CASCADE_DIRECT mode
    if (fSourceMode == CASCADE_DIRECT) {
        // Verify CASCADE has data for requested isotope
        G4CASCADE* tempCascade = new G4CASCADE();
        bool hasData = tempCascade->HasData(fCascadeZ, fCascadeA);
        delete tempCascade;

        if (hasData) {
            // Use requested isotope
            primaryGenerator->SetIsotope(fCascadeZ, fCascadeA);
            primaryGenerator->SetExcitationEnergy(fCascadeSn);
            if (!g_quietMode) {
                G4cout << "CASCADE: Using Z=" << fCascadeZ << " A=" << fCascadeA
                       << " Sn=" << fCascadeSn << " MeV" << G4endl;
            }
        } else {
            // Fall back to Cl-36 with warning
            G4cerr << "WARNING: No CASCADE data for Z=" << fCascadeZ
                   << " A=" << fCascadeA << G4endl;
            G4cerr << "         Falling back to default Cl-36 (Z=17, A=36, Sn=8.579 MeV)"
                   << G4endl;
            primaryGenerator->SetIsotope(17, 36);
            primaryGenerator->SetExcitationEnergy(8.579);
        }
    }

    SetUserAction(primaryGenerator);

    // Run action
    RunAction* runAction = new RunAction();
    SetUserAction(runAction);

    // Event action
    EventAction* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    // Stepping action
    SteppingAction* steppingAction = new SteppingAction(eventAction);
    SetUserAction(steppingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
