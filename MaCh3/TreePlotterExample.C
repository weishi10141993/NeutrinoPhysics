//------------------------------------------------------------------
// Plot a branch for all channels in each SK atm subGeV sample
// Run this macro:
// root -l -b -q TreePlotterExample.C
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

void TreePlotterExample()
{
  bool debug = true; // Print out for debug purpose

  if ( debug ) std::cout << "set input" << std::endl;

  // set input file dir
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
  float fq1rnllSubGeV2rpi0[10][7];
  float fq1rmomSubGeV2rpi0[10][7];

  // Total events in samples
  int nentriesSubGeV2rpi0 = 0;

  if ( debug ) std::cout << "read branch" << std::endl;

  // read branch
  mySKatmSampleSubGeV2rpi0->SetBranchAddress("fq1rnll", fq1rnllSubGeV2rpi0);
  mySKatmSampleSubGeV2rpi0->SetBranchAddress("fq1rmom", fq1rmomSubGeV2rpi0);

  if ( debug ) std::cout << "plots initialize" << std::endl;

  // initialize plots
  // e/mu log likelihood ratio vs e/pi+ log likelihood ratio
  TH2F *dNllemuvseppSG2rpi0 = new TH2F("dNllemuvseppSG2rpi0", "dNllemuvseppSG2rpi0", 1000, -5000., 5000., 1000, -5000., 5000.);

  // e/mu log likelihood ratio vs e momentum
  TH2F *dNllmuevseMomSG2rpi0 = new TH2F("dNllmuevseMomSG2rpi0",  "dNllmuevseMomSG2rpi0",  500, 0., 1000., 1000, -5000., 5000.);

  // e/pi+ log likelihood ratio vs e momentum
  TH2F *dNllppevseMomSG2rpi0 = new TH2F("dNllppevseMomSG2rpi0",  "dNllppevseMomSG2rpi0",  500, 0., 1000., 1000, -5000., 5000.);

  // e/mu log likihood ratio distance to T2K elike cut line
  TH1F *dNllmueDistElikeCutLineSG2rpi0 = new TH1F("dNllmueDistElikeCutLineSG2rpi0", "dNllmueDistElikeCutLineSG2rpi0", 1000, -5000., 5000.);

  // e/pi+ log likihood ratio distance to T2K elike cut line
  TH1F *dNllppeDistElikeCutLineSG2rpi0 = new TH1F("dNllppeDistElikeCutLineSG2rpi0", "dNllppeDistElikeCutLineSG2rpi0", 1000, -5000., 5000.);

  if ( debug ) std::cout << "get tot entries" << std::endl;

  // read events and fill histograms
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
  std::cout << "dNllemuvseppSG2rpi0 correlation factor: " << dNllemuvseppSG2rpi0->GetCorrelationFactor() << std::endl;

  if ( debug ) std::cout << "write output" << std::endl;

  // write plots to output
  TFile outFile("out_QuickTreePlotter.root", "RECREATE");
  TCanvas *c_dNllemuvseppSG2rpi0 = new TCanvas("c_dNllemuvseppSG2rpi0", "c_dNllemuvseppSG2rpi0", 700, 500);
  TCanvas *c_dNllmuevseMomSG2rpi0 = new TCanvas("c_dNllmuevseMomSG2rpi0", "c_dNllmuevseMomSG2rpi0", 700, 500);
  TCanvas *c_dNllppevseMomSG2rpi0 = new TCanvas("c_dNllppevseMomSG2rpi0", "c_dNllppevseMomSG2rpi0", 700, 500);
  TCanvas *c_dNllDistElikeCutLineSG2rpi0 = new TCanvas("c_dNllDistElikeCutLineSG2rpi0", "c_dNllDistElikeCutLineSG2rpi0", 700, 500);

  // draw y=x to guide the eye
  TF1 diagonal("diagonal", "x", -5000., 5000.);
  // T2K cut e like cut: y = likelihood mu (or pi+) - e > 0.2 * e momentum
  TF1 t2kpidcut("t2kpidcut", "0.2*x", 0., 1000.);
  c_dNllemuvseppSG2rpi0->cd(); dNllemuvseppSG2rpi0->GetXaxis()->SetTitle("1rnll(e) - 1rnll(#pi^{+})"); dNllemuvseppSG2rpi0->GetYaxis()->SetTitle("1rnll(e) - 1rnll(#mu)"); dNllemuvseppSG2rpi0->Draw("COLZ"); diagonal.Draw("SAME"); c_dNllemuvseppSG2rpi0->SetGridx(); c_dNllemuvseppSG2rpi0->SetGridy(); c_dNllemuvseppSG2rpi0->SetLogz(); c_dNllemuvseppSG2rpi0->Write();
  c_dNllmuevseMomSG2rpi0->cd(); dNllmuevseMomSG2rpi0->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllmuevseMomSG2rpi0->GetYaxis()->SetTitle("1rnll(#mu) - 1rnll(e)"); dNllmuevseMomSG2rpi0->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllmuevseMomSG2rpi0->SetGridx(); c_dNllmuevseMomSG2rpi0->SetGridy(); c_dNllmuevseMomSG2rpi0->SetLogz(); c_dNllmuevseMomSG2rpi0->Write();
  c_dNllppevseMomSG2rpi0->cd(); dNllppevseMomSG2rpi0->GetXaxis()->SetTitle("p_{e} (MeV)"); dNllppevseMomSG2rpi0->GetYaxis()->SetTitle("1rnll(#pi^{+}) - 1rnll(e)"); dNllppevseMomSG2rpi0->Draw("COLZ"); t2kpidcut.Draw("SAME"); c_dNllppevseMomSG2rpi0->SetGridx(); c_dNllppevseMomSG2rpi0->SetGridy(); c_dNllppevseMomSG2rpi0->SetLogz(); c_dNllppevseMomSG2rpi0->Write();
  c_dNllDistElikeCutLineSG2rpi0->cd();
  dNllmueDistElikeCutLineSG2rpi0->GetXaxis()->SetTitle("Distance"); dNllmueDistElikeCutLineSG2rpi0->GetYaxis()->SetTitle("Events"); dNllmueDistElikeCutLineSG2rpi0->SetLineColor(2); dNllmueDistElikeCutLineSG2rpi0->Draw();
  dNllppeDistElikeCutLineSG2rpi0->SetLineColor(4); dNllppeDistElikeCutLineSG2rpi0->Draw("SAMEs");
  c_dNllDistElikeCutLineSG2rpi0->Write();

  outFile.Close();
}
