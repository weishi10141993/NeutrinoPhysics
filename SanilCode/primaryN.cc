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
#include <fstream>
#include <ostream>


// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h" 
#include "art/Framework/Principal/Event.h" 
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "art_root_io/TFileService.h"

//#include "Utils/AppInit.h"
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "dune/FDSensOpt/FDSensOptData/MVASelectPID.h"
#include "dune/FDSensOpt/FDSensOptData/EnergyRecoOutput.h"
#include "dune/CVN/func/InteractionType.h"
#include "dune/CVN/func/Result.h"
#include "dune/RegCNN/func/RegCNNResult.h"

//new
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"

// dunerw stuff
#include "systematicstools/interface/ISystProviderTool.hh"
#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"
#include "systematicstools/utility/exceptions.hh"
//#include "systematicstools/utility/md5.hh"

// root
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

// pdg
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/ParticleData/PDGLibrary.h"

// genie
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"

#include "nugen/EventGeneratorBase/GENIE/GENIE2ART.h"

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
      std::string fMVASelectLabel;
      std::string fMVASelectNueLabel;
      std::string fMVASelectNumuLabel;

      std::string fCVNLabel;
      std::string fRegCNNLabel;

      std::string fEnergyRecoNueLabel;
      std::string fEnergyRecoNumuLabel;
      std::string fMVAMethod;

      float fOscPro;
      double fWeight;
      TTree* fTree;  
      TTree* fMetaTree;

      // Get reweight knobs from fhicl file -- no hard-coded shifts
      int fNwgt[knShifts];
      double fCvWgts[knShifts];
      double fWgts[knShifts][kmaxRwgts];

      // CAF variables
      
      // event accounting
      int fRun, fSubrun, fEvent;
      // Truth information
	double eN, eDepNplane2, bufferE;

	 int plane2IDEsrn, plane2IDETrackID, plane2IDEPDGCode, plane2IDEMotherTrackID, plane2IDEMotherPDGCode;
	double plane2IDEEnergy, plane2IDEx, plane2IDEy, plane2IDEz;
	int I,J,K,L,leave, count;
	std::ofstream out;

      
      double fMVAResult;
      double fMVAResultNue;
      double fMVAResultNumu;
      double meta_pot;
      int meta_run, meta_subrun, meta_version;

      systtools::provider_list_t fSystProviders;

  }; // class CAFMaker


  //------------------------------------------------------------------------------
  CAFMaker::CAFMaker(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
	evgb::SetEventGeneratorListAndTune();
    this->reconfigure(pset);
  }

  dunemva::CAFMaker::~CAFMaker(){}

  //------------------------------------------------------------------------------
  void CAFMaker::reconfigure(fhicl::ParameterSet const& pset) 
  {

    fMVASelectLabel = pset.get<std::string>("MVASelectLabel");
    fMVASelectNueLabel = pset.get<std::string>("MVASelectNueLabel");
    fMVASelectNumuLabel = pset.get<std::string>("MVASelectNumuLabel");
    fCVNLabel = pset.get<std::string>("CVNLabel");
    fRegCNNLabel = pset.get<std::string>("RegCNNLabel");

    fEnergyRecoNueLabel = pset.get<std::string>("EnergyRecoNueLabel");
    fEnergyRecoNumuLabel = pset.get<std::string>("EnergyRecoNumuLabel");

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
  }


  //------------------------------------------------------------------------------
  void CAFMaker::beginJob()
  {

    art::ServiceHandle<art::TFileService> tfs;
    fTree = tfs->make<TTree>("caf", "caf");
    fMetaTree = tfs->make<TTree>("meta", "meta");

    // book-keeping
    fTree->Branch("run",         &fRun,        "run/I");
    fTree->Branch("subrun",      &fSubrun,     "subrun/I");
    fTree->Branch("event",       &fEvent,      "event/I");
    


    // FS particle counts
   
    
    fTree->Branch("eN",        &eN,         "eN/D");
	fTree->Branch("I",        &I,         "I/I");
	fTree->Branch("J",        &J,         "J/I");
	fTree->Branch("K",        &K,         "K/I");
	fTree->Branch("leave",	&leave,		"leave/I");
	fTree->Branch("bufferE",	&bufferE,		"bufferE/D");
    
    
    fTree->Branch("eDepNplane2",        &eDepNplane2,         "eDepNplane2/D");
  

   

    fMetaTree->Branch("pot", &meta_pot, "pot/D");
    fMetaTree->Branch("run", &meta_run, "run/I");
    fMetaTree->Branch("subrun", &meta_subrun, "subrun/I");
    fMetaTree->Branch("version", &meta_version, "version/I");

    // make DUNErw variables
    for( auto &sp : fSystProviders ) {
      systtools::SystMetaData metaData = sp->GetSystMetaData();
      for( systtools::SystMetaData::iterator itMeta = metaData.begin(); itMeta != metaData.end(); ++itMeta ) {      
        systtools::SystParamHeader head = *itMeta;
        std::string name = head.prettyName;
        unsigned int parId = head.systParamId;
        std::cout << "Adding reweight branch " << parId << " for " << name << " with " << head.paramVariations.size() << " shifts" << std::endl;
        fTree->Branch( Form("%s_nshifts", name.c_str()), &fNwgt[parId], Form("%s_nshifts/I", name.c_str()) );
        fTree->Branch( Form("%s_cvwgt", name.c_str()), &fCvWgts[parId], Form("%s_cvwgt/D", name.c_str()) );
        fTree->Branch( Form("wgt_%s", name.c_str()), fWgts[parId], Form("wgt_%s[%s_nshifts]/D", name.c_str(), name.c_str()) );
      }
    }

    // initialize weight variables -- some won't ever be set
    for( int i = 0; i < knShifts; ++i ) {
      fNwgt[i] = 0;
      fCvWgts[i] = 1.;
      for( int j = 0; j < kmaxRwgts; ++j ) {
        fWgts[i][j] = 0.;
      }
    }

    meta_pot = 0.;
    meta_version = 1;
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
    art::Handle<dunemva::MVASelectPID> pidin;
    evt.getByLabel(fMVASelectLabel, pidin);

    art::Handle<dunemva::MVASelectPID> pidinnue;
    evt.getByLabel(fMVASelectNueLabel, pidinnue);

    art::Handle<dunemva::MVASelectPID> pidinnumu;
    evt.getByLabel(fMVASelectNumuLabel, pidinnumu);

    art::Handle<std::vector<cvn::Result>> cvnin;
    evt.getByLabel(fCVNLabel, "cvnresult", cvnin);

    art::Handle<std::vector<cnn::RegCNNResult>> regcnnin;
    evt.getByLabel(fRegCNNLabel, "regcnnresult", regcnnin);


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

	count++;


    if( !pidinnue.failedToGet() ) {
      fMVAResultNue = pidinnue->pid;
    }

    if( !pidinnumu.failedToGet() ) {
      fMVAResultNumu = pidinnumu->pid;
    }

   

    art::Handle< std::vector<simb::MCTruth> > mct;
    std::vector< art::Ptr<simb::MCTruth> > truth;
    if( evt.getByLabel("generator", mct) )
      art::fill_ptr_vector(truth, mct);
    else
      mf::LogWarning("CAFMaker") << "No MCTruth.";


    art::Handle< std::vector<simb::MCFlux> > mcf;
    std::vector< art::Ptr<simb::MCFlux> > flux;
    if( evt.getByLabel("generator", mcf) )
      art::fill_ptr_vector(flux, mcf);
    else
      mf::LogWarning("CAFMaker") << "No MCFlux.";
/*
    art::Handle< std::vector<simb::GTruth> > gt;
    std::vector< art::Ptr<simb::GTruth> > gtru;
    if( evt.getByLabel("generator", gt) )
      art::fill_ptr_vector(gtru, gt);
    else
      mf::LogWarning("CAFMaker") << "No GTruth.";
*/

	 std::map<int,int> tid_to_primary;
	 std::map<int,int> primary_pdg; // track ID to PDG code of primary particle
    std::map<int,int> tid_to_mother; // track ID to mother track ID

    for(size_t i=0; i<truth.size(); i++){

      if(i>1){
        mf::LogWarning("CAFMaker") << "Skipping MC truth index " << i;
        continue;
      }

      
      // TODO
      fWeight = 0.;
      fOscPro = 0.;
      //      fOscPro   = fMVAAlg.OscPro(fCCNC,fBeamPdg,fNuPDG,fEtrue);

      
      eN = 0.;
      I = 0;
	J = 0;
	K = 0;
	L = 0;
	leave = 0;


	//new
	const std::vector< const simb::MCParticle* > michele = pi_serv->MCTruthToParticles_Ps(truth[i]);
	//const simb::MCParticle* michelid;
	//const simb::MCParticle* michelid1;

	for( size_t ee = 0; ee < michele.size(); ++ee ) {

	if( michele[ee]->EndX() < -310 || michele[ee]->EndX() > 310 || michele[ee]->EndY() < -550 || michele[ee]->EndY() > 550 || michele[ee]->EndZ() < 50 || michele[ee]->EndZ() > 1244) leave++;	//particles leaving the detector


	if (michele[ee]->Mother() == 1 && michele[ee]->PdgCode() == 2112) {
		std::cout << "EndProcess: " << michele[ee]->EndProcess() << std::endl;

		if( michele[ee]->EndProcess() == "neutronInelastic") I++;
		else if( michele[ee]->EndProcess() == "nKiller") J++;
		else if( michele[ee]->EndProcess() == "nCapture") K++;
		else {
			L++;
			std::cout << "NEW: " << michele[ee]->EndProcess() << std::endl;
		}
	}

/*	
	//primary particles info
	if (michele[ee]->TrackId() == 1 ) {
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
/*
	//primary particles info
	if (michele[ee]->Mother() == 3 ) {
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
 	}



	//trying particleinventory method
	
//	 double kee, keee;
	for( size_t p = 0; p < michele.size(); p++ ) {
        if( michele[p]->StatusCode() == genie::kIStStableFinalState && michele[p]->Mother() == 0 ) {

          int pdg = michele[p]->PdgCode();
          double ke = (michele[p]->E() - michele[p]->EndE()); 
         if( pdg == genie::kPdgNeutron ) {
            
            eN += ke;
          } 
        }
      }

	


      // Reweighting variables
      //systtools::ScrubUnityEventResponses(er);

      // struct ParamResponses { 
      //   paramId_t pid;
      //   std::vector<double> responses;
      // }
      // typedef std::vector<ParamResponses> event_unit_response_t;
      // typedef std::vector<event_unit_response_t> EventResponse;
/*
      for( auto &sp : fSystProviders ) {
        std::unique_ptr<systtools::EventResponse> syst_resp = sp->GetEventResponse(evt);
        if( !syst_resp ) {
          std::cout << "[ERROR]: Got nullptr systtools::EventResponse from provider "
                    << sp->GetFullyQualifiedName();
          continue;
        }

        for( systtools::EventResponse::iterator itResp = syst_resp->begin(); itResp != syst_resp->end(); ++itResp ) {
          systtools::event_unit_response_t resp = *itResp;
          for( systtools::event_unit_response_t::iterator it = resp.begin(); it != resp.end(); ++it ) {
            fNwgt[(*it).pid] = (*it).responses.size();
            //fCvWgts[(*it).pid] = (*it).CV_weight;
            for( unsigned int i = 0; i < (*it).responses.size(); ++i ) {
              fWgts[(*it).pid][i] = (*it).responses[i];
            }
          }
        }
      }
*/
	std::cout << "*******TEST********" << std::endl;

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



    //new
    
    eDepNplane2 = 0.;
	bufferE = 0;
    
	const simb::MCParticle* michelid;
    const simb::MCParticle* michelid1;
	std::map<int,double> tid_eDepplane2;

	//simchannel and find missing IDEs
	art::ServiceHandle<geo::Geometry> pGeom;
	//int IDEi =0;
	//int allIDEi = 0;
	int plane2IDEi = 0;
	//int uniqueIDEi = 0;
	int energydepositno = 0;
	int plane=0;
//	int tidcount=0;
//	int tidcount1=0, tidcount2=0, tidcount3=0;
//	double depenergy = 0;
//	double depenergy1 = 0, depenergy2 = 0, depenergy3 = 0;
	double plane2depenergy = 0;
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

		energydepositno++;

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

		//buffer region energy

		if( energyDeposit.x < -310 || energyDeposit.x > 310 || energyDeposit.y < -550 || energyDeposit.y > 550 || energyDeposit.z < 50 || energyDeposit.z > 1244) bufferE += energyDeposit.energy;



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
/*			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == -63 || energyDeposit.trackID == 63) {
		depenergy1 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);


		std::cout << "IDE x:" << energyDeposit.x << std::endl;
		std::cout << "IDE y:" << energyDeposit.y << std::endl;
		std::cout << "IDE z:" << energyDeposit.z << std::endl;
		std::cout << "IDE Energy:" << energyDeposit.energy << std::endl;
		std::cout << "IDE Trackid:" << energyDeposit.trackID << std::endl;
		std::cout << "IDE pdg:" << michelid->PdgCode() << std::endl;
		tidcount1++;
		}
		}

		
			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == -62 || energyDeposit.trackID == 62) {
		depenergy2 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);

		std::cout << "IDE x:" << energyDeposit.x << std::endl;
		std::cout << "IDE y:" << energyDeposit.y << std::endl;
		std::cout << "IDE z:" << energyDeposit.z << std::endl;
		std::cout << "IDE Energy:" << energyDeposit.energy << std::endl;
		std::cout << "IDE Trackid:" << energyDeposit.trackID << std::endl;
		std::cout << "IDE pdg:" << michelid->PdgCode() << std::endl;
		tidcount2++;
		}
		}

		
			//particle info (trackid condition loop)
		primpdg = primary_pdg[energyDeposit.trackID];
        	if( abs(primpdg) != 11 && abs(primpdg) != 13 && abs(primpdg) != 15 ) {
		if(energyDeposit.trackID == -64 || energyDeposit.trackID == 64) {
		depenergy3 += energyDeposit.energy;
		michelid = pi_serv->TrackIdToParticle_P(energyDeposit.trackID);

		std::cout << "IDE x:" << energyDeposit.x << std::endl;
		std::cout << "IDE y:" << energyDeposit.y << std::endl;
		std::cout << "IDE z:" << energyDeposit.z << std::endl;
		std::cout << "IDE Energy:" << energyDeposit.energy << std::endl;
		std::cout << "IDE Trackid:" << energyDeposit.trackID << std::endl;
		std::cout << "IDE pdg:" << michelid->PdgCode() << std::endl;
		tidcount3++;
		}
		}
*/
		}
		}
	   }

	}
     
    }


	 // Deposited energy plane2
    for( std::map<int,double>::iterator itTid = tid_eDepplane2.begin(); itTid != tid_eDepplane2.end(); ++itTid ) {
      int tid = (*itTid).first;
      double energy_deposited = (*itTid).second;

      if( tid < 0 ) tid *= -1;
      int primpdg = primary_pdg[tid];
      if( primpdg == genie::kPdgNeutron ) eDepNplane2 += energy_deposited;
     
    }

	std::cout << "eN:" << eN << std::endl;
	std::cout << "dep:" << eDepNplane2 << std::endl;
	std::cout << "I:" << I << std::endl;
	std::cout << "J:" << J << std::endl;
	std::cout << "K:" << K << std::endl;
	std::cout << "L:" << L << std::endl;
	std::cout << "leave:" << leave << std::endl;
	std::cout << "bufferE:" << bufferE << std::endl;

//	J++; // J=1 means zero nKiller

	out.open ("test.txt", std::ofstream::out | std::ofstream::app);
	out<<J<<" "<<I<<" "<<" "<<K<<" "<<L<<" "<<(eDepNplane2/1000)/eN<<std::endl;
	out.close();


    fTree->Fill();
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

