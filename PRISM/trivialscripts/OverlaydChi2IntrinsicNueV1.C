//------------------------------------------------------------------
// This is used when the fit of one poi is done via 50 jobs for 50 points
// So you need to create the fitted 1d chi2 first before overlay
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q OverlaydChi2IntrinsicNueV1.C
//------------------------------------------------------------------

void OverlaydChi2IntrinsicNueV1() {
  // User edit
	std::vector<TString> pois {"dmsq32"}; //dmsq32, ssth23, dcp
  TString HornCurrent = "FHC"; // FHC, RHC, FourFlavor
  TString Syst = "nodet"; // nodet means xsec+flux
  TString Dir = "/pnfs/dune/persistent/users/weishi/IntrinsicNueFit"; // File on gpvm

  // Initialize
  TString PoI = "undefined";
	float MaxdChi2 = 100;
  int PoiBins = 50; // number of bins for each poi
  float XAxisLow = 0; // range of poi to plot, reset below
  float XAxisHigh = 100;

  // loop over pois
	for (int ipoi = 0; ipoi < pois.size(); ipoi++) {
		PoI = pois.at(ipoi);

		if ( PoI != "dmsq32" && PoI != "ssth23" && PoI != "dcp" ) {
			std::cout << "[ERROR] Unknown PoI: " << PoI << std::endl;
			abort();
		}

		std::cout << "Looking at poi: " << PoI << ", Horn current: " << HornCurrent << std::endl;

    // set poi bins
    if ( PoI == "ssth23" ) {
      PoiBins = 50;
      XAxisLow = 0.4;
      XAxisHigh = 0.65;
    }
    else if ( PoI == "dmsq32" ) {
      PoiBins = 50;
      XAxisLow = 2.3;
      XAxisHigh = 2.55;
    }
    else if ( PoI == "dcp" ) {
      PoiBins = 40;
      XAxisLow = -1;
      XAxisHigh = 1;
    }

    // set dChi2 max in plot
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

    // Two hists
    TH1F *h_flag_off = new TH1F("h_flag_off", "h_flag_off", PoiBins, XAxisLow, XAxisHigh);
    TH1F *h_flag_on  = new TH1F("h_flag_on",  "h_flag_on",  PoiBins, XAxisLow, XAxisHigh);

    // Loop over all bins to add chi2 hist
    for (int bin = 1; bin <= PoiBins; bin++) {
      // Input fit file
  	  TFile *f1 = TFile::Open( TString::Format("%s/%s/%s_%s/flagoff/PRISMScan_%s_bin%d.root",  Dir.Data(), HornCurrent.Data(), PoI.Data(), Syst.Data(), PoI.Data(), bin ) );
  	  TFile *f2 = TFile::Open( TString::Format("%s/%s/%s_%s/flagon/PRISMScan_%s_bin%d.root",   Dir.Data(), HornCurrent.Data(), PoI.Data(), Syst.Data(), PoI.Data(), bin ) );

  		if ( f1->IsZombie() || f2->IsZombie() ) {
  			std::cout << "[ERROR] File not read." << std::endl;
  			abort();
  		}

      TH1F *h_tmp_flag_off = (TH1F*)f1->Get( TString::Format("%s_all/dChi2Scan", PoI.Data() ) )->Clone("h_tmp_flag_off");
  	  TH1F *h_tmp_flag_on  = (TH1F*)f2->Get( TString::Format("%s_all/dChi2Scan", PoI.Data() ) )->Clone("h_tmp_flag_on");

      // Fill histograms
      h_flag_off->SetBinContent(bin, h_tmp_flag_off->GetBinContent(1)); // each hist has one bin
      h_flag_on->SetBinContent(bin,  h_tmp_flag_on->GetBinContent(1));

      delete h_tmp_flag_off;
      delete h_tmp_flag_on;
      f1->Close();
  		f2->Close();
      delete f1;
      delete f2;
    } // end loop over poibins

    // Find minimum Chi2 and the bin --> Probably easiest to use GetMinimumBin?
	  double min_flagoff(1E15), min_flagon(1E15);
	  int minbin_flagoff(1), minbin_flagon(1);
	  // This assumes plots have same number of bins, which is usually true
	  for (int bin = 1; bin <= PoiBins; bin++) {
			if (h_flag_off->GetBinContent(bin) < min_flagoff) {
			  min_flagoff = h_flag_off->GetBinContent(bin);
				minbin_flagoff = bin;
      }
		  if (h_flag_on->GetBinContent(bin) < min_flagon) {
			  min_flagon = h_flag_on->GetBinContent(bin);
			  minbin_flagon = bin;
      }
		}

	  std::cout << "poi: " << PoI.Data() << "h_flag_off bin: " << minbin_flagoff << ", min fit chi2: " << min_flagoff << std::endl;
    std::cout << "poi: " << PoI.Data() << "h_flag_on  bin: " << minbin_flagon  << ", min fit chi2: " << min_flagon  << std::endl;

    // Subtract min Chi2
	  for (int bin = 1; bin <= PoiBins; bin++) {
		  h_flag_off->SetBinContent(bin, h_flag_off->GetBinContent(bin) - min_flagoff);
		  h_flag_on->SetBinContent(bin, h_flag_on->GetBinContent(bin) - min_flagon);
	  }

	  TCanvas *c = new TCanvas("c", "c", 800, 600);
	  c->cd(); c->SetGridx(); c->SetGridy();
    // Plot style
	  h_flag_off->SetTitle( TString::Format("DUNE-PRISM %s Joint Fit: xsec+flux systematics (Exposure: 1 yr)",  HornCurrent.Data() ) );
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
	  leg->SetHeader("FD Intrinsic #nu_{e} Bkg Options", "L");
		leg->AddEntry(h_flag_off, TString::Format("#splitline{MC estimation}{min #chi^{2}: %.5f, bin: %d}",  min_flagoff, minbin_flagoff), "L");
		leg->AddEntry(h_flag_on,  TString::Format("#splitline{Include in LC}{min #chi^{2}: %.5f, bin: %d}",  min_flagon,  minbin_flagon),  "L");
		leg->SetBorderSize(0);
    leg->SetFillStyle(0);
		leg->Draw();

		c->SaveAs( TString::Format("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/%s_FitOverlay_%s.png", HornCurrent.Data(), PoI.Data() ) );

    delete h_flag_off;
    delete h_flag_on;
		delete c;
	} // end poi

} // end main func
