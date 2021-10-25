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

  TString systname = "MaCCQE";
  TFile *file0 = TFile::Open("/dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/app/PRISMPred_EVisReco_MaCCQE.root");

  vector<TString> histnames;
	histnames.clear();
	histnames.push_back("NDData_FDExtrap");
	histnames.push_back("FDFluxCorr");
  histnames.push_back("FDNuTauCCBkg");
  histnames.push_back("FDWrongLepBkg");
  histnames.push_back("FDNCBkg");
  histnames.push_back("FDWSBkg");

  if ( debug ) std::cout << "clone plots" << std::endl;

  // Use unosc FD evt rate as normalization
  TH1F *FDUnOscPred_nom = (TH1F*)file0->Get("numu_EvMatch_nom/FD_nu_numu/FDUnOscPred")->Clone("FDUnOscPred_nom");

  if ( debug ) std::cout << "output" << std::endl;

  TFile myPlot("Overlay.root","RECREATE");

  // Draw each component
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
    n3->GetXaxis()->SetTitle("Reco E_{vis} (GeV)");
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
