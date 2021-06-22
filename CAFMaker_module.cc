////////////////////////////////////////////////////////////////////////
//
// \file CAFMaker_module.cc
//
// Chris Marshall's version
// Largely based on historical FDSensOpt/CAFMaker_module.cc
//
///////////////////////////////////////////////////////////////////////

#ifndef CAFMaker_H
#define CAFMaker_H

// Generic C++ includes
#include <iostream>

// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h" 
#include "art/Framework/Principal/Event.h" 
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "art/Framework/Services/Optional/TFileService.h" 

//#include "Utils/AppInit.h"
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "dune/FDSensOpt/FDSensOptData/MVASelectPID.h"
#include "dune/FDSensOpt/FDSensOptData/EnergyRecoOutput.h"
#include "dune/CVN/func/InteractionType.h"
#include "dune/CVN/func/Result.h"
#include "dune/RegCVN/func/RegCVNResult.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "larreco/Calorimetry/CalorimetryAlg.h"

// nutools
#include "nutools/EventGeneratorBase/GENIE/GENIE2ART.h"


// dunerw stuff
#include "systematicstools/interface/ISystProviderTool.hh"
#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"
#include "systematicstools/utility/exceptions.hh"

// root
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

// pdg
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGLibrary.h"

// genie
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepParticle.h"

// edeposit
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"


constexpr int knShifts = 100; // number of shifts
constexpr int kmaxRwgts = 100; // Largest number of reweights in a shift

namespace dunemva {

  class CAFMaker : public art::EDAnalyzer {

    public:

      explicit CAFMaker(fhicl::ParameterSet const& pset);
      virtual ~CAFMaker();
      void beginJob() override;
      void endJob() override;
      void beginSubRun(const art::SubRun& sr) override;
      void endSubRun(const art::SubRun& sr) override;
      void reconfigure(fhicl::ParameterSet const& pset) /*override*/;
      void analyze(art::Event const & evt) override;


    private:
      void resetCAFvars();

      std::string fMVASelectLabel;
      std::string fMVASelectNueLabel;
      std::string fMVASelectNumuLabel;

      std::string fCVNLabel;
      std::string fRegCVNLabel;

      std::string fEnergyRecoNueLabel;
      std::string fEnergyRecoNumuLabel;
      std::string fMVAMethod;

      std::string fHitsModuleLabel;

      float fOscPro;
      double fWeight;
      TTree* fTree;  
      TTree* fMetaTree;
      TTree* fGenieTree;
	TTree* uniqueIDETree;
/*      TTree* IDETree;
      TTree* allIDETree;
      TTree* hitIDETree;
      TTree* plane0IDETree;
	TTree* plane1IDETree;
*/	TTree* plane2IDETree;

      // Get reweight knobs from fhicl file -- no hard-coded shifts
      int fNwgt[knShifts];
      double fCvWgts[knShifts];
      double fWgts[knShifts][kmaxRwgts];
      bool fisWgtSyst[knShifts];

      // CAF variables
      // configuration variables
      int fIsFD, fIsFHC;
      // event accounting
      int fRun, fSubrun, fEvent;
      // Truth information
      int fIsCC, fNuPDG, fNuPDGunosc, fMode, fLepPDG; 
      double fEv, fQ2, fW, fX, fY, fNuMomX, fNuMomY, fNuMomZ, fLepMomX, fLepMomY, fLepMomZ, fLepE, fLepNuAngle;
      // True particle counts
      int nP, nN, nPip, nPim, nPi0, nKp, nKm, nK0, nEM, nOtherHad, nNucleus, nUNKNOWN, PdaughterN;
	//trying particleinventory method to fill the number and energy
      int nP1, nN1, nPip1, nPim1, nPi01, nKp1, nKm1, nK01, nEM1, nOtherHad1, nNucleus1, nUNKNOWN1;
      double eP, eN, ePip, ePipraw, ePim, ePi0, eOther;
      double eP1, eN1, ePip1, ePipraw1, ePim1, ePi01, eOther1;
      double eRecoP, eRecoN, eRecoPip, eRecoPim, eRecoPi0, eRecoOther, eRecoOtherraw;
	double eRecoOtherplane0, eRecoOtherplane1, eRecoOtherplane2;
      double eDepP, eDepN, eDepPip, eDepPim, eDepPi0, eDepOther, eDepMichel, eDepMichelhit;
      double eDepPraw, eDepNraw, eDepPipraw, eDepPimraw, eDepPi0raw, eDepOtherraw; //different reconstruction
	double eDepPplane0, eDepNplane0, eDepPipplane0, eDepPimplane0, eDepPi0plane0, eDepOtherplane0;
	double eDepPplane1, eDepNplane1, eDepPipplane1, eDepPimplane1, eDepPi0plane1, eDepOtherplane1;
	double eDepPplane2, eDepNplane2, eDepPipplane2, eDepPimplane2, eDepPi0plane2, eDepOtherplane2;
      double vtx_x, vtx_y, vtx_z;
	int pipI, pimI;

      //trackid array
      int trackidmichel[10]={0};
      int trackidpion[10]={0};
      int trackidmuon[10]={0};
      int trackidmichel1[10]={0};
      int trackidpion1[10]={0};
      int trackidmuon1[10]={0};
      int trackidphoton[10]={0};
//	double hitIDEs[100000][5]={0};
//	double plane0IDEs[56937][6]={0};
//	double plane1IDEs[57061][6]={0};
//	double plane2IDEs[57370][6]={0};

	struct uniqueIDEs {
 	 double x;
	 double y;
	 double z;
  	 double Energy;
	 double TrueEnergy;
	 int TrackID;
	 int PDGCode;
	 int Plane;
	} uniqueIDEs [170000];

/*
	//IDETree
	int IDEsrn, IDETrackID, IDEPDGCode;
	double IDEEnergy, IDEx, IDEy, IDEz;
	int allIDEsrn, allIDETrackID, allIDEPDGCode;
	double allIDEEnergy, allIDEx, allIDEy, allIDEz;
	int hitIDEsrn, hitIDETrackID, hitIDEPDGCode;
	double hitIDEEnergy, hitIDEx, hitIDEy, hitIDEz;
*/	int plane0IDEsrn, plane0IDETrackID, plane0IDEPDGCode;
	double plane0IDEEnergy, plane0IDEx, plane0IDEy, plane0IDEz;
	int plane1IDEsrn, plane1IDETrackID, plane1IDEPDGCode;
	double plane1IDEEnergy, plane1IDEx, plane1IDEy, plane1IDEz;
	int plane2IDEsrn, plane2IDETrackID, plane2IDEPDGCode, plane2IDEMotherTrackID, plane2IDEMotherPDGCode;
	double plane2IDEEnergy, plane2IDEx, plane2IDEy, plane2IDEz;
	int uniqueIDEsrn, uniqueIDETrackID, uniqueIDEPDGCode, uniqueIDEPlane;
	double uniqueIDEEnergy, uniqueIDETrueEnergy, uniqueIDEx, uniqueIDEy, uniqueIDEz;
	
	double eDecay, eDecay1;
	int pimdecay = 0, pipdecay = 0;
 
     // Reco information
      double fErecoNue;
      double fRecoLepEnNue;
      double fRecoHadEnNue;
      int fRecoMethodNue; // 1 = longest reco track + hadronic, 2 = reco shower with highest charge + hadronic, 3 = all hit charges, -1 = not set
      double fRecoLepAngNue;
      double fErecoNumu; 
      double fRecoLepEnNumu;
      double fRecoHadEnNumu;
      int fRecoMethodNumu; // 1 = longest reco track + hadronic, 2 = reco shower with highest charge + hadronic, 3 = all hit charges, -1 = not set
      double fRecoLepAngNumu;
      int fLongestTrackContNumu; // 1 = contained, 0 = exiting, -1 = not set
      int fTrackMomMethodNumu; // 1 = range, 0 = MCS, -1 = not set

      double fMVAResult;
      double fMVAResultNue;
      double fMVAResultNumu;

      // CVN outputs
      double fCVNResultIsAntineutrino;
      double fCVNResultNue, fCVNResultNumu, fCVNResultNutau, fCVNResultNC; // flavour
      double fCVNResult0Protons, fCVNResult1Protons, fCVNResult2Protons, fCVNResultNProtons; // #protons
      double fCVNResult0Pions, fCVNResult1Pions, fCVNResult2Pions, fCVNResultNPions; // #pions
      double fCVNResult0Pizeros, fCVNResult1Pizeros, fCVNResult2Pizeros, fCVNResultNPizeros; // #pizeros
      double fCVNResult0Neutrons, fCVNResult1Neutrons, fCVNResult2Neutrons, fCVNResultNNeutrons; // #neutrons

      double fRegCVNNueE;

      double meta_pot;
      int meta_run, meta_subrun, meta_version;

      genie::EventRecord* ghep;

      systtools::provider_list_t fSystProviders;

      calo::CalorimetryAlg fCaloAlg;
      double fRecombFactor;

  }; // class CAFMaker


  //------------------------------------------------------------------------------
  CAFMaker::CAFMaker(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset), fCaloAlg(pset.get<fhicl::ParameterSet>("CalorimetryAlg"))
  {
    this->reconfigure(pset);
  }

  dunemva::CAFMaker::~CAFMaker(){}

  void CAFMaker::resetCAFvars()
  {
/*    IDEsrn = -999;
    IDETrackID = -999;
    IDEPDGCode = -999;
    IDEEnergy = -999;
    IDEx = -999;
    IDEy = -999;
    IDEz = -999;
    allIDEsrn = -999;
    allIDETrackID = -999;
    allIDEPDGCode = -999;
    allIDEEnergy = -999;
    allIDEx = -999;
    allIDEy = -999;
    allIDEz = -999;
    hitIDEsrn = -999;
    hitIDETrackID = -999;
    hitIDEPDGCode = -999;
    hitIDEEnergy = -999;
    hitIDEx = -999;
    hitIDEy = -999;
    hitIDEz = -999;
    plane0IDEsrn = -999;
    plane0IDETrackID = -999;
    plane0IDEPDGCode = -999;
    plane0IDEEnergy = -999;
    plane0IDEx = -999;
    plane0IDEy = -999;
    plane0IDEz = -999;
    plane1IDEsrn = -999;
    plane1IDETrackID = -999;
    plane1IDEPDGCode = -999;
    plane1IDEEnergy = -999;
    plane1IDEx = -999;
    plane1IDEy = -999;
    plane1IDEz = -999;
  */
    plane2IDEsrn = -999;
    plane2IDETrackID = -999;
    plane2IDEPDGCode = -999;
    plane2IDEEnergy = -999;
    plane2IDEx = -999;
    plane2IDEy = -999;
    plane2IDEz = -999;
    plane2IDEMotherTrackID = -999;
    plane2IDEMotherPDGCode = -999;

    fIsFD = -999;
    fIsFHC = -999; // ?
    fRun = -999;
    fSubrun = -999;
    fEvent = -999;
    fIsCC = -999;
    fNuPDG = -999;
    fNuPDGunosc = -999;
    fMode = -999;
    fLepPDG = -999;
    fEv = -999;
    fQ2 = -999;
    fW = -999;
    fX = -999;
    fY = -999;
    fNuMomX= -999;
    fNuMomY= -999;
    fNuMomZ= -999;
    fLepMomX= -999;
    fLepMomY= -999;
    fLepMomZ= -999;
    fLepE= -999;
    fLepNuAngle = -999;
    nP= -999;
    nN= -999;
    nPip= -999;
    nPim= -999;
    nPi0= -999;
    nKp= -999;
    nKm= -999;
    nK0= -999;
    nEM= -999;
    nOtherHad= -999;
    nNucleus= -999;
    nUNKNOWN = -999;
    eP= -999;
    eN= -999;
    ePip= -999;
    ePipraw= -999;
    ePim= -999;
    ePi0= -999;
    eOther = -999;
    nP1= -999;
    nN1= -999;
    nPip1= -999;
    nPim1= -999;
    nPi01= -999;
    nKp1= -999;
    nKm1= -999;
    nK01= -999;
    nEM1= -999;
    nOtherHad1= -999;
    nNucleus1= -999;
    nUNKNOWN1 = -999;
    eP1= -999;
    eN1= -999;
    ePip1= -999;
    ePipraw1= -999;
    ePim1= -999;
    ePi01= -999;
    eOther1 = -999;
    eRecoP= -999;
    eRecoN= -999;
    eRecoPip= -999;
    eRecoPim= -999;
    eRecoPi0= -999;
    eRecoOther = -999;
    eRecoOtherraw = -999;
    eRecoOtherplane0 = -999;
    eRecoOtherplane1 = -999;
    eRecoOtherplane2 = -999;
    eDepP= -999;
    eDepN= -999;
    eDepPip= -999;
    eDepPim= -999;
    eDepPi0= -999;
    eDepOther = -999;
    eDepMichel = -999;
    eDepMichelhit = -999;
    eDepPraw= -999;
    eDepNraw= -999;
    eDepPipraw= -999;
    eDepPimraw= -999;
    eDepPi0raw= -999;
    eDepOtherraw = -999;
    eDepPplane0= -999;
    eDepNplane0= -999;
    eDepPipplane0= -999;
    eDepPimplane0= -999;
    eDepPi0plane0= -999;
    eDepOtherplane0 = -999;
    eDepPplane1= -999;
    eDepNplane1= -999;
    eDepPipplane1= -999;
    eDepPimplane1= -999;
    eDepPi0plane1= -999;
    eDepOtherplane1 = -999;
    eDepPplane2= -999;
    eDepNplane2= -999;
    eDepPipplane2= -999;
    eDepPimplane2= -999;
    eDepPi0plane2= -999;
    eDepOtherplane2 = -999;
    vtx_x = -999;
    vtx_y = -999;
    vtx_z = -999;
    PdaughterN = -999;
    pipI = -999;
    pimI = -999;

    fErecoNue= -999;
    fRecoLepEnNue= -999;
    fRecoHadEnNue = -999;
    fRecoMethodNue = -999;
    fRecoLepAngNue = -999;
    fErecoNumu = -999; 
    fRecoLepEnNumu= -999;
    fRecoHadEnNumu = -999;
    fRecoMethodNumu = -999;
    fRecoLepAngNumu = -999;
    fLongestTrackContNumu = -999;
    fTrackMomMethodNumu= -999;
    fMVAResult= -999;
    fMVAResultNue= -999;
    fMVAResultNumu= -999;
    fCVNResultIsAntineutrino= -999;
    fCVNResultNue = -999;
    fCVNResultNumu = -999;
    fCVNResultNutau = -999;
    fCVNResultNC= -999;
    fCVNResult0Protons = -999;
    fCVNResult1Protons = -999;
    fCVNResult2Protons = -999;
    fCVNResultNProtons = -999;
    fCVNResult0Pions = -999;
    fCVNResult1Pions = -999;
    fCVNResult2Pions = -999;
    fCVNResultNPions= -999;
    fCVNResult0Pizeros = -999;
    fCVNResult1Pizeros = -999;
    fCVNResult2Pizeros = -999;
    fCVNResultNPizeros= -999;
    fCVNResult0Neutrons = -999;
    fCVNResult1Neutrons = -999;
    fCVNResult2Neutrons = -999;
    fCVNResultNNeutrons = -999;
    fRegCVNNueE = -999;

    // Reweight variables
    for( int k_it = 0; k_it < knShifts; ++k_it ) {
      fNwgt[k_it] = 7; // CAFAna assumes -3,-2,-1,0,1,2,3
      fCvWgts[k_it] = fisWgtSyst[k_it] ? 1. : 0; // Non weight systs default to 0.
      for( int r_it = 0; r_it < kmaxRwgts; ++r_it ) {
        fWgts[k_it][r_it] = fisWgtSyst[r_it] ? 1. : 0;
      }
    }
  }

  //------------------------------------------------------------------------------
  void CAFMaker::reconfigure(fhicl::ParameterSet const& pset) 
  {

    fMVASelectLabel = pset.get<std::string>("MVASelectLabel");
    fMVASelectNueLabel = pset.get<std::string>("MVASelectNueLabel");
    fMVASelectNumuLabel = pset.get<std::string>("MVASelectNumuLabel");
    fCVNLabel = pset.get<std::string>("CVNLabel");
    fRegCVNLabel = pset.get<std::string>("RegCVNLabel");

    fEnergyRecoNueLabel = pset.get<std::string>("EnergyRecoNueLabel");
    fEnergyRecoNumuLabel = pset.get<std::string>("EnergyRecoNumuLabel");

    fHitsModuleLabel = pset.get<std::string>("HitsModuleLabel");

    // Get DUNErw stuff from its fhicl, which should be included on the CAFMaker config file
    //if( !pset.has_key("generated_systematic_provider_configuration") ) {
    //  std::cout << "[ERROR]: Could not find producer key: "
    //               "\"generated_systematic_provider_configuration\". This should "
    //               "contain a list of configured systematic providers generated by "
    //               "GenerateSystProviderConfig." << std::endl;
    //  return;
    //}

    fhicl::ParameterSet syst_provider_config = pset.get<fhicl::ParameterSet>("generated_systematic_provider_configuration");

    fSystProviders = systtools::ConfigureISystProvidersFromParameterHeaders(syst_provider_config);

    fRecombFactor = pset.get<double>("RecombFactor");
  }


  //------------------------------------------------------------------------------
  void CAFMaker::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
/*	
    //tree for missing IDEs
    IDETree = tfs->make<TTree>("IDEs", "IDEs");
    IDETree->Branch("IDEsrn",       &IDEsrn,      "IDEsrn/I");
    IDETree->Branch("Event",       &fEvent,      "Event/I");
    IDETree->Branch("IDETrackID",       &IDETrackID,      "IDETrackID/I");
    IDETree->Branch("IDEPDGCode",       &IDEPDGCode,      "IDEPDGCode/I");
    IDETree->Branch("IDEEnergy",       &IDEEnergy,      "IDEEnergy/D");
    IDETree->Branch("IDEx",       &IDEx,      "IDEx/D");
    IDETree->Branch("IDEy",       &IDEy,      "IDEy/D");
    IDETree->Branch("IDEz",       &IDEz,      "IDEz/D");

   //tree for sim IDEs
    allIDETree = tfs->make<TTree>("allIDEs", "allIDEs");
    allIDETree->Branch("allIDEsrn",       &allIDEsrn,      "allIDEsrn/I");
    allIDETree->Branch("Event",       &fEvent,      "Event/I");
    allIDETree->Branch("allIDETrackID",       &allIDETrackID,      "allIDETrackID/I");
    allIDETree->Branch("allIDEPDGCode",       &allIDEPDGCode,      "allIDEPDGCode/I");
    allIDETree->Branch("allIDEEnergy",       &allIDEEnergy,      "allIDEEnergy/D");
    allIDETree->Branch("allIDEx",       &allIDEx,      "allIDEx/D");
    allIDETree->Branch("allIDEy",       &allIDEy,      "allIDEy/D");
    allIDETree->Branch("allIDEz",       &allIDEz,      "allIDEz/D");

    //tree for hit IDEs
    hitIDETree = tfs->make<TTree>("hitIDEs", "hitIDEs");
    hitIDETree->Branch("hitIDEsrn",       &hitIDEsrn,      "hitIDEsrn/I");
    hitIDETree->Branch("Event",       &fEvent,      "Event/I");
    hitIDETree->Branch("hitIDETrackID",       &hitIDETrackID,      "hitIDETrackID/I");
    hitIDETree->Branch("hitIDEPDGCode",       &hitIDEPDGCode,      "hitIDEPDGCode/I");
    hitIDETree->Branch("hitIDEEnergy",       &hitIDEEnergy,      "hitIDEEnergy/D");
    hitIDETree->Branch("hitIDEx",       &hitIDEx,      "hitIDEx/D");
    hitIDETree->Branch("hitIDEy",       &hitIDEy,      "hitIDEy/D");
    hitIDETree->Branch("hitIDEz",       &hitIDEz,      "hitIDEz/D");

    //tree for plane0 IDEs
    plane0IDETree = tfs->make<TTree>("plane0IDEs", "plane0IDEs");
    plane0IDETree->Branch("plane0IDEsrn",       &plane0IDEsrn,      "plane0IDEsrn/I");
    plane0IDETree->Branch("Event",       &fEvent,      "Event/I");
    plane0IDETree->Branch("plane0IDETrackID",       &plane0IDETrackID,      "plane0IDETrackID/I");
    plane0IDETree->Branch("plane0IDEPDGCode",       &plane0IDEPDGCode,      "plane0IDEPDGCode/I");
    plane0IDETree->Branch("plane0IDEEnergy",       &plane0IDEEnergy,      "plane0IDEEnergy/D");
    plane0IDETree->Branch("plane0IDEx",       &plane0IDEx,      "plane0IDEx/D");
    plane0IDETree->Branch("plane0IDEy",       &plane0IDEy,      "plane0IDEy/D");
    plane0IDETree->Branch("plane0IDEz",       &plane0IDEz,      "plane0IDEz/D");

    //tree for plane1 IDEs
    plane1IDETree = tfs->make<TTree>("plane1IDEs", "plane1IDEs");
    plane1IDETree->Branch("plane1IDEsrn",       &plane1IDEsrn,      "plane1IDEsrn/I");
    plane1IDETree->Branch("Event",       &fEvent,      "Event/I");
    plane1IDETree->Branch("plane1IDETrackID",       &plane1IDETrackID,      "plane1IDETrackID/I");
    plane1IDETree->Branch("plane1IDEPDGCode",       &plane1IDEPDGCode,      "plane1IDEPDGCode/I");
    plane1IDETree->Branch("plane1IDEEnergy",       &plane1IDEEnergy,      "plane1IDEEnergy/D");
    plane1IDETree->Branch("plane1IDEx",       &plane1IDEx,      "plane1IDEx/D");
    plane1IDETree->Branch("plane1IDEy",       &plane1IDEy,      "plane1IDEy/D");
    plane1IDETree->Branch("plane1IDEz",       &plane1IDEz,      "plane1IDEz/D");
*/


   //tree for plane2 IDEs
    plane2IDETree = tfs->make<TTree>("plane2IDEs", "plane2IDEs");
    plane2IDETree->Branch("plane2IDEsrn",       &plane2IDEsrn,      "plane2IDEsrn/I");
    plane2IDETree->Branch("Event",       &fEvent,      "Event/I");
    plane2IDETree->Branch("plane2IDETrackID",       &plane2IDETrackID,      "plane2IDETrackID/I");
    plane2IDETree->Branch("plane2IDEPDGCode",       &plane2IDEPDGCode,      "plane2IDEPDGCode/I");
    plane2IDETree->Branch("plane2IDEEnergy",       &plane2IDEEnergy,      "plane2IDEEnergy/D");
    plane2IDETree->Branch("plane2IDEx",       &plane2IDEx,      "plane2IDEx/D");
    plane2IDETree->Branch("plane2IDEy",       &plane2IDEy,      "plane2IDEy/D");
    plane2IDETree->Branch("plane2IDEz",       &plane2IDEz,      "plane2IDEz/D");
    plane2IDETree->Branch("plane2IDEMotherTrackID",       &plane2IDEMotherTrackID,      "plane2IDEMotherTrackID/I");
    plane2IDETree->Branch("plane2IDEMotherPDGCode",       &plane2IDEMotherPDGCode,      "plane2IDEMotherPDGCode/I");


/*
    //tree for unique IDEs
    uniqueIDETree = tfs->make<TTree>("uniqueIDEs", "uniqueIDEs");
    uniqueIDETree->Branch("uniqueIDEsrn",       &uniqueIDEsrn,      "uniqueIDEsrn/I");
    uniqueIDETree->Branch("Event",       &fEvent,      "Event/I");
    uniqueIDETree->Branch("uniqueIDETrackID",       &uniqueIDETrackID,      "uniqueIDETrackID/I");
    uniqueIDETree->Branch("uniqueIDEPDGCode",       &uniqueIDEPDGCode,      "uniqueIDEPDGCode/I");
    uniqueIDETree->Branch("uniqueIDEEnergy",       &uniqueIDEEnergy,      "uniqueIDEEnergy/D");
    uniqueIDETree->Branch("uniqueIDETrueEnergy",       &uniqueIDETrueEnergy,      "uniqueIDETrueEnergy/D");
    uniqueIDETree->Branch("uniqueIDEx",       &uniqueIDEx,      "uniqueIDEx/D");
    uniqueIDETree->Branch("uniqueIDEy",       &uniqueIDEy,      "uniqueIDEy/D");
    uniqueIDETree->Branch("uniqueIDEz",       &uniqueIDEz,      "uniqueIDEz/D"); 
    uniqueIDETree->Branch("uniqueIDEPlane",       &uniqueIDEPlane,      "uniqueIDEPlane/I");   
*/
    fTree = tfs->make<TTree>("caf", "caf");
    fMetaTree = tfs->make<TTree>("meta", "meta");
    fGenieTree = tfs->make<TTree>("genieEvt", "genieEvt");

    ghep = NULL;

    // book-keeping
    fTree->Branch("run",         &fRun,        "run/I");
    fTree->Branch("subrun",      &fSubrun,     "subrun/I");
    fTree->Branch("event",       &fEvent,      "event/I");
    fTree->Branch("isFD",        &fIsFD,       "isFD/I");
    fTree->Branch("isFHC",       &fIsFHC,      "isFHC/I");
    fTree->Branch("isCC",        &fIsCC,       "isCC/I");

    // true interaction quantities
    fTree->Branch("nuPDG",        &fNuPDG,        "nuPDG/I");
    fTree->Branch("nuPDGunosc",   &fNuPDGunosc,   "nuPDGunosc/I");
    fTree->Branch("NuMomX",       &fNuMomX,       "NuMomX/D");
    fTree->Branch("NuMomY",       &fNuMomY,       "NuMomY/D");
    fTree->Branch("NuMomZ",       &fNuMomZ,       "NuMomZ/D");
    fTree->Branch("Ev",           &fEv,           "Ev/D");
    fTree->Branch("mode",         &fMode,         "mode/I");
    fTree->Branch("LepPDG",       &fLepPDG,       "LepPDG/I");
    fTree->Branch("LepMomX",      &fLepMomX,      "LepMomX/D");
    fTree->Branch("LepMomY",      &fLepMomY,      "LepMomY/D");
    fTree->Branch("LepMomZ",      &fLepMomZ,      "LepMomZ/D");
    fTree->Branch("LepE",         &fLepE,         "LepE/D");
    fTree->Branch("LepNuAngle",   &fLepNuAngle,   "LepNuAngle/D");
    fTree->Branch("Q2",           &fQ2,           "Q2/D");
    fTree->Branch("W",            &fW,            "W/D");
    fTree->Branch("X",            &fX,            "X/D");
    fTree->Branch("Y",            &fY,            "Y/D");

    // FS particle counts
    fTree->Branch("nP",        &nP,         "nP/I");
    fTree->Branch("nN",        &nN,         "nN/I");
    fTree->Branch("nipip",     &nPip,       "nipip/I");
    fTree->Branch("nipim",     &nPim,       "nipim/I");
    fTree->Branch("nipi0",     &nPi0,       "nipi0/I");
    fTree->Branch("nikp",      &nKp,        "nikp/I");
    fTree->Branch("nikm",      &nKm,        "nikm/I");
    fTree->Branch("nik0",      &nK0,        "nik0/I");
    fTree->Branch("niem",      &nEM,        "niem/I");
    fTree->Branch("niother",   &nOtherHad,  "niother/I");
    fTree->Branch("nNucleus",  &nNucleus,   "nNucleus/I");
    fTree->Branch("nUNKNOWN",  &nUNKNOWN,   "nUNKNOWN/I");
    fTree->Branch("eP",        &eP,         "eP/D");
    fTree->Branch("eN",        &eN,         "eN/D");
    fTree->Branch("ePip",      &ePip,       "ePip/D");
    fTree->Branch("ePipraw",      &ePipraw,       "ePipraw/D");
    fTree->Branch("ePim",      &ePim,       "ePim/D");
    fTree->Branch("ePi0",      &ePi0,       "ePi0/D");
    fTree->Branch("eOther",    &eOther,     "eOther/D");
    fTree->Branch("nP1",        &nP1,         "nP1/I");
    fTree->Branch("nN1",        &nN1,         "nN1/I");
    fTree->Branch("nipip1",     &nPip1,       "nipip1/I");
    fTree->Branch("nipim1",     &nPim1,       "nipim1/I");
    fTree->Branch("nipi01",     &nPi01,       "nipi01/I");
    fTree->Branch("nikp1",      &nKp1,        "nikp1/I");
    fTree->Branch("nikm1",      &nKm1,        "nikm1/I");
    fTree->Branch("nik01",      &nK01,        "nik01/I");
    fTree->Branch("niem1",      &nEM1,        "niem1/I");
    fTree->Branch("niother1",   &nOtherHad1,  "niother1/I");
    fTree->Branch("nNucleus1",  &nNucleus1,   "nNucleus1/I");
    fTree->Branch("nUNKNOWN1",  &nUNKNOWN1,   "nUNKNOWN1/I");
    fTree->Branch("eP1",        &eP1,         "eP1/D");
    fTree->Branch("eN1",        &eN1,         "eN1/D");
    fTree->Branch("ePip1",      &ePip1,       "ePip1/D");
    fTree->Branch("ePipraw1",      &ePipraw1,       "ePipraw1/D");
    fTree->Branch("ePim1",      &ePim1,       "ePim1/D");
    fTree->Branch("ePi01",      &ePi01,       "ePi01/D");
    fTree->Branch("eOther1",    &eOther1,     "eOther1/D");
    fTree->Branch("eRecoP",        &eRecoP,         "eRecoP/D");
    fTree->Branch("eRecoN",        &eRecoN,         "eRecoN/D");
    fTree->Branch("eRecoPip",      &eRecoPip,       "eRecoPip/D");
    fTree->Branch("eRecoPim",      &eRecoPim,       "eRecoPim/D");
    fTree->Branch("eRecoPi0",      &eRecoPi0,       "eRecoPi0/D");
    fTree->Branch("eRecoOther",    &eRecoOther,     "eRecoOther/D");
    fTree->Branch("eRecoOtherraw",    &eRecoOtherraw,     "eRecoOtherraw/D");
    fTree->Branch("eRecoOtherplane0",    &eRecoOtherplane0,     "eRecoOtherplane0/D");
    fTree->Branch("eRecoOtherplane1",    &eRecoOtherplane1,     "eRecoOtherplane1/D");
    fTree->Branch("eRecoOtherplane2",    &eRecoOtherplane2,     "eRecoOtherplane2/D");
    fTree->Branch("eDepP",        &eDepP,         "eDepP/D");
    fTree->Branch("eDepN",        &eDepN,         "eDepN/D");
    fTree->Branch("eDepPip",      &eDepPip,       "eDepPip/D");
    fTree->Branch("eDepPim",      &eDepPim,       "eDepPim/D");
    fTree->Branch("eDepPi0",      &eDepPi0,       "eDepPi0/D");
    fTree->Branch("eDepOther",    &eDepOther,     "eDepOther/D");
    fTree->Branch("eDepMichel",        &eDepMichel,         "eDepMichel/D");
    fTree->Branch("eDepMichelhit",        &eDepMichelhit,         "eDepMichelhit/D");
    fTree->Branch("eDepPraw",        &eDepPraw,         "eDepPraw/D");
    fTree->Branch("eDepNraw",        &eDepNraw,         "eDepNraw/D");
    fTree->Branch("eDepPipraw",      &eDepPipraw,       "eDepPipraw/D");
    fTree->Branch("eDepPimraw",      &eDepPimraw,       "eDepPimraw/D");
    fTree->Branch("eDepPi0raw",      &eDepPi0raw,       "eDepPi0raw/D");
    fTree->Branch("eDepOtherraw",    &eDepOtherraw,     "eDepOtherraw/D");
    fTree->Branch("eDepPplane0",        &eDepPplane0,         "eDepPplane0/D");
    fTree->Branch("eDepNplane0",        &eDepNplane0,         "eDepNplane0/D");
    fTree->Branch("eDepPipplane0",      &eDepPipplane0,       "eDepPipplane0/D");
    fTree->Branch("eDepPimplane0",      &eDepPimplane0,       "eDepPimplane0/D");
    fTree->Branch("eDepPi0plane0",      &eDepPi0plane0,       "eDepPi0plane0/D");
    fTree->Branch("eDepOtherplane0",    &eDepOtherplane0,     "eDepOtherplane0/D");
    fTree->Branch("eDepPplane1",        &eDepPplane1,         "eDepPplane1/D");
    fTree->Branch("eDepNplane1",        &eDepNplane1,         "eDepNplane1/D");
    fTree->Branch("eDepPipplane1",      &eDepPipplane1,       "eDepPipplane1/D");
    fTree->Branch("eDepPimplane1",      &eDepPimplane1,       "eDepPimplane1/D");
    fTree->Branch("eDepPi0plane1",      &eDepPi0plane1,       "eDepPi0plane1/D");
    fTree->Branch("eDepOtherplane1",    &eDepOtherplane1,     "eDepOtherplane1/D");
    fTree->Branch("eDepPplane2",        &eDepPplane2,         "eDepPplane2/D");
    fTree->Branch("eDepNplane2",        &eDepNplane2,         "eDepNplane2/D");
    fTree->Branch("eDepPipplane2",      &eDepPipplane2,       "eDepPipplane2/D");
    fTree->Branch("eDepPimplane2",      &eDepPimplane2,       "eDepPimplane2/D");
    fTree->Branch("eDepPi0plane2",      &eDepPi0plane2,       "eDepPi0plane2/D");
    fTree->Branch("eDepOtherplane2",    &eDepOtherplane2,     "eDepOtherplane2/D");

	// Number of Neutron daughters for the proton
	fTree->Branch("PdaughterN",        &PdaughterN,         "PdaughterN/I");

	//Number of hadronic intercations in pions
	fTree->Branch("pipI",        &pipI,         "pipI/I");
	fTree->Branch("pimI",        &pimI,         "pimI/I");

    // vertex position
    fTree->Branch("vtx_x",   &vtx_x,    "vtx_x/D");
    fTree->Branch("vtx_y",   &vtx_y,    "vtx_y/D");
    fTree->Branch("vtx_z",   &vtx_z,    "vtx_z/D");

    // Reco variables
    fTree->Branch("mvaresult",   &fMVAResult,  "mvaresult/D");
    fTree->Branch("mvanue",      &fMVAResultNue,  "mvanue/D");
    fTree->Branch("mvanumu",     &fMVAResultNumu, "mvanumu/D");

    fTree->Branch("cvnisantineutrino", &fCVNResultIsAntineutrino, "cvnisantineutrino/D");
    fTree->Branch("cvnnue",            &fCVNResultNue,            "cvnnue/D");
    fTree->Branch("cvnnumu",           &fCVNResultNumu,           "cvnnumu/D");
    fTree->Branch("cvnnutau",          &fCVNResultNutau,          "cvnnutau/D");
    fTree->Branch("cvnnc",             &fCVNResultNC,             "cvnnc/D");
    fTree->Branch("cvn0protons",       &fCVNResult0Protons,       "cvn0protons/D");
    fTree->Branch("cvn1protons",       &fCVNResult1Protons,       "cvn1protons/D");
    fTree->Branch("cvn2protons",       &fCVNResult2Protons,       "cvn2protons/D");
    fTree->Branch("cvnNprotons",       &fCVNResultNProtons,       "cvnNprotons/D");
    fTree->Branch("cvn0pions",         &fCVNResult0Pions,         "cvn0pions/D");
    fTree->Branch("cvn1pions",         &fCVNResult1Pions,         "cvn1pions/D");
    fTree->Branch("cvn2pions",         &fCVNResult2Pions,         "cvn2pions/D");
    fTree->Branch("cvnNpions",         &fCVNResultNPions,         "cvnNpions/D");
    fTree->Branch("cvn0pizeros",       &fCVNResult0Pizeros,       "cvn0pizeros/D");
    fTree->Branch("cvn1pizeros",       &fCVNResult1Pizeros,       "cvn1pizeros/D");
    fTree->Branch("cvn2pizeros",       &fCVNResult2Pizeros,       "cvn2pizeros/D");
    fTree->Branch("cvnNpizeros",       &fCVNResultNPizeros,       "cvnNpizeros/D");
    fTree->Branch("cvn0neutrons",      &fCVNResult0Neutrons,      "cvn0neutrons/D");
    fTree->Branch("cvn1neutrons",      &fCVNResult1Neutrons,      "cvn1neutrons/D");
    fTree->Branch("cvn2neutrons",      &fCVNResult2Neutrons,      "cvn2neutrons/D");
    fTree->Branch("cvnNneutrons",      &fCVNResultNNeutrons,      "cvnNneutrons/D");

    fTree->Branch("RegCVNNueE",  &fRegCVNNueE,   "RegCVNNueE/D");
    fTree->Branch("weight",      &fWeight,     "weight/D");
    fTree->Branch("oscpro",      &fOscPro,     "oscpro/F");

    fTree->Branch("Ev_reco_nue",      &fErecoNue,        "Ev_reco_nue/D");
    fTree->Branch("RecoLepEnNue",     &fRecoLepEnNue,    "RecoLepEnNue/D");
    fTree->Branch("RecoHadEnNue",     &fRecoHadEnNue,    "RecoHadEnNue/D");
    fTree->Branch("RecoMethodNue",    &fRecoMethodNue,   "RecoMethodNue/I");
    fTree->Branch("RecoLepAngNue",    &fRecoLepAngNue,   "RecoLepAngNue/D");
    fTree->Branch("Ev_reco_numu",     &fErecoNumu,       "Ev_reco_numu/D");
    fTree->Branch("RecoLepEnNumu",    &fRecoLepEnNumu,   "RecoLepEnNumu/D");
    fTree->Branch("RecoHadEnNumu",    &fRecoHadEnNumu,   "RecoHadEnNumu/D");
    fTree->Branch("RecoMethodNumu",   &fRecoMethodNumu,  "RecoMethodNumu/I");
    fTree->Branch("RecoLepAngNumu",    &fRecoLepAngNumu,   "RecoLepAngNumu/D");
    fTree->Branch("LongestTrackContNumu",  &fLongestTrackContNumu, "LongestTrackContNumu/I");
    fTree->Branch("TrackMomMethodNumu",    &fTrackMomMethodNumu,   "TrackMomMethodNumu/I");

    fMetaTree->Branch("pot", &meta_pot, "pot/D");
    fMetaTree->Branch("run", &meta_run, "run/I");
    fMetaTree->Branch("subrun", &meta_subrun, "subrun/I");
    fMetaTree->Branch("version", &meta_version, "version/I");

    fGenieTree->Branch( "genie_record", &ghep );

    std::fill_n(fisWgtSyst, knShifts, true);
    // make DUNErw variables
    for( auto &sp : fSystProviders ) {
      systtools::SystMetaData metaData = sp->GetSystMetaData();
      for( systtools::SystMetaData::iterator itMeta = metaData.begin(); itMeta != metaData.end(); ++itMeta ) {      
        systtools::SystParamHeader head = *itMeta;
        std::string name = head.prettyName;
        unsigned int parId = head.systParamId;
        fisWgtSyst[parId] = head.isWeightSystematicVariation;
        std::string wgt_var = fisWgtSyst[parId] ? "wgt" : "var";
        std::cout << "Adding reweight branch " << parId << " for " << name << " with " << head.paramVariations.size() << " shifts" << std::endl;
        fTree->Branch( Form("%s_nshifts", name.c_str()), &fNwgt[parId], Form("%s_nshifts/I", name.c_str()) );
        fTree->Branch( Form("%s_cv%s", name.c_str(),wgt_var.c_str()), &fCvWgts[parId], Form("%s_cv%s/D", name.c_str(),wgt_var.c_str()) );
        fTree->Branch( Form("%s_%s", wgt_var.c_str(),name.c_str()), fWgts[parId], Form("%s_%s[%s_nshifts]/D", wgt_var.c_str(),name.c_str(), name.c_str()) );
      }
    }

    // initialize weight variables -- some won't ever be set
    for( int i = 0; i < knShifts; ++i ) {
      fNwgt[i] = 7; // CAFAna assumes -3,-2,-1,0,1,2,3
      fCvWgts[i] = fisWgtSyst[i] ? 1. : 0; // Non weight systs default to 0.
      for( int j = 0; j < kmaxRwgts; ++j ) {
        fWgts[i][j] = fisWgtSyst[i] ? 1. : 0;
      }
    }

    meta_pot = 0.;
    meta_version = 4;
  }

  //------------------------------------------------------------------------------
  void CAFMaker::beginSubRun(const art::SubRun& sr)
  {
    art::Handle< sumdata::POTSummary > pots;
    if( sr.getByLabel("generator",pots) ) meta_pot += pots->totpot;
  }

  //------------------------------------------------------------------------------
  void CAFMaker::analyze(art::Event const & evt)
  {
    resetCAFvars();

    art::Handle<dunemva::MVASelectPID> pidin;
    evt.getByLabel(fMVASelectLabel, pidin);

    art::Handle<dunemva::MVASelectPID> pidinnue;
    evt.getByLabel(fMVASelectNueLabel, pidinnue);

    art::Handle<dunemva::MVASelectPID> pidinnumu;
    evt.getByLabel(fMVASelectNumuLabel, pidinnumu);

    art::Handle<std::vector<cvn::Result>> cvnin;
    evt.getByLabel(fCVNLabel, "cvnresult", cvnin);

    art::Handle<std::vector<cvn::RegCVNResult>> regcvnin;
    evt.getByLabel(fRegCVNLabel, "regcvnresult", regcvnin);

    art::Handle<dune::EnergyRecoOutput> ereconuein;
    evt.getByLabel(fEnergyRecoNueLabel, ereconuein);

    art::Handle<dune::EnergyRecoOutput> ereconumuin;
    evt.getByLabel(fEnergyRecoNumuLabel, ereconumuin);

    art::ServiceHandle<cheat::ParticleInventoryService> pi_serv;

    fRun = evt.id().run();
    fSubrun = evt.id().subRun();
    fEvent = evt.id().event();
    meta_run = fRun;
    meta_subrun = fSubrun;

    if( !pidin.failedToGet() ) {
      fMVAResult = pidin->pid;
    }

    if( !ereconuein.failedToGet() ) {
      //Fill energy reconstruction
      TVector3 pe( ereconuein->fLepLorentzVector.Px(), ereconuein->fLepLorentzVector.Py(), ereconuein->fLepLorentzVector.Pz() );
      fErecoNue          = ereconuein->fNuLorentzVector.E();
      fRecoLepEnNue      = ereconuein->fLepLorentzVector.E();
      fRecoHadEnNue      = ereconuein->fHadLorentzVector.E();
      fRecoMethodNue     = ereconuein->recoMethodUsed;
      fRecoLepAngNue     = acos( pe.z() / pe.Mag() );
      TVector3 pmu( ereconumuin->fLepLorentzVector.Px(), ereconumuin->fLepLorentzVector.Py(), ereconumuin->fLepLorentzVector.Pz() );
      fErecoNumu         = ereconumuin->fNuLorentzVector.E();
      fRecoLepEnNumu     = ereconumuin->fLepLorentzVector.E();
      fRecoHadEnNumu     = ereconumuin->fHadLorentzVector.E();
      fRecoMethodNumu    = ereconumuin->recoMethodUsed;
      fRecoLepAngNumu    = acos( pmu.z() / pmu.Mag() );
      fLongestTrackContNumu  = ereconumuin->longestTrackContained;
      fTrackMomMethodNumu    = ereconumuin->trackMomMethod;
    }

    if( !pidinnue.failedToGet() ) {
      fMVAResultNue = pidinnue->pid;
    }

    if( !pidinnumu.failedToGet() ) {
      fMVAResultNumu = pidinnumu->pid;
    }

    if( !cvnin.failedToGet() ) {
      //using i = cvn::Interaction;
      //if(cvnin->empty() || (*cvnin)[0].fOutput.size() <= i::kNutauOther){
      if(cvnin->empty()){
        fCVNResultIsAntineutrino = fCVNResultNue = fCVNResultNumu = fCVNResultNutau = fCVNResultNC = \
        fCVNResult0Protons = fCVNResult1Protons = fCVNResult2Protons = fCVNResultNProtons = \
        fCVNResult0Pions = fCVNResult1Pions = fCVNResult2Pions = fCVNResultNPions = \
        fCVNResult0Pizeros = fCVNResult1Pizeros = fCVNResult2Pizeros = fCVNResultNPizeros = \
        fCVNResult0Neutrons = fCVNResult1Neutrons = fCVNResult2Neutrons = fCVNResultNNeutrons = -3;
      }
      else if( std::isnan((*cvnin)[0].GetNueProbability()) ) {
        std::cout << "CVN outputs are not numbers. They really should be numbers" << std::endl;
        fCVNResultIsAntineutrino = fCVNResultNue = fCVNResultNumu = fCVNResultNutau = fCVNResultNC = \
        fCVNResult0Protons = fCVNResult1Protons = fCVNResult2Protons = fCVNResultNProtons = \
        fCVNResult0Pions = fCVNResult1Pions = fCVNResult2Pions = fCVNResultNPions = \
        fCVNResult0Pizeros = fCVNResult1Pizeros = fCVNResult2Pizeros = fCVNResultNPizeros = \
        fCVNResult0Neutrons = fCVNResult1Neutrons = fCVNResult2Neutrons = fCVNResultNNeutrons = -4;
      }
      else{
        //const std::vector<float>& v = (*cvnin)[0].fOutput;
        //fCVNResultNue = v[i::kNueQE] + v[i::kNueRes] + v[i::kNueDIS] + v[i::kNueOther];
        //fCVNResultNumu = v[i::kNumuQE] + v[i::kNumuRes] + v[i::kNumuDIS] + v[i::kNumuOther];
        //fCVNResultNutau = v[i::kNutauQE] + v[i::kNutauRes] + v[i::kNutauDIS] + v[i::kNutauOther]

        fCVNResultIsAntineutrino = (*cvnin)[0].GetIsAntineutrinoProbability();

        fCVNResultNue = (*cvnin)[0].GetNueProbability();
        fCVNResultNumu = (*cvnin)[0].GetNumuProbability();
        fCVNResultNutau = (*cvnin)[0].GetNutauProbability();
        fCVNResultNC = (*cvnin)[0].GetNCProbability();

        fCVNResult0Protons = (*cvnin)[0].Get0protonsProbability();
        fCVNResult1Protons = (*cvnin)[0].Get1protonsProbability();
        fCVNResult2Protons = (*cvnin)[0].Get2protonsProbability();
        fCVNResultNProtons = (*cvnin)[0].GetNprotonsProbability();

        fCVNResult0Pions = (*cvnin)[0].Get0pionsProbability();
        fCVNResult1Pions = (*cvnin)[0].Get1pionsProbability();
        fCVNResult2Pions = (*cvnin)[0].Get2pionsProbability();
        fCVNResultNPions = (*cvnin)[0].GetNpionsProbability();

        fCVNResult0Pizeros = (*cvnin)[0].Get0pizerosProbability();
        fCVNResult1Pizeros = (*cvnin)[0].Get1pizerosProbability();
        fCVNResult2Pizeros = (*cvnin)[0].Get2pizerosProbability();
        fCVNResultNPizeros = (*cvnin)[0].GetNpizerosProbability();

        fCVNResult0Neutrons = (*cvnin)[0].Get0neutronsProbability();
        fCVNResult1Neutrons = (*cvnin)[0].Get1neutronsProbability();
        fCVNResult2Neutrons = (*cvnin)[0].Get2neutronsProbability();
        fCVNResultNNeutrons = (*cvnin)[0].GetNneutronsProbability();
      }
    }

    fRegCVNNueE = -1.;  // initializing
    if(!regcvnin.failedToGet()){
      if (!regcvnin->empty()){
        const std::vector<float>& v = (*regcvnin)[0].fOutput;
        fRegCVNNueE = v[0];
      }
    }

    art::Handle< std::vector<simb::MCTruth> > mct;
    std::vector< art::Ptr<simb::MCTruth> > truth;
    if( evt.getByLabel("generator", mct) )
      art::fill_ptr_vector(truth, mct);
    else
      mf::LogWarning("CAFMaker") << "No MCTruth.";

    art::Handle< std::vector<simb::GTruth> > gt;
    std::vector< art::Ptr<simb::GTruth> > gtruth;
    if( evt.getByLabel("generator", gt) )
      art::fill_ptr_vector(gtruth, gt);
    else
      mf::LogWarning("CAFMaker") << "No GTruth.";

    art::Handle< std::vector<simb::MCFlux> > mcf;
    std::vector< art::Ptr<simb::MCFlux> > flux;
    if( evt.getByLabel("generator", mcf) )
      art::fill_ptr_vector(flux, mcf);
    else
      mf::LogWarning("CAFMaker") << "No MCFlux.";

	
    std::map<int,int> tid_to_primary;
    int xyz=0;
//    double trueE = 0;
    std::map<int,int> primary_pdg; // track ID to PDG code of primary particle
    std::map<int,int> tid_to_mother; // track ID to mother track ID
    for(size_t i=0; i<truth.size(); i++){

      if(i>0){ // multiple interactions per art::Event not currently supported
        mf::LogWarning("CAFMaker") << "Skipping MC truth index " << i;
        continue;
      }
      fIsFD     = 1; // always FD
      fIsFHC    = 999; // don't know how to get this?
      fIsCC     = !(truth[i]->GetNeutrino().CCNC());  // ccnc is 0=CC 1=NC
      fNuPDG    = truth[i]->GetNeutrino().Nu().PdgCode();
      fNuPDGunosc = flux[i]->fntype;
      fMode     = truth[i]->GetNeutrino().Mode(); //0=QE/El, 1=RES, 2=DIS, 3=Coherent production; this is different than mode in ND
      fEv       = truth[i]->GetNeutrino().Nu().E();
      fQ2       = truth[i]->GetNeutrino().QSqr();
      fW        = truth[i]->GetNeutrino().W();
      fX        = truth[i]->GetNeutrino().X();
      fY        = truth[i]->GetNeutrino().Y();
      fNuMomX   = truth[i]->GetNeutrino().Nu().Momentum().X();
      fNuMomY   = truth[i]->GetNeutrino().Nu().Momentum().Y();
      fNuMomZ   = truth[i]->GetNeutrino().Nu().Momentum().Z();

      vtx_x     = truth[i]->GetNeutrino().Nu().Vx();
      vtx_y     = truth[i]->GetNeutrino().Nu().Vy();
      vtx_z     = truth[i]->GetNeutrino().Nu().Vz();

      //Lepton stuff
      fLepPDG     = truth[i]->GetNeutrino().Lepton().PdgCode();
      fLepMomX    = truth[i]->GetNeutrino().Lepton().Momentum().X();
      fLepMomY    = truth[i]->GetNeutrino().Lepton().Momentum().Y();
      fLepMomZ    = truth[i]->GetNeutrino().Lepton().Momentum().Z();
      fLepE       = truth[i]->GetNeutrino().Lepton().Momentum().T();
      fLepNuAngle = truth[i]->GetNeutrino().Nu().Momentum().Vect().Angle(truth[i]->GetNeutrino().Lepton().Momentum().Vect());

      // TODO
      fWeight = 0.;
      fOscPro = 0.;
      //      fOscPro   = fMVAAlg.OscPro(fCCNC,fBeamPdg,fNuPDG,fEtrue);

      nP        = 0;
      nN        = 0;
      nPip      = 0;
      nPim      = 0;
      nPi0      = 0;
      nKp       = 0;
      nKm       = 0;
      nK0       = 0;
      nEM       = 0;
      nOtherHad = 0;
      nNucleus  = 0;
      nUNKNOWN  = 0;

      eP = 0.;
      eN = 0.;
      ePip = 0.;
      ePipraw = 0.;
      ePim = 0.;
      ePi0 = 0.;
      eOther = 0.;

      nP1        = 0;
      nN1        = 0;
      nPip1      = 0;
      nPim1      = 0;
      nPi01      = 0;
      nKp1       = 0;
      nKm1       = 0;
      nK01       = 0;
      nEM1       = 0;
      nOtherHad1 = 0;
      nNucleus1  = 0;
      nUNKNOWN1  = 0;

      eP1 = 0.;
      eN1 = 0.;
      ePip1 = 0.;
      ePipraw1 = 0.;
      ePim1 = 0.;
      ePi01 = 0.;
      eOther1 = 0.;

	    PdaughterN = 0;
	pipI = 0;
	pimI = 0;


	//michel electron and pi0
	const std::vector< const simb::MCParticle* > michele = pi_serv->MCTruthToParticles_Ps(truth[i]);
	const simb::MCParticle* michelid;
	const simb::MCParticle* michelid1;

  	
	for( size_t ee = 0; ee < michele.size(); ++ee ) {
	//	int k=0,j=0;
	//PdaughterN
	if(michele[ee]->PdgCode() == 2112 && michele[ee]->Mother() != 0) {
		michelid = pi_serv->TrackIdToParticle_P(michele[ee]->Mother());
		if(michelid->PdgCode() ==  genie::kPdgProton && michelid->Mother() == 0) xyz++;
	}
	
/*
	if(michele[ee]->PdgCode() == genie::kPdgProton && michele[ee]->Mother() == 0) {
		if ( michele[ee]->NumberDaughters() != 0) {	
		std::cout << "protondaughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "protondaughter1: " << michele[ee]->FirstDaughter() << std::endl;
		tid = michele[ee]->FirstDaughter();
			for (k=0; k < michele[ee]->NumberDaughters(); k++) {
			michelid = pi_serv->TrackIdToParticle_P(tid + k);
			if( michelid->PdgCode() == 2112) xyz++;
			}
			std::cout << "xyz" << xyz << std::endl;
	 }
	}
*/
	// checking the pion interaction
	if ( michele[ee]->PdgCode() == genie::kPdgPiP && michele[ee]->Mother() == 0) {
		if( pipI == 0) {
		if( michele[ee]->EndProcess() == "pi+Inelastic") pipI = 1;
		if( michele[ee]->EndProcess() == "Decay") {
			 pipI = 2;
			 std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
			 std::cout << "fEvent: " << fEvent << std::endl;

		std::cout << "Trackid: " << michele[ee]->TrackId() << std::endl;
		std::cout << "StatusCode: " << michele[ee]->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michele[ee]->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michele[ee]->Mother() << std::endl;
	//	std::cout << "Polarization: " << michele[ee]->Polarization() << std::endl;
		std::cout << "Process: " << michele[ee]->Process() << std::endl;
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michele[ee]->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michele[ee]->Position() << std::endl;
	//	std::cout << "Momentum: " << michele[ee]->Momentum() << std::endl;
		std::cout << "Vx: " << michele[ee]->Vx() << std::endl;
		std::cout << "Vy: " << michele[ee]->Vy() << std::endl;
		std::cout << "Vz: " << michele[ee]->Vz() << std::endl;
		std::cout << "T: " << michele[ee]->T() << std::endl;
	//	std::cout << "EndPosition: " << michele[ee]->EndPosition() << std::endl;
		std::cout << "EndX: " << michele[ee]->EndX() << std::endl;
		std::cout << "EndY: " << michele[ee]->EndY() << std::endl;
		std::cout << "EndZ: " << michele[ee]->EndZ() << std::endl;
		std::cout << "EndT: " << michele[ee]->EndT() << std::endl;
		std::cout << "Px: " << michele[ee]->Px() << std::endl;
		std::cout << "Py: " << michele[ee]->Py() << std::endl;
		std::cout << "Pz: " << michele[ee]->Pz() << std::endl;
		std::cout << "E: " << michele[ee]->E() << std::endl;
		std::cout << "P: " << michele[ee]->P() << std::endl;
		std::cout << "Pt: " << michele[ee]->Pt() << std::endl;
		std::cout << "Mass: " << michele[ee]->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michele[ee]->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michele[ee]->EndPx() << std::endl;
		std::cout << "EndPy: " << michele[ee]->EndPy() << std::endl;
		std::cout << "EndPz: " << michele[ee]->EndPz() << std::endl;
		std::cout << "EndE: " << michele[ee]->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michele[ee]->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michele[ee]->Gvx() << std::endl;
		std::cout << "Gvy: " << michele[ee]->Gvy() << std::endl;
		std::cout << "Gvz: " << michele[ee]->Gvz() << std::endl;
		std::cout << "Gvt: " << michele[ee]->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michele[ee]->FirstDaughter() << std::endl;
		//std::cout << "LastDaughter: " << michele[ee]->LastDaughter() << std::endl;
		std::cout << "Rescatter: " << michele[ee]->Rescatter() << std::endl;
		std::cout << "Trajectory: " << michele[ee]->Trajectory() << std::endl;
		 }
		if( michele[ee]->EndProcess() == "FastScintillation") {
			pipI = 3;
			if( michele[ee]->NumberDaughters() == 2 || michele[ee]->NumberDaughters() == 3) pipI = 2;
			 
			 std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
			 std::cout << "fEvent: " << fEvent << std::endl;

		std::cout << "Trackid: " << michele[ee]->TrackId() << std::endl;
		std::cout << "StatusCode: " << michele[ee]->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michele[ee]->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michele[ee]->Mother() << std::endl;
	//	std::cout << "Polarization: " << michele[ee]->Polarization() << std::endl;
		std::cout << "Process: " << michele[ee]->Process() << std::endl;
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michele[ee]->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michele[ee]->Position() << std::endl;
	//	std::cout << "Momentum: " << michele[ee]->Momentum() << std::endl;
		std::cout << "Vx: " << michele[ee]->Vx() << std::endl;
		std::cout << "Vy: " << michele[ee]->Vy() << std::endl;
		std::cout << "Vz: " << michele[ee]->Vz() << std::endl;
		std::cout << "T: " << michele[ee]->T() << std::endl;
	//	std::cout << "EndPosition: " << michele[ee]->EndPosition() << std::endl;
		std::cout << "EndX: " << michele[ee]->EndX() << std::endl;
		std::cout << "EndY: " << michele[ee]->EndY() << std::endl;
		std::cout << "EndZ: " << michele[ee]->EndZ() << std::endl;
		std::cout << "EndT: " << michele[ee]->EndT() << std::endl;
		std::cout << "Px: " << michele[ee]->Px() << std::endl;
		std::cout << "Py: " << michele[ee]->Py() << std::endl;
		std::cout << "Pz: " << michele[ee]->Pz() << std::endl;
		std::cout << "E: " << michele[ee]->E() << std::endl;
		std::cout << "P: " << michele[ee]->P() << std::endl;
		std::cout << "Pt: " << michele[ee]->Pt() << std::endl;
		std::cout << "Mass: " << michele[ee]->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michele[ee]->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michele[ee]->EndPx() << std::endl;
		std::cout << "EndPy: " << michele[ee]->EndPy() << std::endl;
		std::cout << "EndPz: " << michele[ee]->EndPz() << std::endl;
		std::cout << "EndE: " << michele[ee]->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michele[ee]->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michele[ee]->Gvx() << std::endl;
		std::cout << "Gvy: " << michele[ee]->Gvy() << std::endl;
		std::cout << "Gvz: " << michele[ee]->Gvz() << std::endl;
		std::cout << "Gvt: " << michele[ee]->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michele[ee]->FirstDaughter() << std::endl;
		//std::cout << "LastDaughter: " << michele[ee]->LastDaughter() << std::endl;
		std::cout << "Rescatter: " << michele[ee]->Rescatter() << std::endl;
		std::cout << "Trajectory: " << michele[ee]->Trajectory() << std::endl;
		 }
		}
	  else pipI = 5; // multiple pions in same event
	}

	if ( michele[ee]->PdgCode() == genie::kPdgPiM && michele[ee]->Mother() == 0) {
		if( pimI == 0) {
		if( michele[ee]->EndProcess() == "pi-Inelastic") pimI = 1;
		if( michele[ee]->EndProcess() == "Decay") {
		  pimI = 2;
		  std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
			 std::cout << "fEvent: " << fEvent << std::endl;
		 }
		if( michele[ee]->EndProcess() == "FastScintillation") {
			 pimI = 3;
			 if( michele[ee]->NumberDaughters() == 2 || michele[ee]->NumberDaughters() == 3) pimI = 2;
			 
		 std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
			 std::cout << "fEvent: " << fEvent << std::endl;

		std::cout << "Trackid: " << michele[ee]->TrackId() << std::endl;
		std::cout << "StatusCode: " << michele[ee]->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michele[ee]->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michele[ee]->Mother() << std::endl;
	//	std::cout << "Polarization: " << michele[ee]->Polarization() << std::endl;
		std::cout << "Process: " << michele[ee]->Process() << std::endl;
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michele[ee]->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michele[ee]->Position() << std::endl;
	//	std::cout << "Momentum: " << michele[ee]->Momentum() << std::endl;
		std::cout << "Vx: " << michele[ee]->Vx() << std::endl;
		std::cout << "Vy: " << michele[ee]->Vy() << std::endl;
		std::cout << "Vz: " << michele[ee]->Vz() << std::endl;
		std::cout << "T: " << michele[ee]->T() << std::endl;
	//	std::cout << "EndPosition: " << michele[ee]->EndPosition() << std::endl;
		std::cout << "EndX: " << michele[ee]->EndX() << std::endl;
		std::cout << "EndY: " << michele[ee]->EndY() << std::endl;
		std::cout << "EndZ: " << michele[ee]->EndZ() << std::endl;
		std::cout << "EndT: " << michele[ee]->EndT() << std::endl;
		std::cout << "Px: " << michele[ee]->Px() << std::endl;
		std::cout << "Py: " << michele[ee]->Py() << std::endl;
		std::cout << "Pz: " << michele[ee]->Pz() << std::endl;
		std::cout << "E: " << michele[ee]->E() << std::endl;
		std::cout << "P: " << michele[ee]->P() << std::endl;
		std::cout << "Pt: " << michele[ee]->Pt() << std::endl;
		std::cout << "Mass: " << michele[ee]->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michele[ee]->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michele[ee]->EndPx() << std::endl;
		std::cout << "EndPy: " << michele[ee]->EndPy() << std::endl;
		std::cout << "EndPz: " << michele[ee]->EndPz() << std::endl;
		std::cout << "EndE: " << michele[ee]->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michele[ee]->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michele[ee]->Gvx() << std::endl;
		std::cout << "Gvy: " << michele[ee]->Gvy() << std::endl;
		std::cout << "Gvz: " << michele[ee]->Gvz() << std::endl;
		std::cout << "Gvt: " << michele[ee]->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michele[ee]->FirstDaughter() << std::endl;
		//std::cout << "LastDaughter: " << michele[ee]->LastDaughter() << std::endl;
		std::cout << "Rescatter: " << michele[ee]->Rescatter() << std::endl;
		std::cout << "Trajectory: " << michele[ee]->Trajectory() << std::endl;
		 }
		}
	 else pimI = 5; // multiple pions in same event
	}

	//primary particles info
	if (michele[ee]->Mother() == 0 ) {
			//info
		std::cout << "proton: " << std::endl;
		std::cout << "Trackid: " << michele[ee]->TrackId() << std::endl;
		std::cout << "StatusCode: " << michele[ee]->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michele[ee]->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michele[ee]->Mother() << std::endl;
	//	std::cout << "Polarization: " << michele[ee]->Polarization() << std::endl;
		std::cout << "Process: " << michele[ee]->Process() << std::endl;
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michele[ee]->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michele[ee]->Position() << std::endl;
	//	std::cout << "Momentum: " << michele[ee]->Momentum() << std::endl;
		std::cout << "Vx: " << michele[ee]->Vx() << std::endl;
		std::cout << "Vy: " << michele[ee]->Vy() << std::endl;
		std::cout << "Vz: " << michele[ee]->Vz() << std::endl;
		std::cout << "T: " << michele[ee]->T() << std::endl;
	//	std::cout << "EndPosition: " << michele[ee]->EndPosition() << std::endl;
		std::cout << "EndX: " << michele[ee]->EndX() << std::endl;
		std::cout << "EndY: " << michele[ee]->EndY() << std::endl;
		std::cout << "EndZ: " << michele[ee]->EndZ() << std::endl;
		std::cout << "EndT: " << michele[ee]->EndT() << std::endl;
		std::cout << "Px: " << michele[ee]->Px() << std::endl;
		std::cout << "Py: " << michele[ee]->Py() << std::endl;
		std::cout << "Pz: " << michele[ee]->Pz() << std::endl;
		std::cout << "E: " << michele[ee]->E() << std::endl;
		std::cout << "P: " << michele[ee]->P() << std::endl;
		std::cout << "Pt: " << michele[ee]->Pt() << std::endl;
		std::cout << "Mass: " << michele[ee]->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michele[ee]->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michele[ee]->EndPx() << std::endl;
		std::cout << "EndPy: " << michele[ee]->EndPy() << std::endl;
		std::cout << "EndPz: " << michele[ee]->EndPz() << std::endl;
		std::cout << "EndE: " << michele[ee]->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michele[ee]->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michele[ee]->Gvx() << std::endl;
		std::cout << "Gvy: " << michele[ee]->Gvy() << std::endl;
		std::cout << "Gvz: " << michele[ee]->Gvz() << std::endl;
		std::cout << "Gvt: " << michele[ee]->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michele[ee]->FirstDaughter() << std::endl;
		//std::cout << "LastDaughter: " << michele[ee]->LastDaughter() << std::endl;
		std::cout << "Rescatter: " << michele[ee]->Rescatter() << std::endl;
		std::cout << "Trajectory: " << michele[ee]->Trajectory() << std::endl;
		std::cout << "Weight: " << michele[ee]->Weight() << std::endl;
		}

/*
	//particles info
	if (michele[ee]->Mother() == 3403) {
		michelid = pi_serv->TrackIdToParticle_P(michele[ee]->Mother());
//		if(michelid->PdgCode() == 2112) trueE += michelid->E() - michelid->Mass();
			//info
		std::cout << "proton: " << std::endl;
		std::cout << "Trackid: " << michele[ee]->TrackId() << std::endl;
		std::cout << "StatusCode: " << michele[ee]->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michele[ee]->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michele[ee]->Mother() << std::endl;
	//	std::cout << "Polarization: " << michele[ee]->Polarization() << std::endl;
		std::cout << "Process: " << michele[ee]->Process() << std::endl;
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michele[ee]->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michele[ee]->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michele[ee]->Position() << std::endl;
	//	std::cout << "Momentum: " << michele[ee]->Momentum() << std::endl;
		std::cout << "Vx: " << michele[ee]->Vx() << std::endl;
		std::cout << "Vy: " << michele[ee]->Vy() << std::endl;
		std::cout << "Vz: " << michele[ee]->Vz() << std::endl;
		std::cout << "T: " << michele[ee]->T() << std::endl;
	//	std::cout << "EndPosition: " << michele[ee]->EndPosition() << std::endl;
		std::cout << "EndX: " << michele[ee]->EndX() << std::endl;
		std::cout << "EndY: " << michele[ee]->EndY() << std::endl;
		std::cout << "EndZ: " << michele[ee]->EndZ() << std::endl;
		std::cout << "EndT: " << michele[ee]->EndT() << std::endl;
		std::cout << "Px: " << michele[ee]->Px() << std::endl;
		std::cout << "Py: " << michele[ee]->Py() << std::endl;
		std::cout << "Pz: " << michele[ee]->Pz() << std::endl;
		std::cout << "E: " << michele[ee]->E() << std::endl;
		std::cout << "P: " << michele[ee]->P() << std::endl;
		std::cout << "Pt: " << michele[ee]->Pt() << std::endl;
		std::cout << "Mass: " << michele[ee]->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michele[ee]->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michele[ee]->EndPx() << std::endl;
		std::cout << "EndPy: " << michele[ee]->EndPy() << std::endl;
		std::cout << "EndPz: " << michele[ee]->EndPz() << std::endl;
		std::cout << "EndE: " << michele[ee]->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michele[ee]->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michele[ee]->Gvx() << std::endl;
		std::cout << "Gvy: " << michele[ee]->Gvy() << std::endl;
		std::cout << "Gvz: " << michele[ee]->Gvz() << std::endl;
		std::cout << "Gvt: " << michele[ee]->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michele[ee]->FirstDaughter() << std::endl;
		//std::cout << "LastDaughter: " << michele[ee]->LastDaughter() << std::endl;
		std::cout << "Rescatter: " << michele[ee]->Rescatter() << std::endl;
		std::cout << "Trajectory: " << michele[ee]->Trajectory() << std::endl;
		std::cout << "Weight: " << michele[ee]->Weight() << std::endl;
		}

*/
	if( michele[ee]->PdgCode() == -11 && michele[ee]->Mother() != 0) {
		michelid = pi_serv->TrackIdToParticle_P(michele[ee]->Mother());
		if( michelid->PdgCode() == -13 && michelid->Mother() != 0) {
			michelid1 = pi_serv->TrackIdToParticle_P(michelid->Mother());
			if( michelid1->PdgCode() == genie::kPdgPiP && michelid1->Mother() == 0) {
				trackidmichel[pipdecay] = michele[ee]->TrackId();
					trackidpion[pipdecay] = michelid1->TrackId();
					trackidmuon[pipdecay] = michelid->TrackId();
					std::cout << "trackidpion:" << michele[ee]->TrackId() << std::endl;
					eDepMichel = michelid->E();
					pipdecay++;
			}
		}
	}

	if( michele[ee]->PdgCode() == 11 && michele[ee]->Mother() != 0) {
		michelid = pi_serv->TrackIdToParticle_P(michele[ee]->Mother());
		if( michelid->PdgCode() == 13 && michelid->Mother() != 0) {
			michelid1 = pi_serv->TrackIdToParticle_P(michelid->Mother());
			if( michelid1->PdgCode() == genie::kPdgPiM  && michelid1->Mother() == 0) {
				std::cout << "check" << std::endl;
					trackidmichel1[pimdecay] = michele[ee]->TrackId();
					trackidpion1[pimdecay] = michelid1->TrackId();
					trackidmuon1[pimdecay] = michelid->TrackId();
					std::cout << "trackidpion:" << michele[ee]->TrackId() << std::endl;
					eDepMichel = michelid->E();
					pimdecay++;
			}
		}
	}

 }

	
//	std::cout << "numberP:" << xyz << std::endl;
	    PdaughterN = xyz;



    
      for( int p = 0; p < truth[i]->NParticles(); p++ ) {
        if( truth[i]->GetParticle(p).StatusCode() == genie::kIStStableFinalState ) {

          int pdg = truth[i]->GetParticle(p).PdgCode();
          double ke = truth[i]->GetParticle(p).E() - truth[i]->GetParticle(p).Mass();
	  
          if     ( pdg == genie::kPdgProton ) {
            nP++;
            eP += ke;
          } else if( pdg == genie::kPdgNeutron ) {
            nN++;
            eN += ke;
          } else if( pdg == genie::kPdgPiP ) {
            nPip++;
	    ePipraw += ke;
	    ePip += ke;
		
          } else if( pdg == genie::kPdgPiM ) {
            nPim++;
	    ePim += ke;
          } else if( pdg == genie::kPdgPi0 ) {
            nPi0++;
            ePi0 += ke;
          } else if( pdg == genie::kPdgKP ) {
            nKp++;
            eOther += ke;
          } else if( pdg == genie::kPdgKM ) {
            nKm++;
            eOther += ke;
          } else if( pdg == genie::kPdgK0 || pdg == genie::kPdgAntiK0 || pdg == genie::kPdgK0L || pdg == genie::kPdgK0S ) {
            nK0++;
            eOther += ke;
          } else if( pdg == genie::kPdgGamma ) {
            nEM++;
            eOther += ke;
          } else if( genie::pdg::IsHadron(pdg) ) {
            nOtherHad++; // charm mesons, strange and charm baryons, antibaryons, etc.
            eOther += ke;
          } else if( genie::pdg::IsIon(pdg) ) {
            nNucleus++;
          } else {
            nUNKNOWN++;
          }
	

        }
      }

//trying particleinventory method
	 int flag=0, flag1=0;
//	 double kee, keee;
	for( size_t p = 0; p < michele.size(); p++ ) {
        if( michele[p]->StatusCode() == genie::kIStStableFinalState && michele[p]->Mother() == 0 ) {

          int pdg = michele[p]->PdgCode();
          double ke = (michele[p]->E() - michele[p]->EndE()); 
          if     ( pdg == genie::kPdgProton ) {
            nP1++;
            eP1 += ke;
          } else if( pdg == genie::kPdgNeutron ) {
            nN1++;
            eN1 += ke;
          } else if( pdg == genie::kPdgPiP ) {
            nPip1++;
	    ePipraw1 += ke;
/*		//removing the muon and michel e- energy
		for( b=0; b < 10; b++) {
			if ( michele[p]->TrackId() == trackidpion[b] ) {
				 std::cout << "found" << std::endl;
				 michelid = pi_serv->TrackIdToParticle_P(trackidmuon[b]);
				 kee = michelid->E();
				 std::cout << "muon+ energy:" << kee << std::endl;
				 michelid = pi_serv->TrackIdToParticle_P(trackidmichel[b]);
				 keee = michelid->E();
				 std::cout << "michel e+ energy:" << keee << std::endl;
				 ePip1 += (ke + michele[p]->Mass() - kee - keee);
				 flag++;
			}
		}
*/		if ( flag == 0) ePip1 += ke;		
		
		
          } else if( pdg == genie::kPdgPiM ) {
            nPim1++;
/*		//removing the muon and michel e- energy
		for( b=0; b < 10; b++) {
			if ( michele[p]->TrackId() == trackidpion[b] ) {
				 std::cout << "found" << std::endl;
				 michelid = pi_serv->TrackIdToParticle_P(trackidmuon[b]);
				 kee = michelid->E();
				 std::cout << "muon- energy:" << kee << std::endl;
				 michelid = pi_serv->TrackIdToParticle_P(trackidmichel[b]);
				 keee = michelid->E();
				 std::cout << "michel e- energy:" << keee << std::endl;
				 ePim1 += (ke + michele[p]->Mass() - kee -keee);
				 flag1++;
			}
		}
*/		if ( flag1 == 0) ePim1 += ke;		


          } else if( pdg == genie::kPdgPi0 ) {
            nPi01++;
            ePi01 += ke;
          } else if( pdg == genie::kPdgKP ) {
            nKp1++;
            eOther1 += ke;
          } else if( pdg == genie::kPdgKM ) {
            nKm1++;
            eOther1 += ke;
          } else if( pdg == genie::kPdgK0 || pdg == genie::kPdgAntiK0 || pdg == genie::kPdgK0L || pdg == genie::kPdgK0S ) {
            nK01++;
            eOther1 += ke;
          } else if( pdg == genie::kPdgGamma ) {
            nEM1++;
            eOther1 += ke;
          } else if( genie::pdg::IsHadron(pdg) ) {
            nOtherHad1++; // charm mesons, strange and charm baryons, antibaryons, etc.
            eOther1 += ke;
          } else if( genie::pdg::IsIon(pdg) ) {
            nNucleus1++;
          } else {
            nUNKNOWN1++;
          }
	

        }
      }


//	std::cout << "epip1:" << ePip1 << std::endl;
//	std::cout << "ePip:" << ePip << std::endl;
	
      // Reweighting variables

      // struct VarAndCVResponse {
      //   systtools::paramId_t pid;
      //   double CV_response;
      //   std::vector<double> responses;
      // };
      // typedef std::vector<VarAndCVResponse> event_unit_response_w_cv_t;
      //
      // typedef std::vector<event_unit_response_w_cv_t> EventAndCVResponse; 

      for( auto &sp : fSystProviders ) {
        std::unique_ptr<systtools::EventAndCVResponse> syst_resp = sp->GetEventVariationAndCVResponse(evt);
        if( !syst_resp ) {
          std::cout << "[ERROR]: Got nullptr systtools::EventResponse from provider "
                    << sp->GetFullyQualifiedName();
          continue;
        }

        for( systtools::EventAndCVResponse::const_iterator itResp = syst_resp->begin(); itResp != syst_resp->end(); ++itResp ) {
          systtools::event_unit_response_w_cv_t const &resp = *itResp;
          for( systtools::event_unit_response_w_cv_t::const_iterator it = resp.begin(); it != resp.end(); ++it ) {
            fNwgt[it->pid] = it->responses.size();
            fCvWgts[it->pid] = it->CV_response;
            std::copy_n(it->responses.begin(), it->responses.size(), fWgts[it->pid]);
          }
        }
      }

      const std::vector< const simb::MCParticle* > parts = pi_serv->MCTruthToParticles_Ps(truth[i]);
      for( size_t pp = 0; pp < parts.size(); ++pp ) {
        int tid = parts[pp]->TrackId();
        int mother = parts[pp]->Mother();
        tid_to_mother.emplace(tid, mother);
        if( mother == 0 ) primary_pdg.emplace(tid, parts[pp]->PdgCode());
        int primaryTid = tid;
        while( mother != 0 ) {
          primaryTid = mother;
          mother = tid_to_mother[primaryTid]; // find primary
        }
        if( primary_pdg.find(primaryTid) == primary_pdg.end() ) std::cout << "Something is wrong" << std::endl;
        else {
          primary_pdg.emplace(tid, primary_pdg[primaryTid]);
	  tid_to_primary.emplace(tid,primaryTid);
        }
      }

	



    } // loop through MC truth i


    // truth-matching hadronic energy to particles
    art::ServiceHandle<cheat::BackTrackerService> bt_serv;
    art::Handle< std::vector<recob::Hit> > hitListHandle;
    std::vector<art::Ptr<recob::Hit> > hitlist;
    if (evt.getByLabel(fHitsModuleLabel,hitListHandle))
      art::fill_ptr_vector(hitlist, hitListHandle);

    eRecoP = 0.;
    eRecoN = 0.;
    eRecoPip = 0.;
    eRecoPim = 0.;
    eRecoPi0 = 0.;
    eRecoOther = 0.;
    eRecoOtherraw = 0.;
    eRecoOtherplane0 = 0.;
    eRecoOtherplane1 = 0.;
    eRecoOtherplane2 = 0.;

    eDepP = 0.;
    eDepN = 0.;
    eDepPip = 0.;
    eDepPim = 0.;
    eDepPi0 = 0.;
    eDepOther = 0.;
    eDepMichel = 0.;
    eDepMichelhit = 0.;

    eDepPraw = 0.;
    eDepNraw = 0.;
    eDepPipraw = 0.;
    eDepPimraw = 0.;
    eDepPi0raw = 0.;
    eDepOtherraw = 0.;

    eDepPplane0 = 0.;
    eDepNplane0 = 0.;
    eDepPipplane0 = 0.;
    eDepPimplane0 = 0.;
    eDepPi0plane0 = 0.;
    eDepOtherplane0 = 0.;

    eDepPplane1 = 0.;
    eDepNplane1 = 0.;
    eDepPipplane1 = 0.;
    eDepPimplane1 = 0.;
    eDepPi0plane1 = 0.;
    eDepOtherplane1 = 0.;

    eDepPplane2 = 0.;
    eDepNplane2 = 0.;
    eDepPipplane2 = 0.;
    eDepPimplane2 = 0.;
    eDepPi0plane2 = 0.;
    eDepOtherplane2 = 0.;



//Edit with true variables for deposited energy and raw
    // Need t0 for electron lifetime correction
    auto const *detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
    double t0 = detprop->TriggerOffset();
    const simb::MCParticle* michelid;
    const simb::MCParticle* michelid1;
    std::map<int,double> tid_charge;
    std::map<int,double> tid_eDep;
    std::map<int,double> tid_eDepraw;
    std::map<int,double> tid_eDepplane0;
    std::map<int,double> tid_eDepplane1;
    std::map<int,double> tid_eDepplane2;

	std::cout << "hitlist.size:" << hitlist.size() << std::endl;
	size_t checkhits = 0;
//	int hitIDEi = 0;
    double total_charge = 0.;
    for( size_t i = 0; i < hitlist.size(); ++i ) {
    	art::Ptr<recob::Hit> hit = hitlist[i];
      double charge = hit->Integral();

      double charge_eLifetimeCorrected = hit->Integral() * fCaloAlg.LifetimeCorrection(hit->PeakTime(), t0);

      std::vector<sim::TrackIDE> TrackIDs = bt_serv->HitToTrackIDEs(hit); 
	checkhits += TrackIDs.size();    
      std::vector< const sim::IDE* > IDEs = bt_serv->HitToSimIDEs_Ps(hit);
      double deposited_energy = 0.;
      double deposited_energyraw = 0.;
      if (hit->WireID().Plane == 2) {
	deposited_energyraw = fCaloAlg.ElectronsFromADCArea( charge_eLifetimeCorrected , 2) * (1.0 / fRecombFactor) / util::kGeVToElectrons;
	}
/*
//check photon IDEs
		for ( size_t pp = 0; pp < IDEs.size(); ++pp ){
		if(fEvent == 2 && IDEs[pp]->trackID == -6) {
		int primpdg = primary_pdg[IDEs[pp]->trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		hitIDEsno++;
		hitdepenergy += IDEs[pp]->energy;
		}
		}
		}

*/
/*	//hit IDEs
//	if (hit->WireID().Plane == 2) {
	for ( size_t pp = 0; pp < IDEs.size(); ++pp ){
		hitIDEs[hitIDEsno][0] = IDEs[pp]->energy;
		hitIDEs[hitIDEsno][1] = IDEs[pp]->x;
		hitIDEs[hitIDEsno][2] = IDEs[pp]->y;
		hitIDEs[hitIDEsno][3] = IDEs[pp]->z;
		hitIDEs[hitIDEsno][4] = IDEs[pp]->trackID;
		hitIDEsno++;

		int primpdg = primary_pdg[IDEs[pp]->trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		michelid = pi_serv->TrackIdToParticle_P(IDEs[pp]->trackID);
		hitIDEsrn = hitIDEi;
		hitIDETrackID = IDEs[pp]->trackID;
		hitIDEPDGCode = michelid->PdgCode();
		hitIDEEnergy = IDEs[pp]->energy;
		hitIDEx = IDEs[pp]->x;
		hitIDEy = IDEs[pp]->y;
		hitIDEz = IDEs[pp]->z;
		hitIDETree->Fill();
		hitIDEi++;
		}
		}
*/
/*		//IDETree template
		int primpdg = primary_pdg[IDEs[pp]->trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		michelid = pi_serv->TrackIdToParticle_P(IDEs[pp]->trackID);
		IDEsrn = IDEi;
		IDETrackID = IDEs[pp]->trackID;
		IDEPDGCode = michelid->PdgCode();
		IDEEnergy = IDEs[pp]->energy;
		IDEx = IDEs[pp]->x;
		IDEy = IDEs[pp]->y;
		IDEz = IDEs[pp]->z;
		IDETree->Fill();
		IDEi++;
		}
	
//	}
*/
	if( hit->WireID().Plane == 2) {
/*	for ( sim::IDE IDE : IDEs ){
		//pi0 photons deposited energy
		if( nPi0 != 0) {
		for(int pp = 0; pp < 10; pp++) {
				if (trackidphoton[pp] != 0) {
				michelid = pi_serv->TrackIdToParticle_P(trackidphoton[pp]);
				if((-1)*IDE.trackID == michelid->TrackId()) {
				ephotonIDE += IDE.energy;
				std::cout << "Photon Energy:" << michelid->E() << std::endl;
				std::cout << "Photon Deposited Energy ide:" << IDE.energy << std::endl; 
				} 
			} 
		}
		}


	int primpdg = primary_pdg[IDE.trackID];
	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		deposited_energy+= IDE.energy;
		tid_eDep[IDE.trackID] += IDE.energy;
			}
		}
*/	
		for(size_t e = 0; e < TrackIDs.size(); ++e) {
			int primpdg = primary_pdg[TrackIDs[e].trackID];
			if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
			deposited_energy+= TrackIDs[e].energy*TrackIDs[e].energyFrac;
//			tid_eDep[TrackIDs[e].trackID] += TrackIDs[e].energy*TrackIDs[e].energyFrac;
			}
		}

}

      double tote = 0.;
      for(size_t e = 0; e < TrackIDs.size(); ++e) tote += TrackIDs[e].energy;
		//tote and deposited energy are same unless zero
      for(size_t e = 0; e < TrackIDs.size(); ++e) {

        int primpdg = primary_pdg[TrackIDs[e].trackID];
        if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
          tid_charge[TrackIDs[e].trackID] += charge*TrackIDs[e].energy/tote;
	  
//	  tid_eDep[TrackIDs[e].trackID] += deposited_energy*TrackIDs[e].energy/tote;

	  tid_eDepraw[TrackIDs[e].trackID] += deposited_energyraw*TrackIDs[e].energy/tote;

          total_charge += charge*TrackIDs[e].energy/tote;

        }
      }
    }

//simchannel and find missing IDEs
	art::ServiceHandle<geo::Geometry> pGeom;
	//int IDEi =0;
	//int allIDEi = 0;
	int plane0IDEi = 0, plane1IDEi = 0, plane2IDEi = 0;
	//int uniqueIDEi = 0;
	int energydepositno = 0;
	int plane=0;
//	int tidcount=0;
	int tidcount1=0, tidcount2=0, tidcount3=0;
//	double depenergy = 0;
	double depenergy1 = 0, depenergy2 = 0, depenergy3 = 0;
	double plane0depenergy = 0, plane1depenergy = 0, plane2depenergy = 0;
	 art::Handle< std::vector<sim::SimChannel> > simchan;
	evt.getByLabel("largeant", simchan);
    for ( auto const& channel : (*simchan) ) {
	//WireID and Plane
	std::vector<geo::WireID> const Wires = pGeom->ChannelToWire(channel.Channel());
	/*	for( size_t temp = 0; temp < Wires.size(); temp++) {
		std::cout << temp << " " << "plane:" << Wires[temp].planeID().Plane << std::endl;
		}
	*/	plane = Wires[0].planeID().Plane;
	//	std::cout << "channel:" << channel.Channel() << std::endl;
	//	std::cout << "plane:" << plane << std::endl;
			
        auto const& timeSlices = channel.TDCIDEMap();
        for ( auto const& timeSlice : timeSlices ) {
//		if(timeSlice.first == 1400) {
		//	std::cout << "TDC:" << timeSlice.first << std::endl;
            auto const& energyDeposits = timeSlice.second;
            for ( auto const& energyDeposit : energyDeposits ) {
		
/*		//not to be used
		int repeat = 0;
		//sorting IDEs
		int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {

			for(int temp=0; temp < energydepositno; temp++){
				if( energyDeposit.x == uniqueIDEs[temp].x && energyDeposit.y == uniqueIDEs[temp].y && energyDeposit.z == uniqueIDEs[temp].z && energyDeposit.energy == uniqueIDEs[temp].Energy && energyDeposit.trackID == uniqueIDEs[temp].TrackID) {
				repeat++;
				if( uniqueIDEs[temp].Plane == 0) {
				 if( plane == 0) {
					 duplicate0++;
					 michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
				  else if( plane == 1 || plane == 2) {
					uniqueIDEs[temp].Plane = plane*10;
					break;
					}
				}
				else if( uniqueIDEs[temp].Plane == 1) {
					if( plane == 0 ) {
						 uniqueIDEs[temp].Plane =10;
						 break;
						}
					else if( plane == 1) {
					 duplicate1++;
					 michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
					else if( plane == 2) {
					 uniqueIDEs[temp].Plane = 21;
					 break;
					}
				}
				else if( uniqueIDEs[temp].Plane == 2) {
					if( plane == 0 ) {
						uniqueIDEs[temp].Plane =20;
						break;
						}
					else if( plane == 2)  {
					 duplicate2++;
					 michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
					else if(plane == 1) {
						uniqueIDEs[temp].Plane = 21;
						break;
						}
		
				}
				else if( uniqueIDEs[temp].Plane == 10) {
				 if( plane == 2) {
				 uniqueIDEs[temp].Plane = 210;
				 break;
				 }
				 else if(plane == 1 || plane == 0) {
					michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
				}
				else if( uniqueIDEs[temp].Plane == 20) {
				 if( plane == 1) {
				 uniqueIDEs[temp].Plane = 210;
				 break;
				 }
				 else if(plane == 2 || plane == 0) {
					michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
				}
				else if( uniqueIDEs[temp].Plane == 21) {
				 if( plane == 0) {
				 uniqueIDEs[temp].Plane = 210;
				 break;
				 }
				 else if(plane == 1 || plane == 2) {
					michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
				}
				else if( uniqueIDEs[temp].Plane == 210) {
					michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
					break;
					}
		}//IDE already present loop
	}//for loop
				if( repeat == 0) {
					michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
					uniqueIDEs[uniqueIDEi].x = energyDeposit.x;
					uniqueIDEs[uniqueIDEi].y = energyDeposit.y;
					uniqueIDEs[uniqueIDEi].z = energyDeposit.z;
					uniqueIDEs[uniqueIDEi].Energy = energyDeposit.energy;
					uniqueIDEs[uniqueIDEi].TrueEnergy = michelid->E();
					uniqueIDEs[uniqueIDEi].TrackID = energyDeposit.trackID;
					uniqueIDEs[uniqueIDEi].PDGCode = michelid->PdgCode();
					uniqueIDEs[uniqueIDEi].Plane = plane;
					uniqueIDEi++;
				}
				

		}
*/
		energydepositno++;

		//plane IDEs
		if( plane == 0) {
			int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		tid_eDepplane0[energyDeposit.trackID] += energyDeposit.energy;//deposited energy
		plane0depenergy += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
			plane0IDEsrn = plane0IDEi;
			plane0IDETrackID = energyDeposit.trackID;
			plane0IDEPDGCode = michelid->PdgCode();
			plane0IDEEnergy = energyDeposit.energy;
			plane0IDEx = energyDeposit.x;
			plane0IDEy = energyDeposit.y;
			plane0IDEz = energyDeposit.z;
		//	plane0IDETree->Fill();
			plane0IDEi++;
/*			//array
			plane0IDEs[plane0IDEi][0] = plane0IDEx;
			plane0IDEs[plane0IDEi][1] = plane0IDEy;
			plane0IDEs[plane0IDEi][2] = plane0IDEz;
			plane0IDEs[plane0IDEi][3] = plane0IDEEnergy;
			plane0IDEs[plane0IDEi][4] = plane0IDETrackID;
			plane0IDEs[plane0IDEi][5] = plane0IDEPDGCode;
			
*/		}
		}

		if( plane == 1) {
			int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		tid_eDepplane1[energyDeposit.trackID] += energyDeposit.energy;//deposited energy
		plane1depenergy += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
			plane1IDEsrn = plane1IDEi;
			plane1IDETrackID = energyDeposit.trackID;
			plane1IDEPDGCode = michelid->PdgCode();
			plane1IDEEnergy = energyDeposit.energy;
			plane1IDEx = energyDeposit.x;
			plane1IDEy = energyDeposit.y;
			plane1IDEz = energyDeposit.z;
		//	plane1IDETree->Fill();
			plane1IDEi++;

		

/*			//array
			plane1IDEs[plane1IDEi][0] = plane1IDEx;
			plane1IDEs[plane1IDEi][1] = plane1IDEy;
			plane1IDEs[plane1IDEi][2] = plane1IDEz;
			plane1IDEs[plane1IDEi][3] = plane1IDEEnergy;
			plane1IDEs[plane1IDEi][4] = plane1IDETrackID;
			plane1IDEs[plane1IDEi][5] = plane1IDEPDGCode;
			
*/		}
		}

		if( plane == 2) {
			int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		tid_eDepplane2[energyDeposit.trackID] += energyDeposit.energy;//deposited energy
		plane2depenergy += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
			plane2IDEsrn = plane2IDEi;
			plane2IDETrackID = energyDeposit.trackID;
			plane2IDEPDGCode = michelid->PdgCode();
			plane2IDEEnergy = energyDeposit.energy;
			plane2IDEx = energyDeposit.x;
			plane2IDEy = energyDeposit.y;
			plane2IDEz = energyDeposit.z;
			if(michelid->Mother() != 0) {
			michelid1 = pi_serv->TrackIdToParticle_P(michelid->Mother());
			plane2IDEMotherTrackID = michelid->Mother();
			plane2IDEMotherPDGCode = michelid1->PdgCode();
			}
			else {
			plane2IDEMotherTrackID = 0;
			plane2IDEMotherPDGCode = 0;
			}
			//plane2IDETree->Fill();
			plane2IDEi++;

		//removing muon and electron
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {

		
		//pip
		
			if ( energyDeposit.trackID == trackidmuon[0] || energyDeposit.trackID == (-1)*trackidmuon[0] ) {
				
				eDecay += energyDeposit.energy;
				
				}
			

		
			if ( energyDeposit.trackID == trackidmichel[0] || energyDeposit.trackID == (-1)*trackidmichel[0] ) {
				
				eDecay += energyDeposit.energy;
				
				}
			
		
		
		
		//pim
		
			if ( energyDeposit.trackID == trackidmuon1[0] || energyDeposit.trackID == (-1)*trackidmuon1[0] ) {
				
				eDecay1 += energyDeposit.energy;
				
				}
			

		
			if ( energyDeposit.trackID == trackidmichel1[0]  || energyDeposit.trackID == (-1)*trackidmichel1[0]  ) {
				
				eDecay1 += energyDeposit.energy;
				
				}
			
		

	}//removing muon and e loop

/*
			//particle info (trackid condition loop)
		int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(plane2IDEMotherTrackID == 2080) {
		depenergy += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
		michelid1 = pi_serv->TrackIdToParticle_P(michelid->Mother());
		
		
		std::cout << "IDE x:" << energyDeposit.x << std::endl;
		std::cout << "IDE y:" << energyDeposit.y << std::endl;
		std::cout << "IDE z:" << energyDeposit.z << std::endl;
		std::cout << "IDE Energy:" << energyDeposit.energy << std::endl;
		std::cout << "IDE Trackid:" << energyDeposit.trackID << std::endl;
		std::cout << "IDE pdg:" << michelid->PdgCode() << std::endl;
		std::cout << "mother Trackid:" << michelid1->TrackId() << std::endl;
		std::cout << "." << std::endl;
		tidcount++;
		
		}
		}

*/		
			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == 5 || energyDeposit.trackID == -5) {
		depenergy1 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);

/*
		std::cout << "IDE x:" << energyDeposit.x << std::endl;
		std::cout << "IDE y:" << energyDeposit.y << std::endl;
		std::cout << "IDE z:" << energyDeposit.z << std::endl;
		std::cout << "IDE Energy:" << energyDeposit.energy << std::endl;
		std::cout << "IDE Trackid:" << energyDeposit.trackID << std::endl;
		std::cout << "IDE pdg:" << michelid->PdgCode() << std::endl;
	*/	tidcount1++;
		}
		}

		
			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == -17 || energyDeposit.trackID == 17) {
		depenergy2 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);

		
		tidcount2++;
		}
		}

		
			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == -15 || energyDeposit.trackID == 15) {
		depenergy3 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);

		
		tidcount3++;
		}
		}

/*			//array
			plane2IDEs[plane2IDEi][0] = plane2IDEx;
			plane2IDEs[plane2IDEi][1] = plane2IDEy;
			plane2IDEs[plane2IDEi][2] = plane2IDEz;
			plane2IDEs[plane2IDEi][3] = plane2IDEEnergy;
			plane2IDEs[plane2IDEi][4] = plane2IDETrackID;
			plane2IDEs[plane2IDEi][5] = plane2IDEPDGCode;
			
*/		}
		}
/*
		//IDEtree simchannel
		int primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
			allIDEsrn = allIDEi;
			allIDETrackID = energyDeposit.trackID;
			allIDEPDGCode = michelid->PdgCode();
			allIDEEnergy = energyDeposit.energy;
			allIDEx = energyDeposit.x;
			allIDEy = energyDeposit.y;
			allIDEz = energyDeposit.z;
			allIDETree->Fill();
			allIDEi++;
		}

		//missing IDEs
		int pseudo = 0;
		for(int t=0; t < hitIDEsno; t++) {
				if ( energyDeposit.energy == hitIDEs[t][0] && energyDeposit.x == hitIDEs[t][1] && energyDeposit.y == hitIDEs[t][2] && energyDeposit.z == hitIDEs[t][3] && energyDeposit.trackID == hitIDEs[t][4] ) pseudo = 1;
			}

		if(pseudo == 0) {
			missing++;
			michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);
			IDEsrn = IDEi;
			IDETrackID = energyDeposit.trackID;
			IDEPDGCode = michelid->PdgCode();
			IDEEnergy = energyDeposit.energy;
			IDEx = energyDeposit.x;
			IDEy = energyDeposit.y;
			IDEz = energyDeposit.z;
			IDETree->Fill();
			IDEi++;
			}
	*/	
		//all deposited

		int primpdg = primary_pdg[energyDeposit.trackID];
		if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		tid_eDep[energyDeposit.trackID] += energyDeposit.energy/3;
			}

                
            }
//        }//TDC condition loop
      }
     
    }

/*	
	//particle info
	michelid = pi_serv->TrackIdToParticle_P(10);
	std::cout << "trackid16: " << std::endl;
		std::cout << "Trackid: " << michelid->TrackId() << std::endl;
		std::cout << "StatusCode: " << michelid->StatusCode() << std::endl;
		std::cout << "PdgCode: " << michelid->PdgCode() << std::endl;
		std::cout << "Mother trackid: " << michelid->Mother() << std::endl;
	//	std::cout << "Polarization: " << michelid->Polarization() << std::endl;
		std::cout << "Process: " << michelid->Process() << std::endl;
		std::cout << "EndProcess: " << michelid->EndProcess() << std::endl;
		std::cout << "Number of daughters: " << michelid->NumberDaughters() << std::endl;
		std::cout << "Number of tracjectory points: " << michelid->NumberTrajectoryPoints() << std::endl;
	//	std::cout << "Position: " << michelid->Position() << std::endl;
	//	std::cout << "Momentum: " << michelid->Momentum() << std::endl;
		std::cout << "Vx: " << michelid->Vx() << std::endl;
		std::cout << "Vy: " << michelid->Vy() << std::endl;
		std::cout << "Vz: " << michelid->Vz() << std::endl;
		std::cout << "T: " << michelid->T() << std::endl;
	//	std::cout << "EndPosition: " << michelid->EndPosition() << std::endl;
		std::cout << "EndX: " << michelid->EndX() << std::endl;
		std::cout << "EndY: " << michelid->EndY() << std::endl;
		std::cout << "EndZ: " << michelid->EndZ() << std::endl;
		std::cout << "EndT: " << michelid->EndT() << std::endl;
		std::cout << "Px: " << michelid->Px() << std::endl;
		std::cout << "Py: " << michelid->Py() << std::endl;
		std::cout << "Pz: " << michelid->Pz() << std::endl;
		std::cout << "E: " << michelid->E() << std::endl;
		std::cout << "P: " << michelid->P() << std::endl;
		std::cout << "Pt: " << michelid->Pt() << std::endl;
		std::cout << "Mass: " << michelid->Mass() << std::endl;
	//	std::cout << "EndMomentum: " << michelid->EndMomentum() << std::endl;
		std::cout << "EndPx: " << michelid->EndPx() << std::endl;
		std::cout << "EndPy: " << michelid->EndPy() << std::endl;
		std::cout << "EndPz: " << michelid->EndPz() << std::endl;
		std::cout << "EndE: " << michelid->EndE() << std::endl;
	//	std::cout << "GetGvtx: " << michelid->GetGvtx() << std::endl;
		std::cout << "Gvx: " << michelid->Gvx() << std::endl;
		std::cout << "Gvy: " << michelid->Gvy() << std::endl;
		std::cout << "Gvz: " << michelid->Gvz() << std::endl;
		std::cout << "Gvt: " << michelid->Gvt() << std::endl;
		std::cout << "FirstDaughter: " << michelid->FirstDaughter() << std::endl;
		std::cout << "Trajectory: " << michelid->Trajectory() << std::endl;
*/

    // choose the hadronic energy for the best reco
    double ehad = ( fCVNResultNue > fCVNResultNumu ? fRecoHadEnNue : fRecoHadEnNumu );
    for( std::map<int,double>::iterator itTid = tid_charge.begin(); itTid != tid_charge.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy = (*itTid).second * (ehad/total_charge);

      if( tid < 0 ) tid *= -1;

      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton ) eRecoP += energy;
      else if( primpdg == genie::kPdgNeutron ) eRecoN += energy;
      else if( primpdg == genie::kPdgPiP ) eRecoPip += energy;
      else if( primpdg == genie::kPdgPiM ) eRecoPim += energy;
      else if( primpdg == genie::kPdgPi0 ) eRecoPi0 += energy;
      else {
	eRecoOther += energy;
	eRecoOtherraw += energy;
	eRecoOtherplane0 += energy;
	eRecoOtherplane1 += energy;
	eRecoOtherplane2 += energy;
	   };
    }

    // Deposited energy
	
    for( std::map<int,double>::iterator itTid = tid_eDep.begin(); itTid != tid_eDep.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
/*
	//pi0 photons deposited energy
		if( nPi0 != 0) {
		for(int pp = 0; pp < 10; pp++) {
				if (trackidphoton[pp] != 0) {
				michelid = pi_serv->TrackIdToParticle_P(trackidphoton[pp]);
				if(tid == michelid->TrackId()) {
				std::cout << "Photon Energy:" << michelid->E() << std::endl;
				std::cout << "Photon Deposited Energynew:" << energy_deposited << std::endl; 
				} 
			} 
		}
		}

*/
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton )       eDepP += energy_deposited;
      else if( primpdg == genie::kPdgNeutron ) eDepN += energy_deposited;
      else if( primpdg == genie::kPdgPiP )     eDepPip += energy_deposited;
      else if( primpdg == genie::kPdgPiM )     eDepPim += energy_deposited;
      else if( primpdg == genie::kPdgPi0 )     eDepPi0 += energy_deposited;
      else eRecoOther += energy_deposited;
    }


    // Deposited energy raw
    for( std::map<int,double>::iterator itTid = tid_eDepraw.begin(); itTid != tid_eDepraw.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton )       eDepPraw += energy_deposited;
      else if( primpdg == genie::kPdgNeutron ) eDepNraw += energy_deposited;
      else if( primpdg == genie::kPdgPiP ) {
	eDepPipraw += energy_deposited;
	}
      else if( primpdg == genie::kPdgPiM )     eDepPimraw += energy_deposited;
      else if( primpdg == genie::kPdgPi0 )     eDepPi0raw += energy_deposited;
      else eRecoOtherraw += energy_deposited;
    }

    // Deposited energy plane0
	
    for( std::map<int,double>::iterator itTid = tid_eDepplane0.begin(); itTid != tid_eDepplane0.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton )       eDepPplane0 += energy_deposited;
      else if( primpdg == genie::kPdgNeutron ) eDepNplane0 += energy_deposited;
      else if( primpdg == genie::kPdgPiP )     eDepPipplane0 += energy_deposited;
      else if( primpdg == genie::kPdgPiM )     eDepPimplane0 += energy_deposited;
      else if( primpdg == genie::kPdgPi0 )     eDepPi0plane0 += energy_deposited;
      else eRecoOtherplane0 += energy_deposited;
    }

    // Deposited energy plane1
	
    for( std::map<int,double>::iterator itTid = tid_eDepplane1.begin(); itTid != tid_eDepplane1.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton )       eDepPplane1 += energy_deposited;
      else if( primpdg == genie::kPdgNeutron ) eDepNplane1 += energy_deposited;
      else if( primpdg == genie::kPdgPiP )     eDepPipplane1 += energy_deposited;
      else if( primpdg == genie::kPdgPiM )     eDepPimplane1 += energy_deposited;
      else if( primpdg == genie::kPdgPi0 )     eDepPi0plane1 += energy_deposited;
      else eRecoOtherplane1 += energy_deposited;
    }

    // Deposited energy plane2
    for( std::map<int,double>::iterator itTid = tid_eDepplane2.begin(); itTid != tid_eDepplane2.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgProton )  {
	     eDepPplane2 += energy_deposited;
	//	std::cout << "trackids for P:" << tid << std::endl;
		
	}
      else if( primpdg == genie::kPdgNeutron ) eDepNplane2 += energy_deposited;
      else if( primpdg == genie::kPdgPiP ){
		eDepPipplane2 += energy_deposited;
/*		//removing michel e and muon IDEs
		for(int b=0; b < 10; b++) {
			if ( tid == trackidmuon[b] ) {
				std::cout << "eDecaymuon:" << energy_deposited << std::endl;
				eDecaymuon[decay] = energy_deposited;
				decay++;
				}
			}

		for(int b=0; b < 10; b++) {
			if ( tid == trackidmichel[b] ) {
				std::cout << "eDecaye:" << energy_deposited << std::endl;
				eDecaye[decay] = energy_deposited;
				decaye++;
				}
			}
*/
	}
      else if( primpdg == genie::kPdgPiM ){
	     eDepPimplane2 += energy_deposited;
/*		//removing michel e and muon IDEs
		for(int b=0; b < 10; b++) {
			if ( tid == trackidmuon[b] ) {
				std::cout << "eDecaymuon1:" << energy_deposited << std::endl;
				eDecaymuon1[decay] = energy_deposited;
				decay1++;
				}
			}

		for(int b=0; b < 10; b++) {
			if ( tid == trackidmichel[b] ) {
				std::cout << "eDecaye1:" << energy_deposited << std::endl;
				eDecaye1[decay] = energy_deposited;
				decaye1++;
				}
			}
*/
		}
      else if( primpdg == genie::kPdgPi0 )     eDepPi0plane2 += energy_deposited;
      else eRecoOtherplane2 += energy_deposited;
    }
	std::cout << "eDecay:" << eDecay << std::endl;
	std::cout << "eDecay1:" << eDecay1 << std::endl;
	//removing muon and electron
	if( pipdecay > 0) eDepPipplane2 += (-1)*eDecay;
	if( pimdecay > 0) eDepPimplane2 += (-1)*eDecay1;

/*	//removing michel e and muon
	if( decay != 0) {
		for(int b=0; b < 10; b++) eDepPipplane2 += (-1)*eDecaymuon[b] + (-1)*eDecaye[b];
	}
	if( decay1 != 0) {
		for(int b=0; b < 10; b++) eDepPimplane2 += (-1)*eDecaymuon1[b] + (-1)*eDecaye1[b];
	}
*/
/*	
	std::cout << "truE:" << trueE << std::endl;
	std::cout << "eDeppipraw:" << eDepPipplane1 << std::endl;
	std::cout << "eDeppip:" << eDepPipplane2 << std::endl;
	std::cout << "trackIDcount:" << tidcount << std::endl;
  	std::cout << "eDeptrackID:" << depenergy << std::endl;
*/	std::cout << "trackIDcount:" << tidcount1 << std::endl;
	std::cout << "eDeptrackID:" << depenergy1 << std::endl;
	std::cout << "trackIDcount:" << tidcount2 << std::endl;
	std::cout << "eDeptrackID:" << depenergy2 << std::endl;
	std::cout << "trackIDcount:" << tidcount3 << std::endl;
	std::cout << "eDeptrackID:" << depenergy3 << std::endl;
	

	for (int e=0; e < 10; e++) {
		std::cout << "trackidmichel:" << trackidmichel[e] << std::endl;
		std::cout << "trackidmuon:" << trackidmuon[e] << std::endl;
		std::cout << "trackidpion:" << trackidpion[e] << std::endl;
		trackidpion[e] = 0;
		trackidmuon[e] = 0;
		trackidmichel[e] = 0;
		trackidpion1[e] = 0;
		trackidmuon1[e] = 0;
		trackidmichel1[e] = 0;
		trackidphoton[e] = 0;
	}

	std::cout << "eDepNplane2:" << eDepNplane2 << std::endl;
		std::cout << "eN:" << eN << std::endl;
	
    fTree->Fill();
	pipdecay = 0;
	pimdecay = 0;
	eDecay = 0;
	eDecay1 = 0;
    // Get the genie::EventRecord
    ghep = evgb::RetrieveGHEP(mct->at(0), gt->at(0));
    fGenieTree->Fill();

    return;
  }

  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  void CAFMaker::endSubRun(const art::SubRun& sr){
  }

  void CAFMaker::endJob()
  {
    fMetaTree->Fill();
  }

  DEFINE_ART_MODULE(CAFMaker)

} // namespace dunemva

#endif // CAFMaker_H

