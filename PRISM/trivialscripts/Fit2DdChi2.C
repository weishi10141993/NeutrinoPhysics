//------------------------------------------------------------------
// This script plots dChi2=1, 4, 9 ( 1, 2, 3 sigma) contours
// when the fit of 2 pois are done via 50x50=2500 jobs
// It first plot the 2D dChi2 with its min subtracted,
// Then it finds points where the dChi2 crosses 1, 4 and 9
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q Fit2DdChi2.C
//------------------------------------------------------------------

void Fit2DdChi2() {
  // User edit
  TString FitChannel = "Numu_disp"; // Numu_disp
  TString Syst = "StatOnly"; // StatOnly
	std::vector<TString> pois {"ssth23_dmsq32"}; //ssth23_dmsq32, dcp_sstth13
  TString Dir = "/pnfs/dune/persistent/users/weishi/Fit_ELepEHadVisReco_lep_default_binning_TrueObs_fixed"; // File on gpvm
	TString AnalysisVar = "ELepEHadVisReco";

  // Initialize
  TString PoI = "undefined";
	float MaxdChi2 = 100000;
  int PoixBins = 50; // number of bins for poi x
	int PoiyBins = 50; // .......................y
  float XAxisLow = 0; // range of poi to plot, reset below
  float XAxisHigh = 100;
	float YAxisLow = 0; // range of poi to plot, reset below
  float YAxisHigh = 100;

  // loop over pois
	for (int ipoi = 0; ipoi < pois.size(); ipoi++) {
		PoI = pois.at(ipoi);

		if ( PoI != "ssth23_dmsq32" && PoI != "dcp_sstth13" ) {
			std::cout << "[ERROR] Unknown PoI: " << PoI << std::endl;
			abort();
		}

    // set poi bins
    if ( PoI == "ssth23_dmsq32" ) {
      PoixBins  = 50;
			PoiyBins  = 50;
      XAxisLow  = 0.40;
      XAxisHigh = 0.65;
			YAxisLow  = 2.3;
			YAxisHigh = 2.55;
    }
    else if ( PoI == "dcp_sstth13" ) {
      PoixBins  = 50;
			PoiyBins  = 50;
      XAxisLow  = -1;
      XAxisHigh = 1;
			YAxisLow  = 0.05;
			YAxisHigh = 0.15;
    }

    // set dChi2 max in plot
    if ( FitChannel ==  "Numu_disp" ) {
      if ( PoI == "ssth23_dmsq32" ) MaxdChi2 = 30;
  		else if ( PoI == "dcp_sstth13" ) MaxdChi2 = 20;
    } else if ( FitChannel ==  "Numubar_disp" ) {
      if ( PoI == "ssth23_dmsq32" ) MaxdChi2 = 20;
  		else if ( PoI == "dcp_sstth13" ) MaxdChi2 = 15;
    } else if ( FitChannel ==  "DispJoint" ) {
      if ( PoI == "ssth23_dmsq32" ) MaxdChi2 = 40;
  		else if ( PoI == "dcp_sstth13" ) MaxdChi2 = 30;
    } else {
      std::cout << "[ERROR] Unknown FitChannel: " << FitChannel << std::endl;
			abort();
    }

    // Summarize
		std::cout << "Looking at poi: " << PoI << ", Fit channel: " << FitChannel << std::endl;

		// Store dChi2 of all fit points
    TH2D *h_dChi2_2D = new TH2D("h_dChi2_2D", "h_dChi2_2D", PoixBins, XAxisLow, XAxisHigh, PoiyBins, YAxisLow, YAxisHigh);
		double binxvalue = -1;
		double binyvalue = -1;
		double chi2      = -1;

		// Find minimum Chi2 and the bin
	  double min_Chi2(1E15);
	  int min_Chi2_binxvalue(1), min_Chi2_binyvalue(1);

    // Loop over all bins to add chi2 hist
    for (int binx = 1; binx <= PoixBins; binx++) {
			for (int biny = 1; biny <= PoiyBins; biny++) {

				// Input fit result file
				TFile *f1 = TFile::Open( TString::Format("%s/%s/%s/%s/%s_%s_%s_stat_only_%d_%d.root", Dir.Data(), Syst.Data(), PoI.Data(), FitChannel.Data(), PoI.Data(), FitChannel.Data(), AnalysisVar.Data(), binx, biny ) );
				if ( f1 == NULL ) continue; // skip if some jobs failed for certain reasons
				// abort();

				TH2D *h_chi2_tmp = (TH2D*)f1->Get("fit_nom/dChi2Scan");

				binxvalue = h_chi2_tmp->GetXaxis()->GetBinCenter(1);
				binyvalue = h_chi2_tmp->GetYaxis()->GetBinCenter(1);
				chi2      = h_chi2_tmp->GetBinContent(1, 1);

				//h_dChi2_2D->Fill(binxvalue, binyvalue, weight);
				h_dChi2_2D->SetBinContent(binx, biny, chi2);
				/*
				std::cout << "h_dChi2_2D->GetXaxis()->GetBinCenter(binx) - binxvalue: " << h_dChi2_2D->GetXaxis()->GetBinCenter(binx) - binxvalue << std::endl;
				std::cout << "h_dChi2_2D->GetXaxis()->GetBinCenter(binx): " << setprecision(12) << h_dChi2_2D->GetXaxis()->GetBinCenter(binx) << std::endl; // THIS is having some extra floating numbers
				std::cout << "binxvalue: " << setprecision(12) << binxvalue << std::endl;
				std::cout << "h_dChi2_2D->GetYaxis()->GetBinCenter(biny) - binyvalue: " << h_dChi2_2D->GetYaxis()->GetBinCenter(biny) - binyvalue << std::endl;
				std::cout << "h_dChi2_2D->GetYaxis()->GetBinCenter(biny): " << setprecision(12) << h_dChi2_2D->GetYaxis()->GetBinCenter(biny) << std::endl;
				std::cout << "binyvalue: " << setprecision(12) << binyvalue << std::endl;
				*/
				/*
				h_dChi2_2D->GetXaxis()->GetBinCenter(binx) - binxvalue: 5.66244e-09
				h_dChi2_2D->GetXaxis()->GetBinCenter(binx): 0.402500005662
				binxvalue: 0.4025
				h_dChi2_2D->GetYaxis()->GetBinCenter(biny) - binyvalue: -4.76837156427e-08
				h_dChi2_2D->GetYaxis()->GetBinCenter(biny): 2.30249995232
				binyvalue: 2.3025
				*/

				// Sanity check: bins should match input file
				if ( (h_dChi2_2D->GetXaxis()->GetBinCenter(binx) - binxvalue)/binxvalue > 0.01 || (h_dChi2_2D->GetYaxis()->GetBinCenter(biny) - binyvalue)/binyvalue > 0.01 ) {
					std::cout << "[ERROR] Bins disagree too much." << std::endl;
					std::cout << "  Input file binx " << binx << " value: " << binxvalue << "; biny "<< biny << " value: " << binyvalue << std::endl;
					std::cout << "  But TH2D at binx " << binx << " has value " << h_dChi2_2D->GetXaxis()->GetBinCenter(binx) << ", and at biny "<< biny << " has value " << h_dChi2_2D->GetYaxis()->GetBinCenter(biny) << std::endl;
					abort();
				}

				if ( chi2 < min_Chi2) {
					min_Chi2 = chi2;
					min_Chi2_binxvalue = binxvalue;
					min_Chi2_binyvalue = binyvalue;
				}

        delete h_chi2_tmp;
				f1->Close();
				delete f1;
			} // end loop over PoixBins
		} // end loop over PoiyBins

	  std::cout << "min Chi2 bin x value: " << min_Chi2_binxvalue << ", min Chi2 bin y value: " << min_Chi2_binyvalue <<  ", min chi2: " << min_Chi2 << std::endl;

    // Subtract min Chi2
		for (int binx = 1; binx <= PoixBins; binx++) {
			for (int biny = 1; biny <= PoiyBins; biny++) {
				h_dChi2_2D->SetBinContent(binx, biny, h_dChi2_2D->GetBinContent(binx, biny) - min_Chi2);
			}
		}

	  TCanvas *c = new TCanvas("c", "c", 800, 600);
	  c->cd(); c->SetGridx(); c->SetGridy();
    // Plot style
	  h_dChi2_2D->SetTitle( TString::Format("DUNE-PRISM %s Joint Fit: %s (Exposure: 3.5 yr)",  FitChannel.Data(), Syst.Data() ) );
	  h_dChi2_2D->SetStats(0);
	  if ( PoI == "ssth23_dmsq32" ) {
			h_dChi2_2D->GetXaxis()->SetTitle("sin^{2}(#theta_{23})");
			h_dChi2_2D->GetYaxis()->SetTitle("#Delta m^{2}_{32} (#times 10^{-3} eV^{2})");
		} else if ( PoI == "dcp_sstth13" ) {
			h_dChi2_2D->GetXaxis()->SetTitle("#delta_{cp} (#pi)");
			h_dChi2_2D->GetYaxis()->SetTitle("sin^{2}(2#theta_{13})");
		}
		//h_dChi2_2D->SetMaximum(MaxdChi2);
		h_dChi2_2D->GetXaxis()->SetTitleOffset(1.2);
		h_dChi2_2D->GetXaxis()->SetTitleSize(0.04);
		h_dChi2_2D->GetXaxis()->SetLabelOffset(0.01);
		h_dChi2_2D->GetXaxis()->SetLabelSize(0.04);
		h_dChi2_2D->GetYaxis()->SetTitleSize(0.04);
		h_dChi2_2D->GetYaxis()->SetLabelSize(0.04);
		h_dChi2_2D->GetYaxis()->SetLabelOffset(0.015);
		h_dChi2_2D->SetLineColor(1);
		h_dChi2_2D->SetLineWidth(3);
		h_dChi2_2D->SetLineStyle(7);
		h_dChi2_2D->Draw("HIST");

		TLegend *leg = new TLegend(0.19, 0.62, 0.56, 0.89);
	  leg->SetHeader("Placeholder", "L");
		leg->AddEntry(h_dChi2_2D, TString::Format("#splitline{Best-fit point}{#chi^{2}: %.5f}", min_Chi2), "L");
		leg->SetBorderSize(0);
    leg->SetFillStyle(0);
		leg->Draw();

		c->SaveAs( TString::Format("/dune/app/users/weishi/PRISMAnalysis/CAFAna/PRISM/scripts/Fit2DVar_%s_%s.png", FitChannel.Data(), PoI.Data() ) );

    delete h_dChi2_2D;
		delete c;
	} // end poi

} // end main func
