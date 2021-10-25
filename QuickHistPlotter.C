//------------------------------------------------------------------
// Overlay several histograms from a root file first use for sigma variation
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q QuickHistPlotter.C
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

void QuickHistPlotter()
{
  bool debug = true; // Print out for debug purpose

  if ( debug ) std::cout << "set input" << std::endl;

  TFile *file0 = TFile::Open("/dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/app/PRISMPred_EVisReco_MaCCQE.root");

  TString systname = "MaCCQE";
  vector<TString> histnames;
	histnames.clear();
	histnames.push_back("NDData_FDExtrap");
	histnames.push_back("FDFluxCorr");
  histnames.push_back("FDNuTauCCBkg");
  histnames.push_back("FDWrongLepBkg");
  histnames.push_back("FDNCBkg");
  histnames.push_back("FDWSBkg");

  if ( debug ) std::cout << "clone plots" << std::endl;

  //
  // Here is an overall comparison
  //

  TH1F *NDDataCorr_FDExtrap_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/NDDataCorr_FDExtrap")->Clone("NDDataCorr_FDExtrap_nom");
  TH1F *FDOscPred_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDOscPred")->Clone("FDOscPred_nom");
  TH1F *FDUnOscPred_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDUnOscPred")->Clone("FDUnOscPred_nom");

  TH1F *NDDataCorr_FDExtrap_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/NDDataCorr_FDExtrap")->Clone("NDDataCorr_FDExtrap_MaCCQE_n3");
  TH1F *NDDataCorr_FDExtrap_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/NDDataCorr_FDExtrap")->Clone("NDDataCorr_FDExtrap_MaCCQE_n1");
  TH1F *NDDataCorr_FDExtrap_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/NDDataCorr_FDExtrap")->Clone("NDDataCorr_FDExtrap_MaCCQE_p1");
  TH1F *NDDataCorr_FDExtrap_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/NDDataCorr_FDExtrap")->Clone("NDDataCorr_FDExtrap_MaCCQE_p3");

  TH1F *FDOscPred_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDOscPred")->Clone("FDOscPred_MaCCQE_n3");
  TH1F *FDOscPred_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDOscPred")->Clone("FDOscPred_MaCCQE_n1");
  TH1F *FDOscPred_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDOscPred")->Clone("FDOscPred_MaCCQE_p1");
  TH1F *FDOscPred_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDOscPred")->Clone("FDOscPred_MaCCQE_p3");

  //
  // Compare each component
  //

  // NDData_FDExtrap
  TH1F *NDData_FDExtrap_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/NDData_FDExtrap")->Clone("NDData_FDExtrap_nom");
  TH1F *NDData_FDExtrap_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/NDData_FDExtrap")->Clone("NDData_FDExtrap_MaCCQE_n3");
  TH1F *NDData_FDExtrap_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/NDData_FDExtrap")->Clone("NDData_FDExtrap_MaCCQE_n1");
  TH1F *NDData_FDExtrap_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/NDData_FDExtrap")->Clone("NDData_FDExtrap_MaCCQE_p1");
  TH1F *NDData_FDExtrap_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/NDData_FDExtrap")->Clone("NDData_FDExtrap_MaCCQE_p3");

  // FDFluxCorr
  TH1F *FDFluxCorr_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDFluxCorr")->Clone("FDFluxCorr_nom");
  TH1F *FDFluxCorr_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDFluxCorr")->Clone("FDFluxCorr_MaCCQE_n3");
  TH1F *FDFluxCorr_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDFluxCorr")->Clone("FDFluxCorr_MaCCQE_n1");
  TH1F *FDFluxCorr_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDFluxCorr")->Clone("FDFluxCorr_MaCCQE_p1");
  TH1F *FDFluxCorr_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDFluxCorr")->Clone("FDFluxCorr_MaCCQE_p3");

  // FDNuTauCCBkg
  TH1F *FDNuTauCCBkg_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDNuTauCCBkg")->Clone("FDNuTauCCBkg_nom");
  TH1F *FDNuTauCCBkg_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDNuTauCCBkg")->Clone("FDNuTauCCBkg_MaCCQE_n3");
  TH1F *FDNuTauCCBkg_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDNuTauCCBkg")->Clone("FDNuTauCCBkg_MaCCQE_n1");
  TH1F *FDNuTauCCBkg_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDNuTauCCBkg")->Clone("FDNuTauCCBkg_MaCCQE_p1");
  TH1F *FDNuTauCCBkg_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDNuTauCCBkg")->Clone("FDNuTauCCBkg_MaCCQE_p3");

  // FDWrongLepBkg
  TH1F *FDWrongLepBkg_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDWrongLepBkg")->Clone("FDWrongLepBkg_nom");
  TH1F *FDWrongLepBkg_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDWrongLepBkg")->Clone("FDWrongLepBkg_MaCCQE_n3");
  TH1F *FDWrongLepBkg_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDWrongLepBkg")->Clone("FDWrongLepBkg_MaCCQE_n1");
  TH1F *FDWrongLepBkg_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDWrongLepBkg")->Clone("FDWrongLepBkg_MaCCQE_p1");
  TH1F *FDWrongLepBkg_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDWrongLepBkg")->Clone("FDWrongLepBkg_MaCCQE_p3");

  // FDNCBkg
  TH1F *FDNCBkg_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDNCBkg")->Clone("FDNCBkg_nom");
  TH1F *FDNCBkg_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDNCBkg")->Clone("FDNCBkg_MaCCQE_n3");
  TH1F *FDNCBkg_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDNCBkg")->Clone("FDNCBkg_MaCCQE_n1");
  TH1F *FDNCBkg_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDNCBkg")->Clone("FDNCBkg_MaCCQE_p1");
  TH1F *FDNCBkg_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDNCBkg")->Clone("FDNCBkg_MaCCQE_p3");

  // FDWSBkg
  TH1F *FDWSBkg_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDWSBkg")->Clone("FDWSBkg_nom");
  TH1F *FDWSBkg_MaCCQE_n3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-3/FD_nu_numu/FDWSBkg")->Clone("FDWSBkg_MaCCQE_n3");
  TH1F *FDWSBkg_MaCCQE_n1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_-1/FD_nu_numu/FDWSBkg")->Clone("FDWSBkg_MaCCQE_n1");
  TH1F *FDWSBkg_MaCCQE_p1 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_1/FD_nu_numu/FDWSBkg")->Clone("FDWSBkg_MaCCQE_p1");
  TH1F *FDWSBkg_MaCCQE_p3 = (TH1F*)file0->Get("numu_EvMatch_MaCCQE_3/FD_nu_numu/FDWSBkg")->Clone("FDWSBkg_MaCCQE_p3");

  //
  // Calculation
  //

  if ( debug ) std::cout << "manipulate" << std::endl;

  // (ND@Nsigma - ND@nom)/Unosc FD numu (nom)
  NDDataCorr_FDExtrap_MaCCQE_n3->Add(NDDataCorr_FDExtrap_nom, -1); NDDataCorr_FDExtrap_MaCCQE_n3->Divide(FDUnOscPred_nom);
  NDDataCorr_FDExtrap_MaCCQE_n1->Add(NDDataCorr_FDExtrap_nom, -1); NDDataCorr_FDExtrap_MaCCQE_n1->Divide(FDUnOscPred_nom);
  NDDataCorr_FDExtrap_MaCCQE_p1->Add(NDDataCorr_FDExtrap_nom, -1); NDDataCorr_FDExtrap_MaCCQE_p1->Divide(FDUnOscPred_nom);
  NDDataCorr_FDExtrap_MaCCQE_p3->Add(NDDataCorr_FDExtrap_nom, -1); NDDataCorr_FDExtrap_MaCCQE_p3->Divide(FDUnOscPred_nom);

  // (FD@Nsigma - FD@nom)/Unosc FD numu (nom)
  FDOscPred_MaCCQE_n3->Add(FDOscPred_nom, -1); FDOscPred_MaCCQE_n3->Divide(FDUnOscPred_nom);
  FDOscPred_MaCCQE_n1->Add(FDOscPred_nom, -1); FDOscPred_MaCCQE_n1->Divide(FDUnOscPred_nom);
  FDOscPred_MaCCQE_p1->Add(FDOscPred_nom, -1); FDOscPred_MaCCQE_p1->Divide(FDUnOscPred_nom);
  FDOscPred_MaCCQE_p3->Add(FDOscPred_nom, -1); FDOscPred_MaCCQE_p3->Divide(FDUnOscPred_nom);

  //
  // Plotting and output
  //

  if ( debug ) std::cout << "output" << std::endl;

  TFile myPlot("Overlay.root","RECREATE");

  if ( debug ) std::cout << "Create canvas" << std::endl;

  TCanvas *C1=new TCanvas("C1", "C1", 700, 500);

  // Draw overall plot
  C1->Clear();
  // Create two pads
  TPad *uppadC1 = new TPad("uppadC1", "uppadC1", 0, 0.3, 1, 1.0); // xlow, ylow, xup, yup
  uppadC1->SetBottomMargin(0); uppadC1->SetGridx(); uppadC1->SetGridy(); uppadC1->Draw();
  TPad *dnpadC1 = new TPad("dnpadC1", "uppadC1", 0, 0.0, 1, 0.29);
  dnpadC1->SetTopMargin(0); dnpadC1->SetBottomMargin(0.3); dnpadC1->SetGridx(); dnpadC1->SetGridy(); dnpadC1->Draw();

  uppadC1->cd();

  NDDataCorr_FDExtrap_MaCCQE_n3->SetTitle("#nu_{#mu} #rightarrow #nu_{#mu}: MaCCQE");
  NDDataCorr_FDExtrap_MaCCQE_n3->SetMinimum(-0.2);
  NDDataCorr_FDExtrap_MaCCQE_n3->SetMaximum(0.2);
  NDDataCorr_FDExtrap_MaCCQE_n3->SetStats(0);
  NDDataCorr_FDExtrap_MaCCQE_n3->GetYaxis()->SetTitle("(N_{varied} - N_{nom})/N_{FD, unosc}");
  NDDataCorr_FDExtrap_MaCCQE_n3->GetYaxis()->SetTitleSize(0.05);
  NDDataCorr_FDExtrap_MaCCQE_n3->GetYaxis()->SetTitleOffset(0.6);

  NDDataCorr_FDExtrap_MaCCQE_n3->SetLineColor(kRed+3); NDDataCorr_FDExtrap_MaCCQE_n3->SetLineStyle(1); NDDataCorr_FDExtrap_MaCCQE_n3->SetLineWidth(2);
  NDDataCorr_FDExtrap_MaCCQE_n1->SetLineColor(kRed+1); NDDataCorr_FDExtrap_MaCCQE_n1->SetLineStyle(1); NDDataCorr_FDExtrap_MaCCQE_n1->SetLineWidth(2);
  NDDataCorr_FDExtrap_MaCCQE_p1->SetLineColor(kBlue+1); NDDataCorr_FDExtrap_MaCCQE_p1->SetLineStyle(1); NDDataCorr_FDExtrap_MaCCQE_p1->SetLineWidth(2);
  NDDataCorr_FDExtrap_MaCCQE_p3->SetLineColor(kBlue+3); NDDataCorr_FDExtrap_MaCCQE_p3->SetLineStyle(1); NDDataCorr_FDExtrap_MaCCQE_p3->SetLineWidth(2);

  FDOscPred_MaCCQE_n3->SetLineColor(kRed+3); FDOscPred_MaCCQE_n3->SetLineStyle(2); FDOscPred_MaCCQE_n3->SetLineWidth(2);
  FDOscPred_MaCCQE_n1->SetLineColor(kRed+1); FDOscPred_MaCCQE_n1->SetLineStyle(2); FDOscPred_MaCCQE_n1->SetLineWidth(2);
  FDOscPred_MaCCQE_p1->SetLineColor(kBlue+1); FDOscPred_MaCCQE_p1->SetLineStyle(2); FDOscPred_MaCCQE_p1->SetLineWidth(2);
  FDOscPred_MaCCQE_p3->SetLineColor(kBlue+3); FDOscPred_MaCCQE_p3->SetLineStyle(2); FDOscPred_MaCCQE_p3->SetLineWidth(2);

  NDDataCorr_FDExtrap_MaCCQE_n3->Draw("HIST");
  FDOscPred_MaCCQE_n3->Draw("HISTSAME");
  NDDataCorr_FDExtrap_MaCCQE_n1->Draw("HISTSAME");
  FDOscPred_MaCCQE_n1->Draw("HISTSAME");
  NDDataCorr_FDExtrap_MaCCQE_p1->Draw("HISTSAME");
  FDOscPred_MaCCQE_p1->Draw("HISTSAME");
  NDDataCorr_FDExtrap_MaCCQE_p3->Draw("HISTSAME");
  FDOscPred_MaCCQE_p3->Draw("HISTSAME");

  TLegend* L1 = new TLegend(0.5, 0.0, 0.9, 0.35);
  L1->SetBorderSize(0); L1->SetFillStyle(0); L1->SetNColumns(2);
  L1->AddEntry(NDDataCorr_FDExtrap_MaCCQE_n3, "NDDataCorr_FDExtrap: -3#sigma", "l");
  L1->AddEntry(FDOscPred_MaCCQE_n3, "FDOscPred: -3#sigma", "l");
  L1->AddEntry(NDDataCorr_FDExtrap_MaCCQE_n1, "NDDataCorr_FDExtrap: -1#sigma", "l");
  L1->AddEntry(FDOscPred_MaCCQE_n1, "FDOscPred: -1#sigma", "l");
  L1->AddEntry(NDDataCorr_FDExtrap_MaCCQE_p1, "NDDataCorr_FDExtrap: 1#sigma", "l");
  L1->AddEntry(FDOscPred_MaCCQE_p1, "FDOscPred: 1#sigma", "l");
  L1->AddEntry(NDDataCorr_FDExtrap_MaCCQE_p3, "NDDataCorr_FDExtrap: 3#sigma", "l");
  L1->AddEntry(FDOscPred_MaCCQE_p3, "FDOscPred: 3#sigma", "l");
  L1->Draw();

  uppadC1->Update(); uppadC1->Modified();
  gPad->RedrawAxis();
  C1->cd(); C1->Update();

  // (ND - FD) at each varied sigma
  dnpadC1->cd();
  // -3sigma
  TH1F *NDMinusFD_MaCCQE_n3 = (TH1F*)NDDataCorr_FDExtrap_MaCCQE_n3->Clone("NDMinusFD_MaCCQE_n3"); // Clone to avoid changing the original hist
  NDMinusFD_MaCCQE_n3->Add(FDOscPred_MaCCQE_n3, -1);
  NDMinusFD_MaCCQE_n3->SetTitle("");
  NDMinusFD_MaCCQE_n3->GetXaxis()->SetTitleSize(30);
  NDMinusFD_MaCCQE_n3->GetXaxis()->SetTitleFont(43);
  NDMinusFD_MaCCQE_n3->GetXaxis()->SetTitleOffset(4.0);
  NDMinusFD_MaCCQE_n3->GetXaxis()->SetLabelSize(25);
  NDMinusFD_MaCCQE_n3->GetXaxis()->SetLabelFont(43);
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetTitle("ND - FD");
  NDMinusFD_MaCCQE_n3->GetYaxis()->CenterTitle();
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetTitleSize(30);
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetTitleFont(43);
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetTitleOffset(1.0);
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetLabelSize(20);
  NDMinusFD_MaCCQE_n3->GetYaxis()->SetLabelFont(43);
  NDMinusFD_MaCCQE_n3->SetMinimum(-0.2);
  NDMinusFD_MaCCQE_n3->SetMaximum(0.2);
  NDMinusFD_MaCCQE_n3->Draw();
  // -1sigma
  TH1F *NDMinusFD_MaCCQE_n1 = (TH1F*)NDDataCorr_FDExtrap_MaCCQE_n1->Clone("NDMinusFD_MaCCQE_n1");
  NDMinusFD_MaCCQE_n1->Add(FDOscPred_MaCCQE_n1, -1);
  NDMinusFD_MaCCQE_n1->SetTitle("");
  NDMinusFD_MaCCQE_n1->Draw("SAME");
  // 1sigma
  TH1F *NDMinusFD_MaCCQE_p1 = (TH1F*)NDDataCorr_FDExtrap_MaCCQE_p1->Clone("NDMinusFD_MaCCQE_p1");
  NDMinusFD_MaCCQE_p1->Add(FDOscPred_MaCCQE_p1, -1);
  NDMinusFD_MaCCQE_p1->SetTitle("");
  NDMinusFD_MaCCQE_p1->Draw("SAME");
  // 3sigma
  TH1F *NDMinusFD_MaCCQE_p3 = (TH1F*)NDDataCorr_FDExtrap_MaCCQE_p3->Clone("NDMinusFD_MaCCQE_p3");
  NDMinusFD_MaCCQE_p3->Add(FDOscPred_MaCCQE_p3, -1);
  NDMinusFD_MaCCQE_p3->SetTitle("");
  NDMinusFD_MaCCQE_p3->Draw("SAME");
  C1->Write();

  // Draw for each component

  for ( size_t i = 0; i < histnames.size(); i++ ) {

    //
    // Clone histogram
    //

    if ( debug ) std::cout << "looking at histogram: " << histnames[i] << std::endl;

    // Check dir printout before clone !!!

    TH1F *nom = (TH1F*)file0->Get( TString::Format("numu_EvMatch_nom/FD_nu_numu/%s",   histnames[i].Data()) )->Clone("nom");
    TH1F *n3  = (TH1F*)file0->Get( TString::Format("numu_EvMatch_%s_-3/FD_nu_numu/%s", systname.Data(), histnames[i].Data()) )->Clone("n3");
    TH1F *n1  = (TH1F*)file0->Get( TString::Format("numu_EvMatch_%s_-1/FD_nu_numu/%s", systname.Data(), histnames[i].Data()) )->Clone("n1");
    TH1F *p1  = (TH1F*)file0->Get( TString::Format("numu_EvMatch_%s_1/FD_nu_numu/%s",  systname.Data(), histnames[i].Data()) )->Clone("p1");
    TH1F *p3  = (TH1F*)file0->Get( TString::Format("numu_EvMatch_%s_3/FD_nu_numu/%s",  systname.Data(), histnames[i].Data()) )->Clone("p3");

    //
    // Calculate histogram: (varied - nom)/Unosc FD numu (nom)
    //

    n3->Add(nom, -1); n3->Divide(FDUnOscPred_nom);
    n1->Add(nom, -1); n1->Divide(FDUnOscPred_nom);
    p1->Add(nom, -1); p1->Divide(FDUnOscPred_nom);
    p3->Add(nom, -1); p3->Divide(FDUnOscPred_nom);

    //
    // Plot
    //

    TCanvas *canvas = new TCanvas("canvas", "canvas", 700, 500);
    canvas->cd(); canvas->SetGridx(); canvas->SetGridy();
    n3->SetTitle( TString::Format("#nu_{#mu} #rightarrow #nu_{#mu}: %s", systname.Data()) );
    n3->SetMinimum(-0.1);
    n3->SetMaximum(0.1);
    n3->SetStats(0);
    // Xtitle

    n3->GetYaxis()->SetTitle("(N_{varied} - N_{nom})/N_{FD, unosc}");
    n3->GetYaxis()->SetTitleSize(0.05);
    n3->GetYaxis()->SetTitleOffset(0.9);

    n3->SetLineColor(kRed+3);  n3->SetLineStyle(1); n3->SetLineWidth(2); n3->Draw("HIST");
    n1->SetLineColor(kRed+1);  n1->SetLineStyle(1); n1->SetLineWidth(2); n1->Draw("HISTSAME");
    p1->SetLineColor(kBlue+1); p1->SetLineStyle(1); p1->SetLineWidth(2); p1->Draw("HISTSAME");
    p3->SetLineColor(kBlue+3); p3->SetLineStyle(1); p3->SetLineWidth(2); p3->Draw("HISTSAME");

    TLegend* legend = new TLegend(0.1, 0.7, 0.4, 0.9);
    legend->SetBorderSize(0); legend->SetFillStyle(0); legend->SetNColumns(1);
    legend->AddEntry(n3, TString::Format("%s: -3#sigma", histnames[i].Data()), "l");
    legend->AddEntry(n1, TString::Format("%s: -1#sigma", histnames[i].Data()), "l");
    legend->AddEntry(p1, TString::Format("%s: 1#sigma",  histnames[i].Data()), "l");
    legend->AddEntry(p3, TString::Format("%s: 3#sigma",  histnames[i].Data()), "l");
    legend->Draw();
    canvas->Write();

    // delete created objects in current scope
    delete nom;
    delete n3;
    delete n1;
    delete p1;
    delete p3;
    delete canvas;
    delete legend;

  } // end loop over histnames

  file0->Close();
  myPlot.Close();
}
