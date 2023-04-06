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

// Library methods
// this needs to come first before std headers to avoid _POSIX_C_SOURCE redefition error
#include "DUNE_ND_GeoEff/include/geoEff.h"
#include "DUNE_ND_GeoEff/app/Helpers.h"

// Generic C++ includes
#include <iostream>
#include <iomanip>
using namespace std;
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <math.h>

// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Utilities/Exception.h"

#include "duneanaobj/StandardRecord/StandardRecord.h"
#include "duneanaobj/StandardRecord/SRGlobal.h"
#include "duneanaobj/StandardRecord/Flat/FlatRecord.h"

//#include "Utils/AppInit.h"
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "nusimdata/SimulationBase/MCParticle.h"

#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "larsim/Simulation/LArG4Parameters.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "dunereco/FDSensOpt/FDSensOptData/MVASelectPID.h"
#include "dunereco/FDSensOpt/FDSensOptData/EnergyRecoOutput.h"
#include "dunereco/CVN/func/InteractionType.h"
#include "dunereco/CVN/func/Result.h"
#include "dunereco/RegCNN/func/RegCNNResult.h"

// dunerw stuff
#include "systematicstools/interface/ISystProviderTool.hh"
#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"
#include "systematicstools/utility/exceptions.hh"
//#include "systematicstools/utility/md5.hh"

// root
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TInterpreter.h"

// pdg
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/ParticleData/PDGLibrary.h"

// genie
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"

namespace caf {

  class CAFMaker : public art::EDAnalyzer {

    public:

      explicit CAFMaker(fhicl::ParameterSet const& pset);
      virtual ~CAFMaker();
      void beginJob() override;
      void endJob() override;
      void beginSubRun(const art::SubRun& sr) override;
      void endSubRun(const art::SubRun& sr) override;
      void analyze(art::Event const & evt) override;


    private:
      void AddGlobalTreeToFile(TFile* f, SRGlobal& global);

      std::string fMVASelectLabel;
      std::string fMVASelectNueLabel;
      std::string fMVASelectNumuLabel;

      std::string fCVNLabel;
      std::string fRegCNNLabel;

      std::string fEnergyRecoNueLabel;
      std::string fEnergyRecoNumuLabel;
      std::string fMVAMethod;

      TFile* fOutFile = 0;
      TTree* fTree = 0;
      TTree* fMetaTree = 0;

      TFile* fFlatFile = 0;
      TTree* fFlatTree = 0;
      flat::Flat<caf::StandardRecord>* fFlatRecord = 0;

      double meta_pot;
      int meta_run, meta_subrun, meta_version;

      systtools::provider_list_t fSystProviders;

      // DUNE-PRISM needed definition
      geo::GeometryCore const* geom;

      // A separate tree to store random throws of translation and rotation for DUNE-PRISM analysis
      TTree* ThrowsFD = 0;
      int seed;
      vector<float> throwVtxY;
      vector<float> throwVtxZ;
      vector<float> throwRot;

      // A separate tree to store FD event geometric efficiency at ND
      TTree* ThrowResultsFD = 0;

      // Decay point (neutrino production point) in beam coordinate
      float decayZbeamCoord;
      // Decay point (neutrino production point) in detector coordinate
      float decayXdetCoord;
      float decayYdetCoord;
      float decayZdetCoord;
      // Primary muon info
      double Sim_mu_start_vx;
      double Sim_mu_start_vy;
      double Sim_mu_start_vz;
      double Sim_mu_start_px;
      double Sim_mu_start_py;
      double Sim_mu_start_pz;
      // Hadronic hits
      int SimTrackID;
      int Sim_n_hadronic_Edep; // GEANT4 level simulated for now
      double Sim_Ehad_veto; // Total hadronic deposited energy in FD veto region
      vector<float> Sim_hadronic_hit_x;
      vector<float> Sim_hadronic_hit_y;
      vector<float> Sim_hadronic_hit_z;
      vector<float> Sim_hadronic_hit_Edep;
      // Feed to geoeff
      vector<float> HadronHitEdeps; // MeV
      vector<float> HadronHitPoss;  // [cm]
      // These number defs should reside in DUNE ND GEO code so that we can control !!!
      // ND LAr detector off-axis choices for each FD evt, unit: cm
      vector<double> ND_LAr_dtctr_pos_vec = {-2800, -2575, -2400, -2175, -2000, -1775, -1600, -1375, -1200, -975, -800, -575, -400, -175, 0};
      // Vtx x choices for each FD evt in ND LAr: unit: cm
      vector<double> ND_vtx_vx_vec = {-299, -292, -285, -278, -271, -264, -216, -168, -120, -72, -24, 24, 72, 120, 168, 216, 264, 271, 278, 285, 292, 299};

      // Intermediate vars

      // Step 3
      double ND_OffAxis_Unrotated_Sim_mu_start_pos[3]; // Position of the muon trajectory at start point [cm]
      vector<double> ND_OffAxis_Unrotated_Sim_mu_start_v; // Vector of ND_OffAxis_Unrotated_Sim_mu_start_pos in (x1,y1,z1,x2,y2,z2,......) order
      vector<vector<double>> ND_OffAxis_Unrotated_Sim_mu_start_v_vtx; // nested vector: <vtx_pos<ND_OffAxis_Unrotated_Sim_mu_start_pos>>
      vector<vector<vector<double>>> ND_OffAxis_Unrotated_Sim_mu_start_v_LAr; // nested vector: <ND_LAr_pos<vtx_pos<ND_OffAxis_Unrotated_Sim_mu_start_pos>>>

      vector<double> ND_OffAxis_Unrotated_Sim_hadronic_hit_v; // Position of each energy deposit [cm]
      vector<vector<double>> ND_OffAxis_Unrotated_Sim_hadronic_hits_v; // Position of each energy deposit [cm] : <ihadhit < ND_OffAxis_Unrotated_Sim_hadronic_hit_v>>

      // Step 4
      double ND_OffAxis_Sim_mu_start_mom[3]; // Momentum of the muon trajectory at start point on the x-axis [GeV]
      vector<double> ND_OffAxis_Sim_mu_start_p; // Vector of ND_OffAxis_Sim_mu_start_mom in (x1,y1,z1,x2,y2,z2,......) order
      vector<vector<double>> ND_OffAxis_Sim_mu_start_p_vtx; // nested vector: <vtx_pos<ND_OffAxis_Sim_mu_start_mom>>
      vector<vector<vector<double>>> ND_OffAxis_Sim_mu_start_p_LAr; // nested vector: <ND_LAr_pos<vtx_pos<ND_OffAxis_Sim_mu_start_mom>>>

      vector<double> ND_OffAxis_Sim_hadronic_hit_v; //order is differert from previous
      vector<vector<double>> ND_OffAxis_Sim_hadronic_hits_v; // Position of each energy deposit [cm]: <ihadhit < hadronic hits xyz>>

      // Step 5
      // Nested vector (vetoSize > vetoEnergy > 64_bit_throw_result)
      std::vector<std::vector<std::vector<uint64_t>>> hadron_throw_result;
      // vtx_pos > vetoSize > vetoEnergy > 64_bit_throw_result
      std::vector<std::vector<std::vector<std::vector<uint64_t>>>> hadron_throw_result_vtx;
      // LAr_pos > vtx_pos > vetoSize > vetoEnergy > 64_bit_throw_result
      std::vector<std::vector<std::vector<std::vector<std::vector<uint64_t>>>>> hadron_throw_result_LAr;

  }; // class CAFMaker


  //------------------------------------------------------------------------------
  CAFMaker::CAFMaker(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
    fMVASelectLabel = pset.get<std::string>("MVASelectLabel");
    fMVASelectNueLabel = pset.get<std::string>("MVASelectNueLabel");
    fMVASelectNumuLabel = pset.get<std::string>("MVASelectNumuLabel");
    fCVNLabel = pset.get<std::string>("CVNLabel");
    fRegCNNLabel = pset.get<std::string>("RegCNNLabel");

    fEnergyRecoNueLabel = pset.get<std::string>("EnergyRecoNueLabel");
    fEnergyRecoNumuLabel = pset.get<std::string>("EnergyRecoNumuLabel");

    // Get DUNErw stuff from its fhicl, which should be included on the CAFMaker config file
    if( !pset.has_key("generated_systematic_provider_configuration") ) {
     std::cout << "[ERROR]: Could not find producer key: "
                  "\"generated_systematic_provider_configuration\". This should "
                  "contain a list of configured systematic providers generated by "
                  "GenerateSystProviderConfig." << std::endl;
     return;
    }

    fhicl::ParameterSet syst_provider_config = pset.get<fhicl::ParameterSet>("generated_systematic_provider_configuration");

    // TODO - this was crashing with NULL genie Registry
    //    fSystProviders = systtools::ConfigureISystProvidersFromParameterHeaders(syst_provider_config);


    if(pset.get<bool>("CreateCAF", true)){
      fOutFile = new TFile("caf.root", "RECREATE");
    }

    if(pset.get<bool>("CreateFlatCAF", true)){
      // LZ4 is the fastest format to decompress. I get 3x faster loading with
      // this compared to the default, and the files are only slightly larger.
      fFlatFile = new TFile("flatcaf.root", "RECREATE", "",
                            ROOT::CompressionSettings(ROOT::kLZ4, 1));
    }

    geom = lar::providerFrom<geo::Geometry>();

  }

  //------------------------------------------------------------------------------
  caf::CAFMaker::~CAFMaker()
  {
  }

  //------------------------------------------------------------------------------
  void CAFMaker::beginJob()
  {
    if(fOutFile){
      fOutFile->cd();
      fTree = new TTree("cafTree", "cafTree");

      // Create the branch. We will update the address before we write the tree
      caf::StandardRecord* rec = 0;
      fTree->Branch("rec", &rec);

      ThrowsFD = new TTree("geoEffThrows", "geoEffThrows");
      ThrowsFD->Branch("seed",      &seed);
      ThrowsFD->Branch("throwVtxY", &throwVtxY);
      ThrowsFD->Branch("throwVtxZ", &throwVtxZ);
      ThrowsFD->Branch("throwRot",  &throwRot);

      // A separate tree to store throwresult and muon stuff for NN

      // Generate new dictionary for nested vectors to write to TTree
      gInterpreter->GenerateDictionary("vector<vector<vector<double> > >", "vector");
      gInterpreter->GenerateDictionary("vector<vector<vector<vector<vector<uint64_t> > > > >", "vector");

      ThrowResultsFD = new TTree("throwResults", "throwResults");
      ThrowResultsFD->Branch("FD_Sim_mu_start_vx",                  &Sim_mu_start_vx,               "FD_Sim_mu_start_vx/D"); // for FD fiducial volume cut
      ThrowResultsFD->Branch("FD_Sim_mu_start_vy",                  &Sim_mu_start_vy,               "FD_Sim_mu_start_vy/D");
      ThrowResultsFD->Branch("FD_Sim_mu_start_vz",                  &Sim_mu_start_vz,               "FD_Sim_mu_start_vz/D");
      ThrowResultsFD->Branch("FD_Sim_n_hadronic_hits",              &Sim_n_hadronic_Edep,           "FD_Sim_n_hadronic_hits/I"); // for offline analysis cut
      ThrowResultsFD->Branch("FD_Sim_Ehad_veto",                    &Sim_Ehad_veto,                 "FD_Sim_Ehad_veto/D");
      ThrowResultsFD->Branch("FD_evt_NDLAr_OffAxis_Sim_mu_start_v", &ND_OffAxis_Unrotated_Sim_mu_start_v_LAr); // for lepton NN
      ThrowResultsFD->Branch("FD_evt_NDLAr_OffAxis_Sim_mu_start_p", &ND_OffAxis_Sim_mu_start_p_LAr);
      ThrowResultsFD->Branch("FD_evt_hadron_throw_result_NDLAr",    &hadron_throw_result_LAr); // for FD hadronic GEC in ND
    }

    if(fFlatFile){
      fFlatFile->cd();
      fFlatTree = new TTree("cafTree", "cafTree");

      fFlatRecord = new flat::Flat<caf::StandardRecord>(fFlatTree, "rec", "", 0);
    }


    fMetaTree = new TTree("meta", "meta");

    fMetaTree->Branch("pot", &meta_pot, "pot/D");
    fMetaTree->Branch("run", &meta_run, "run/I");
    fMetaTree->Branch("subrun", &meta_subrun, "subrun/I");
    fMetaTree->Branch("version", &meta_version, "version/I");

    meta_pot = 0.;
    meta_version = 1;


    caf::SRGlobal global;

    // make DUNErw variables
    for( auto &sp : fSystProviders ) {
      for(const systtools::SystParamHeader& head: sp->GetSystMetaData()){
        std::cout << "Adding reweight " << head.systParamId << " for " << head.prettyName << " with " << head.paramVariations.size() << " shifts" << std::endl;

        caf::SRSystParamHeader hdr;
        hdr.nshifts = head.paramVariations.size();
        hdr.name = head.prettyName;
        hdr.id = head.systParamId; // TODO is this necessary?

        global.wgts.params.push_back(hdr);
      }
    }

    if(fOutFile) AddGlobalTreeToFile(fOutFile, global);
    if(fFlatFile) AddGlobalTreeToFile(fFlatFile, global);
  }

  //------------------------------------------------------------------------------
  void CAFMaker::AddGlobalTreeToFile(TFile* f, SRGlobal& global)
  {
    f->cd();
    TTree* globalTree = new TTree("globalTree", "globalTree");
    caf::SRGlobal* pglobal = &global;
    TBranch* br = globalTree->Branch("global", "caf::SRGlobal", &pglobal);
    if(!br) abort();
    globalTree->Fill();
    globalTree->Write();
  }

  //------------------------------------------------------------------------------
  void CAFMaker::beginSubRun(const art::SubRun& sr)
  {
    auto pots = sr.getHandle< sumdata::POTSummary >("generator");
    if( pots ) meta_pot += pots->totpot;
  }

  //------------------------------------------------------------------------------
  void CAFMaker::analyze(art::Event const & evt)
  {
    caf::StandardRecord sr;

    if(fTree){
      caf::StandardRecord* psr = &sr;
      fTree->SetBranchAddress("rec", &psr);
    }

    auto pidin = evt.getHandle<dunemva::MVASelectPID>(fMVASelectLabel);
    auto pidinnue = evt.getHandle<dunemva::MVASelectPID>(fMVASelectNueLabel);
    auto pidinnumu = evt.getHandle<dunemva::MVASelectPID>(fMVASelectNumuLabel);
    art::InputTag itag1(fCVNLabel, "cvnresult");
    auto cvnin = evt.getHandle<std::vector<cvn::Result>>(itag1);
    art::InputTag itag2(fRegCNNLabel, "regcnnresult");
    auto regcnnin = evt.getHandle<std::vector<cnn::RegCNNResult>>(itag2);
    auto ereconuein = evt.getHandle<dune::EnergyRecoOutput>(fEnergyRecoNueLabel);
    auto ereconumuin = evt.getHandle<dune::EnergyRecoOutput>(fEnergyRecoNumuLabel);

    sr.run = evt.id().run();
    sr.subrun = evt.id().subRun();
    sr.event = evt.id().event();
    meta_run = sr.run;
    meta_subrun = sr.subrun;

    if( !pidin.failedToGet() ) {
      sr.mvaresult = pidin->pid;

      //Fill MVA reco stuff
      sr.Ev_reco_nue     = ereconuein->fNuLorentzVector.E();
      sr.RecoLepEnNue    = ereconuein->fLepLorentzVector.E();
      sr.RecoHadEnNue    = ereconuein->fHadLorentzVector.E();
      sr.RecoMethodNue   = ereconuein->recoMethodUsed;
      sr.Ev_reco_numu    = ereconumuin->fNuLorentzVector.E();
      sr.RecoLepEnNumu   = ereconumuin->fLepLorentzVector.E();
      sr.RecoHadEnNumu   = ereconumuin->fHadLorentzVector.E();
      sr.RecoMethodNumu  = ereconumuin->recoMethodUsed;
      sr.LongestTrackContNumu = ereconumuin->longestTrackContained;
      sr.TrackMomMethodNumu   = ereconumuin->trackMomMethod;
    }

    if( !pidinnue.failedToGet() ) {
      sr.mvanue = pidinnue->pid;
    }

    if( !pidinnumu.failedToGet() ) {
      sr.mvanumu = pidinnumu->pid;
    }

    if( !cvnin.failedToGet() ) {
      //using i = cvn::Interaction;
      //if(cvnin->empty() || (*cvnin)[0].fOutput.size() <= i::kNutauOther){
      if(cvnin->empty()){
        sr.CVNResultIsAntineutrino = sr.CVNResultNue = sr.CVNResultNumu = sr.CVNResultNutau = sr.CVNResultNC = \
        sr.CVNResult0Protons = sr.CVNResult1Protons = sr.CVNResult2Protons = sr.CVNResultNProtons = \
        sr.CVNResult0Pions = sr.CVNResult1Pions = sr.CVNResult2Pions = sr.CVNResultNPions = \
        sr.CVNResult0Pizeros = sr.CVNResult1Pizeros = sr.CVNResult2Pizeros = sr.CVNResultNPizeros = \
        sr.CVNResult0Neutrons = sr.CVNResult1Neutrons = sr.CVNResult2Neutrons = sr.CVNResultNNeutrons = -3;
      }
      else{
        //const std::vector<float>& v = (*cvnin)[0].fOutput;
        //sr.CVNResultNue = v[i::kNueQE] + v[i::kNueRes] + v[i::kNueDIS] + v[i::kNueOther];
        //sr.CVNResultNumu = v[i::kNumuQE] + v[i::kNumuRes] + v[i::kNumuDIS] + v[i::kNumuOther];
        //sr.CVNResultNutau = v[i::kNutauQE] + v[i::kNutauRes] + v[i::kNutauDIS] + v[i::kNutauOther]

        sr.CVNResultIsAntineutrino = (*cvnin)[0].GetIsAntineutrinoProbability();

        sr.cvnnue = (*cvnin)[0].GetNueProbability();
        sr.cvnnumu = (*cvnin)[0].GetNumuProbability();
        sr.cvnnutau = (*cvnin)[0].GetNutauProbability();
        sr.cvnnc = (*cvnin)[0].GetNCProbability();

        sr.CVNResult0Protons = (*cvnin)[0].Get0protonsProbability();
        sr.CVNResult1Protons = (*cvnin)[0].Get1protonsProbability();
        sr.CVNResult2Protons = (*cvnin)[0].Get2protonsProbability();
        sr.CVNResultNProtons = (*cvnin)[0].GetNprotonsProbability();

        sr.CVNResult0Pions = (*cvnin)[0].Get0pionsProbability();
        sr.CVNResult1Pions = (*cvnin)[0].Get1pionsProbability();
        sr.CVNResult2Pions = (*cvnin)[0].Get2pionsProbability();
        sr.CVNResultNPions = (*cvnin)[0].GetNpionsProbability();

        sr.CVNResult0Pizeros = (*cvnin)[0].Get0pizerosProbability();
        sr.CVNResult1Pizeros = (*cvnin)[0].Get1pizerosProbability();
        sr.CVNResult2Pizeros = (*cvnin)[0].Get2pizerosProbability();
        sr.CVNResultNPizeros = (*cvnin)[0].GetNpizerosProbability();

        sr.CVNResult0Neutrons = (*cvnin)[0].Get0neutronsProbability();
        sr.CVNResult1Neutrons = (*cvnin)[0].Get1neutronsProbability();
        sr.CVNResult2Neutrons = (*cvnin)[0].Get2neutronsProbability();
        sr.CVNResultNNeutrons = (*cvnin)[0].GetNneutronsProbability();
      }
    }

    sr.RegCNNNueE = -1.;  // initializing
    if(!regcnnin.failedToGet()){
      if (!regcnnin->empty()){
        const std::vector<float>& v = (*regcnnin)[0].fOutput;
        sr.RegCNNNueE = v[0];
      }
    }

    std::vector< art::Ptr<simb::MCTruth> > truth;
    auto mct = evt.getHandle< std::vector<simb::MCTruth> >("generator");
    if ( mct )
      art::fill_ptr_vector(truth, mct);
    else
      mf::LogWarning("CAFMaker") << "No MCTruth.";

    std::vector< art::Ptr<simb::MCFlux> > flux;
    auto mcf = evt.getHandle< std::vector<simb::MCFlux> >("generator");
    if ( mcf )
      art::fill_ptr_vector(flux, mcf);
    else
      mf::LogWarning("CAFMaker") << "No MCFlux.";
/*
    std::vector< art::Ptr<simb::GTruth> > gtru;
    auto gt = evt.getHandle< std::vector<simb::GTruth> >("generator");
    if ( gt )
      art::fill_ptr_vector(gtru, gt);
    else
      mf::LogWarning("CAFMaker") << "No GTruth.";
*/

    for(size_t i=0; i<truth.size(); i++){

      if(i>1){
        mf::LogWarning("CAFMaker") << "Skipping MC truth index " << i;
        continue;
      }

      sr.isFD   = 1; // always FD
      sr.isFHC  = 999; // don't know how to get this?
      sr.isCC   = !(truth[i]->GetNeutrino().CCNC());  // ccnc is 0=CC 1=NC
      sr.nuPDG  = truth[i]->GetNeutrino().Nu().PdgCode();
      sr.nuPDGunosc = flux[i]->fntype;
      sr.mode   = truth[i]->GetNeutrino().Mode(); //0=QE/El, 1=RES, 2=DIS, 3=Coherent production; this is different than mode in ND
      sr.Ev     = truth[i]->GetNeutrino().Nu().E();
      sr.Q2     = truth[i]->GetNeutrino().QSqr();
      sr.W      = truth[i]->GetNeutrino().W();
      sr.X      = truth[i]->GetNeutrino().X();
      sr.Y      = truth[i]->GetNeutrino().Y();
      sr.NuMomX = truth[i]->GetNeutrino().Nu().Momentum().X();
      sr.NuMomY = truth[i]->GetNeutrino().Nu().Momentum().Y();
      sr.NuMomZ = truth[i]->GetNeutrino().Nu().Momentum().Z();

      sr.vtx_x  = truth[i]->GetNeutrino().Lepton().Vx();
      sr.vtx_y  = truth[i]->GetNeutrino().Lepton().Vy();
      sr.vtx_z  = truth[i]->GetNeutrino().Lepton().Vz();

      //Lepton stuff
      sr.LepPDG   = truth[i]->GetNeutrino().Lepton().PdgCode();
      sr.LepMomX  = truth[i]->GetNeutrino().Lepton().Momentum().X();
      sr.LepMomY  = truth[i]->GetNeutrino().Lepton().Momentum().Y();
      sr.LepMomZ  = truth[i]->GetNeutrino().Lepton().Momentum().Z();
      sr.LepE     = truth[i]->GetNeutrino().Lepton().Momentum().T();
      sr.LepNuAngle = truth[i]->GetNeutrino().Nu().Momentum().Vect().Angle(truth[i]->GetNeutrino().Lepton().Momentum().Vect());

      sr.nP     = 0;
      sr.nN     = 0;
      sr.nipip  = 0;
      sr.nipim  = 0;
      sr.nipi0  = 0;
      sr.nikp   = 0;
      sr.nikm   = 0;
      sr.nik0   = 0;
      sr.niem   = 0;
      sr.niother = 0;
      sr.nNucleus = 0;
      sr.nUNKNOWN = 0;

      sr.eP = 0.;
      sr.eN = 0.;
      sr.ePip = 0.;
      sr.ePim = 0.;
      sr.ePi0 = 0.;
      sr.eOther = 0.;

      for( int p = 0; p < truth[i]->NParticles(); p++ ) {
        if( truth[i]->GetParticle(p).StatusCode() == genie::kIStStableFinalState ) {

          int pdg = truth[i]->GetParticle(p).PdgCode();
          double ke = truth[i]->GetParticle(p).E() - truth[i]->GetParticle(p).Mass();
          if     ( pdg == genie::kPdgProton ) {
            sr.nP++;
            sr.eP += ke;
          } else if( pdg == genie::kPdgNeutron ) {
            sr.nN++;
            sr.eN += ke;
          } else if( pdg == genie::kPdgPiP ) {
            sr.nipip++;
            sr.ePip += ke;
          } else if( pdg == genie::kPdgPiM ) {
            sr.nipim++;
            sr.ePim += ke;
          } else if( pdg == genie::kPdgPi0 ) {
            sr.nipi0++;
            sr.ePi0 += ke;
          } else if( pdg == genie::kPdgKP ) {
            sr.nikp++;
            sr.eOther += ke;
          } else if( pdg == genie::kPdgKM ) {
            sr.nikm++;
            sr.eOther += ke;
          } else if( pdg == genie::kPdgK0 || pdg == genie::kPdgAntiK0 || pdg == genie::kPdgK0L || pdg == genie::kPdgK0S ) {
            sr.nik0++;
            sr.eOther += ke;
          } else if( pdg == genie::kPdgGamma ) {
            sr.niem++;
            sr.eOther += ke;
          } else if( genie::pdg::IsHadron(pdg) ) {
            sr.niother++; // charm mesons, strange and charm baryons, antibaryons, etc.
            sr.eOther += ke;
          } else if( genie::pdg::IsIon(pdg) ) {
            sr.nNucleus++;
          } else {
            sr.nUNKNOWN++;
          }

        }
      }

      // Reweighting variables

      // Consider
      //systtools::ScrubUnityEventResponses(er);

      sr.total_xsSyst_cv_wgt = 1;

      for(auto &sp : fSystProviders ) {
        std::unique_ptr<systtools::EventAndCVResponse> syst_resp = sp->GetEventVariationAndCVResponse(evt);
        if( !syst_resp ) {
          std::cout << "[ERROR]: Got nullptr systtools::EventResponse from provider "
                    << sp->GetFullyQualifiedName();
          abort();
        }

        // The iteration order here is the same as how we filled SRGlobal, so
        // no need to do any work to make sure they align.
        //
        // NB this will all go wrong if we ever support more than one MCTruth
        // per event.
        for(const std::vector<systtools::VarAndCVResponse>& resp: *syst_resp){
          for(const systtools::VarAndCVResponse& it: resp){
            // Need begin/end to convert double to float
            sr.xsSyst_wgt.emplace_back(it.responses.begin(), it.responses.end());
            sr.cvwgt.push_back(it.CV_response);
            sr.total_xsSyst_cv_wgt *= it.CV_response;
          }
        }
      }
    } // loop through MC truth i

    // ============================================================
    // DUNE-PRISM geometric efficiency correction starts here
    // If want to modify this section,
    // contact DUNE-PRISM group or Wei Shi: wei.shi.1@stonybrook.edu
    // ============================================================

    // Process Sim MCparticles info at GEANT 4 level
    auto particleHandle = evt.getValidHandle<std::vector<simb::MCParticle>>("largeant");
    if ( ! particleHandle ) mf::LogWarning("CAFMaker") << "No MCParticle.";
    // Create a map pf MCParticle to its track ID, to be used below
    std::map<int, const simb::MCParticle*> particleMap;

    Sim_mu_start_vx = -9999.;
    Sim_mu_start_vy = -9999.;
    Sim_mu_start_vz = -9999.;
    Sim_mu_start_px = -9999.;
    Sim_mu_start_py = -9999.;
    Sim_mu_start_pz = -9999.;

    // Loop over MCParticle
    for ( auto const& particle : (*particleHandle) ) {
      SimTrackID = particle.TrackId();
      particleMap[SimTrackID] = &particle;

      // Primary muon in the event
      if ( particle.Process() == "primary" && abs(particle.PdgCode()) == 13 ) {

        // For trajectories, as for vectors and arrays, the first point is #0, not #1.
        Sim_mu_start_vx = particle.Vx(0);
        Sim_mu_start_vy = particle.Vy(0);
        Sim_mu_start_vz = particle.Vz(0);
        Sim_mu_start_px = particle.Px(0);
        Sim_mu_start_py = particle.Py(0);
        Sim_mu_start_pz = particle.Pz(0);

      } // end primary muon

    } // end loop over MCParticle

    // Get all the simulated channels for the event. These channels
    // include the energy deposited for each simulated track.
    auto simChannelHandle = evt.getValidHandle<std::vector<sim::SimChannel>>("largeant");
    if ( ! simChannelHandle ) mf::LogWarning("CAFMaker") << "No SimChannel.";

    Sim_n_hadronic_Edep = 0;
    Sim_Ehad_veto       = 0.;
    Sim_hadronic_hit_x.clear();
    Sim_hadronic_hit_y.clear();
    Sim_hadronic_hit_z.clear();
    Sim_hadronic_hit_Edep.clear();

    // Loop over the SimChannel objects in the event to look at the energy deposited by particle's track.
    for ( auto const& channel : (*simChannelHandle) ) {
      // Get the numeric ID associated with this channel.
      auto const channelNumber = channel.Channel();

      // Each channel has a map inside it that connects a time slice to energy deposits in the detector.
      // The full type of this map is std::map<unsigned short, std::vector<sim::IDE>>; we'll use "auto" here
      auto const& timeSlices = channel.TDCIDEMap();
      for ( auto const& timeSlice : timeSlices ) {
        // For the timeSlices map, the 'first' is a time slice number; The 'second' is a vector of IDE objects.
        auto const& energyDeposits = timeSlice.second;

        // An "energy deposit" object stores how much charge/energy was deposited in a small volume, by which particle, and where.
        // The type of 'energyDeposit' will be sim::IDE, here use auto.
        for ( auto const& energyDeposit : energyDeposits )
        {
          auto search = particleMap.find( energyDeposit.trackID );
          if ( search == particleMap.end() ) continue;

          // "search" points to a pair in the map: <track ID, MCParticle*>
          const simb::MCParticle& particle = *((*search).second);

          // If it's not a primary lepton, count as hadronic energy deposit
          if ( ! ( particle.Process() == "primary" && ( abs(particle.PdgCode()) == 11 || abs(particle.PdgCode()) == 13 || abs(particle.PdgCode()) == 15 ) ) ){

            // Here navigate via channel -> wire -> plane ID, and require planeID to be 0.
            // But apparently other methods exist as well
            std::vector<geo::WireID> const Wires = geom->ChannelToWire(channelNumber);
            if ( Wires[0].planeID().Plane == 0 ) {

              // Store position and E for each deposit
              Sim_hadronic_hit_x.push_back(energyDeposit.x);
              Sim_hadronic_hit_y.push_back(energyDeposit.y);
              Sim_hadronic_hit_z.push_back(energyDeposit.z);
              Sim_hadronic_hit_Edep.push_back(energyDeposit.energy);
            } // end if access plane

          } // end if hadronic

        } // end loop over energyDeposit

      } // end loop over timeslice

    } // end loop over channel

    Sim_n_hadronic_Edep = Sim_hadronic_hit_x.size();

    // Calculate FD hadronic energy in 30cm veto region
    for ( int ihadhit = 0; ihadhit < Sim_n_hadronic_Edep; ihadhit++ ){
      // Veto region size: 30 cm from the active volume
      if ( ( Sim_hadronic_hit_x.at(ihadhit) > FDActiveVol_min[0] && Sim_hadronic_hit_x.at(ihadhit) < FDActiveVol_min[0] + 30 ) ||
           ( Sim_hadronic_hit_y.at(ihadhit) > FDActiveVol_min[1] && Sim_hadronic_hit_y.at(ihadhit) < FDActiveVol_min[1] + 30 ) ||
           ( Sim_hadronic_hit_z.at(ihadhit) > FDActiveVol_min[2] && Sim_hadronic_hit_z.at(ihadhit) < FDActiveVol_min[2] + 30 ) ||
           ( Sim_hadronic_hit_x.at(ihadhit) > FDActiveVol_max[0] - 30 && Sim_hadronic_hit_x.at(ihadhit) < FDActiveVol_max[0] ) ||
           ( Sim_hadronic_hit_y.at(ihadhit) > FDActiveVol_max[1] - 30 && Sim_hadronic_hit_y.at(ihadhit) < FDActiveVol_max[1] ) ||
           ( Sim_hadronic_hit_z.at(ihadhit) > FDActiveVol_max[2] - 30 && Sim_hadronic_hit_z.at(ihadhit) < FDActiveVol_max[2] ) )
           Sim_Ehad_veto += Sim_hadronic_hit_Edep.at(ihadhit);
    } // end loop over hadron E deposits

    //
    // Now all inputs are ready, start geo eff calculation
    //

    // Mean neutrino production point (beam coordinate) on z axis as a function of ND off-axis position
    int nOffAxisPoints = sizeof(OffAxisPoints)/sizeof(double);
    int nmeanPDPZ = sizeof(meanPDPZ)/sizeof(double);
    if( nOffAxisPoints != nmeanPDPZ ) {
      std::cout << "[ERROR]: Number of offaxis points and decay positions doesn't match " << std::endl;
      abort();
    }
    TGraph* gDecayZ = new TGraph(nOffAxisPoints, OffAxisPoints, meanPDPZ);

    //
    // Get beam parameters: hardcoded here !!! Eventually should read this number from XML file and/or reside in ND geo code
    //

    double beamLineRotation = -0.101; // unit: rad, clockwise rotate beamline around ND local x axis
    double beamRefDetCoord[3] = {0.0, 0.05387, 6.6}; // unit: m, NDLAr detector coordinate origin is (0, 0, 0)
    double detRefBeamCoord[3] = {0., 0., 574.}; // unit: m, beam coordinate origin is (0, 0, 0)
    decayXdetCoord = beamRefDetCoord[0] - detRefBeamCoord[0]; // Calculate neutrino production x in detector coordinate

    // Initialize geometric efficiency module
    random_device rd; // generate random seed number
    seed = rd();
    geoEff * eff = new geoEff(seed, false);
    eff->setNthrows(N_throws);
    // Rotate w.r.t. neutrino direction, rather than fixed beam direction
    eff->setUseFixedBeamDir(false);
    // 30 cm veto
    eff->setVetoSizes(vector<float>(1, 30.));
    // 30 MeV
    eff->setVetoEnergyThresholds(vector<float>(1, 30.));
    // Active detector dimensions for ND
    eff->setActiveX(NDActiveVol_min[0], NDActiveVol_max[0]);
    eff->setActiveY(NDActiveVol_min[1], NDActiveVol_max[1]);
    eff->setActiveZ(NDActiveVol_min[2], NDActiveVol_max[2]);
    // Range for translation throws. Use full active volume but fix X.
    eff->setRangeX(-1, -1);
    eff->setRandomizeX(false);
    eff->setRangeY(NDActiveVol_min[1], NDActiveVol_max[1]);
    eff->setRangeZ(NDActiveVol_min[2], NDActiveVol_max[2]);
    // Set offset between MC coordinate system and det volumes
    eff->setOffsetX(NDLAr_OnAxis_offset[0]);
    eff->setOffsetY(NDLAr_OnAxis_offset[1]);
    eff->setOffsetZ(NDLAr_OnAxis_offset[2]);

    // Produce N random throws defined at setNthrows(N)
    // Same throws applied for hadron below
    eff->throwTransforms();
    throwVtxY.clear();
    throwVtxZ.clear();
    throwRot.clear();
    throwVtxY = eff->getCurrentThrowTranslationsY();
    throwVtxZ = eff->getCurrentThrowTranslationsZ();
    throwRot  = eff->getCurrentThrowRotations();
    ThrowsFD->Fill();

    //
    // Step 1 - FD to ND: correct for earth curvature
    // Step 2 - Put FD event at the beam center in ND LAr
    //

    // First do these two steps for muon
    // Step 1 for muon
    // New position and momentum after earth curvature corr.
    double ND_RandomVtx_Sim_mu_start_v[3];
    double ND_RandomVtx_Sim_mu_start_p[3];
    double FD_Sim_mu_start_v[3] = {Sim_mu_start_vx, Sim_mu_start_vy, Sim_mu_start_vz};
    double FD_Sim_mu_start_p[3] = {Sim_mu_start_px, Sim_mu_start_py, Sim_mu_start_pz};
    for(int i=0; i<3; i++) ND_RandomVtx_Sim_mu_start_v[i] = eff->getEarthCurvature(FD_Sim_mu_start_v, beamLineRotation, i);
    for(int i=0; i<3; i++) ND_RandomVtx_Sim_mu_start_p[i] = eff->getEarthCurvature(FD_Sim_mu_start_p, beamLineRotation, i);
    // Step 2 for muon
    // No operation on muon momentum as it conserves in translation
    double ND_OnAxis_Sim_mu_start_v[3] = {beamRefDetCoord[0]*100., beamRefDetCoord[1]*100., beamRefDetCoord[2]*100.};

    // Then do these two steps for hadronic hits
    double ND_RandomVtx_Sim_hadronic_hit_v[3]; // New position of each energy deposit [cm] after earth curvature corr.
    vector<double> ND_OnAxis_Sim_hadronic_hit_v; // New position of each energy deposit [cm] after translation to beam center at ND LAr
    vector<vector<double>> ND_OnAxis_Sim_hadronic_hits_v; // all deposits pos

    for ( int ihadhit = 0; ihadhit < Sim_n_hadronic_Edep; ihadhit++ ){
      // Step 1 for each hadronic hit
      double Sim_hadronic_hit_pos[3] = {Sim_hadronic_hit_x.at(ihadhit), Sim_hadronic_hit_y.at(ihadhit), Sim_hadronic_hit_z.at(ihadhit)};
      for (int i =0; i<3; i++) ND_RandomVtx_Sim_hadronic_hit_v[i] = eff->getEarthCurvature(Sim_hadronic_hit_pos, beamLineRotation, i);
      // Step 2 for each hadronic hit
      for (int i =0; i<3; i++) ND_OnAxis_Sim_hadronic_hit_v.emplace_back(eff->getTranslations(ND_RandomVtx_Sim_hadronic_hit_v, ND_RandomVtx_Sim_mu_start_v, ND_OnAxis_Sim_mu_start_v, i));
      ND_OnAxis_Sim_hadronic_hits_v.emplace_back(ND_OnAxis_Sim_hadronic_hit_v);
      ND_OnAxis_Sim_hadronic_hit_v.clear();
    }

    // Set On-axis vertex where beam crosses ND LAr center
    eff->setOnAxisVertex(ND_OnAxis_Sim_mu_start_v[0], ND_OnAxis_Sim_mu_start_v[1], ND_OnAxis_Sim_mu_start_v[2]);

    // Put FD event in many ND LAr positions
    for ( double i_ND_off_axis_pos : ND_LAr_dtctr_pos_vec ){
      eff->setOffsetX(NDLAr_OnAxis_offset[0] + i_ND_off_axis_pos); // update offset since we moved the detector

      // Also put FD event in many positions in ND LAr
      for ( double i_vtx_vx : ND_vtx_vx_vec ) {

        // Interpolate event neutrino production point (in beam coordinate)
        decayZbeamCoord = gDecayZ->Eval( i_ND_off_axis_pos + i_vtx_vx - detRefBeamCoord[0] );

        // Calculate neutrino production point in detector coordinate
        decayYdetCoord = beamRefDetCoord[1] - detRefBeamCoord[1]*cos(beamLineRotation) + ( decayZbeamCoord - detRefBeamCoord[2] )*sin(beamLineRotation);
        decayZdetCoord = beamRefDetCoord[2] + detRefBeamCoord[1]*sin(beamLineRotation) + ( decayZbeamCoord - detRefBeamCoord[2] )*cos(beamLineRotation);
        // Set production point in unit: cm
        eff->setDecayPos(decayXdetCoord*100., decayYdetCoord*100., decayZdetCoord*100.);

        //
        // Step 3 - translate FD event from OnAxis NDLAr to OffAxis NDLAr (but no rotation yet --> next step)
        //

        // Momentum conserves at this step, only affect positions
        ND_OffAxis_Unrotated_Sim_mu_start_pos[0] = ND_OnAxis_Sim_mu_start_v[0] + i_ND_off_axis_pos + i_vtx_vx;
        ND_OffAxis_Unrotated_Sim_mu_start_pos[1] = ND_OnAxis_Sim_mu_start_v[1];
        ND_OffAxis_Unrotated_Sim_mu_start_pos[2] = ND_OnAxis_Sim_mu_start_v[2];

        ND_OffAxis_Unrotated_Sim_mu_start_v.emplace_back(ND_OffAxis_Unrotated_Sim_mu_start_pos[0]);
        ND_OffAxis_Unrotated_Sim_mu_start_v.emplace_back(ND_OffAxis_Unrotated_Sim_mu_start_pos[1]);
        ND_OffAxis_Unrotated_Sim_mu_start_v.emplace_back(ND_OffAxis_Unrotated_Sim_mu_start_pos[2]);
        ND_OffAxis_Unrotated_Sim_mu_start_v_vtx.emplace_back(ND_OffAxis_Unrotated_Sim_mu_start_v);
        ND_OffAxis_Unrotated_Sim_mu_start_v.clear();

        // Translation doesn't affect muon p, no operation (still ND_RandomVtx_Sim_mu_start_p)

        for ( int ihadhit = 0; ihadhit < Sim_n_hadronic_Edep; ihadhit++ ){
          double ND_OnAxis_Sim_hadronic_hit_pos[3] = {ND_OnAxis_Sim_hadronic_hits_v[ihadhit][0], ND_OnAxis_Sim_hadronic_hits_v[ihadhit][1], ND_OnAxis_Sim_hadronic_hits_v[ihadhit][2]};
          for (int i =0; i<3; i++) ND_OffAxis_Unrotated_Sim_hadronic_hit_v.emplace_back(eff->getTranslations(ND_OnAxis_Sim_hadronic_hit_pos, ND_OnAxis_Sim_mu_start_v, ND_OffAxis_Unrotated_Sim_mu_start_pos, i));
          ND_OffAxis_Unrotated_Sim_hadronic_hits_v.emplace_back(ND_OffAxis_Unrotated_Sim_hadronic_hit_v);
          ND_OffAxis_Unrotated_Sim_hadronic_hit_v.clear();
        }

        //
        // Step 4 - Complete step 3 by properly rotate the FD event
        //

        // Muon start point remain the same as step 3: ND_OffAxis_Unrotated_Sim_mu_start_pos
        eff->setOffAxisVertex(ND_OffAxis_Unrotated_Sim_mu_start_pos[0], ND_OffAxis_Unrotated_Sim_mu_start_pos[1], ND_OffAxis_Unrotated_Sim_mu_start_pos[2]);

        eff->setMuStartP(ND_RandomVtx_Sim_mu_start_p[0], ND_RandomVtx_Sim_mu_start_p[1], ND_RandomVtx_Sim_mu_start_p[2]); // because p is not impacted in step 2 & 3
        for(int i=0; i<3; i++) ND_OffAxis_Sim_mu_start_mom[i] = eff->getOffAxisMuStartP(i);
        ND_OffAxis_Sim_mu_start_p.emplace_back(ND_OffAxis_Sim_mu_start_mom[0]);
        ND_OffAxis_Sim_mu_start_p.emplace_back(ND_OffAxis_Sim_mu_start_mom[1]);
        ND_OffAxis_Sim_mu_start_p.emplace_back(ND_OffAxis_Sim_mu_start_mom[2]);
        ND_OffAxis_Sim_mu_start_p_vtx.emplace_back(ND_OffAxis_Sim_mu_start_p);
        ND_OffAxis_Sim_mu_start_p.clear();

        for ( int ihadhit = 0; ihadhit < Sim_n_hadronic_Edep; ihadhit++ ){
          eff->setHadronHitV(ND_OffAxis_Unrotated_Sim_hadronic_hits_v[ihadhit][0], ND_OffAxis_Unrotated_Sim_hadronic_hits_v[ihadhit][1], ND_OffAxis_Unrotated_Sim_hadronic_hits_v[ihadhit][2]);
          for (int i =0; i<3; i++) ND_OffAxis_Sim_hadronic_hit_v.emplace_back(eff->getOffAxisHadronHitV(i));
          ND_OffAxis_Sim_hadronic_hits_v.emplace_back(ND_OffAxis_Sim_hadronic_hit_v);
          ND_OffAxis_Sim_hadronic_hit_v.clear();
        }

        //
        // Step 5 - Generate random throws for FD event
        //
        HadronHitEdeps.clear();
        HadronHitPoss.clear();
        HadronHitEdeps.reserve(Sim_n_hadronic_Edep);
        HadronHitPoss.reserve(Sim_n_hadronic_Edep*3);

        for ( int ihadhit = 0; ihadhit < Sim_n_hadronic_Edep; ihadhit++ ){
          for (int i =0; i<3; i++) HadronHitPoss.emplace_back(ND_OffAxis_Sim_hadronic_hits_v[ihadhit][i]);
          HadronHitEdeps.emplace_back( Sim_hadronic_hit_Edep.at(ihadhit) );
        }

        eff->setVertex(ND_OffAxis_Unrotated_Sim_mu_start_pos[0], ND_OffAxis_Unrotated_Sim_mu_start_pos[1], ND_OffAxis_Unrotated_Sim_mu_start_pos[2]);
        eff->setHitSegEdeps(HadronHitEdeps);
        eff->setHitSegPoss(HadronHitPoss);

        // Set offset between MC coordinate system and det volumes
        eff->setOffAxisOffsetX(i_ND_off_axis_pos); // make sure had veto is correct
        eff->setOffAxisOffsetY(NDLAr_OnAxis_offset[1]);
        eff->setOffAxisOffsetZ(NDLAr_OnAxis_offset[2]);
        // Get hadron containment result after everything is set to ND coordinate sys
        // Do random throws regardless whether FD evt is contained in ND volume by setting a false flag
        hadron_throw_result = eff->getHadronContainmentThrows(false); // Every 64 throw results stored into a 64 bit unsigned int: 0101101...

        hadron_throw_result_vtx.emplace_back(hadron_throw_result);
        hadron_throw_result.clear();

        ND_OffAxis_Unrotated_Sim_hadronic_hits_v.clear();
        ND_OffAxis_Sim_hadronic_hits_v.clear();

      } // end loop over ND_vtx_vx_vec

      // These will write to FD CAF
      ND_OffAxis_Unrotated_Sim_mu_start_v_LAr.emplace_back(ND_OffAxis_Unrotated_Sim_mu_start_v_vtx);
      ND_OffAxis_Unrotated_Sim_mu_start_v_vtx.clear();
      ND_OffAxis_Sim_mu_start_p_LAr.emplace_back(ND_OffAxis_Sim_mu_start_p_vtx);
      ND_OffAxis_Sim_mu_start_p_vtx.clear();
      hadron_throw_result_LAr.emplace_back(hadron_throw_result_vtx);
      hadron_throw_result_vtx.clear();

    } // end loop over ND_LAr_dtctr_pos_vec

    ThrowResultsFD->Fill();

    // =====================================================
    // Here ends DUNE-PRISM geometric efficiency correction
    // =====================================================

    if(fTree){
      fTree->Fill();
    }

    if(fFlatTree){
      fFlatRecord->Clear();
      fFlatRecord->Fill(sr);
      fFlatTree->Fill();
    }
  }

  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  void CAFMaker::endSubRun(const art::SubRun& sr){
  }

  void CAFMaker::endJob()
  {
    fMetaTree->Fill();

    if(fOutFile){
      fOutFile->cd();
      fTree->Write();
      fMetaTree->Write();
      ThrowsFD->Write();
      ThrowResultsFD->Write();
      fOutFile->Close();
    }

    if(fFlatFile){
      fFlatFile->cd();
      fFlatTree->Write();
      fMetaTree->Write();
      fFlatFile->Close();
    }
  }

  DEFINE_ART_MODULE(CAFMaker)

} // namespace caf

#endif // CAFMaker_H
