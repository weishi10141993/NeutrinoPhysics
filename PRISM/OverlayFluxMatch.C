//------------------------------------------------------------------
// This is used to overlay the target flux and matched flux
//
// Run this macro:
// source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
// root -l -b -q OverlayFluxMatch.C
//------------------------------------------------------------------

void OverlayFluxMatch() {
  // User edit
  TString Dir = "/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app"; // File on gpvm
	std::vector<TString> IntrinsicNueInLCFlags {"flagoff", "flagon"}; // flagon, flagoff
	std::vector<TString> HornCurrents {"FHC", "RHC"}; // FHC, RHC

	TString HornCurrent = "undefined";
	TString Mode = "undefined"; // nu, nub (equivalent to Horncurrent above, this is just for the folder name)
	TString Flag = "undefined";
	Int_t TrueEnuBins = 0; // used in flux hist

	Float_t MinY = -1.1; // fraction plot range
	Float_t MaxY = 1.1;

  for (int ihorncurrent = 0; ihorncurrent < HornCurrents.size(); ihorncurrent++) {

		HornCurrent = HornCurrents.at(ihorncurrent);

		// set folder name
		if ( HornCurrent ==  "FHC" ) {
			Mode = "nu";
		} else if (HornCurrent ==  "RHC") {
			Mode = "nub";
		} else {
			std::cout << "[ERROR] Unknown HornCurrent: " << HornCurrent << std::endl;
			abort();
		}

		std::cout << "Looking at horn current: " << HornCurrent << std::endl;

    // Overlay flux target vs match
		for (int iflag = 0; iflag < IntrinsicNueInLCFlags.size(); iflag++) {
			Flag = IntrinsicNueInLCFlags.at(iflag);
			std::cout << " >> Intrinsic nue bkg: flux match " << Flag << std::endl;
			// Input fit file
			TFile *f1 = TFile::Open( TString::Format("%s/PRISMPred_EVisReco_IntrinsicNue_nodet_withxseccorr_%s_%s_App.root",  Dir.Data(), Flag.Data(), HornCurrent.Data() ) );

			if ( f1->IsZombie() ) {
				std::cout << "[ERROR] File not read." << std::endl;
				abort();
			}

			// Plots
			TH1F *h_target_flux = (TH1F*)f1->Get( TString::Format("numu_EvMatch_nom/FD_%s_nue/NDFD_matcher/last_match_DebugTarget", Mode.Data()) )->Clone("h_target_flux");
			TH1F *h_match_flux  = (TH1F*)f1->Get( TString::Format("numu_EvMatch_nom/FD_%s_nue/NDFD_matcher/last_match_DebugMatch",  Mode.Data()) )->Clone("h_match_flux");

      //
			// Overlay match and target flux under same flag
			//

			TCanvas *c = new TCanvas("c", "c", 2400, 1800);
			TPad *cpad1 = new TPad("cpad1", "cpad1", 0, 0.3, 1, 1.0); // xlow, ylow, xup, yup
			cpad1->SetBottomMargin(0); cpad1->Draw();
			TPad *cpad2 = new TPad("cpad2", "cpad2", 0, 0.0, 1, 0.29);
			cpad2->SetTopMargin(0); cpad2->SetBottomMargin(0.3); cpad2->SetGridy(); cpad2->Draw();

			cpad1->cd(); cpad1->SetGridx(); cpad1->SetGridy();
			h_target_flux->SetTitle( TString::Format("DUNE-PRISM %s Appearance Flux",  HornCurrent.Data() ) );
			h_target_flux->SetStats(0);
			h_target_flux->GetXaxis()->SetTitleOffset(1.2);
			h_target_flux->GetXaxis()->SetTitleSize(0.04);
			h_target_flux->GetXaxis()->SetLabelOffset(0.01);
			h_target_flux->GetXaxis()->SetLabelSize(0.04);
			h_target_flux->GetYaxis()->SetTitleSize(0.04);
			h_target_flux->GetYaxis()->SetLabelSize(0.04);
			h_target_flux->GetYaxis()->SetLabelOffset(0.015);
			h_target_flux->SetLineColor(1);
			h_target_flux->SetLineWidth(3);
			h_target_flux->SetLineStyle(7);
			h_target_flux->Draw("HIST");
			h_match_flux->SetLineColor(2);
			h_match_flux->SetLineWidth(2);
			h_match_flux->Draw("HIST SAME");

			TLegend *leg = new TLegend(0.56, 0.62, 0.96, 0.89);
			leg->SetHeader(TString::Format("FD Intrinsic #nu_{e} Bkg: flux match %s",  Flag.Data()), "L");
			leg->AddEntry(h_target_flux, "Target FD flux", "L");
			leg->AddEntry(h_match_flux,  "Linearly combined ND flux", "L");
			leg->SetBorderSize(0);
			leg->SetFillStyle(0);
			leg->Draw();
			gPad->RedrawAxis();

			// Plot difference
      cpad2->cd();
			TrueEnuBins = h_match_flux->GetNbinsX();
			if ( h_target_flux->GetNbinsX() != TrueEnuBins ) {
				std::cout << "[ERROR] Number of bins in flux histogrmas not the same " << std::endl;
				abort();
			}
			TH1F *MismatchFraction = new TH1F("MismatchFraction", "", TrueEnuBins, 0, TrueEnuBins);
			for(int iB=1; iB <= TrueEnuBins; iB++){
				MismatchFraction->SetBinContent(iB, ( h_match_flux->GetBinContent(iB) - h_target_flux->GetBinContent(iB) ) / h_target_flux->GetBinContent(iB) ); //iB starts from #1
			}
			MismatchFraction->GetXaxis()->SetTitle("True E_{#nu} bins");
			MismatchFraction->GetXaxis()->SetTitleSize(0.1);
			MismatchFraction->GetXaxis()->SetTitleOffset(1.1);
		  MismatchFraction->GetXaxis()->SetLabelSize(0.08); // labels will be 15 pixels
		  MismatchFraction->GetYaxis()->SetTitle("(ND LC - FD)/FD");
		  MismatchFraction->GetYaxis()->CenterTitle();
		  MismatchFraction->GetYaxis()->SetTitleSize(0.08);
		  MismatchFraction->GetYaxis()->SetTitleOffset(0.5);
		  MismatchFraction->GetYaxis()->SetLabelSize(0.1);
			MismatchFraction->SetLineColor(2);
		  MismatchFraction->SetMinimum(MinY);
		  MismatchFraction->SetMaximum(MaxY);
		  MismatchFraction->SetStats(0);
		  MismatchFraction->Draw("HIST");

			c->SaveAs(TString::Format("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/FluxOverlay_IntrinsicNue_%s_%s_App.png", Flag.Data(), HornCurrent.Data() ) );

			delete MismatchFraction;
			delete leg;
			delete cpad1;
			delete cpad2;
			delete c;
	    delete h_target_flux;
			delete h_match_flux;
			f1->Close();
			delete f1;
		} // end loop over flags

		//
		// Overlay target flux with flag on and off
		//

		// Input files
		TFile *f1 = TFile::Open( TString::Format("%s/PRISMPred_EVisReco_IntrinsicNue_nodet_withxseccorr_flagoff_%s_App.root",  Dir.Data(), HornCurrent.Data() ) );
		TFile *f2 = TFile::Open( TString::Format("%s/PRISMPred_EVisReco_IntrinsicNue_nodet_withxseccorr_flagon_%s_App.root",  Dir.Data(), HornCurrent.Data() ) );

		if ( f1->IsZombie() || f2->IsZombie() ) {
			std::cout << "[ERROR] File not read." << std::endl;
			abort();
		}

		// Plots
		TH1F *h_target_flux_flagoff = (TH1F*)f1->Get( TString::Format("numu_EvMatch_nom/FD_%s_nue/NDFD_matcher/last_match_DebugTarget", Mode.Data()) )->Clone("h_target_flux_flagoff");
		TH1F *h_target_flux_flagon  = (TH1F*)f2->Get( TString::Format("numu_EvMatch_nom/FD_%s_nue/NDFD_matcher/last_match_DebugTarget",  Mode.Data()) )->Clone("h_target_flux_flagon");

		TCanvas *c2 = new TCanvas("c2", "c2", 2400, 1800);
		c2->cd(); c2->SetGridx(); c2->SetGridy();

		h_target_flux_flagon->SetTitle( TString::Format("%s Appearance FD Target Flux", HornCurrent.Data() ) );
		h_target_flux_flagon->SetStats(0);
		h_target_flux_flagon->GetXaxis()->SetTitle("True E_{#nu} bins");
		h_target_flux_flagon->GetXaxis()->SetTitleOffset(1.2);
		h_target_flux_flagon->GetXaxis()->SetTitleSize(0.04);
		h_target_flux_flagon->GetXaxis()->SetLabelOffset(0.01);
		h_target_flux_flagon->GetXaxis()->SetLabelSize(0.04);
		h_target_flux_flagon->GetYaxis()->SetTitleSize(0.04);
		h_target_flux_flagon->GetYaxis()->SetLabelSize(0.04);
		h_target_flux_flagon->GetYaxis()->SetLabelOffset(0.015);
		h_target_flux_flagon->SetLineColor(2);
		h_target_flux_flagon->SetLineWidth(2);
	  h_target_flux_flagon->Draw("HIST");
		h_target_flux_flagoff->SetLineColor(1);
		h_target_flux_flagoff->SetLineWidth(3);
		h_target_flux_flagoff->SetLineStyle(7);
		h_target_flux_flagoff->Draw("HISTSAME");

		TLegend *leg = new TLegend(0.56, 0.62, 0.96, 0.89);
	  leg->SetHeader("FD Intrinsic #nu_{e} Bkg Options", "L");
		leg->AddEntry(h_target_flux_flagoff, "MC estimation", "L");
		leg->AddEntry(h_target_flux_flagon,  "Include in LC", "L");
		leg->SetBorderSize(0);
    leg->SetFillStyle(0);
		leg->Draw();

		c2->SaveAs(TString::Format("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/TargetFluxOverlay_IntrinsicNue_%s_App.png", HornCurrent.Data() ) );
		delete c2;
		delete leg;
		delete h_target_flux_flagon;
		delete h_target_flux_flagoff;
		delete f1;
		delete f2;

	} // loop over horn currents

} // end main func
