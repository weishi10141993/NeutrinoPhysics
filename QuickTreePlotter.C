//------------------------------------------------------------------
// Plot a branch for all channels in each SK atm subGeV sample
// Run this macro:
// root -l -b -q QuickTreePlotter.C
//------------------------------------------------------------------

// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TCut.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <THStack.h>
#include <TFitResultPtr.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TEfficiency.h>
#include <TMath.h>
#include "TLorentzVector.h"
#include <TRandom3.h>
#include "TSystem.h"
#include "TROOT.h"
// C++ includes
#include <iostream>
#include <iomanip>
using namespace std;
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <vector>

void QuickTreePlotter()
{
  bool debug = true; // Print out for debug purpose

  if ( debug ) std::cout << "set input" << std::endl;

  // set input file dir

  // subGeV 0dcy: elike + mulike, sample 1+4
  TChain *mySKatmSampleSubGeV0dcy = new TChain("atm_minituple");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel10.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel11.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel12.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel1.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel2.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel3.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel4.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel5.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel6.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel7.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel8.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel9.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel10.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel11.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel12.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel1.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel2.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel3.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel4.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel5.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel6.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel7.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel8.root");
  mySKatmSampleSubGeV0dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel9.root");

  // subGeV 1dcy: elike + mulike, sample 2+5
  TChain *mySKatmSampleSubGeV1dcy = new TChain("atm_minituple");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel10.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel11.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel12.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel1.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel2.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel3.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel4.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel5.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel6.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel7.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel8.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel9.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel10.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel11.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel12.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel1.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel2.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel3.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel4.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel5.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel6.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel7.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel8.root");
  mySKatmSampleSubGeV1dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel9.root");

  // subGeV 2dcy: sample 6
  TChain *mySKatmSampleSubGeV2dcy = new TChain("atm_minituple");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel10.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel11.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel12.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel1.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel2.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel3.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel4.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel5.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel6.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel7.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel8.root");
  mySKatmSampleSubGeV2dcy->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel9.root");

  // subGeV 2R pi0: sample 7
  TChain *mySKatmSampleSubGeV2rpi0 = new TChain("atm_minituple");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel10.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel11.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel12.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel1.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel2.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel3.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel4.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel5.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel6.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel7.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel8.root");
  mySKatmSampleSubGeV2rpi0->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel9.root");

  if ( debug ) std::cout << "initialize" << std::endl;

  // initialize variable
  float fq1rnllSubGeV0dcy[10][7];
  float fq1rnllSubGeV1dcy[10][7];
  float fq1rnllSubGeV2dcy[10][7];
  float fq1rnllSubGeV2rpi0[10][7];
  float fq1rmomSubGeV0dcy[10][7];
  float fq1rmomSubGeV1dcy[10][7];
  float fq1rmomSubGeV2dcy[10][7];
  float fq1rmomSubGeV2rpi0[10][7];
  int nentriesSubGeV0dcy = 0; // Total events in samples
  int nentriesSubGeV1dcy = 0;
  int nentriesSubGeV2dcy = 0;
  int nentriesSubGeV2rpi0 = 0;

  if ( debug ) std::cout << "read branch" << std::endl;

  // read branch
  mySKatmSampleSubGeV0dcy->SetBranchAddress("fq1rnll", fq1rnllSubGeV0dcy);
  mySKatmSampleSubGeV0dcy->SetBranchAddress("fq1rmom", fq1rmomSubGeV0dcy);
  mySKatmSampleSubGeV1dcy->SetBranchAddress("fq1rnll", fq1rnllSubGeV1dcy);
  mySKatmSampleSubGeV1dcy->SetBranchAddress("fq1rmom", fq1rmomSubGeV1dcy);
  mySKatmSampleSubGeV2dcy->SetBranchAddress("fq1rnll", fq1rnllSubGeV2dcy);
  mySKatmSampleSubGeV2dcy->SetBranchAddress("fq1rmom", fq1rmomSubGeV2dcy);
  mySKatmSampleSubGeV2rpi0->SetBranchAddress("fq1rnll", fq1rnllSubGeV2rpi0);
  mySKatmSampleSubGeV2rpi0->SetBranchAddress("fq1rmom", fq1rmomSubGeV2rpi0);

  if ( debug ) std::cout << "plots initialize" << std::endl;

  // initialize plots
  TH2F *dNllemuvseppSG0dcy  = new TH2F("dNllemuvseppSG0dcy",  "dNllemuvseppSG0dcy",  1000, -5000., 5000., 1000, -5000., 5000.);
  TH2F *dNllemuvseppSG1dcy  = new TH2F("dNllemuvseppSG1dcy",  "dNllemuvseppSG1dcy",  1000, -5000., 5000., 1000, -5000., 5000.);
  TH2F *dNllemuvseppSG2dcy  = new TH2F("dNllemuvseppSG2dcy",  "dNllemuvseppSG2dcy",  1000, -5000., 5000., 1000, -5000., 5000.);
  TH2F *dNllemuvseppSG2rpi0 = new TH2F("dNllemuvseppSG2rpi0", "dNllemuvseppSG2rpi0", 1000, -5000., 5000., 1000, -5000., 5000.);

  // e/mu log likelihood ratio vs e momentum
  TH2F *dNllmuevseMomSG0dcy = new TH2F("dNllmuevseMomSG0dcy",  "dNllmuevseMomSG0dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllmuevseMomSG1dcy = new TH2F("dNllmuevseMomSG1dcy",  "dNllmuevseMomSG1dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllmuevseMomSG2dcy = new TH2F("dNllmuevseMomSG2dcy",  "dNllmuevseMomSG2dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllmuevseMomSG2rpi0 = new TH2F("dNllmuevseMomSG2rpi0",  "dNllmuevseMomSG2rpi0",  500, 0., 1000., 1000, -5000., 5000.);

  // e/pi+ log likelihood ratio vs e momentum
  TH2F *dNllppevseMomSG0dcy = new TH2F("dNllppevseMomSG0dcy",  "dNllppevseMomSG0dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllppevseMomSG1dcy = new TH2F("dNllppevseMomSG1dcy",  "dNllppevseMomSG1dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllppevseMomSG2dcy = new TH2F("dNllppevseMomSG2dcy",  "dNllppevseMomSG2dcy",  500, 0., 1000., 1000, -5000., 5000.);
  TH2F *dNllppevseMomSG2rpi0 = new TH2F("dNllppevseMomSG2rpi0",  "dNllppevseMomSG2rpi0",  500, 0., 1000., 1000, -5000., 5000.);

  // e/mu log likihood ratio distance to T2K elike cut line
  TH1F *dNllmueDistElikeCutLineSG0dcy = new TH1F("dNllmueDistElikeCutLineSG0dcy", "dNllmueDistElikeCutLineSG0dcy", 1000, -5000., 5000.);
  TH1F *dNllmueDistElikeCutLineSG1dcy = new TH1F("dNllmueDistElikeCutLineSG1dcy", "dNllmueDistElikeCutLineSG1dcy", 1000, -5000., 5000.);
  TH1F *dNllmueDistElikeCutLineSG2dcy = new TH1F("dNllmueDistElikeCutLineSG2dcy", "dNllmueDistElikeCutLineSG2dcy", 1000, -5000., 5000.);
  TH1F *dNllmueDistElikeCutLineSG2rpi0 = new TH1F("dNllmueDistElikeCutLineSG2rpi0", "dNllmueDistElikeCutLineSG2rpi0", 1000, -5000., 5000.);

  TH1F *dNllppeDistElikeCutLineSG0dcy = new TH1F("dNllppeDistElikeCutLineSG0dcy", "dNllppeDistElikeCutLineSG0dcy", 1000, -5000., 5000.);
  TH1F *dNllppeDistElikeCutLineSG1dcy = new TH1F("dNllppeDistElikeCutLineSG1dcy", "dNllppeDistElikeCutLineSG1dcy", 1000, -5000., 5000.);
  TH1F *dNllppeDistElikeCutLineSG2dcy = new TH1F("dNllppeDistElikeCutLineSG2dcy", "dNllppeDistElikeCutLineSG2dcy", 1000, -5000., 5000.);
  TH1F *dNllppeDistElikeCutLineSG2rpi0 = new TH1F("dNllppeDistElikeCutLineSG2rpi0", "dNllppeDistElikeCutLineSG2rpi0", 1000, -5000., 5000.);

  if ( debug ) std::cout << "get tot entries" << std::endl;

  // read events and fill histograms

  nentriesSubGeV0dcy = mySKatmSampleSubGeV0dcy->GetEntries();
  if ( debug ) std::cout << "start subGeV 0dcy loop, tot events: " << nentriesSubGeV0dcy << std::endl;

  for ( int ievt = 0; ievt < nentriesSubGeV0dcy; ievt++ ) {
    mySKatmSampleSubGeV0dcy->GetEntry(ievt);
    dNllemuvseppSG0dcy->Fill(fq1rnllSubGeV0dcy[0][1]-fq1rnllSubGeV0dcy[0][3], fq1rnllSubGeV0dcy[0][1]-fq1rnllSubGeV0dcy[0][2]);
    if ( ievt%100000 == 0 ) std::cout << "Looking at ievt " << ievt << ", fq1rnllSubGeV0dcy[0][1]: " << fq1rnllSubGeV0dcy[0][1] << ", fq1rnllSubGeV0dcy[0][2]: " << fq1rnllSubGeV0dcy[0][2] << ", fq1rnllSubGeV0dcy[0][3]: " << fq1rnllSubGeV0dcy[0][3] << std::endl;

    dNllmuevseMomSG0dcy->Fill(fq1rmomSubGeV0dcy[0][1], fq1rnllSubGeV0dcy[0][2]-fq1rnllSubGeV0dcy[0][1]);
    dNllppevseMomSG0dcy->Fill(fq1rmomSubGeV0dcy[0][1], fq1rnllSubGeV0dcy[0][3]-fq1rnllSubGeV0dcy[0][1]);

    // calculate y difference of each point to the T2K elike cut line y = 0.2 * x
    // above cutline set difference as positive, below as negative
    dNllmueDistElikeCutLineSG0dcy->Fill(fq1rnllSubGeV0dcy[0][2]-fq1rnllSubGeV0dcy[0][1] - 0.2*fq1rmomSubGeV0dcy[0][1]);
    dNllppeDistElikeCutLineSG0dcy->Fill(fq1rnllSubGeV0dcy[0][3]-fq1rnllSubGeV0dcy[0][1] - 0.2*fq1rmomSubGeV0dcy[0][1]);
  }

  nentriesSubGeV1dcy = mySKatmSampleSubGeV1dcy->GetEntries();
  if ( debug ) std::cout << "end loop, start subGeV 1dcy loop, tot events: " << nentriesSubGeV1dcy << std::endl;

  for ( int ievt = 0; ievt < nentriesSubGeV1dcy; ievt++ ) {
    mySKatmSampleSubGeV1dcy->GetEntry(ievt);
    dNllemuvseppSG1dcy->Fill(fq1rnllSubGeV1dcy[0][1]-fq1rnllSubGeV1dcy[0][3], fq1rnllSubGeV1dcy[0][1]-fq1rnllSubGeV1dcy[0][2]);

    dNllmuevseMomSG1dcy->Fill(fq1rmomSubGeV1dcy[0][1], fq1rnllSubGeV1dcy[0][2]-fq1rnllSubGeV1dcy[0][1]);
    dNllppevseMomSG1dcy->Fill(fq1rmomSubGeV1dcy[0][1], fq1rnllSubGeV1dcy[0][3]-fq1rnllSubGeV1dcy[0][1]);

    dNllmueDistElikeCutLineSG1dcy->Fill(fq1rnllSubGeV1dcy[0][2]-fq1rnllSubGeV1dcy[0][1] - 0.2*fq1rmomSubGeV1dcy[0][1]);
    dNllppeDistElikeCutLineSG1dcy->Fill(fq1rnllSubGeV1dcy[0][3]-fq1rnllSubGeV1dcy[0][1] - 0.2*fq1rmomSubGeV1dcy[0][1]);
  }

  nentriesSubGeV2dcy = mySKatmSampleSubGeV2dcy->GetEntries();
  if ( debug ) std::cout << "end loop, start subGeV 2dcy loop, tot events: " << nentriesSubGeV2dcy << std::endl;

  for ( int ievt = 0; ievt < nentriesSubGeV2dcy; ievt++ ) {
    mySKatmSampleSubGeV2dcy->GetEntry(ievt);
    dNllemuvseppSG2dcy->Fill(fq1rnllSubGeV2dcy[0][1]-fq1rnllSubGeV2dcy[0][3], fq1rnllSubGeV2dcy[0][1]-fq1rnllSubGeV2dcy[0][2]);

    dNllmuevseMomSG2dcy->Fill(fq1rmomSubGeV2dcy[0][1], fq1rnllSubGeV2dcy[0][2]-fq1rnllSubGeV2dcy[0][1]);
    dNllppevseMomSG2dcy->Fill(fq1rmomSubGeV2dcy[0][1], fq1rnllSubGeV2dcy[0][3]-fq1rnllSubGeV2dcy[0][1]);

    dNllmueDistElikeCutLineSG2dcy->Fill(fq1rnllSubGeV2dcy[0][2]-fq1rnllSubGeV2dcy[0][1] - 0.2*fq1rmomSubGeV2dcy[0][1]);
    dNllppeDistElikeCutLineSG2dcy->Fill(fq1rnllSubGeV2dcy[0][3]-fq1rnllSubGeV2dcy[0][1] - 0.2*fq1rmomSubGeV2dcy[0][1]);
  }

  nentriesSubGeV2rpi0 = mySKatmSampleSubGeV2rpi0->GetEntries();
  if ( debug ) std::cout << "end loop, start subGeV 2rpi0 loop, tot events: " << nentriesSubGeV2rpi0 << std::endl;

  for ( int ievt = 0; ievt < nentriesSubGeV2rpi0; ievt++ ) {
    mySKatmSampleSubGeV2rpi0->GetEntry(ievt);
    dNllemuvseppSG2rpi0->Fill(fq1rnllSubGeV2rpi0[0][1]-fq1rnllSubGeV2rpi0[0][3], fq1rnllSubGeV2rpi0[0][1]-fq1rnllSubGeV2rpi0[0][2]);

    dNllmuevseMomSG2rpi0->Fill(fq1rmomSubGeV2rpi0[0][1], fq1rnllSubGeV2rpi0[0][2]-fq1rnllSubGeV2rpi0[0][1]);
    dNllppevseMomSG2rpi0->Fill(fq1rmomSubGeV2rpi0[0][1], fq1rnllSubGeV2rpi0[0][3]-fq1rnllSubGeV2rpi0[0][1]);

    dNllmueDistElikeCutLineSG2rpi0->Fill(fq1rnllSubGeV2rpi0[0][2]-fq1rnllSubGeV2rpi0[0][1] - 0.2*fq1rmomSubGeV2rpi0[0][1]);
    dNllppeDistElikeCutLineSG2rpi0->Fill(fq1rnllSubGeV2rpi0[0][3]-fq1rnllSubGeV2rpi0[0][1] - 0.2*fq1rmomSubGeV2rpi0[0][1]);
  }

  if ( debug ) std::cout << "end loop" << std::endl;

  // print correlation factor
  std::cout << "dNllemuvseppSG0dcy correlation factor: " << dNllemuvseppSG0dcy->GetCorrelationFactor() << std::endl;
  std::cout << "dNllemuvseppSG1dcy correlation factor: " << dNllemuvseppSG1dcy->GetCorrelationFactor() << std::endl;
  std::cout << "dNllemuvseppSG2dcy correlation factor: " << dNllemuvseppSG2dcy->GetCorrelationFactor() << std::endl;
  std::cout << "dNllemuvseppSG2rpi0 correlation factor: " << dNllemuvseppSG2rpi0->GetCorrelationFactor() << std::endl;

  if ( debug ) std::cout << "write output" << std::endl;

  // write plots to output
  TFile outFile("out_QuickTreePlotter.root", "RECREATE");
  TCanvas *c_dNllemuvseppSG0dcy = new TCanvas("c_dNllemuvseppSG0dcy", "c_dNllemuvseppSG0dcy", 700, 500);
  TCanvas *c_dNllemuvseppSG1dcy = new TCanvas("c_dNllemuvseppSG1dcy", "c_dNllemuvseppSG1dcy", 700, 500);
  TCanvas *c_dNllemuvseppSG2dcy = new TCanvas("c_dNllemuvseppSG2dcy", "c_dNllemuvseppSG2dcy", 700, 500);
  TCanvas *c_dNllemuvseppSG2rpi0 = new TCanvas("c_dNllemuvseppSG2rpi0", "c_dNllemuvseppSG2rpi0", 700, 500);

  TCanvas *c_dNllmuevseMomSG0dcy = new TCanvas("c_dNllmuevseMomSG0dcy", "c_dNllmuevseMomSG0dcy", 700, 500);
  TCanvas *c_dNllmuevseMomSG1dcy = new TCanvas("c_dNllmuevseMomSG1dcy", "c_dNllmuevseMomSG1dcy", 700, 500);
  TCanvas *c_dNllmuevseMomSG2dcy = new TCanvas("c_dNllmuevseMomSG2dcy", "c_dNllmuevseMomSG2dcy", 700, 500);
  TCanvas *c_dNllmuevseMomSG2rpi0 = new TCanvas("c_dNllmuevseMomSG2rpi0", "c_dNllmuevseMomSG2rpi0", 700, 500);

  TCanvas *c_dNllppevseMomSG0dcy = new TCanvas("c_dNllppevseMomSG0dcy", "c_dNllppevseMomSG0dcy", 700, 500);
  TCanvas *c_dNllppevseMomSG1dcy = new TCanvas("c_dNllppevseMomSG1dcy", "c_dNllppevseMomSG1dcy", 700, 500);
  TCanvas *c_dNllppevseMomSG2dcy = new TCanvas("c_dNllppevseMomSG2dcy", "c_dNllppevseMomSG2dcy", 700, 500);
  TCanvas *c_dNllppevseMomSG2rpi0 = new TCanvas("c_dNllppevseMomSG2rpi0", "c_dNllppevseMomSG2rpi0", 700, 500);

  TCanvas *c_dNllDistElikeCutLineSG0dcy = new TCanvas("c_dNllDistElikeCutLineSG0dcy", "c_dNllDistElikeCutLineSG0dcy", 700, 500);
  TCanvas *c_dNllDistElikeCutLineSG1dcy = new TCanvas("c_dNllDistElikeCutLineSG1dcy", "c_dNllDistElikeCutLineSG1dcy", 700, 500);
  TCanvas *c_dNllDistElikeCutLineSG2dcy = new TCanvas("c_dNllDistElikeCutLineSG2dcy", "c_dNllDistElikeCutLineSG2dcy", 700, 500);
  TCanvas *c_dNllDistElikeCutLineSG2rpi0 = new TCanvas("c_dNllDistElikeCutLineSG2rpi0", "c_dNllDistElikeCutLineSG2rpi0", 700, 500);

  TF1 diagonal("diagonal", "x", -5000., 5000.);
  // T2K cut e like cut: y = likelihood mu (or pi+) - e > 0.2 * e momentum
  TF1 t2kpidcut("t2kpidcut", "0.2*x", 0., 1000.);

  c_dNllemuvseppSG0dcy->cd(); dNllemuvseppSG0dcy->GetXaxis()->SetTitle("1rnll(e) - 1rnll(#pi^{+})"); dNllemuvseppSG0dcy->GetYaxis()->SetTitle("1rnll(e) - 1rnll(#mu)"); dNllemuvseppSG0dcy->Draw("COLZ"); diagonal.Draw("SAME"); c_dNllemuvseppSG0dcy->SetGridx(); c_dNllemuvseppSG0dcy->SetGridy(); c_dNllemuvseppSG0dcy->SetLogz(); c_dNllemuvseppSG0dcy->Write();
  c_dNllemuvseppSG1dcy->cd(); dNllemuvseppSG1dcy->GetXaxis()->SetTitle("1rnll(e) - 1rnll(#pi^{+})"); dNllemuvseppSG1dcy->GetYaxis()->SetTitle("1rnll(e) - 1rnll(#mu)"); dNllemuvseppSG1dcy->Draw("COLZ"); diagonal.Draw("SAME"); c_dNllemuvseppSG1dcy->SetGridx(); c_dNllemuvseppSG1dcy->SetGridy(); c_dNllemuvseppSG1dcy->SetLogz(); c_dNllemuvseppSG1dcy->Write();
  c_dNllemuvseppSG2dcy->cd(); dNllemuvseppSG2dcy->GetXaxis()->SetTitle("1rnll(e) - 1rnll(#pi^{+})"); dNllemuvseppSG2dcy->GetYaxis()->SetTitle("1rnll(e) - 1rnll(#mu)"); dNllemuvseppSG2dcy->Draw("COLZ"); diagonal.Draw("SAME"); c_dNllemuvseppSG2dcy->SetGridx(); c_dNllemuvseppSG2dcy->SetGridy(); c_dNllemuvseppSG2dcy->SetLogz(); c_dNllemuvseppSG2dcy->Write();
  c_dNllemuvseppSG2rpi0->cd(); dNllemuvseppSG2rpi0->GetXaxis()->SetTitle("1rnll(e) - 1rnll(#pi^{+})"); dNllemuvseppSG2rpi0->GetYaxis()->SetTitle("1rnll(e) - 1rnll(#mu)"); dNllemuvseppSG2rpi0->Draw("COLZ"); diagonal.Draw("SAME"); c_dNllemuvseppSG2rpi0->SetGridx(); c_dNllemuvseppSG2rpi0->SetGridy(); c_dNllemuvseppSG2rpi0->SetLogz(); c_dNllemuvseppSG2rpi0->Write();

  c_dNllmuevseMomSG0dcy->cd(); dNllmuevseMomSG0dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllmuevseMomSG0dcy->GetYaxis()->SetTitle("1rnll(#mu) - 1rnll(e)"); dNllmuevseMomSG0dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllmuevseMomSG0dcy->SetGridx(); c_dNllmuevseMomSG0dcy->SetGridy(); c_dNllmuevseMomSG0dcy->SetLogz(); c_dNllmuevseMomSG0dcy->Write();
  c_dNllmuevseMomSG1dcy->cd(); dNllmuevseMomSG1dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllmuevseMomSG1dcy->GetYaxis()->SetTitle("1rnll(#mu) - 1rnll(e)"); dNllmuevseMomSG1dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllmuevseMomSG1dcy->SetGridx(); c_dNllmuevseMomSG1dcy->SetGridy(); c_dNllmuevseMomSG1dcy->SetLogz(); c_dNllmuevseMomSG1dcy->Write();
  c_dNllmuevseMomSG2dcy->cd(); dNllmuevseMomSG2dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllmuevseMomSG2dcy->GetYaxis()->SetTitle("1rnll(#mu) - 1rnll(e)"); dNllmuevseMomSG2dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllmuevseMomSG2dcy->SetGridx(); c_dNllmuevseMomSG2dcy->SetGridy(); c_dNllmuevseMomSG2dcy->SetLogz(); c_dNllmuevseMomSG2dcy->Write();
  c_dNllmuevseMomSG2rpi0->cd(); dNllmuevseMomSG2rpi0->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllmuevseMomSG2rpi0->GetYaxis()->SetTitle("1rnll(#mu) - 1rnll(e)"); dNllmuevseMomSG2rpi0->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllmuevseMomSG2rpi0->SetGridx(); c_dNllmuevseMomSG2rpi0->SetGridy(); c_dNllmuevseMomSG2rpi0->SetLogz(); c_dNllmuevseMomSG2rpi0->Write();

  c_dNllppevseMomSG0dcy->cd(); dNllppevseMomSG0dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllppevseMomSG0dcy->GetYaxis()->SetTitle("1rnll(#pi^{+}) - 1rnll(e)"); dNllppevseMomSG0dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllppevseMomSG0dcy->SetGridx(); c_dNllppevseMomSG0dcy->SetGridy(); c_dNllppevseMomSG0dcy->SetLogz(); c_dNllppevseMomSG0dcy->Write();
  c_dNllppevseMomSG1dcy->cd(); dNllppevseMomSG1dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllppevseMomSG1dcy->GetYaxis()->SetTitle("1rnll(#pi^{+}) - 1rnll(e)"); dNllppevseMomSG1dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllppevseMomSG1dcy->SetGridx(); c_dNllppevseMomSG1dcy->SetGridy(); c_dNllppevseMomSG1dcy->SetLogz(); c_dNllppevseMomSG1dcy->Write();
  c_dNllppevseMomSG2dcy->cd(); dNllppevseMomSG2dcy->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllppevseMomSG2dcy->GetYaxis()->SetTitle("1rnll(#pi^{+}) - 1rnll(e)"); dNllppevseMomSG2dcy->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllppevseMomSG2dcy->SetGridx(); c_dNllppevseMomSG2dcy->SetGridy(); c_dNllppevseMomSG2dcy->SetLogz(); c_dNllppevseMomSG2dcy->Write();
  c_dNllppevseMomSG2rpi0->cd(); dNllppevseMomSG2rpi0->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllppevseMomSG2rpi0->GetYaxis()->SetTitle("1rnll(#pi^{+}) - 1rnll(e)"); dNllppevseMomSG2rpi0->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllppevseMomSG2rpi0->SetGridx(); c_dNllppevseMomSG2rpi0->SetGridy(); c_dNllppevseMomSG2rpi0->SetLogz(); c_dNllppevseMomSG2rpi0->Write();

  c_dNllDistElikeCutLineSG0dcy->cd();
  dNllmueDistElikeCutLineSG0dcy->GetXaxis()->SetTitle("Distance"); dNllmueDistElikeCutLineSG0dcy->GetYaxis()->SetTitle("Events"); dNllmueDistElikeCutLineSG0dcy->SetLineColor(2); dNllmueDistElikeCutLineSG0dcy->Draw();
  dNllppeDistElikeCutLineSG0dcy->SetLineColor(4); dNllppeDistElikeCutLineSG0dcy->Draw("SAMEs");
  c_dNllDistElikeCutLineSG0dcy->Write();

  c_dNllDistElikeCutLineSG1dcy->cd();
  dNllmueDistElikeCutLineSG1dcy->GetXaxis()->SetTitle("Distance"); dNllmueDistElikeCutLineSG1dcy->GetYaxis()->SetTitle("Events"); dNllmueDistElikeCutLineSG1dcy->SetLineColor(2); dNllmueDistElikeCutLineSG1dcy->Draw();
  dNllppeDistElikeCutLineSG1dcy->SetLineColor(4); dNllppeDistElikeCutLineSG1dcy->Draw("SAMEs");
  c_dNllDistElikeCutLineSG1dcy->Write();

  c_dNllDistElikeCutLineSG2dcy->cd();
  dNllmueDistElikeCutLineSG2dcy->GetXaxis()->SetTitle("Distance"); dNllmueDistElikeCutLineSG2dcy->GetYaxis()->SetTitle("Events"); dNllmueDistElikeCutLineSG2dcy->SetLineColor(2); dNllmueDistElikeCutLineSG2dcy->Draw();
  dNllppeDistElikeCutLineSG2dcy->SetLineColor(4); dNllppeDistElikeCutLineSG2dcy->Draw("SAMEs");
  c_dNllDistElikeCutLineSG2dcy->Write();

  c_dNllDistElikeCutLineSG2rpi0->cd();
  dNllmueDistElikeCutLineSG2rpi0->GetXaxis()->SetTitle("Distance"); dNllmueDistElikeCutLineSG2rpi0->GetYaxis()->SetTitle("Events"); dNllmueDistElikeCutLineSG2rpi0->SetLineColor(2); dNllmueDistElikeCutLineSG2rpi0->Draw();
  dNllppeDistElikeCutLineSG2rpi0->SetLineColor(4); dNllppeDistElikeCutLineSG2rpi0->Draw("SAMEs");
  c_dNllDistElikeCutLineSG2rpi0->Write();

  outFile.Close();
}
