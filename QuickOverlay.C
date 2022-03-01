//
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q QuickOverlay.C
//

#include "TFile.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"

void QuickOverlay() {

  double ssth23[3] = {0.55, 0.55, 0.60};
  double dmsq32[3] = {2.45, 2.50, 2.55}; //unit 10^-3 eV^2

  TCanvas *FD_FHC_Numu_total = new TCanvas("FD_FHC_Numu_total", "FD_FHC_Numu_total", 700, 500);
  FD_FHC_Numu_total->cd();

  TFile *file1 = TFile::Open("hists_index_1.root");
  TH1F *FD_FHC_Numu_total_1 = (TH1F*)file1->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_1->SetTitle("FD FHC Numu");
  FD_FHC_Numu_total_1->SetMaximum(300);
  FD_FHC_Numu_total_1->GetXaxis()->SetTitle("Reco E_{vis.} (GeV)");
  FD_FHC_Numu_total_1->SetLineColor(1);
  FD_FHC_Numu_total_1->Draw("HIST");

  TFile *file1m = TFile::Open("hists_index_1_missing_proton.root");
  TH1F *FD_FHC_Numu_total_1m = (TH1F*)file1m->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_1m->SetLineStyle(7);
  FD_FHC_Numu_total_1m->SetLineColor(1);
  FD_FHC_Numu_total_1m->Draw("HISTSAME");

  TFile *file2 = TFile::Open("hists_index_2.root");
  TH1F *FD_FHC_Numu_total_2 = (TH1F*)file2->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_2->SetLineColor(2);
  FD_FHC_Numu_total_2->Draw("HISTSAME");

  TFile *file2m = TFile::Open("hists_index_2_missing_proton.root");
  TH1F *FD_FHC_Numu_total_2m = (TH1F*)file2m->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_2m->SetLineStyle(7);
  FD_FHC_Numu_total_2m->SetLineColor(2);
  FD_FHC_Numu_total_2m->Draw("HISTSAME");

  TFile *file3 = TFile::Open("hists_index_3.root");
  TH1F *FD_FHC_Numu_total_3 = (TH1F*)file3->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_3->SetLineColor(4);
  FD_FHC_Numu_total_3->Draw("HISTSAME");

  TFile *file3m = TFile::Open("hists_index_3_missing_proton.root");
  TH1F *FD_FHC_Numu_total_3m = (TH1F*)file3m->Get("FD_FHC_Numu_total");
  FD_FHC_Numu_total_3m->SetLineStyle(7);
  FD_FHC_Numu_total_3m->SetLineColor(4);
  FD_FHC_Numu_total_3m->Draw("HISTSAME");

  TLegend* legend = new TLegend(0.1, 0.7, 0.4, 0.9);
  legend->SetBorderSize(0); legend->SetFillStyle(0); legend->SetNColumns(1);
  legend->AddEntry(FD_FHC_Numu_total_1,  TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}", ssth23[0], dmsq32[0]), "l");
  legend->AddEntry(FD_FHC_Numu_total_1m, TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}, missing proton fake data", ssth23[0], dmsq32[0]), "l");
  legend->AddEntry(FD_FHC_Numu_total_2,  TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}", ssth23[1], dmsq32[1]), "l");
  legend->AddEntry(FD_FHC_Numu_total_2m, TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}, missing proton fake data", ssth23[1], dmsq32[1]), "l");
  legend->AddEntry(FD_FHC_Numu_total_3,  TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}", ssth23[2], dmsq32[2]), "l");
  legend->AddEntry(FD_FHC_Numu_total_3m, TString::Format("sin^{2}(#theta_{23}) = %.2f, #Delta m^{2}_{32} = %.2f #times 10^{-3}eV^{2}, missing proton fake data", ssth23[2], dmsq32[2]), "l");
  legend->Draw();

  // Write to output file
  TFile myPlot("Overlay.root", "RECREATE");

  FD_FHC_Numu_total->Write();
  myPlot.Close();

} // End function: void
