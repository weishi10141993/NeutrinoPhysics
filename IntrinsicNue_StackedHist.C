// plot flux fit
//
#include <string>
#include <iostream>

void IntrinsicNue_StackedHist() {

  // Oscillation channels: FD_nu_numu, FD_nu_nue, FD_nu_nutau, FD_nub_numu, FD_nub_nue, FD_nub_nutau
  TString channame = "FD_nub_nue";
  TString dirname = "numu_EvMatch_nom_2";

  TFile *f = new TFile("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app/PRISMPred_EVisReco_IntrinsicNue_RHC_App.root");
  TCanvas *newC = new TCanvas("C", "C", 800, 600);
  newC->SetRightMargin(0.07);
  newC->SetLeftMargin(0.12);
  TLegend *leg = new TLegend(0.5,0.45,0.9,0.88);
  if ( channame == "FD_nu_nue" ) {
    leg->SetHeader("DUNE-PRISM FHC #nu_{e} Appearance");
  } else if ( channame == "FD_nub_nue" ) {
    leg->SetHeader("DUNE-PRISM RHC #bar{#nu}_{e} Appearance");
  }
  leg->SetBorderSize(0);

  THStack *Hstack = new THStack("Hstack", "");

  TH1D *hFDOsc = (TH1D*)gDirectory->Get( TString::Format("%s/%s/DataPred_Total", dirname.Data(), channame.Data() ) );
  TH1D *hNDFit = (TH1D*)gDirectory->Get( TString::Format("%s/%s/NDData_FDExtrap", dirname.Data(), channame.Data() ) ); // NDLinearComb NDData_FDExtrap
  TH1D *hMCCorr = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDFluxCorr", dirname.Data(), channame.Data() ) );
  TH1D *hNCBkg = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDNCBkg", dirname.Data(), channame.Data() ) );
  TH1D *hWSBkg = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDWSBkg", dirname.Data(), channame.Data() ) );
  TH1D *hWLBkg = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDWrongLepBkg", dirname.Data(), channame.Data() ) );
  TH1D *hNTBkg = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDNuTauCCBkg", dirname.Data(), channame.Data() ) ); // for nutau, this is not bkg, do not plot
  TH1D *hInBkg = (TH1D*)gDirectory->Get( TString::Format("%s/%s/FDIntrinsicBkg", dirname.Data(), channame.Data() ) ); // nutau/bar from nue/bar
  TH1D *hPRISMPred = (TH1D*)gDirectory->Get( TString::Format("%s/%s/NDDataCorr_FDExtrap", dirname.Data(), channame.Data() ) ); // PRISMPred NDDataCorr_FDExtrap

  double chi2(0);
  int totbin(0);
  totbin = hFDOsc->GetXaxis()->GetNbins() - 1; // exclude overflow bin
  std::cout << "Tot bins = " << totbin << std::endl;
  for (int bin = 1; bin <= totbin; bin++) {
    chi2 += pow(hFDOsc->GetBinContent(bin) - hPRISMPred->GetBinContent(bin), 2) / hPRISMPred->GetBinContent(bin);
  }
  std::cout << "Chi2 = " << chi2 << std::endl;

  gStyle->SetOptStat(0);

  hPRISMPred->SetLineWidth(1);
  hPRISMPred->SetLineColor(kRed);
  //hPRISMPred->SetMarkerStyle(2);
  hPRISMPred->SetFillColorAlpha(46, 0.35);

  hFDOsc->SetLineWidth(3);
  hFDOsc->SetLineColor(kBlue + 2);
  hFDOsc->SetMarkerStyle(8);

  hNDFit->SetLineColor(kRed+1);
  hNDFit->SetLineWidth(1);
  hNDFit->SetFillStyle(3002);
  hNDFit->SetFillColor(kRed);

  hMCCorr->SetLineColor(kBlack);
  hMCCorr->SetFillStyle(3002);
  hMCCorr->SetFillColor(kBlack);

  hNCBkg->SetLineColor(kYellow);
  hNCBkg->SetFillStyle(3001);
  hNCBkg->SetFillColor(kYellow);

  hInBkg->SetLineColor(kGreen);
  hInBkg->SetFillStyle(3001);
  hInBkg->SetFillColor(kGreen);

  hWSBkg->SetLineColor(kBlue);
  hWSBkg->SetFillStyle(3001);
  hWSBkg->SetFillColor(kBlue);

  hWLBkg->SetLineColor(kMagenta);
  hWLBkg->SetFillStyle(3001);
  hWLBkg->SetFillColor(kMagenta);

  hNTBkg->SetLineColor(kCyan);
  hNTBkg->SetFillStyle(3001);
  hNTBkg->SetFillColor(kCyan);

  Hstack->Add(hMCCorr, "HIST");
  Hstack->Add(hWSBkg, "HIST");
  Hstack->Add(hNCBkg, "HIST");
  Hstack->Add(hInBkg, "HIST");
  Hstack->Add(hWLBkg, "HIST");
  Hstack->Add(hNTBkg, "HIST");
  Hstack->Add(hNDFit, "HIST");

  leg->AddEntry(hFDOsc, "FD #nu_{e} Data", "PL");
  if ( channame == "FD_nu_nue" ) { // from nue or nuebar
    leg->AddEntry(hNDFit, "ND Data LC (signal + intrinsic #nu_{e})", "F");
  } else if ( channame == "FD_nub_nue" ) {
    leg->AddEntry(hNDFit, "ND Data LC (signal + intrinsic #bar{#nu}_{e})", "F");
  }
  leg->AddEntry(hPRISMPred, "ND Linear Comb. Error", "F");
  leg->AddEntry(hNTBkg, "(#nu_{#tau} + #bar{#nu}_{#tau}) CC", "F"); // don't need this for nutau
  leg->AddEntry(hWLBkg, "(#nu_{#mu} + #bar{#nu}_{#mu}) CC", "F");
  leg->AddEntry(hNCBkg, "NC", "F");
  if ( channame == "FD_nu_nue" ) { // from nue or nuebar
    leg->AddEntry(hInBkg, "Intrinsic #bar{#nu}_{e} CC", "F");
    leg->AddEntry(hWSBkg, "(#bar{#nu}_{#mu} #rightarrow #bar{#nu}_{e}) CC", "F");
  } else if ( channame == "FD_nub_nue" ) {
    leg->AddEntry(hInBkg, "Intrinsic #nu_{e} CC", "F");
    leg->AddEntry(hWSBkg, "(#nu_{#mu} #rightarrow #nu_{e}) CC", "F");
  }
  leg->AddEntry(hMCCorr, "Flux Match Corr.", "F");

  newC->cd();
  newC->SetTopMargin(0.08);
  Hstack->Draw("HIST");
  if ( channame == "FD_nu_nue" ) {
    Hstack->SetTitle("48 kt-MW-Years FHC #nu_{#mu} #rightarrow #nu_{e}"); // Extrapolated Prediction with Selection
    Hstack->SetMaximum(230);
    Hstack->SetMinimum(-20);
  } else if ( channame == "FD_nub_nue" ) {
    Hstack->SetTitle("48 kt-MW-Years RHC #bar{#nu}_{#mu} #rightarrow #bar{#nu}_{e}");
    Hstack->SetMaximum(57);
    Hstack->SetMinimum(-6);
  } else if ( channame == "FD_nu_nutau" ) {
    Hstack->SetTitle("48 kt-MW-Years FHC #nu_{#mu} #rightarrow #nu_{#tau}");
    Hstack->SetMaximum(7000);
    Hstack->SetMinimum(-10);
  }
  Hstack->GetXaxis()->SetRangeUser(0, 10); // don't draw overflow bin
  Hstack->GetXaxis()->SetTitleOffset(1.2);
  Hstack->GetXaxis()->SetTitle("Reco E_{vis.} (GeV)");
  Hstack->GetYaxis()->SetTitle("Pred. Event Rate per 1 GeV");

  hPRISMPred->Draw("E2P SAME");

  hFDOsc->Draw("P SAME");
  leg->Draw();

  // Save canvas
  if ( channame == "FD_nu_nue" ) {
    newC->SaveAs("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app/NueAppStacked.pdf");
  } else if ( channame == "FD_nub_nue" ) {
    newC->SaveAs("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app/NueBarAppStacked.pdf");
  } else if ( channame == "FD_nu_nutau" ) {
    newC->SaveAs("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app/NutauAppStacked.pdf");
  }

  //***********************************************
  // Get coefficients
  //***********************************************


  TCanvas *cW = new TCanvas("Cw", "Cw", 200, 10, 800, 500);
  cW->SetLeftMargin(0.15);
  cW->cd();
  TPad *NomP = new TPad("NomW", "Nom HC weights", 0.0, 0.0, 0.8, 1.0);
  TPad *AltP = new TPad("AltW", "Alt HC weights", 0.8, 0.0, 1.0, 1.0);
  NomP->Draw();
  AltP->Draw();

  TLegend *legW = new TLegend(0.12,0.7,0.6,0.88);
  legW->SetBorderSize(0);
  legW->AddEntry((TObject*)0, "#Delta m^{2}_{32} = 2.38 #times 10^{-3} eV^{2}", "");
  legW->AddEntry((TObject*)0, "sin^{2}(#theta_{23}) = 0.55", "");

  TH1D *W = (TH1D*)gDirectory->Get( TString::Format("%s/%s/NDFD_matcher/last_match_293kA", dirname.Data(), channame.Data() ) );
  const double maxYaxis = W->GetMaximum() + 7e-6;
  const double minYaxis = W->GetMinimum() - 7e-6;

  NomP->cd();
  NomP->SetRightMargin(0.01);

  W->GetXaxis()->SetTitle("Distance Off-Axis (m) 293kA");
  W->GetYaxis()->SetTitle("Off-Axis Weights");
  W->SetAxisRange(minYaxis, maxYaxis, "Y");
  W->GetXaxis()->SetLabelSize(0.045);
  W->GetYaxis()->SetLabelSize(0.045);
  W->GetXaxis()->SetTitleSize(0.04);
  W->GetYaxis()->SetTitleSize(0.04);
  W->GetXaxis()->SetTitleOffset(1.1);
  W->GetYaxis()->SetTitleOffset(1.1);
  W->SetLineWidth(2);

  W->Draw("HIST");

  AltP->cd();
  AltP->SetLeftMargin(0.01);

  TH1D *altW = (TH1D*)gDirectory->Get( TString::Format("%s/%s/NDFD_matcher/last_match_280kA", dirname.Data(), channame.Data() ) );
  altW->SetAxisRange(minYaxis, maxYaxis, "Y");
  altW->GetYaxis()->SetLabelSize(0);
  altW->GetYaxis()->SetTitleSize(0);
  altW->GetYaxis()->SetTickLength(0.1);
  altW->GetXaxis()->SetTitle("On-Axis 280 kA");
  altW->GetXaxis()->SetTitleSize(0.12);
  altW->GetXaxis()->SetTitleOffset(0.35);
  altW->GetXaxis()->SetLabelOffset(0.000001);
  altW->GetXaxis()->SetLabelSize(0.0);
  altW->GetXaxis()->SetMaxDigits(3);
  altW->SetLineWidth(2);
  altW->Draw("HIST");


  // Set directories to zero
  //nomW->SetDirectory(0);
  altW->SetDirectory(0);
  W->SetDirectory(0);
  hFDOsc->SetDirectory(0);
  hNDFit->SetDirectory(0);
  hMCCorr->SetDirectory(0);
  hPRISMPred->SetDirectory(0);
  hNCBkg->SetDirectory(0);
  hWSBkg->SetDirectory(0);
  hWLBkg->SetDirectory(0);
  hNTBkg->SetDirectory(0);

  f->Close();
}
