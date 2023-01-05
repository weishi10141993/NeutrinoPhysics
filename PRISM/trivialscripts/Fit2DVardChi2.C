//------------------------------------------------------------------
// This script plots dChi2=1, 4, 9 ( 1, 2, 3 sigma) contours
// when the fit of 2 pois are done via 50x50=2500 jobs
// It first plot the 2D dChi2 with its min subtracted,
// Then it finds points where the dChi2 crosses 1, 4 and 9
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q Fit2DVardChi2.C
//------------------------------------------------------------------

void Fit2DVardChi2() {
  // User edit
  TString FitChannel = "Numu_disp"; // Numu_disp, Numubar_disp
  TString Syst = "StatOnly"; // StatOnly
	std::vector<TString> pois {"ssth23_dmsq32"}; //ssth23_dmsq32, dcp_sstth13
  //TString Dir = "root://fndca1.fnal.gov:1094//pnfs/fnal.gov/usr/dune/persistent/users/weishi/Fit_ELepEHadVisReco_lep_default_binning_TrueObs_fixed"; // File on gpvm
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
  double truepx = -999; // true osc parameter
  double truepy = -999;

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
      truepx = 0.58;  // ssth23 @NO, cf LBL paper2
      truepy = 2.451; // dmsq32 @NO
    }
    else if ( PoI == "dcp_sstth13" ) {
      PoixBins  = 50;
			PoiyBins  = 50;
      XAxisLow  = -1;
      XAxisHigh = 1;
			YAxisLow  = 0.05;
			YAxisHigh = 0.15;
      truepx    = -0.8; // dcp @NO, cf LBL paper2
      truepy    = 0.087; // sstth13
    }

    // set dChi2 max in plot
    if ( FitChannel == "Numu_disp" ) {
      if ( PoI == "ssth23_dmsq32" ) MaxdChi2 = 30;
  		else if ( PoI == "dcp_sstth13" ) MaxdChi2 = 20;
    } else if ( FitChannel == "Numubar_disp" ) {
      if ( PoI == "ssth23_dmsq32" ) MaxdChi2 = 20;
  		else if ( PoI == "dcp_sstth13" ) MaxdChi2 = 15;
    } else if ( FitChannel == "DispJoint" ) {
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

        // sanity check: bin value should between defined range
        if ( binxvalue < XAxisLow || binxvalue > XAxisHigh ||  binyvalue < YAxisLow || binyvalue > YAxisHigh ) {
          std::cout << "[ERROR] Bin value from input file out of bound." << std::endl;
          abort();
        }

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
					std::cout << "[ERROR] Bins disagree too much. " << std::endl;
					std::cout << "  Input file binx " << binx << " value: " << binxvalue << "; biny "<< biny << " value: " << binyvalue << std::endl;
					std::cout << "  But TH2D at binx " << binx << " has value " << h_dChi2_2D->GetXaxis()->GetBinCenter(binx) << ", and at biny "<< biny << " has value " << h_dChi2_2D->GetYaxis()->GetBinCenter(biny) << std::endl;
					abort();
				}



        delete h_chi2_tmp;
				f1->Close();
				delete f1;
			} // end loop over PoixBins
		} // end loop over PoiyBins

    // For failed jobs, the bin content is 0, interpolate
    double tmp_sum;
    int tmp_count;
		for (int binx = 1; binx <= PoixBins; binx++) {
			for (int biny = 1; biny <= PoiyBins; biny++) {

        if ( h_dChi2_2D->GetBinContent(binx, biny) == 0 ) {
          std::cout << "[Info] Interpolate point due to failed jobs. " << std::endl;

          tmp_sum = 0;
          tmp_count = 0;

          if ( binx-1 > 0          && h_dChi2_2D->GetBinContent(binx-1, biny) != 0 ) { tmp_sum += h_dChi2_2D->GetBinContent(binx-1, biny); tmp_count++;}
          if ( binx+1 < PoixBins+1 && h_dChi2_2D->GetBinContent(binx+1, biny) != 0 ) { tmp_sum += h_dChi2_2D->GetBinContent(binx+1, biny); tmp_count++;}
          if ( biny-1 > 0          && h_dChi2_2D->GetBinContent(binx, biny-1) != 0 ) { tmp_sum += h_dChi2_2D->GetBinContent(binx, biny-1); tmp_count++;}
          if ( biny+1 < PoiyBins+1 && h_dChi2_2D->GetBinContent(binx, biny+1) != 0 ) { tmp_sum += h_dChi2_2D->GetBinContent(binx, biny+1); tmp_count++;}

          if (tmp_count != 0) h_dChi2_2D->SetBinContent(binx, biny, tmp_sum/tmp_count);
          // otherwise probably need to find 2nd nearest neighbor? or too many jobs failed
        }

			}
		}

    // Find minimum Chi2 and the bin
    double min_Chi2(1E15);
    double min_Chi2_binxvalue(1E15), min_Chi2_binyvalue(1E15);
    for (int binx = 1; binx <= PoixBins; binx++) {
			for (int biny = 1; biny <= PoiyBins; biny++) {

        if ( h_dChi2_2D->GetBinContent(binx, biny) < min_Chi2) {
					min_Chi2 = h_dChi2_2D->GetBinContent(binx, biny);
					min_Chi2_binxvalue = h_dChi2_2D->GetXaxis()->GetBinCenter(binx);
					min_Chi2_binyvalue = h_dChi2_2D->GetYaxis()->GetBinCenter(biny);
				}

			}
		}

    // Subtract min Chi2
    std::cout << "min Chi2 bin x value: " << min_Chi2_binxvalue << ", min Chi2 bin y value: " << min_Chi2_binyvalue << ", min chi2: " << min_Chi2 << std::endl;

		for (int binx = 1; binx <= PoixBins; binx++) {
			for (int biny = 1; biny <= PoiyBins; biny++) {
				h_dChi2_2D->SetBinContent(binx, biny, h_dChi2_2D->GetBinContent(binx, biny) - min_Chi2);
			}
		}

	  TCanvas *c = new TCanvas("c", "c", 800, 600);
	  c->cd(); c->SetGridx(); c->SetGridy();
    // Plot style
	  h_dChi2_2D->SetTitle( TString::Format("DUNE-PRISM %s Joint Fit: %s (Exposure: 3.5 yr)",  FitChannel.Data(), Syst.Data() ) );
    h_dChi2_2D->GetZaxis()->SetTitle("#Delta #chi^{2}");
	  h_dChi2_2D->SetStats(0);
	  if ( PoI == "ssth23_dmsq32" ) {
			h_dChi2_2D->GetXaxis()->SetTitle("sin^{2}(#theta_{23})");
			h_dChi2_2D->GetYaxis()->SetTitle("#Delta m^{2}_{32} (#times 10^{-3} eV^{2})");
		} else if ( PoI == "dcp_sstth13" ) {
			h_dChi2_2D->GetXaxis()->SetTitle("#delta_{CP} (#pi)");
			h_dChi2_2D->GetYaxis()->SetTitle("sin^{2}(2#theta_{13})");
		}

		h_dChi2_2D->GetXaxis()->SetTitleOffset(1.2);
		h_dChi2_2D->GetXaxis()->SetTitleSize(0.04);
		h_dChi2_2D->GetXaxis()->SetLabelOffset(0.01);
		h_dChi2_2D->GetXaxis()->SetLabelSize(0.04);
    h_dChi2_2D->GetYaxis()->SetTitleOffset(1.25);
		h_dChi2_2D->GetYaxis()->SetTitleSize(0.04);
		h_dChi2_2D->GetYaxis()->SetLabelSize(0.04);
		h_dChi2_2D->GetYaxis()->SetLabelOffset(0.015);
		h_dChi2_2D->SetLineColor(1);
		h_dChi2_2D->SetLineWidth(3);
		h_dChi2_2D->SetLineStyle(7);
		h_dChi2_2D->Draw("COLZ");

    // Plot best fit point and true point
    TMarker *bestfitp = new TMarker(min_Chi2_binxvalue, min_Chi2_binyvalue, 20);
    bestfitp->SetMarkerColor(2);
    bestfitp->Draw();

    TMarker *truep = new TMarker(truepx, truepy, 29);
    truep->SetMarkerColor(6);
    truep->Draw();

    TLegend *leg = new TLegend(0.12, 0.14, 0.52, 0.26);
    if ( PoI == "ssth23_dmsq32") {
      if (FitChannel == "Numu_disp" || FitChannel == "Numubar_disp" || FitChannel == "DispJoint") leg->SetHeader("#theta_{13} = 0.150 #pm 0.002 rad", "C"); // no need to specify dcp in disp chan
      else leg->SetHeader("#splitline{#theta_{13} = 0.15 rad}{#delta_{CP} = -2.53 rad}", "C");
    }
    leg->AddEntry(bestfitp,     "Best-fit point", "P");
    leg->AddEntry(truep,        "True point",     "P");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

		c->SaveAs( TString::Format("/dune/app/users/weishi/PRISMAnalysis/CAFAna/PRISM/scripts/Fit2DVar_%s_%s.root", FitChannel.Data(), PoI.Data() ) );

    Double_t contour1sigma[1] = {1};
    Double_t contour2sigma[1] = {4};
    Double_t contour3sigma[1] = {9};

    h_dChi2_2D->SetContour(1, contour1sigma); h_dChi2_2D->SetLineStyle(1); h_dChi2_2D->Draw("CONT3");
    TH2D* h_dChi2_2D_2 = (TH2D*)h_dChi2_2D->Clone();
    h_dChi2_2D_2->SetContour(1, contour2sigma); h_dChi2_2D_2->SetLineStyle(2); h_dChi2_2D_2->Draw("CONT3 SAME");
    TH2D* h_dChi2_2D_3 = (TH2D*)h_dChi2_2D->Clone();
    h_dChi2_2D_3->SetContour(1, contour3sigma); h_dChi2_2D_3->SetLineStyle(3); h_dChi2_2D_3->Draw("CONT3 SAME");
    c->Update();

    bestfitp->Draw();
    truep->Draw();

    TLegend *leg_contour = new TLegend(0.13, 0.13, 0.42, 0.34);
    if ( PoI == "ssth23_dmsq32") {
      if (FitChannel == "Numu_disp" || FitChannel == "Numubar_disp" || FitChannel == "DispJoint") leg_contour->SetHeader("#theta_{13} = 0.150 #pm 0.002 rad", "C"); // no need to specify dcp in disp chan
      else leg_contour->SetHeader("#splitline{#theta_{13} = 0.15 rad}{#delta_{CP} = -2.53 rad}", "C");
    }
    leg_contour->AddEntry(bestfitp,     "Best-fit point", "P");
    leg_contour->AddEntry(h_dChi2_2D,   "1 #sigma",       "L");
    leg_contour->AddEntry(h_dChi2_2D_2, "2 #sigma",       "L");
    leg_contour->AddEntry(h_dChi2_2D_3, "3 #sigma",       "L");
    leg_contour->AddEntry(truep,        "True point",     "P");
    leg_contour->SetBorderSize(0);
    leg_contour->SetFillStyle(0);
    leg_contour->Draw();

    c->SaveAs( TString::Format("/dune/app/users/weishi/PRISMAnalysis/CAFAna/PRISM/scripts/Fit2DVar_%s_%s_contour.root", FitChannel.Data(), PoI.Data() ) );

	} // end poi

} // end main func
