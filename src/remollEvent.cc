#include "remollEvent.hh"
#include <math.h>

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4ParticleTable.hh"

remollEvent *remollEvent::gSingletonEvent = NULL;

remollEvent::remollEvent(){
    Reset();
    //NEW 
    gSingletonEvent = this;
}

remollEvent::~remollEvent(){
}

// NEW mimicking remollBeamTarget.cc 
remollEvent *remollEvent::GetRemollEvent() {
    if( gSingletonEvent == NULL ){
        G4cout << "Error: remollEvent is NULL " << G4endl;
        gSingletonEvent = new remollEvent();
    }
    return gSingletonEvent;
}

//

void remollEvent::ProduceNewParticle( G4ThreeVector pos, G4ThreeVector mom, G4String name ){
    fPartPos.push_back(pos);
//    fPartLastPos.push_back(pos); // NEW Initialize the previous position to be the same as inition pos
//    fPartDeltaE.push_back(0.0);  // NEW Initialize to 0 (make sure the units are working here)
//    fPartDeltaTh.push_back(0.0); // NEW Initialize to 0 (make sure the units are working here)
    fPartMom.push_back(mom);
    fPartRealMom.push_back(mom);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(name);

    fPartType.push_back(particle);

    return;
}


void remollEvent::Reset(){
    fPartPos.clear();
//    fPartLastPos.clear(); // NEW
//    fPartDeltaE.clear(); // NEW
//    fPartDeltaTh.clear(); // NEW
    fPartMom.clear();
    fPartRealMom.clear();
    fPartType.clear();

    fBeamMomentum = G4ThreeVector(-1e9, -1e9, -1e9);
    fVertexPos    = G4ThreeVector(-1e9, -1e9, -1e9);

    fRate  = 0.0/s;
    fEffXs = -1e9*nanobarn;
    fAsym  = -1e9;

    fQ2    = -1e9*GeV*GeV;

    // Only care about for certain processes
    fW2    = -1e9*GeV*GeV;
    fThCoM = -1e9;
}

    // CONSIDER adding a method that undoes previous LastVariables and replaces them with new ones via pop_back() and push_back() 
    /*void remollEvent::UpdateLastParticle( G4ThreeVector lastPos, G4double deltaE, G4double deltaAng ) {
    int len = fPartPos.size();
    G4cout << len << " test2 " << deltaE << G4endl;
    fPartDeltaE[len] = deltaE;
    //fPartDeltaE.pop_back();
    G4cout << " test3 " << G4endl;
    //fPartDeltaE.push_back( deltaE );
    G4cout << " test4 " << deltaAng << G4endl;
    fPartDeltaTh[len] = deltaAng;
    //fPartDeltaTh.pop_back();
    G4cout << " test5 " << G4endl;
    //fPartDeltaTh.push_back( deltaAng );
    G4cout << " test6 " << lastPos << G4endl;
    fPartLastPos[len] = lastPos;
    //fPartLastPos.pop_back();
    G4cout << " test7 " << G4endl;
    //fPartLastPos.push_back( lastPos );
    G4cout << " test8 " << G4endl;
}
*/


void remollEvent::UndoLastParticle(){
    fPartPos.pop_back();
//    fPartLastPos.pop_back(); // NEW
//    fPartDeltaE.pop_back();  // NEW
//    fPartDeltaTh.pop_back(); // NEW
    fPartMom.pop_back();
    fPartRealMom.pop_back();
    fPartType.pop_back();
}

G4bool remollEvent::EventIsSane(){
    // Here we check all the variables and make sure there is nothing 
    // kinematically wrong and there aren't stuff like nans and infs

    unsigned int i;

    if( std::isnan(fEffXs) || std::isinf(fEffXs) || fEffXs < 0.0 ) return false;
    if( std::isnan(fAsym) || std::isinf(fAsym) || fAsym < -1.0 || fAsym > 1.0 ) return false;
    if( std::isnan(fThCoM) || std::isinf(fThCoM) ) return false;
    if( std::isnan(fQ2) || std::isinf(fQ2) ) return false;
    if( std::isnan(fW2) || std::isinf(fW2) ) return false;

    if( fPartPos.size() < 1 && fEffXs > 0.0 ){ 
        return false;
    }

    for( i = 0; i < fPartPos.size(); i++ ){
        if( !fPartType[i] ){ return false; }

        if( std::isnan(fPartPos[i].x()) || std::isinf(fPartPos[i].x()) ) return false;
	      if( std::isnan(fPartPos[i].y()) || std::isinf(fPartPos[i].y()) ) return false;
	      if( std::isnan(fPartPos[i].z()) || std::isinf(fPartPos[i].z()) ) return false;

	
        if( std::isnan(fPartMom[i].x()) || std::isinf(fPartMom[i].x()) ) return false;
        if( std::isnan(fPartMom[i].y()) || std::isinf(fPartMom[i].y()) ) return false;
        if( std::isnan(fPartMom[i].z()) || std::isinf(fPartMom[i].z()) ) return false;
    }

    return true;
}


void remollEvent::Print(){
    G4cout << "Event " << this << " dump" << G4endl;
    G4cout << "\t" << fEffXs/nanobarn << " nb effective cross section " << G4endl;
    G4cout << "\t" << fAsym*1e6 << " ppm asymmetry" << G4endl;
    G4cout << "\t" << "Q2 = " << fQ2/GeV/GeV << " GeV2" << G4endl;
    G4cout << "\t" << "W2 = " << fW2/GeV/GeV << " GeV2" << G4endl;
    G4cout << "\t" << "th_com = " << fThCoM/deg << " deg" << G4endl;

    G4cout << "\t" << fPartPos.size() << " particles generated" << G4endl;

    unsigned int i;

    for( i = 0; i < fPartPos.size(); i++ ){
        if( !fPartType[i] ){
            G4cout << "\tParticle type for " << i << " not defined" << G4endl;
        } else {
            G4cout << "\t" << fPartType[i]->GetParticleName() << ":" << G4endl;
            G4cout << "\t\tat (" << fPartPos[i].x()/m << ", " << fPartPos[i].y()/m << ", " << fPartPos[i].z()/m  << ") m" << G4endl;
            G4cout << "\t\tof (" << fPartMom[i].x()/GeV << ", " << fPartMom[i].y()/GeV << ", " << fPartMom[i].z()/GeV  << ") GeV" << G4endl;
        }
    }
}



