//
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q dm2res.C
//

#include "TFile.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"

void dm2res() {

  TString Dir = "/pnfs/dune/persistent/users/callumw/low_exposure_sensitivities";
  TString BiasDir = "/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei";
  bool drawPOT = true; // Draw POT on xaxis for the res plot, default is exposure
  //std::vector<double> exposure = {6, 12, 24, 66, 100, 150, 197, 336, 646, 1104}; // unit: kt-MW-yr
  std::vector<double> exposure = {6, 12, 24, 66, 100}; // unit: kt-MW-yr

  TString exposureunit = "kt-MW-yrs";
  std::vector<double> PoT; // unit: 10^21
  std::vector<double> xAxisVar;
  std::vector<double> dm2resolution; // unit: 10^(-3) eV^2
  std::vector<double> dm2bias;
  std::vector<double> dm2reswithbias;

  int exposures = exposure.size();
  if ( exposures == 0 ) {
    std::cout << "!!! Please check: no input exposures !!! " << std::endl;
    exit(1);
  }

  //
  // First calculate dm2 resolution at each exposure:
  // 1 data point per exposure, then interpolate linearly
  //

  // Access true and best fit dm2 for each throw (118 parameters) from tree

  TH2F** dm2res_truedm2 = new TH2F*[exposures]; // dm2 resolution vs true dm2 for each exposure
  TCanvas** c_dm2res_truedm2 = new TCanvas*[exposures];

  // Loop over files with exposure
  for ( int iexposure = 0; iexposure < exposures; iexposure++ ) {

    std::cout << "Looking at exposure [kt-MW-yr]: " << exposure.at(iexposure) << std::endl;

    // Convert exposure to POT: 24 kt-MW-yrs = 1.1E21 POT
    PoT.push_back( exposure.at(iexposure)*1.1/24 );

    std::cout << "File check: " << TString::Format( "%s/CAFAna_throws_ndfd_%.0fktMWyr_NH_th13.root", Dir.Data(), exposure.at(iexposure) ) << std::endl;

    dm2res_truedm2[iexposure] = new TH2F(TString::Format("dm2res_truedm2_%.0f", exposure.at(iexposure) ), TString::Format("%.0f %s", exposure.at(iexposure), exposureunit.Data() ), 80, 2.3, 2.7, 200, -0.2, 0.2);

    TFile *file = new TFile( TString::Format( "%s/CAFAna_throws_ndfd_%.0fktMWyr_NH_th13.root", Dir.Data(), exposure.at(iexposure) ) );
    TTree* tree = (TTree*) file->Get("global_fit_info");

    vector<double> *truedm2 = 0;
    vector<double> *bestfitdm2 = 0;
    tree->SetBranchAddress("fFakeDataVals", &truedm2);
    tree->SetBranchAddress("fPostFitValues", &bestfitdm2);

    int throws = 0; // Total number of throws of this exposure
    double tss = 0; // Total sum of squares of deviation from each throw

    throws = tree->GetEntries();
    std::cout << "    Total throws: " << throws << std::endl;

    // Loop over throws
    for ( int ithrow = 0; ithrow < throws; ithrow++ ) {

      tree->GetEntry(ithrow);

      double ideviation = 0; // best fit deviation from true for each throw

      // Get the deviation in this throw
      ideviation = bestfitdm2->at(1) - truedm2->at(1);
      tss = tss + pow(ideviation, 2);

      // Fill resolution hist
      dm2res_truedm2[iexposure]->Fill(truedm2->at(1), ideviation);

    } // end loop over throws

    // Calculate standard deviation
    dm2resolution.push_back( sqrt( tss/(throws-1) ) );

    // Close the file
    delete truedm2;
    delete bestfitdm2;
    delete tree;
    delete file;

    //
    // Then get the missing proton fake data bias:
    // also 1 const for each exposure, linear interpolation
    //

    std::cout << "Bias file check: " << TString::Format( "%s/asimov_dmsq32_ndfd%.0f_allsyst_th13_asimov0_hie1_MissingProtonFakeData:1.root", BiasDir.Data(), exposure.at(iexposure) ) << std::endl;

    TFile *biasfile = new TFile( TString::Format( "%s/asimov_dmsq32_ndfd%.0f_allsyst_th13_asimov0_hie1_MissingProtonFakeData:1.root", BiasDir.Data(), exposure.at(iexposure) ) );
    TTree* biastree = (TTree*) biasfile->Get("global_tree");

    vector<double> *biastruedm2 = 0;
    vector<double> *biasbestfitdm2 = 0;
    biastree->SetBranchAddress("fFakeDataVals", &biastruedm2);
    biastree->SetBranchAddress("fPostFitValues", &biasbestfitdm2);

    // ONLY one entry in this bias file
    biastree->GetEntry(0);

    dm2bias.push_back( biasbestfitdm2->at(1) - biastruedm2->at(1) );

    // Add in quadrature
    dm2reswithbias.push_back( sqrt( pow(biasbestfitdm2->at(1) - biastruedm2->at(1), 2) + tss/(throws-1) ) );

    delete biastruedm2;
    delete biasbestfitdm2;
    delete biastree;
    delete biasfile;

  } // end loop over exposures

  // Double check vector size
  if ( dm2bias.size() != exposures || dm2resolution.size() != exposures ) {
    std::cout << "!!! Please check: vector size doesn't match, can't draw TGraph !!! " << std::endl;
    exit(2);
  }

  // Output file
  TFile myPlot("dm2res.root", "RECREATE");

  // Three curves: resolution, bias, combine in quadrature
  TCanvas *dm2res_exposure = new TCanvas("dm2res_exposure", "dm2res_exposure", 700, 500);
  dm2res_exposure->cd();

  if ( drawPOT ) xAxisVar = PoT;
  else xAxisVar = exposure;

  TGraph *g1 = new TGraph(exposures, &xAxisVar[0], &dm2resolution[0]);
  TGraph *g2 = new TGraph(exposures, &xAxisVar[0], &dm2bias[0]);
  TGraph *g3 = new TGraph(exposures, &xAxisVar[0], &dm2reswithbias[0]);
  g1->SetLineColor(1);
  g2->SetLineColor(2);
  g3->SetLineColor(4);

  TMultiGraph *mg = new TMultiGraph();
  mg->Add(g1);
  //mg->Add(g2);
  mg->Add(g3);
  if ( drawPOT ) mg->GetXaxis()->SetTitle( "POT (10^{21})" );
  else mg->GetXaxis()->SetTitle( TString::Format( "Exposure (%s)", exposureunit.Data() ) );
  mg->GetYaxis()->SetTitle("#Delta m^{2}_{32} Resolution (10^{-3}eV^{2})");
  mg->SetTitle("DUNE Sensitivity (All Systematics)");
  mg->Draw("AL");

  TLegend* legend = new TLegend(0.67, 0.53, 0.88, 0.89);
  legend->SetBorderSize(0); legend->SetFillStyle(0); legend->SetNColumns(1);
  //legend->SetHeader("#splitline{Normal Ordering}{#theta_{13}}", "C");
  legend->AddEntry(g1, "ND-LAr + TMS + PRISM", "l");
  //legend->AddEntry(g2, "#splitline{Missing proton bias:}{best fit - true dm2}", "l");
  legend->AddEntry(g3, "ND-LAr + TMS on-axis only", "l");
  legend->Draw();

  dm2res_exposure->Write();

  // Draw 2D histograms
  for ( int iexposure = 0; iexposure < exposures; iexposure++ ) {
    c_dm2res_truedm2[iexposure] = new TCanvas(TString::Format( "c_dm2res_truedm2_%.0f", exposure.at(iexposure) ), TString::Format("dm2 resolution vs true dm2 (%.0f %s)", exposure.at(iexposure), exposureunit.Data() ), 700, 500);
    c_dm2res_truedm2[iexposure]->cd();
    dm2res_truedm2[iexposure]->GetXaxis()->SetTitle("True dm2");
    dm2res_truedm2[iexposure]->GetYaxis()->SetTitle("Best fit - true dm2");
    dm2res_truedm2[iexposure]->GetZaxis()->SetTitle("Throws");
    dm2res_truedm2[iexposure]->GetZaxis()->SetTitleOffset(0.7);
    dm2res_truedm2[iexposure]->SetStats(0);
    dm2res_truedm2[iexposure]->Draw("COLZ");
    c_dm2res_truedm2[iexposure]->Write();
  }

  myPlot.Close();

} // End function: void
