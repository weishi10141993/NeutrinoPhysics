//------------------------------------------------------------------
// Adapted from C. Hasnip's original code to plot the 1D dChi2 (min Chi2 subtracted)
// for including two different set of systematics
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q PlotdChiSq1DScan.C
//------------------------------------------------------------------

void PlotdChiSq1DScan() {
	TString NSyst = "nsyst"; // all N systs
	TString N_nSyst = NSyst + "_minus_n"; // exclude n systs
	TString PoI = "poi"; // Options: dmsq32, ssth23
	TString Dir = "/dune/app/users/weishi/FitChi2"; // File on FNAL dunegpvm machine
	float MaxdChi2 = 120;

	if ( PoI != "dmsq32" && PoI != "ssth23") {
		std::cout << "[ERROR] Unknown PoI" << std::endl;
		abort();
	}

	if ( PoI == "ssth23" ) MaxdChi2 = 45;
	else if ( PoI == "dmsq32" ) MaxdChi2 = 40;

	// this is all systs + step by step
	TFile *f1 = TFile::Open( TString::Format("%s/%s_1D_Chi2_%s.root", Dir.Data(), PoI.Data(), NSyst.Data() ) );
	// exclude some systs + step by step
	TFile *f2 = TFile::Open( TString::Format("%s/%s_1D_Chi2_%s.root", Dir.Data(), PoI.Data(), N_nSyst.Data() ) );

	if ( f1->IsZombie() || f2->IsZombie() ) {
		std::cout << "[ERROR] File not read." << std::endl;
		abort();
	}

	// Three curves: stats only, N syst, N-n syst
	TH1F *hnosyst = (TH1F*)f1->Get( TString::Format("%s_nom/h_dChi2_Total", PoI.Data() ) )->Clone("hnosyst"); // stat only is always same in each file
	TH1F *hallsyst = (TH1F*)f1->Get( TString::Format("%s_all/h_dChi2_Total", PoI.Data() ) )->Clone("hallsyst");
	TH1F *hexcludesyst = (TH1F*)f2->Get( TString::Format("%s_all/h_dChi2_Total", PoI.Data() ) )->Clone("hexcludesyst");

  // Find minimum Chi2 and the bin
	double min_nosyst(1E15), min_allsyst(1E15), min_excludesyst(1E15);
	int minbin_nosyst(1), minbin_allsyst(1), minbin_excludesyst(1);
	// This assumes plots have same number of bins, which is usually true
	for (int bin = 1; bin <= hallsyst->GetXaxis()->GetNbins(); bin++) {
		if (hnosyst->GetBinContent(bin) < min_nosyst) {
			min_nosyst = hnosyst->GetBinContent(bin);
			minbin_nosyst = bin;
    }
		if (hallsyst->GetBinContent(bin) < min_allsyst) {
			min_allsyst = hallsyst->GetBinContent(bin);
			minbin_allsyst = bin;
    }
		if (hexcludesyst->GetBinContent(bin) < min_excludesyst) {
			min_excludesyst = hexcludesyst->GetBinContent(bin);
			minbin_excludesyst = bin;
    }
	}

	std::cout << "hnosyst bin: "      << minbin_nosyst      << ", min chi2: " << min_nosyst << std::endl;
  std::cout << "hallsyst bin: "     << minbin_allsyst     << ", min chi2: " << min_allsyst << std::endl;
	std::cout << "hexcludesyst bin: " << minbin_excludesyst << ", min chi2: " << min_excludesyst << std::endl;

  // Subtract min Chi2
	for (int bin = 1; bin <= hallsyst->GetXaxis()->GetNbins(); bin++) {
		hnosyst->SetBinContent(bin, hnosyst->GetBinContent(bin) - min_nosyst);
		hallsyst->SetBinContent(bin, hallsyst->GetBinContent(bin) - min_allsyst);
		hexcludesyst->SetBinContent(bin, hexcludesyst->GetBinContent(bin) - min_excludesyst);
	}

	TFile output( TString::Format("%s/%s_1D_dChi2_%s.root", Dir.Data(), PoI.Data(), N_nSyst.Data() ), "RECREATE");

	TCanvas *c = new TCanvas("c", "c", 800, 600);
	c->cd(); c->SetGridx(); c->SetGridy();
  // Plot style
	hnosyst->SetTitle("DUNE-PRISM Disapperance Joint Fit");
	hnosyst->SetStats(0);
	if ( PoI == "ssth23" ) hnosyst->GetXaxis()->SetTitle("sin^{2}(#theta_{23})");
	else if ( PoI == "dmsq32" ) hnosyst->GetXaxis()->SetTitle("#Delta m^{2}_{32} (#times 10^{-3} eV^{2})");
	hnosyst->SetMaximum(MaxdChi2);
	hnosyst->GetXaxis()->SetTitleOffset(1.2);
	hnosyst->GetXaxis()->SetTitleSize(0.04);
	hnosyst->GetXaxis()->SetLabelOffset(0.01);
	hnosyst->GetXaxis()->SetLabelSize(0.04);
	hnosyst->GetYaxis()->SetTitle("#Delta #chi^{2}");
	hnosyst->GetYaxis()->SetTitleSize(0.04);
	hnosyst->GetYaxis()->SetLabelSize(0.04);
	hnosyst->GetYaxis()->SetLabelOffset(0.015);
	hnosyst->SetLineColor(1);
	hnosyst->SetLineWidth(3);
	hnosyst->SetLineStyle(7);
	hnosyst->Draw("HIST");
	hallsyst->SetLineColor(2);
	hallsyst->SetLineWidth(2);
	hallsyst->Draw("HIST SAME");
	hexcludesyst->SetLineColor(4);
	hexcludesyst->SetLineWidth(2);
	hexcludesyst->Draw("HIST SAME");

	// Add ratio plot?

	TLegend *leg = new TLegend(0.48, 0.50, 0.89, 0.77);
  leg->SetHeader("Included Systematics");
	leg->AddEntry(hnosyst, "No Syst.", "L");
	leg->AddEntry(hallsyst, "Xsec Systs", "L");
	leg->AddEntry(hexcludesyst, "Xsec Systs - n", "L");
	leg->AddEntry((TObject*)0, "sin^{2}(2#theta_{13}) = 0.088 #pm 0.003", "");
	leg->AddEntry((TObject*)0, "Exposure = 3.5 yrs", "");
	leg->SetBorderSize(0);
	leg->Draw();

	c->Write();

	f1->Close();
	f2->Close();
	output.Close();
}
