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

			TCanvas *c = new TCanvas("c", "c", 800, 600);
			c->cd(); c->SetGridx(); c->SetGridy();
			// Plot style
			h_target_flux->SetTitle( TString::Format("DUNE-PRISM %s Appearance Flux",  HornCurrent.Data() ) );
			h_target_flux->SetStats(0);
			h_target_flux->GetXaxis()->SetTitle("True E_{#nu} bins");
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
			leg->SetHeader(TString::Format("FD Intrinsic #nu_{e} Bkg: flux match %s",  Flag.Data()), "C");
			leg->AddEntry(h_target_flux, "Target FD flux", "L");
			leg->AddEntry(h_match_flux,  "Linearly combined ND flux", "L");
			leg->SetBorderSize(0);
			leg->SetFillStyle(0);
			leg->Draw();

			c->SaveAs( TString::Format("/dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/FluxOverlay_IntrinsicNue_%s_%s_App.png", Flag.Data(), HornCurrent.Data() ) );

			delete leg;
			delete c;
	    delete h_target_flux;
			delete h_match_flux;
			f1->Close();
			delete f1;
		} // loop over flags
	} // loop over horn currents

} // end main func
