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
#include "TBranch.h"

struct Para
{
  //static constexpr const char *const S;
  //constexpr const *char , VTX_X="vtx_x", *VTX_Y="vtx_y", *VTX_Z="vtx_z";
  //const char *LMX="LepMomX", *LMY="LepMomY", *LMZ="LepMomZ";
  char field[20];
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
double e_vis_true, ev;
const char* list_of_directories[40]={"0mgsimple","0m","1.75m","2m","4m","5.75m","8m","9.75m","12m","13.75m","16m","17.75m","20m","21.75m","24m","25.75m","26.75m","28m",
"28.25m","28.5m","0mgsimpleRHC","0mRHC","1.75mRHC","2mRHC","4mRHC","5.75mRHC","8mRHC","9.75mRHC","12mRHC","13.75mRHC","16mRHC","17.75mRHC","20mRHC","21.75mRHC","24mRHC",
"25.75mRHC","26.75mRHC","28mRHC","28.25mRHC","28.5mRHC"};
const int NUM_FIELDS=11;

Para pr[]= //position is in units of cm, momentum is in units of GeV/c, angle is in units of rad, and energy is in  units of GeV
{
  {"vtx_x", true, -300., 300. , &x_pos},
  {"vtx_y", true, -100., 100., &y_pos},
  {"vtx_z", true, 50., 350., &z_pos},
  {"LepMomX", true, -3., 3., &XLepMom},
  {"LepMomY", true, -4.5, 2., &YLepMom},
  {"LepMomZ", true, -1., 14, &ZLepMom},
  {"TotMom", false, 0., 16.,&TotalMom},
  {"cos_LepNuAngle", false, 0., 1.,&cos_angle},
  {"LongMom", false, -1., 16.,&LongitudinalMom}
  {"E_vis_true", false, 0., 10., &e_vis_true},
  {"Ev", true, 0., 10., &ev}
};

vector<Sel_type> br=
{
  Sel_type("muon_contained", "muon_contained_eff", false, &muon_cont, &muon_cont_eff),
  Sel_type("muon_tracker", "muon_tracker_eff", false, &muon_tra, &muon_tra_eff),
  Sel_type("muon_selected", "muon_sel_eff", true, &muon_sel, &muon_sel_eff),
  Sel_type("hadron_selected", "hadron_selected_eff", false, &hadr, &hadr_eff ),
  Sel_type("combined", "combined_eff", false, &comb, &comb_eff)
};

void histogram_files()
{
  gROOT->SetBatch(kTRUE);
  char eff[99];
  char caf[99];
  vector<TH1D*> histograms1, histograms2, histograms3;
  int first_pass=0;
  for(auto sel:br)
  {
    const char* dt=sel.sel_name;
    for(auto item:pr)
    {
      char *fd=item.field;
      double l=item.l;
      double h=item.h; //insert 11 check
      if (first_pass<NUM_FIELDS) histograms1.push_back(new TH1D(Form("raw_%s", fd), Form("raw %s", fd), 200, l, h)); //remove dt from name
      histograms2.push_back(new TH1D(Form("selection-cut_%s_%s", dt, fd), Form("selected %s %s", dt, fd), 200, l, h));
      histograms3.push_back(new TH1D(Form("geo-corrected_%s_%s", dt, fd), Form("geo corrected %s %s", dt, fd), 200, l, h));
    }
    first_pass=1;
  }

  const float directory_number=0; // Sometimes the directory # is an integer, sometimes its a fraction. Remember to change the wildcard and variable type accordingly.
  cout<<directory_number<<endl;
  for (int j=0; j<30000; j++)
  {
    memset(eff, 0, 99); // clear array each time
    memset(caf, 0, 99);
    // sprintf(eff,"/storage/shared/barwu/10thTry/combined1/%02dm/%02d/FHC.%03d%04d.CAF_Eff.root",directory_number,j/1000,int((directory_number+1)*100+j/10000),j%10000);
    // sprintf(caf,"/storage/shared/barwu/10thTry/NDCAF/%02dm/%02d/FHC.%03d%04d.CAF.root",directory_number,j/1000,int((directory_number+1)*100+j/10000),j%10000);
    sprintf(eff,"/storage/shared/barwu/10thTry/combined1/0m/%02d/FHC.10%05d.CAF_Eff.root",j/1000,j);
    sprintf(caf,"/storage/shared/wshi/CAFs/NDFHC_PRISM/%02d/FHC.10%05d.CAF.root",j/1000,j);
    if(access(eff, 0)!=0) {continue;}
    TFile eff_file(eff);
    TFile caf_file(caf);
    TTree *event_data=(TTree*)eff_file.Get("event_data");
    TTree *caf_tree=(TTree*)caf_file.Get("caf");

    int isCC=0, inFV=0;
    for(auto sel:br) 
    {
      if(sel.calced) continue;
      event_data->SetBranchAddress(sel.sel_name, sel.sel_value);
      event_data->SetBranchAddress(sel.eff_name, sel.eff_value);
    }
    double LepE=0., eP=0., ePip=0., ePim=0., ePi0=0., eOther=0.;
    int nipi0=0; //why is this int?
    caf_tree->SetBranchAddress("isCC",&isCC);
    event_data->SetBranchAddress("inFV",&inFV);   
    caf_tree->SetBranchAddress("LepE", &LepE);
    caf_tree->SetBranchAddress("eP", &eP);
    caf_tree->SetBranchAddress("ePip", &ePip);
    caf_tree->SetBranchAddress("ePim", &ePim);
    caf_tree->SetBranchAddress("ePi0", &ePi0);
    caf_tree->SetBranchAddress("eOther", &eOther);
    caf_tree->SetBranchAddress("nipi0", &nipi0);
    const double pi0_mass=0.1349768; //GeV //true energy
    for (auto item:pr) 
    {
      TTree *tree=item.iscaf?caf_tree:event_data;
      tree->SetBranchAddress(item.field, item.field_value);
    }

    //there are some non-CC events
    Long64_t nentries1=caf_tree->GetEntries();
    Long64_t nentries2=event_data->GetEntries();
    if (nentries1!=nentries2) {cout<<"The efficiency file "<<eff<<" has"<<nentries2<<" events, and the CAF file "<<caf<<" has"<<nentries1<<" events."<<endl;}
    for (int i=0;i<nentries2;i++) {
      caf_tree->GetEntry(i);
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
        cout<<"bad val for muon-selected check! "<<muon_sel<<endl<<"Event #"<<i<<" in file "<<eff<<endl<<"contained-"<<muon_cont<<", tracker-matched-"<<muon_tra<<endl;
        continue;
      }
      e_vis_true=LepE+eP+ePip+ePim+ePi0+eOther+nipi0*pi0_mass;

      int n=0;
      for (auto sel:br) {
        for (auto item:pr) {
          const char *fd=item.field;
          TH1D* hist1;
          if (n<NUM_FIELDS) hist1=histograms1.at(n);
          TH1D* hist2=histograms2.at(n);
          TH1D* hist3=histograms3.at(n);
          if (n<NUM_FIELDS) hist1->Fill(*item.field_value);
          n++;
          double geo_eff=*sel.eff_value;
          if (geo_eff<=0.005) {
            continue;
          } else {
            hist2->Fill(*item.field_value,*sel.sel_value);
            hist3->Fill(*item.field_value,*sel.sel_value/geo_eff);
          }
        }
      }
    }
    eff_file.Close();
    caf_file.Close();
  }

  TFile *raw_files[NUM_FIELDS];
  TFile *sel_files[int(NUM_FIELDS*5)];
  TFile *geo_files[int(NUM_FIELDS*5)];
  int index=0;
  for (auto sel:br) {
    const char *dt=sel.sel_name;
    for (Para item:pr) {
      const char *fd=item.field;
      if (index<NUM_FIELDS) {
        raw_files[index]=new TFile(Form("/storage/shared/barwu/10thTry/0m_histograms/0.005_eff_veto_cut/%s/raw_%s.root",fd,fd),"recreate");
        TH1D* raw_hist=histograms1.at(index);
        raw_hist->Write();
        raw_files[index]->Close();
      }
      sel_files[index]=new TFile(Form("/storage/shared/barwu/10thTry/0m_histograms/0.005_eff_veto_cut/%s/selection-cut_%s_%s.root",fd,dt,fd),"recreate");
      TH1D* sel_hist=histograms2.at(index);
      sel_hist->Write();
      sel_files[index]->Close();
      geo_files[index]=new TFile(Form("/storage/shared/barwu/10thTry/0m_histograms/0.005_eff_veto_cut/%s/geo-corrected_%s_%s.root",fd,dt,fd),"recreate");
      TH1D* geo_hist=histograms3.at(index);
      geo_hist->Write();
      geo_files[index]->Close();
      index++;
    }
  }
}