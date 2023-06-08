// root -l -b -q ReadTTreeExample.C

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
#include <TGraph2D.h>
#include <TRandom.h>
#include <TF2.h>

// C++ includes
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

void ReadTTreeExample()
{
  //
  // Read branch from input trees
  //
  TString FileIn = "/dune/app/users/flynnguo/NDEff/DUNE_ND_GeoEff/bin/Output_FDGeoEff_hadron_61454381.root";
  TChain *mytree = new TTree("myntuple");

  // these are branches you want to read from the above root file
  double ND_E_vis_true;
  double Second_variable;
  mytree->SetBranchAddress("ND_E_vis_true", &ND_E_vis_true);
  mytree->SetBranchAddress("Second_variable", &Second_variable);

  TH1F *plot_1D = new TH1F("plot_1D", "plot_1D", 16, 0, 16);
  TH2F *plot_2D = new TH2F("plot_2D", "plot_2D", 16, 0, 16, 16, 0, 16);

  // Loop over all events
  int nentries = 0; // Total input events
  nentries = mytree->GetEntries();
  cout<< "nentries:" << nentries<<endl;
  for ( int ientry = 0; ientry < nentries; ientry++ )
  {
    mytree->GetEntry(ientry);
    plot_1D->Fill(ND_E_vis_true);
    plot_2D->Fill(ND_E_vis_true, Second_variable);
  }// end ientry

  // Create output files
  TFile * outFile = new TFile("out.root", "RECREATE");

  // Create Canvas
  TCanvas *c1 = new TCanvas("c1", "c1", 1600, 1200);
  c1->cd();
  plot_2D->Draw();
  c1->SaveAs("plot2D.pdf");


  outFile->Close();

} // end
