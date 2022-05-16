//------------------------------------------------------------------
// Adapted from C. Hasnip's original code to plot the 1D dChi2 (min Chi2 subtracted)
// for including two different set of systematics
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q OverlaydChi2IntrinsicNue.C
//------------------------------------------------------------------

void OverlaydChi2IntrinsicNue() {
  TString HornCurrent = "FourFlavor"; // FHC, RHC, FourFlavor
	TString FileFlagOff = "Chi2_dmsq32_ssth23_dcp_stat_only_FourFlavor_flagoff.root";
	TString FileFlagOn  = "Chi2_dmsq32_ssth23_dcp_stat_only_FourFlavor_flagon.root";
	TString Dir = "/pnfs/dune/persistent/users/weishi/IntrinsicNueFit"; // File on local

	std::vector<TString> pois {"dmsq32", "ssth23", "dcp"};
  TString PoI = "undefined";
	float MaxdChi2 = 100;

	for (int ipoi = 0; ipoi < pois.size(); ipoi++) {
		PoI = pois.at(ipoi);

		if ( PoI != "dmsq32" && PoI != "ssth23" && PoI != "dcp" ) {
			std::cout << "[ERROR] Unknown PoI: " << PoI << std::endl;
			abort();
		}

		std::cout << "Looking at poi: " << PoI << std::endl;

    if ( HornCurrent ==  "FHC" ) {
      if ( PoI == "ssth23" ) MaxdChi2 = 30;
  		else if ( PoI == "dmsq32" ) MaxdChi2 = 20;
  		else if ( PoI == "dcp" ) MaxdChi2 = 30;
    } else if ( HornCurrent ==  "RHC" ) {
      if ( PoI == "ssth23" ) MaxdChi2 = 20;
  		else if ( PoI == "dmsq32" ) MaxdChi2 = 15;
  		else if ( PoI == "dcp" ) MaxdChi2 = 3;
    } else if ( HornCurrent ==  "FourFlavor" ) {
      if ( PoI == "ssth23" ) MaxdChi2 = 40;
  		else if ( PoI == "dmsq32" ) MaxdChi2 = 30;
  		else if ( PoI == "dcp" ) MaxdChi2 = 40;
    } else {
      std::cout << "[ERROR] Unknown HornCurrent: " << HornCurrent << std::endl;
			abort();
    }

		// input fit file
	  TFile *f1 = TFile::Open( TString::Format("%s/%s/%s", Dir.Data(), HornCurrent.Data(), FileFlagOff.Data() ) );
	  TFile *f2 = TFile::Open( TString::Format("%s/%s/%s", Dir.Data(), HornCurrent.Data(), FileFlagOn.Data() ) );

		if ( f1->IsZombie() || f2->IsZombie() ) {
			std::cout << "[ERROR] File not read." << std::endl;
			abort();
		}

		// Two hists
  	TH1F *h_flag_off = (TH1F*)f1->Get( TString::Format("%s_nom/dChi2Scan", PoI.Data() ) )->Clone("h_flag_off");
	  TH1F *h_flag_on  = (TH1F*)f2->Get( TString::Format("%s_nom/dChi2Scan", PoI.Data() ) )->Clone("h_flag_on");

    // Find minimum Chi2 and the bin
	  double min_flagoff(1E15), min_flagon(1E15);
	  int minbin_flagoff(1), minbin_flagon(1);
	  // This assumes plots have same number of bins, which is usually true
	  for (int bin = 1; bin <= h_flag_off->GetXaxis()->GetNbins(); bin++) {
			if (h_flag_off->GetBinContent(bin) < min_flagoff) {
			  min_flagoff = h_flag_off->GetBinContent(bin);
				minbin_flagoff = bin;
      }
		  if (h_flag_on->GetBinContent(bin) < min_flagon) {
			  min_flagon = h_flag_on->GetBinContent(bin);
			  minbin_flagon = bin;
      }
		}

	  std::cout << "poi: " << PoI.Data() << "h_flag_off bin: " << minbin_flagoff << ", min chi2: " << min_flagoff << std::endl;
    std::cout << "poi: " << PoI.Data() << "h_flag_on  bin: " << minbin_flagon  << ", min chi2: " << min_flagon  << std::endl;

    // Subtract min Chi2
	  for (int bin = 1; bin <= h_flag_off->GetXaxis()->GetNbins(); bin++) {
		  h_flag_off->SetBinContent(bin, h_flag_off->GetBinContent(bin) - min_flagoff);
		  h_flag_on->SetBinContent(bin, h_flag_on->GetBinContent(bin) - min_flagon);
	  }

	  TCanvas *c = new TCanvas("c", "c", 800, 600);
	  c->cd(); c->SetGridx(); c->SetGridy();
    // Plot style
	  h_flag_off->SetTitle( TString::Format("DUNE-PRISM %s Joint Fit (Exposure: 1 yr)",  HornCurrent.Data() ) );
	  h_flag_off->SetStats(0);
	  if ( PoI == "ssth23" ) h_flag_off->GetXaxis()->SetTitle("sin^{2}(#theta_{23})");
	  else if ( PoI == "dmsq32" ) h_flag_off->GetXaxis()->SetTitle("#Delta m^{2}_{32} (#times 10^{-3} eV^{2})");
		else if ( PoI == "dcp" ) h_flag_off->GetXaxis()->SetTitle("#delta_{cp}");
		h_flag_off->SetMaximum(MaxdChi2);
		h_flag_off->GetXaxis()->SetTitleOffset(1.2);
		h_flag_off->GetXaxis()->SetTitleSize(0.04);
		h_flag_off->GetXaxis()->SetLabelOffset(0.01);
		h_flag_off->GetXaxis()->SetLabelSize(0.04);
		h_flag_off->GetYaxis()->SetTitle("#Delta #chi^{2}");
		h_flag_off->GetYaxis()->SetTitleSize(0.04);
		h_flag_off->GetYaxis()->SetLabelSize(0.04);
		h_flag_off->GetYaxis()->SetLabelOffset(0.015);
		h_flag_off->SetLineColor(1);
		h_flag_off->SetLineWidth(3);
		h_flag_off->SetLineStyle(7);
		h_flag_off->Draw("HIST");
		h_flag_on->SetLineColor(2);
		h_flag_on->SetLineWidth(2);
		h_flag_on->Draw("HIST SAME");

		TLegend *leg = new TLegend(0.19, 0.62, 0.56, 0.89);
	  leg->SetHeader("FD Intrinsic #nu_{e} Bkg Options", "C");
		leg->AddEntry(h_flag_off, TString::Format("#splitline{MC estimation}{min #chi^{2}: %.5f, bin: %d}",  min_flagoff, minbin_flagoff), "L");
		leg->AddEntry(h_flag_on,  TString::Format("#splitline{Include in LC}{min #chi^{2}: %.5f, bin: %d}",  min_flagon,  minbin_flagon),  "L");
		leg->SetBorderSize(0);
    leg->SetFillStyle(0);
		leg->Draw();

		c->SaveAs( TString::Format("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/%s_FitOverlay_%s.png", HornCurrent.Data(), PoI.Data() ) );

		f1->Close();
		f2->Close();
		delete c;
	} // end poi

} // end main func
