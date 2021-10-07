// plot the different number of systematic dials in a 1d deltaChiSq scan
//

double LinearInterpolate(TH1D *h, int bin0, int binx, int bin1) {

	double y0 = h->GetBinContent(bin0);
        double y1 = h->GetBinContent(bin1);
        double x0 = h->GetBinCenter(bin0);
	double x1 = h->GetBinCenter(bin1);
	double x = h->GetBinCenter(binx);

	double y = y0 * (1 - ((x - x0) / (x1 - x0))) + y1 * ((x - x0) / (x1 - x0)); 

	return y;
}

void Plot1DChiSqScan_XSecFluxNomComp() {
	TCanvas *c = new TCanvas("c", "c", 800, 600);
	TFile *f = new TFile(
			"/dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/ssth23_1D_fit.root");
	TLegend *leg = new TLegend(0.58,0.67,0.85,0.87);
  	leg->SetHeader("Included Systematics");

	if (f->IsZombie()) {
		std::cout << "[ERROR] File not read." << std::endl;
		abort();
	}

	f->cd();
	TH1D *hnosyst = (TH1D*)gDirectory->Get("/ssth23_nom/h_dChi2_Total"); // /dmsq32_nom/FD_nu_numu/Chi2_Total1DFit
        hnosyst->SetDirectory(nullptr);
	TH1D *hflux = (TH1D*)gDirectory->Get("/ssth23_flux/h_dChi2_Total"); // /dmsq32_flux/FD_nu_numu/Chi2_Total1DFit
	hflux->SetDirectory(nullptr);
	TH1D *hfluxxsec = (TH1D*)gDirectory->Get("/ssth23_all/h_dChi2_Total"); // /dmsq32_all/FD_nu_numu/Chi2_Total1DFit
	hfluxxsec->SetDirectory(nullptr);

	bool fix_point(true);
	int bin0(21), binbad(22), bin1(23);
	// Hack missing point!
	if (fix_point) {
		double intNom = LinearInterpolate(hnosyst, bin0, binbad, bin1);
		hnosyst->SetBinContent(binbad, intNom);
		double intFlux = LinearInterpolate(hflux, bin0, binbad, bin1);
		hflux->SetBinContent(binbad, intFlux);
		double intFluxXSec = LinearInterpolate(hfluxxsec, bin0, binbad, bin1);
		hfluxxsec->SetBinContent(binbad, intFluxXSec);
	}

        double min_nosyst(1E15), min_flux(1E15), min_fluxxsec(1E15);
        int minbin_nosyst(1), minbin_flux(1), minbin_fluxxsec(1);
        for (int bin = 1; bin <= hnosyst->GetXaxis()->GetNbins(); bin++) {
                if (hnosyst->GetBinContent(bin) < min_nosyst) {
                        min_nosyst = hnosyst->GetBinContent(bin);
                        minbin_nosyst = bin;
                }
		if (hflux->GetBinContent(bin) < min_flux) {
                        min_flux = hflux->GetBinContent(bin);
                        minbin_flux = bin;
                }
		if (hfluxxsec->GetBinContent(bin) < min_fluxxsec) {
                        min_fluxxsec = hfluxxsec->GetBinContent(bin);
                        minbin_fluxxsec = bin;
                }
        }
        std::cout << "min is in bin " << minbin_fluxxsec << " with min = " << min_fluxxsec << std::endl;
        for (int bin = 1; bin <= hnosyst->GetXaxis()->GetNbins(); bin++) {
                hnosyst->SetBinContent(bin, hnosyst->GetBinContent(bin) - min_nosyst);
		hflux->SetBinContent(bin, hflux->GetBinContent(bin) - min_flux);
		hfluxxsec->SetBinContent(bin, hfluxxsec->GetBinContent(bin) - min_fluxxsec);
        }

	hnosyst->SetLineColor(kBlack);
	hnosyst->SetLineStyle(7);
	hflux->SetLineColor(38);
	hfluxxsec->SetLineColor(46);
	hnosyst->SetLineWidth(3);
	hflux->SetLineWidth(3);
	hfluxxsec->SetLineWidth(3);

	//hnosyst->GetXaxis()->SetTitle("#Delta m^{2}_{32} (10^{-3})");
	hnosyst->GetXaxis()->SetTitle("sin^{2}(#theta_{23})");
	hnosyst->GetXaxis()->SetTitleOffset(1.2);
	hnosyst->GetXaxis()->SetTitleSize(0.04);
	hnosyst->GetXaxis()->SetLabelOffset(0.01);
	hnosyst->GetXaxis()->SetLabelSize(0.04);
	hnosyst->GetYaxis()->SetTitle("#Delta #chi^{2}");
	hnosyst->GetYaxis()->SetTitleSize(0.04);
	hnosyst->GetYaxis()->SetLabelSize(0.04);
	hnosyst->GetYaxis()->SetLabelOffset(0.015);
	hnosyst->SetTitle("Flux & X-Section Systematics with Osc Params Profiled");
	//hnom1->SetAxisRange(2.35, 2.55, "X");

	leg->AddEntry(hnosyst, "No Syst. Params.", "L");
	leg->AddEntry(hflux, "Flux Systs.", "L");
	leg->AddEntry(hfluxxsec, "Flux + X-Sec Systs.", "L");
	leg->AddEntry((TObject*)0, "sin^{2}(2#theta_{13}) = 0.088 #pm 0.003", "");
	leg->AddEntry((TObject*)0, "Exposure = 1 Year", "");

	gStyle->SetOptStat(0);
	leg->SetBorderSize(0);

	hnosyst->Draw("HIST C");
	hflux->Draw("HIST C SAME");
	hfluxxsec->Draw("HIST C SAME");

	leg->Draw();
}
