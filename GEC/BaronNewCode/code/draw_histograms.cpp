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
  char field[20];
  const char* units;
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
const char* list_of_directories[40]={"0mgsimple","0m","1.75m","2m","4m","5.75m","8m","9.75m","12m","13.75m","16m","17.75m","20m","21.75m","24m","25.75m","26.75m","28m",
"28.25m","28.5m","0mgsimpleRHC","0mRHC","1.75mRHC","2mRHC","4mRHC","5.75mRHC","8mRHC","9.75mRHC","12mRHC","13.75mRHC","16mRHC","17.75mRHC","20mRHC","21.75mRHC","24mRHC",
"25.75mRHC","26.75mRHC","28mRHC","28.25mRHC","28.5mRHC"};

Para pr[]= //position is in units of cm, momentum is in units of GeV/c, angle is in units of rad, and energy is in  units of GeV
{ //match the x-ranges with the FD histograms' x-ranges
  {"vtx_x", "cm", true, -300., 300., &x_pos},
  {"vtx_y", "cm", true, -100., 100., &y_pos},
  {"vtx_z", "cm", true, 50., 350., &z_pos},
  {"LepMomX", "GeV", true, -2., 2., &XLepMom},
  {"LepMomY", "GeV", true, -4.5, 2., &YLepMom},
  {"LepMomZ", "GeV", true, -0.5, 7., &ZLepMom},
  {"TotMom", "GeV", false, 0., 7., &TotalMom},
  {"cos_LepNuAngle", "", false, 0., 1., &cos_angle},
  {"LongMom", "GeV", false, -1., 7., &LongitudinalMom}
};

vector<Sel_type> br=
{
  Sel_type("muon_contained", "muon_contained_eff", false, &muon_cont, &muon_cont_eff),
  Sel_type("muon_tracker", "muon_tracker_eff", false, &muon_tra, &muon_tra_eff),
  Sel_type("muon_selected", "muon_sel_eff", true, &muon_sel, &muon_sel_eff),
  Sel_type("hadron_selected", "hadron_selected_eff", false, &hadr, &hadr_eff ),
  Sel_type("combined", "combined_eff", false, &comb, &comb_eff)
};

void draw_histograms()
{
  char raw_path[99];
  char sel_path[99];
  char geo_path[99];
  memset(raw_path, 0, 99); //clear array each time
  memset(sel_path, 0, 99); 
  memset(geo_path, 0, 99);
  TFile* raw_files[9];
  TFile* sel_files[45];
  TFile* geo_files[45];
  TH1D* raw_histograms[9];
  TH1D* sel_histograms[45];
  TH1D* geo_histograms[45];
  int index=0;
  for(auto sel:br)
  {
    const char* dt=sel.sel_name;
    for (Para item:pr)
    {
      const char *fd=item.field;
      if (index<9)
      {
        sprintf(raw_path,"/storage/shared/barwu/10thTry/0m_histograms/0.1_eff_veto_cut/%s/raw_%s.root",fd,fd);
        raw_files[int(index)]=new TFile(raw_path, "read");
        raw_histograms[int(index)]=(TH1D*)raw_files[int(index)]->Get(Form("raw_%s",fd));
      }
      sprintf(sel_path,"/storage/shared/barwu/10thTry/0m_histograms/0.1_eff_veto_cut/%s/selection-cut_%s_%s.root",fd,dt,fd);
      sel_files[index]=new TFile(sel_path, "read");
      sel_histograms[index]=(TH1D*)sel_files[index]->Get(Form("selection-cut_%s_%s",dt,fd));
      sprintf(geo_path,"/storage/shared/barwu/10thTry/0m_histograms/0.1_eff_veto_cut/%s/geo-corrected_%s_%s.root",fd,dt,fd);
      geo_files[index]=new TFile(geo_path, "read");
      geo_histograms[index]=(TH1D*)geo_files[index]->Get(Form("geo-corrected_%s_%s",dt,fd));
      index++;
    }
  }

  gStyle->SetOptStat(000000000);
  //gStyle->SetOptStat(111111111);
  TCanvas *c=new TCanvas("c", "all graphs", 1800, 1000);
  c->Divide(9,5);
  TCanvas *r=new TCanvas("r", "all ratios", 1800, 1000);
  r->Divide(9,5);
  index=0;
  for(auto sel:br)
  {
    const char *dt=sel.sel_name;
    for(int k=0;k<9;k++)
    {
      Para item=pr[k];
      double lowerbound=item.l;
      double upperbound=item.h;
      const char *fd=item.field;
      const char *var_unit=item.units;
      TVirtualPad *pad=c->cd(index+1);
      if (k%9==7) {pad->SetLogy();} //pad needs to be made logarithmic, not canvas
      TH1D *hist3=geo_histograms[index];
      hist3->SetLineColor(kBlue);
      hist3->Draw("histS");
      TH1D *hist2=sel_histograms[index];
      //hist2->SetLineColor(kGreen);
      hist2->SetLineColor(kTeal+10);
      hist2->Draw("samehistS");
      TH1D *hist1=raw_histograms[int(index%9)];
      hist1->SetLineColor(kPink);
      hist1->Draw("samehistS");

      float max1=hist1->GetMaximum();
      float max2=hist2->GetMaximum();
      float max3=hist3->GetMaximum();
      float upper_y_bound=max(max(max2,max3), max1)*1.25;
      hist3->SetAxisRange(lowerbound,upperbound,"X");
      if (k%9!=7) {hist3->SetAxisRange(0.,upper_y_bound,"Y");}
      else {hist3->SetAxisRange(0.1,max1,"Y");}
      hist3->SetTitle(Form("%s: %s",fd,dt));
      hist3->GetXaxis()->SetTitle(Form("%s (%s)",fd,var_unit));
      hist3->GetYaxis()->SetTitle("# of events");
      TLegend *leg=new TLegend(0.1,0.77,0.4,0.9);
      leg->SetHeader("comparison"); 
      leg->AddEntry(hist1, "raw distribution");
      leg->AddEntry(hist2, "selection-cut distribution");
      leg->AddEntry(hist3, "geo corrected distribution");
      leg->Draw();
      gPad->Update();
      // TPaveStats *ps;
      // ps=(TPaveStats*)hist3->GetListOfFunctions()->FindObject("stats");
      // ps->SetFillStyle(0);
      
      r->cd(index+1);
      TH1D *rplot1=(TH1D*)hist3->Clone();
      rplot1->Divide(hist1);
      rplot1->SetAxisRange(lowerbound,upperbound,"X");
      rplot1->SetAxisRange(0.,1.7,"Y");
      rplot1->SetLineColor(kViolet-3);
      rplot1->Draw("hist");
      TH1D *rplot2=(TH1D*)hist2->Clone();
      rplot2->Divide(hist1);
      rplot2->SetLineColor(kOrange+7);
      rplot2->Draw("samehist");
      TH1D *rplot3=(TH1D*)hist2->Clone();
      rplot3->Divide(hist3);
      rplot3->SetLineColor(kCyan);
      rplot3->Draw("samehist");
      // TLegend *rleg=new TLegend(0.1,0.77,0.4,0.9);
      // rleg->SetHeader("comparison"); 
      // rleg->AddEntry(rplot1, "geo vs raw");
      // rleg->AddEntry(rplot2, "sel vs raw");
      // rleg->AddEntry(rplot3, "sel vs geo");
      // rleg->Draw();
      // TPaveStats *prs;
      // prs=(TPaveStats*)rplot->GetListOfFunctions()->FindObject("stats");
      // prs->SetFillStyle(0);
      index++;
    }
  }
  c->Update();
  c->SaveAs("/home/barwu/repos/MuonEffNN/images/0m_PRISM_0.1_eff_veto_cut_all_hists_200_bins.png");
  r->Update();
  r->SaveAs("/home/barwu/repos/MuonEffNN/images/0m_PRISM_0.1_eff_veto_cut_all_hists_200_bins_ratios.png");

  TCanvas *cs[5];
  TCanvas *rs[5];
  index=0;
  int i=0;
  for(auto sel:br)
  {
    const char *dt=sel.sel_name;
    cs[i]=new TCanvas(Form("c%01d",i+1),dt,1800,1000);
    cs[i]->Divide(3,3);
    rs[i]=new TCanvas(Form("r%01d",i+1),Form("%s ratios",dt),1800,1000);
    rs[i]->Divide(3,3);
    for(int k=0;k<9;k++)
    {
      Para item=pr[k];
      double lowerbound=item.l;
      double upperbound=item.h;
      const char *fd=item.field;
      const char *var_unit=item.units;
      TVirtualPad *p=cs[i]->cd(k+1);
      if (k==7) {p->SetLogy();} //pad needs to be made logarithmic, not canvas
      TH1D *hist3=geo_histograms[index];
      hist3->SetLineColor(kBlue);
      hist3->Draw("histS");
      TH1D *hist2=sel_histograms[index];
      //hist2->SetLineColor(kGreen);
      hist2->SetLineColor(kTeal+10);
      hist2->Draw("samehistS");
      TH1D *hist1=raw_histograms[int(index%9)];
      hist1->SetLineColor(kPink);
      hist1->Draw("samehistS");

      float max1=hist1->GetMaximum();
      float max2=hist2->GetMaximum();
      float max3=hist3->GetMaximum();
      float upper_y_bound=max(max(max2,max3), max1)*1.1;
      hist3->SetAxisRange(lowerbound,upperbound,"X");
      if (k!=7) hist3->SetAxisRange(0.,upper_y_bound,"Y");
      else hist3->SetAxisRange(10,max1,"Y");
      hist3->SetTitle(Form("%s: %s",fd,dt));
      hist3->GetXaxis()->SetTitle(Form("%s (%s)",fd,var_unit));
      hist3->GetYaxis()->SetTitle("# of events");
      TLegend *leg=new TLegend(0.1,0.75,0.33,0.9);
      leg->SetHeader("comparison"); 
      leg->AddEntry(hist1, "raw distribution");
      leg->AddEntry(hist2, "selection-cut distribution");
      leg->AddEntry(hist3, "geo corrected distribution");
      leg->Draw();
      gPad->Update();
      // TPaveStats *ps;
      // ps=(TPaveStats*)hist3->GetListOfFunctions()->FindObject("stats");
      // ps->SetFillStyle(0);

      rs[i]->cd(k+1);
      TH1D *rplot1=(TH1D*)hist3->Clone();
      rplot1->Divide(hist1);
      rplot1->SetAxisRange(lowerbound,upperbound,"X");
      rplot1->SetAxisRange(0.,1.7,"Y");
      rplot1->SetLineColor(kViolet-3);
      rplot1->Draw("hist");
      TH1D *rplot2=(TH1D*)hist2->Clone();
      rplot2->Divide(hist1);
      rplot2->SetLineColor(kOrange+7);
      rplot2->Draw("samehist");
      TH1D *rplot3=(TH1D*)hist2->Clone();
      rplot3->Divide(hist3);
      rplot3->SetLineColor(kCyan);
      rplot3->Draw("samehist");
      TLegend *rleg=new TLegend(0.1,0.77,0.4,0.9);
      rleg->SetHeader("comparison"); 
      rleg->AddEntry(rplot1, "geo vs raw");
      rleg->AddEntry(rplot2, "sel vs raw");
      rleg->AddEntry(rplot3, "sel vs geo");
      rleg->Draw();
      // TPaveStats *prs;
      // prs=(TPaveStats*)rplot->GetListOfFunctions()->FindObject("stats");
      // prs->SetFillStyle(0);
      index++;
    }
    cs[i]->Update();
    cs[i]->SaveAs(Form("/home/barwu/repos/MuonEffNN/images/0m_%s_PRISM_0.1_eff_veto_cut_hists_200_bins.png",dt));
    rs[i]->Update();
    rs[i]->SaveAs(Form("/home/barwu/repos/MuonEffNN/images/0m_%s_PRISM_0.1_eff_veto_cut_hists_200_bins_ratios.png",dt));;
    i++;
  }
}