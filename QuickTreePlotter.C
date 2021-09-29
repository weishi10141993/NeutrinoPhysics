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
  TChain *mySKatmSample1 = new TChain("atm_minituple");
  mySKatmSample1->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample1_Channel5.root");

  TChain *mySKatmSample2 = new TChain("atm_minituple");
  mySKatmSample2->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel5.root");

  TChain *mySKatmSample4 = new TChain("atm_minituple");
  mySKatmSample4->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample4_Channel5.root");

  TChain *mySKatmSample5 = new TChain("atm_minituple");
  mySKatmSample5->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample5_Channel5.root");

  TChain *mySKatmSample6 = new TChain("atm_minituple");
  mySKatmSample6->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample6_Channel5.root");

  TChain *mySKatmSample7 = new TChain("atm_minituple");
  mySKatmSample7->Add("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample7_Channel5.root");

  if ( debug ) std::cout << "initialize" << std::endl;

  // initialize variable
  gSystem->Exec("rm -f AutoDict*vector*vector*float*"); // Remove old dictionary if exists
  gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");// Generate new dictionary
  vector<vector<float>> *fq1rnll1 = 0;
  vector<vector<float>> *fq1rnll2 = 0;
  vector<vector<float>> *fq1rnll4 = 0;
  vector<vector<float>> *fq1rnll5 = 0;
  vector<vector<float>> *fq1rnll6 = 0;
  vector<vector<float>> *fq1rnll7 = 0;
  int nentries1 = 0; // Total events in samples
  int nentries2 = 0;
  int nentries4 = 0;
  int nentries5 = 0;
  int nentries6 = 0;
  int nentries7 = 0;

  if ( debug ) std::cout << "read branch" << std::endl;

  // read branch
  mySKatmSample1->SetBranchAddress("fq1rnll", &fq1rnll1);
  mySKatmSample2->SetBranchAddress("fq1rnll", &fq1rnll2);
  mySKatmSample4->SetBranchAddress("fq1rnll", &fq1rnll4);
  mySKatmSample5->SetBranchAddress("fq1rnll", &fq1rnll5);
  mySKatmSample6->SetBranchAddress("fq1rnll", &fq1rnll6);
  mySKatmSample7->SetBranchAddress("fq1rnll", &fq1rnll7);

  if ( debug ) std::cout << "plots initialize" << std::endl;

  // initialize plots
  TH2F *dNll1 = new TH2F();
  TH2F *dNll2 = new TH2F();
  TH2F *dNll4 = new TH2F();
  TH2F *dNll5 = new TH2F();
  TH2F *dNll6 = new TH2F();
  TH2F *dNll7 = new TH2F();

  if ( debug ) std::cout << "get tot entries" << std::endl;

  // read events and fill histograms
  nentries1 = mySKatmSample1->GetEntries();
  nentries2 = mySKatmSample2->GetEntries();
  nentries4 = mySKatmSample4->GetEntries();
  nentries5 = mySKatmSample5->GetEntries();
  nentries6 = mySKatmSample6->GetEntries();
  nentries7 = mySKatmSample7->GetEntries();

  if ( debug ) std::cout << "start 1st loop, tot events: " << nentries1 << std::endl;

  for ( int ievt = 0; ievt < nentries1; ievt++ ) {
    mySKatmSample1->GetEntry(ievt);
    dNll1->Fill((*fq1rnll1)[0][1]-(*fq1rnll1)[0][3], (*fq1rnll1)[0][1]-(*fq1rnll1)[0][2]);
  }

  if ( debug ) std::cout << "end 1st sample loop" << std::endl;

  for ( int ievt = 0; ievt < nentries2; ievt++ ) {
    mySKatmSample2->GetEntry(ievt);
    dNll2->Fill((*fq1rnll2)[0][1]-(*fq1rnll2)[0][3], (*fq1rnll2)[0][1]-(*fq1rnll2)[0][2]);
  }

  for ( int ievt = 0; ievt < nentries4; ievt++ ) {
    mySKatmSample4->GetEntry(ievt);
    dNll4->Fill((*fq1rnll4)[0][1]-(*fq1rnll4)[0][3], (*fq1rnll4)[0][1]-(*fq1rnll4)[0][2]);
  }

  for ( int ievt = 0; ievt < nentries5; ievt++ ) {
    mySKatmSample5->GetEntry(ievt);
    dNll5->Fill((*fq1rnll5)[0][1]-(*fq1rnll5)[0][3], (*fq1rnll5)[0][1]-(*fq1rnll5)[0][2]);
  }

  for ( int ievt = 0; ievt < nentries6; ievt++ ) {
    mySKatmSample6->GetEntry(ievt);
    dNll6->Fill((*fq1rnll6)[0][1]-(*fq1rnll6)[0][3], (*fq1rnll6)[0][1]-(*fq1rnll6)[0][2]);
  }

  for ( int ievt = 0; ievt < nentries7; ievt++ ) {
    mySKatmSample7->GetEntry(ievt);
    dNll7->Fill((*fq1rnll7)[0][1]-(*fq1rnll7)[0][3], (*fq1rnll7)[0][1]-(*fq1rnll7)[0][2]);
  }

  // print correlation factor
  std::cout << "dNll1 correlation factor: " << dNll1->GetCorrelationFactor() << std::endl;
  std::cout << "dNll2 correlation factor: " << dNll2->GetCorrelationFactor() << std::endl;
  std::cout << "dNll4 correlation factor: " << dNll4->GetCorrelationFactor() << std::endl;
  std::cout << "dNll5 correlation factor: " << dNll5->GetCorrelationFactor() << std::endl;
  std::cout << "dNll6 correlation factor: " << dNll6->GetCorrelationFactor() << std::endl;
  std::cout << "dNll7 correlation factor: " << dNll7->GetCorrelationFactor() << std::endl;

  if ( debug ) std::cout << "write output" << std::endl;

  // write plots to output
  TFile outFile = new TFile("out_QuickTreePlotter.root", "RECREATE");
  TCanvas *c_dNll1 = new TCanvas("c_dNll1", "c_dNll1", 700, 500);
  TCanvas *c_dNll2 = new TCanvas("c_dNll2", "c_dNll2", 700, 500);
  TCanvas *c_dNll4 = new TCanvas("c_dNll4", "c_dNll4", 700, 500);
  TCanvas *c_dNll5 = new TCanvas("c_dNll5", "c_dNll5", 700, 500);
  TCanvas *c_dNll6 = new TCanvas("c_dNll6", "c_dNll6", 700, 500);
  TCanvas *c_dNll7 = new TCanvas("c_dNll7", "c_dNll7", 700, 500);

  c_dNll1->cd(); dNll1->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll1->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll1->Draw("COLZ"); c_dNll1->Write();
  c_dNll2->cd(); dNll2->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll2->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll2->Draw("COLZ"); c_dNll2->Write();
  c_dNll4->cd(); dNll4->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll4->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll4->Draw("COLZ"); c_dNll4->Write();
  c_dNll5->cd(); dNll5->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll5->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll5->Draw("COLZ"); c_dNll5->Write();
  c_dNll6->cd(); dNll6->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll6->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll6->Draw("COLZ"); c_dNll6->Write();
  c_dNll7->cd(); dNll7->GetXaxis()->SetTitle("1rnll(e) - 1rnll(pi)"); dNll7->GetYaxis()->SetTitle("1rnll(e) - 1rnll(mu)"); dNll7->Draw("COLZ"); c_dNll7->Write();

  outFile.Close();
}
