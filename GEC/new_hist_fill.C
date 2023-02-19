#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
using namespace std;
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPaveStats.h"

struct Para
{
  //static constexpr const char *const S;
  //constexpr const *char , VTX_X="vtx_x", *VTX_Y="vtx_y", *VTX_Z="vtx_z";
  //const char *LMX="LepMomX", *LMY="LepMomY", *LMZ="LepMomZ";
  char field[30];
  bool iscaf;
  double l;
  double h;
  double* field_value;
};

struct Sel_type
{
  const char* sel_name;
  const char* eff_name;
  bool calced=false;
  int* sel_value;
  double* eff_value;
  Sel_type() {}
  Sel_type(const char* sn, const char* en, bool c, int* sv, double* ev)
  :sel_name(sn),eff_name(en),calced(c),sel_value(sv),eff_value(ev) {}
};

int muon_cont, muon_tra, muon_sel, hadr, comb;
double muon_cont_eff, muon_tra_eff, muon_sel_eff, hadr_eff, comb_eff;
double x_pos, y_pos, z_pos, XLepMom, YLepMom, ZLepMom;
double TotalMom, cos_angle, LongitudinalMom;

Para pr[]=
{
  {"vtx_x", true, -300., 300. , &x_pos},
  {"vtx_y", true, -100., 100., &y_pos},
  {"vtx_z", true, 50., 350., &z_pos},
  {"LepMomX", true, -2., 2., &XLepMom},
  {"LepMomY", true, -4., 2., &YLepMom},
  {"LepMomZ", true, -0.5, 4.5, &ZLepMom},
  {"TotMom", false, 0., 5.,&TotalMom},
  {"cos_LepNuAngle", false, 0., 1.,&cos_angle},
  {"LongMom", false, -1., 5.,&LongitudinalMom}
};

vector<Sel_type> br=
{
  Sel_type("muon_contained", "muon_contained_eff", false, &muon_cont, &muon_cont_eff),
  Sel_type("muon_tracker", "muon_tracker_eff", false, &muon_tra, &muon_tra_eff),
  Sel_type("muon_selected", "muon_sel_eff", true, &muon_sel, &muon_sel_eff),
  Sel_type("hadron_selected", "hadron_selected_eff", false, &hadr, &hadr_eff ),
  Sel_type("combined", "combined_eff", false, &comb, &comb_eff)
};

void populate_histograms(char* eff,char* CAF,vector<TH1D*>hists1,vector<TH1D*>hists2,vector<TH1D*>hists3)
{
  TFile eff_file(eff);
  TFile caf_file(CAF);
  TTree *event_data=(TTree*)eff_file.Get("event_data");
  TTree *caf=(TTree*)caf_file.Get("caf");
  int isCC=0, inFV=0;
  for(auto item:br) 
  {
    if(item.calced) continue;
    event_data->SetBranchAddress(item.sel_name, item.sel_value);
    event_data->SetBranchAddress(item.eff_name, item.eff_value);
  }
  caf->SetBranchAddress("isCC",&isCC);
  event_data->SetBranchAddress("inFV",&inFV);   
  for (auto item:pr) 
  {
    TTree *tree=item.iscaf?caf:event_data;
    tree->SetBranchAddress(item.field, item.field_value);
  }

  //there are some non-CC events
  Long64_t nentries1=caf->GetEntries();
  Long64_t nentries2=event_data->GetEntries();
  if (nentries1!=nentries2) {cout<<"The efficiency file"<<eff<<"has"<<nentries2
  <<" events, and the CAF file"<<CAF<<"has"<<nentries1<<"events."<<endl;}
  for (int i=0;i<nentries2;i++) {
    caf->GetEntry(i);
    event_data->GetEntry(i); 
    int sel_cut=isCC*inFV;
    if (sel_cut==0) {continue;}
    if (sel_cut!=1) {
      cout<<"sel_cut="<<sel_cut<<endl;
      continue;
    }
    //calculation for the muon-selected cut
    muon_sel=muon_cont+muon_tra;
    muon_sel_eff=muon_cont_eff+muon_tra_eff;
    if (muon_sel!=0&&muon_sel!=1) {
      cout<<"bad val for muon-selected check! "<<muon_sel<<endl;
      continue;
    }

    int n=0;
    for (auto sel:br) {
      for (auto item:pr) {
        const char *fd=item.field;
        TH1D* hist1=hists1.at(n);
        TH1D* hist2=hists2.at(n);
        TH1D* hist3=hists3.at(n);
        n++;
	      hist1->Fill(*item.field_value);
        hist2->Fill(*item.field_value,*sel.sel_value);
	      double geo_eff = *sel.eff_value;
        if (geo_eff<=0.0001) {
	        continue;
	      } else {
	        hist3->Fill(*item.field_value,*sel.sel_value/geo_eff);
	      }
      }
    }
  }
  eff_file.Close();
  caf_file.Close();
}

void new_hist_fill()
{
  gROOT->SetBatch(kTRUE);
  char eff[99];
  char caf[99];
  vector<TH1D*> histograms1, histograms2, histograms3;
  for(auto sel:br)
  {
    const char* dt=sel.sel_name;
    for(auto item:pr)
    {
      char *fd=item.field;
      double l=item.l;
      double h=item.h;
      histograms1.push_back(new TH1D(Form("h1_%s_%s", fd, dt), Form("raw %s %s",fd, dt), 100, l, h));
      histograms2.push_back(new TH1D(Form("h2_%s_%s", fd, dt), Form("selected %s %s",fd, dt), 100, l, h));
      histograms3.push_back(new TH1D(Form("h3_%s_%s", fd, dt), Form("geo corrected %s %s",fd, dt), 100, l, h));
    }
  }

  for (int j=0; j<=29999; j++)
  {
    memset(eff, 0, 99); // clear array each time
    memset(caf, 0, 99);
    sprintf(eff,"/storage/shared/barwu/10thTry/combined1/0m/FHC.10%05d.CAF_Eff.root",j);
    sprintf(caf,"/storage/shared/wshi/CAFs/NDFHC_PRISM/%02d/FHC.10%05d.CAF.root",j/1000,j);
    //sprintf(eff,"/storage/shared/barwu/9thTry/eff_trees/FHC.100%04d.CAF_MuonEff.root",j);
    //sprintf(caf,"/storage/shared/cvilela/CAF/ND_v7/0%01d/FHC.100%04d.CAF.root",j/1000,j);
    if(access(eff, 0)==0)
    {
      populate_histograms(eff,caf,histograms1,histograms2,histograms3);
    } else {
      cout<<"Warning: missing file:"<<eff<<endl;
      continue;
    }
  }

  gROOT->SetBatch(kFALSE);
  //gStyle->SetOptStat(000000000);
  gStyle->SetOptStat(111111111);
  TCanvas *c=new TCanvas("c", "c", 1800, 1000);
  c->Divide(9,5);
  int n=0;
  for(auto sel:br)
  {
    const char *dt=sel.sel_name;
    for(int k=0;k<9;k++)
    {
      Para item=pr[k];
      const char *fd=item.field;
      TVirtualPad *pad=c->cd(n+1);
      if (k%9==7) {pad->SetLogy();} //pad needs to be made logarithmic, not canvas
      TH1D *hist3=histograms3.at(n);
      hist3->SetLineColor(kBlue);
      hist3->Draw("histS");
      TH1D *hist2=histograms2.at(n);
      hist2->SetLineColor(kGreen);
      hist2->Draw("samehistS");
      TH1D *hist1=histograms1.at(n);
      hist1->SetLineColor(kRed);
      hist1->Draw("samehistS");

      float max1=hist1->GetMaximum();
      float max2=hist2->GetMaximum();
      float max3=hist3->GetMaximum();
      float upper_y_bound=max(max(max2,max3), max1)*1.4;
      if (k%9!=7) {hist3->SetAxisRange(0.,upper_y_bound,"Y");}
      //else {hist3->SetAxisRange(0.1,upper_y_bound,"Y");}
      hist3->SetTitle(Form("%s: %s",fd,dt));
      hist3->GetXaxis()->SetTitle(Form("%s",fd));
      hist3->GetYaxis()->SetTitle("# of events");
      TLegend *leg=new TLegend(0.1,0.77,0.4,0.9);
      leg->SetHeader("comparison"); 
      leg->AddEntry(hist1, "raw distribution");
      leg->AddEntry(hist2, "selection-cut distribution");
      leg->AddEntry(hist3, "geo corrected distribution");
      leg->Draw();
      gPad->Update();
      TPaveStats *ps;
      ps=(TPaveStats*)hist3->GetListOfFunctions()->FindObject("stats");
      ps->SetFillStyle(0); 
      n++;
    }
  }
  c->SaveAs("images/0m_PRISM_hists_all.png");

  TCanvas *cs[5];
  n=0;
  int i=0;
  for(auto sel:br)
  {
    cs[i]=new TCanvas(Form("c%01d",i+1),Form("c%01d",i+1),1800,1000);
    cs[i]->Divide(3,3);
    const char *dt=sel.sel_name;
    for(int k=0;k<9;k++)
    {
      Para item=pr[k];
      const char *fd=item.field;
      TVirtualPad *p=cs[i]->cd(k+1);
      if (k==7) {p->SetLogy();} //pad needs to be made logarithmic, not canvas
      TH1D *hist3=histograms3.at(n);
      hist3->SetLineColor(kBlue);
      hist3->Draw("histS");
      TH1D *hist2=histograms2.at(n);
      hist2->SetLineColor(kGreen);
      hist2->Draw("samehistS");
      TH1D *hist1=histograms1.at(n);
      hist1->SetLineColor(kRed);
      hist1->Draw("samehistS");

      float max1=hist1->GetMaximum();
      float max2=hist2->GetMaximum();
      float max3=hist3->GetMaximum();
      float upper_y_bound=max(max(max2,max3), max1);
      if (k!=7) {hist3->SetAxisRange(0.,upper_y_bound,"Y");}
      //else {hist3->SetAxisRange(0.1,upper_y_bound,"Y");}
      hist3->SetTitle(Form("%s: %s",fd,dt));
      hist3->GetXaxis()->SetTitle(Form("%s",fd));
      hist3->GetYaxis()->SetTitle("# of events");
      TLegend *leg=new TLegend(0.1,0.75,0.33,0.9);
      leg->SetHeader("comparison"); 
      leg->AddEntry(hist1, "raw distribution");
      leg->AddEntry(hist2, "selection-cut distribution");
      leg->AddEntry(hist3, "geo corrected distribution");
      leg->Draw();
      gPad->Update();
      TPaveStats *ps;
      ps=(TPaveStats*)hist3->GetListOfFunctions()->FindObject("stats");
      ps->SetFillStyle(0);
      n++;
    }
    cs[i]->Update();
    cs[i]->SaveAs(Form("images/0m_PRISM_hists_%s.png",dt));
    i++;
  }
}