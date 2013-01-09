
// Make this appear first!
#include "G4Timer.hh"

#include "remollRunAction.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "remollIO.hh"

remollRunAction::remollRunAction()
{
  timer = new G4Timer;
}

remollRunAction::~remollRunAction()
{
  delete timer;
}

void remollRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  //  timer->Start();
  fIO->InitializeTree();
}

void remollRunAction::EndOfRunAction(const G4Run* aRun)
{
  timer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent() << G4endl;
  //       << " " << *timer << G4endl;

  fIO->WriteTree();
}

