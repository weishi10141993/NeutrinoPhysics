#ifndef __MiniTupleInterface__
#define __MiniTupleInterface__

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <math.h>

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TList.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TSpline.h"
#include "TF1.h"

#include "globalVars.h"


class MiniTupleInterface
{

 public:

   MiniTupleInterface( TTree * h, TTree *sfrfgw = NULL, TTree *splineweights = NULL, std::string name = "tmp.root" );//Read in MC
   ~MiniTupleInterface();

 public:
   void Fill();
   void Finish();

   float ComputeErec( float cos, float eLep, int id);
   float ComputeCosBeam( float dx, float dy, float dz);

   float GetCosZenith();   
   float GetElossMax(int iPID, float mom); // maximum energy deposit above cherenkov threshold
   float GetEnergy(int iPID, float mom);
   float GetRingEvis(int iring);
   float GetEvisTot();
   float GetQ2();
   int GetnMueDcy();
   int GetByNEUTMode();
   int GetByFSMode();
   int GetByFSIMode();
   double GetExtraWeights();
   float GetDwall(float x, float y, float z);
   float GetTowall(float x, float y, float z, float dx, float dy, float dz);

   float GetRCPar();
   short GetNring();
   float GetPIDemu();
   float GetPIDepi0();
   float GetPIDmupip();
   
   //   virtual float ComputeOscWeight ( ); // different for T2K and SK
   //   virtual float ComputeFluxWeight( ); // different for T2K and SK

   void   CalcDerived();
   bool   PassPreSelection ( );
   void   GetEntry(int i);
   void   SetOscChannel(int i) { OscChannel = i; }
   void   InitVars();
   void   PrintOscChannel() {std::cout<<OscChannel<<std::endl;}      
 public:   
   int  ifQtoUse;
   bool sortByMom; // or sort rings by visible energy
   
 protected: // functions
   void EnableCopyBranches  ( );  
   void DefineExtraBranches ( );
   void LoadBranchAddresses ( );
   void SortRings();
   void CountVisPar();
   void CalcVtx();
   void InitTF1();
   void MakeSpline();
   void ClearSpline();
   void CalcPolyCoef();
   
 protected: //variables

   TTree  * parenttree;
   TTree  * splinetree;
   TTree  * sfrfgtree;
   
   // input variables from parent tree 

   UInt_t          nhitac;
   Int_t           numnu;
   Int_t           ipnu [50];
   Float_t         pnu  [50];   
   Float_t         dirnu[50][3];
   Int_t           mode;
   Int_t           npar;
   Int_t           ipv[50];
   Float_t         pmomv[50];
   Int_t           Npvc;
   Int_t           Ipvc[200];   //[Npvc]
   Int_t           Ichvc[200];   //[Npvc]
   Int_t           Iorgvc[100];   //[Npvc]
   Int_t           Iflvc[100];   //[Npvc]
   Float_t         Abspvc[100];   //[Npvc]
   Float_t         Pvc[100][3];   //[Npvc]
   Int_t           nscndprt;
   Int_t           itrkscnd[1000];   //[nscndprt]
   Int_t           istakscnd[1000];   //[nscndprt]
   Float_t         vtxscnd[1000][3];   //[nscndprt]
   Float_t         pscnd[1000][3];   //[nscndprt]
   Int_t           iprtscnd[1000];   //[nscndprt]
   Float_t         tscnd[1000];   //[nscndprt]
   Int_t           iprntprt[1000];   //[nscndprt]
   Int_t           lmecscnd[1000];   //[nscndprt]
   Int_t           iprnttrk[1000];   //[nscndprt]
   Int_t           iorgprt[1000];   //[nscndprt]
   Float_t         pprnt[1000][3];   //[nscndprt]
   Int_t           iflgscnd[1000];   //[nscndprt]
   Int_t           iprntidx[1000];   //[nscndprt]
   Int_t           nchilds[1000];   //[nscndprt]
   Float_t         fqpi0nll[2];
   Float_t         fqpi0mass[2];
   Float_t         fqpi0pos[2][3];
   Float_t         fqpi0momtot[2];
   Float_t         fqpi0dirtot[2][3];
   Int_t           fqnmrfit;
   Int_t           fqmrifit[32];
   Int_t           fqmrnring[32];
   Float_t         fqmrnll[32];
   Int_t           fqmrpid[32][6];
   Float_t         fqmrmom[32][6];
   Float_t         fqmreloss[32][6];
   Float_t         fqmrdir[32][6][3];
   Float_t         fqmrpos[32][6][3];
   Float_t         fqmrt0[32][6];
   Int_t           fqnse;
   Float_t         fq1rnll[10][7];
   Float_t         fq1rdir[10][7][3];
   Float_t         fq1rpos[10][7][3];
   Float_t         fq1rmom[10][7];
   Float_t         fq1rt0[10][7];
   Float_t         fq1reloss[10][7];
   Float_t         flxh11[3];
   Float_t         flxh06[3];
   Float_t         oscwgt;
   Float_t         t2kposc[2][6];

   Float_t         fWeight_sfrfg;

   // Xsec
   TGraph *byEv_maqe_gr;
   TGraph *byEv_pfo_gr;
   TGraph *byEv_ca5_gr;
   TGraph *byEv_manff_gr;
   TGraph *byEv_bgscl_gr;
   TGraph *byEv_dismpishp_gr;
   TGraph *byEv_pdd_gr;
   // FSI/SI
   TGraph *byEv_InelLow_gr;
   TGraph *byEv_InelHigh_gr;
   TGraph *byEv_PiProd_gr;
   TGraph *byEv_Abs_gr;
   TGraph *byEv_CExLow_gr;

   TF1 *f1[12];
   int npions;
   int fitflag;
   std::vector<int> fqmrindex; // fq multi-ring index ranked by either visible energy or momentum
   int              fVisible[10]; // number of visible particles in each category (e, mu, gm, pi0, pi+-, p) based on MC truth
   Float_t          posx, posy, posz, dirx, diry, dirz;
   
 protected:  // output variables

   TFile * outfile ;
   TTree * childtree;
   TTree * spchildtree;
   int  OscChannel;
   // derived variables;
   Float_t   wgtosc;
   Float_t   wgtflx;
   Float_t   wgtsfrfg;
   Float_t   prefit_ncgamma;
   Float_t   prefit_cccoh;
   Float_t   coszenith;
   Float_t   evisible;
   Float_t   cosbm_e;
   Float_t   cosbm_mu;
   Float_t   erec_e;
   Float_t   erec_mu;
   Float_t   pid_emu;
   Float_t   pid_mupip;
   Float_t   pid_epi0;
   Float_t   rcpar;
   Short_t   fqnring;
   Short_t   ndcye;
   Float_t   fqdwall_e;
   Float_t   fqtowall_e;
   Float_t   fqdwall_mu;
   Float_t   fqtowall_mu;
   Short_t   idByNEUT;
   Short_t   idByFS;
   Short_t   idByFSI;
   // xsec & FSI/SI polynomials
   Float_t   coefmaqe[6];
   Float_t   coefpfo[6];
   Float_t   coefca5[3];
   Float_t   coefmanff[3];
   Float_t   coefbgscl[3];
   Float_t   coefdis[4];
   Float_t   coefpdd[2];
   Int_t    has_pion;
   Float_t   coefinelo[5];
   Float_t   coefinehi[5];
   Float_t   coefpiprod[5];
   Float_t   coefpiabs[5];
   Float_t   coefcexlo[5];
   /*
   TSpline3 *byEv_maqe_sp;
   TSpline3 *byEv_pfo_sp;
   TSpline3 *byEv_ca5_sp;
   TSpline3 *byEv_manff_sp;
   TSpline3 *byEv_bgscl_sp;
   TSpline3 *byEv_dismpishp_sp;
   TSpline3 *byEv_pdd_sp;
   TSpline3 *byEv_berpa_a_sp;
   TSpline3 *byEv_berpa_b_sp;
   TSpline3 *byEv_berpa_c_sp;
   TSpline3 *byEv_berpa_d_sp;
   */
};

#endif
