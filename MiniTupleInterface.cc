#include "MiniTupleInterface.h"
#include "TFitResultPtr.h"

#include <cmath>
#include <sstream>


MiniTupleInterface::MiniTupleInterface( TTree * h, TTree *sfrfgw, TTree *spline, std::string name )
{
  parenttree = h ;
  sfrfgtree = sfrfgw;
  splinetree = spline;
  if (!sfrfgtree) {
    std::cout<<"no sfrfgtree!!!"<<std::endl;
  }
  EnableCopyBranches( ); 
  LoadBranchAddresses( );

  outfile = new TFile( name.c_str() , "recreate" );
  childtree = (TTree*) parenttree->CloneTree(0);
  if (splinetree) spchildtree = (TTree*) splinetree->CloneTree(0);
  //if (splinetree) spchildtree = new TTree("SplinesByEvent", "Event-by-event splines");

  DefineExtraBranches( );

  ifQtoUse = 0;
  sortByMom = true;
  OscChannel = -1;
  InitTF1();
  InitVars();
}

MiniTupleInterface::~MiniTupleInterface( )
{  
}

void MiniTupleInterface::GetEntry(int i)
{
  parenttree->GetEntry(i);
  if (splinetree) splinetree->GetEntry(i);
  if (sfrfgtree) {
    sfrfgtree->GetEntry(i);
    wgtsfrfg = fWeight_sfrfg;
  } else {
    wgtsfrfg = 1;
  }
}

void MiniTupleInterface::LoadBranchAddresses() // fixme
{
  parenttree->SetBranchAddress( "nhitac"      ,   &nhitac       );
  parenttree->SetBranchAddress( "numnu"       ,   &numnu        );
  parenttree->SetBranchAddress( "ipnu"        ,   ipnu          );
  parenttree->SetBranchAddress( "pnu"         ,   pnu           );
  parenttree->SetBranchAddress( "mode"        ,   &mode         );
  parenttree->SetBranchAddress( "npar"        ,   &npar         );
  parenttree->SetBranchAddress( "ipv"         ,   ipv           );
  parenttree->SetBranchAddress( "pmomv"       ,   pmomv         ); 
  parenttree->SetBranchAddress( "dirnu"       ,   dirnu         );
  parenttree->SetBranchAddress( "fq1rnll"     ,   fq1rnll       );
  parenttree->SetBranchAddress( "fqpi0nll"    ,   fqpi0nll      );
  parenttree->SetBranchAddress( "fqpi0mass"   ,   fqpi0mass     );
  parenttree->SetBranchAddress( "fqpi0momtot" ,   fqpi0momtot   );
  parenttree->SetBranchAddress( "fqpi0dirtot" ,   fqpi0dirtot   );
  parenttree->SetBranchAddress( "fqpi0pos"    ,   fqpi0pos      );
  parenttree->SetBranchAddress( "fqmrnll"     ,   fqmrnll       );  
  parenttree->SetBranchAddress( "fqnse"       ,   &fqnse        );
  parenttree->SetBranchAddress( "fq1rdir"     ,   fq1rdir       );
  parenttree->SetBranchAddress( "fq1rpos"     ,   fq1rpos       );
  parenttree->SetBranchAddress( "fq1rmom"     ,   fq1rmom       );
  parenttree->SetBranchAddress( "fq1rt0"      ,   fq1rt0        );
  parenttree->SetBranchAddress( "fq1reloss"   ,   fq1reloss     );
  parenttree->SetBranchAddress( "flxh11"      ,   flxh11        ); 
  parenttree->SetBranchAddress( "flxh06"      ,   flxh06        );
  parenttree->SetBranchAddress( "fqnmrfit"    ,   &fqnmrfit     );
  parenttree->SetBranchAddress( "fqmrifit"    ,   fqmrifit      );
  parenttree->SetBranchAddress( "fqmrnring"   ,   fqmrnring     );
  parenttree->SetBranchAddress( "fqmrpid"     ,   fqmrpid       );
  parenttree->SetBranchAddress( "fqmrmom"     ,   fqmrmom       );
  parenttree->SetBranchAddress( "fqmreloss"   ,   fqmreloss     );
  parenttree->SetBranchAddress( "fqmrdir"     ,   fqmrdir       );
  parenttree->SetBranchAddress( "fqmrpos"     ,   fqmrpos       );
  parenttree->SetBranchAddress( "fqmrt0"      ,   fqmrt0        );
  parenttree->SetBranchAddress("nscndprt"     ,   &nscndprt     );
  parenttree->SetBranchAddress("itrkscnd", itrkscnd);
  parenttree->SetBranchAddress("istakscnd", istakscnd);
  parenttree->SetBranchAddress("vtxscnd", vtxscnd);
  parenttree->SetBranchAddress("pscnd", pscnd);
  parenttree->SetBranchAddress("iprtscnd", iprtscnd);
  parenttree->SetBranchAddress("tscnd", tscnd);
  parenttree->SetBranchAddress("iprntprt", iprntprt);
  parenttree->SetBranchAddress("lmecscnd", lmecscnd);
  parenttree->SetBranchAddress("iprnttrk", iprnttrk);
  parenttree->SetBranchAddress("iorgprt", iorgprt);
  parenttree->SetBranchAddress("pprnt", pprnt);
  parenttree->SetBranchAddress("iflgscnd", iflgscnd);
  parenttree->SetBranchAddress("iprntidx", iprntidx);
  parenttree->SetBranchAddress("nchilds", nchilds);
  parenttree->SetBranchAddress("Npvc", &Npvc);
  parenttree->SetBranchAddress("Ipvc", Ipvc);
  parenttree->SetBranchAddress("Ichvc", Ichvc);
  parenttree->SetBranchAddress("Iorgvc", Iorgvc);
  parenttree->SetBranchAddress("Iflvc", Iflvc);
  parenttree->SetBranchAddress("Abspvc", Abspvc);
  parenttree->SetBranchAddress("oscwgt", &oscwgt);
  if (((TList*)parenttree->GetListOfBranches())->FindObject("t2kposc")) {
    parenttree->SetBranchStatus("t2kposc", 1);
    parenttree->SetBranchAddress("t2kposc", t2kposc);
  }

  if (sfrfgtree) sfrfgtree->SetBranchAddress("fWeight", &fWeight_sfrfg);
  //if (sfrfgtree) sfrfgtree->SetBranchAddress("fWeight", &wgtsfrfg);

  byEv_maqe_gr = 0;
  byEv_pfo_gr = 0;
  byEv_ca5_gr = 0;
  byEv_manff_gr = 0;
  byEv_bgscl_gr = 0;
  byEv_dismpishp_gr = 0;
  byEv_pdd_gr = 0;
  byEv_InelLow_gr = 0;
  byEv_InelHigh_gr = 0;
  byEv_PiProd_gr = 0;
  byEv_Abs_gr = 0;
  byEv_CExLow_gr = 0;
    
  if (splinetree) {
    splinetree->SetBranchAddress("byEv_maqe_gr", &byEv_maqe_gr);
    splinetree->SetBranchAddress("byEv_pfo_gr", &byEv_pfo_gr);
    splinetree->SetBranchAddress("byEv_ca5_gr", &byEv_ca5_gr);
    splinetree->SetBranchAddress("byEv_manff_gr", &byEv_manff_gr);
    splinetree->SetBranchAddress("byEv_bgscl_gr", &byEv_bgscl_gr);
    splinetree->SetBranchAddress("byEv_dismpishp_gr", &byEv_dismpishp_gr);
    splinetree->SetBranchAddress("byEv_pdd_gr", &byEv_pdd_gr);
    splinetree->SetBranchAddress("byEv_InelLow_gr", &byEv_InelLow_gr);
    splinetree->SetBranchAddress("byEv_InelHigh_gr", &byEv_InelHigh_gr);
    splinetree->SetBranchAddress("byEv_PiProd_gr", &byEv_PiProd_gr);
    splinetree->SetBranchAddress("byEv_Abs_gr", &byEv_Abs_gr);
    splinetree->SetBranchAddress("byEv_CExLow_gr", &byEv_CExLow_gr);
  }
}

void MiniTupleInterface::InitVars()
{
   wgtosc   =  1. ;
   wgtflx   =  1. ;
   wgtsfrfg =  1. ;
   prefit_ncgamma = 1;
   prefit_cccoh = 1;
   evisible = -999.;
   coszenith = -999.;
   cosbm_e = -999;
   erec_e = -999;
   cosbm_mu = -999;
   erec_mu = -999;
   ndcye = -1;
   idByNEUT = -1;
   idByFS = -1;
   idByFSI = -1;
   fqdwall_e = -999;
   fqtowall_e = -999;
   fqdwall_mu = -999;
   fqtowall_mu = -999;
   rcpar = -999;
   pid_emu = -999;
   pid_epi0 = -999;
   pid_mupip = -999;
   fqnring = 0;

   has_pion = 0;
   for (int i=0; i<6; ++i) {
     coefmaqe[i] = 0;
     coefpfo[i] = 0;
     if (i>4) continue;
     coefinelo[i] = 0;
     coefinehi[i] = 0;
     coefpiprod[i] = 0;
     coefpiabs[i] = 0;
     coefcexlo[i] = 0;
     if (i>3) continue;
     coefdis[i] = 0;
     if (i>2) continue;
     coefca5[i] = 0;
     coefmanff[i] = 0;
     coefbgscl[i] = 0;
     if (i>1) continue;
     coefpdd[i] = 0;
   }
}

void MiniTupleInterface::CalcDerived()
{
  //InitVars();
  SortRings();
  CountVisPar();
  CalcVtx();
  //if (splinetree) MakeSpline();
  if (splinetree) CalcPolyCoef();

  if (abs(mode)==38 || abs(mode)==39) prefit_ncgamma = GetExtraWeights();
  else if (abs(mode)==16) prefit_cccoh = GetExtraWeights();
  evisible = GetEvisTot();
  coszenith = GetCosZenith();
  cosbm_e = ComputeCosBeam(fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2]);
  erec_e = ComputeErec(fq1rmom[0][1], cosbm_e, 1);
  cosbm_mu = ComputeCosBeam(fq1rdir[0][2][0], fq1rdir[0][2][1], fq1rdir[0][2][2]);
  erec_mu = ComputeErec(fq1rmom[0][2], cosbm_mu, 2);
  ndcye = GetnMueDcy();
  idByNEUT = GetByNEUTMode();
  idByFS = GetByFSMode();
  idByFSI = GetByFSIMode();
  fqdwall_e = GetDwall(fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2]);
  fqtowall_e = GetTowall(fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2], fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2]);
  fqdwall_mu = GetDwall(fq1rpos[0][2][0], fq1rpos[0][2][1], fq1rpos[0][2][2]);
  fqtowall_mu = GetTowall(fq1rpos[0][2][0], fq1rpos[0][2][1], fq1rpos[0][2][2], fq1rdir[0][2][0], fq1rdir[0][2][1], fq1rdir[0][2][2]);
  rcpar = GetRCPar();
  pid_emu = GetPIDemu();
  pid_epi0 = GetPIDepi0();
  pid_mupip = GetPIDmupip();
  fqnring = GetNring();
}

void MiniTupleInterface::InitTF1()
{
  for (int i=0; i<12; ++i) {
    switch (i) {
    case 0:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x*x*x*x+[1]*x*x*x*x*x+[2]*x*x*x*x+[3]*x*x*x+[4]*x*x+[5]*x+1", -0.9, 0.45);
      break;
    case 1:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x*x*x*x+[1]*x*x*x*x*x+[2]*x*x*x*x+[3]*x*x*x+[4]*x*x+[5]*x+1", -0.45, 0.28);
      break;
    case 2:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x+[1]*x*x+[2]*x+1", -0.4, 0.4);
      break;
    case 3:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x+[1]*x*x+[2]*x+1", -0.6, 0.6);
      break;
    case 4:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x+[1]*x*x+[2]*x+1", -0.6, 0.6);
      break;
    case 5:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x*x+[1]*x*x*x+[2]*x*x+[3]*x+1", -1.2, 1.2);
      break;
    case 6:
      f1[i] = new TF1(Form("f%d", i), "(x<0)*([0]*x+1)+(x>0)*([1]*x+1)", -1.5, 1);
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x*x*x+[1]*x*x*x*x+[2]*x*x*x+[3]*x*x+[4]*x+1", -1, 1);
      break;
    default:
      f1[i] = new TF1(Form("f%d", i), "[0]*x*x*x*x*x*x+[1]*x*x*x*x*x+[2]*x*x*x*x+[3]*x*x*x+[4]*x*x+[5]*x+1", -1.5, 1);
      break;
    }
  }

}

void MiniTupleInterface::CalcPolyCoef()
{
  for (int i=0; i<6; ++i) {
    f1[0]->SetParameter(i, 0);
    f1[1]->SetParameter(i, 0);
    if (i>4) continue;
    f1[7]->SetParameter(i, 0);
    f1[8]->SetParameter(i, 0);
    f1[9]->SetParameter(i, 0);
    f1[10]->SetParameter(i, 0);
    f1[11]->SetParameter(i, 0);
    if (i>3) continue;
    f1[5]->SetParameter(i, 0);
    if (i>2) continue;
    f1[2]->SetParameter(i, 0);
    f1[3]->SetParameter(i, 0);
    f1[4]->SetParameter(i, 0);
    if (i>1) continue;
    f1[6]->SetParameter(i, 0);
  }
  TFitResultPtr fitresult[12];
  int fitStatus[12] = {0};
  fitresult[0] = byEv_maqe_gr->Fit(f1[0], "QWRFS");
  fitresult[1] = byEv_pfo_gr->Fit(f1[1], "QWRFS");
  fitresult[2] = byEv_ca5_gr->Fit(f1[2], "QWRFS");
  fitresult[3] = byEv_manff_gr->Fit(f1[3], "QWRFS");
  fitresult[4] = byEv_bgscl_gr->Fit(f1[4], "QWRFS");
  fitresult[5] = byEv_dismpishp_gr->Fit(f1[5], "QWRFS");
  fitresult[6] = byEv_pdd_gr->Fit(f1[6], "QWRFS");
  fitresult[7] = byEv_InelLow_gr->Fit(f1[7], "QWRFS");
  fitresult[8] = byEv_InelHigh_gr->Fit(f1[8], "QWRFS");
  fitresult[9] = byEv_PiProd_gr->Fit(f1[9], "QWRFS");
  fitresult[10] = byEv_Abs_gr->Fit(f1[10], "QWRFS");
  fitresult[11] = byEv_CExLow_gr->Fit(f1[11], "QWRFS");
  fitresult[0] = byEv_maqe_gr->Fit(f1[0], "QWRFS");
  fitresult[1] = byEv_pfo_gr->Fit(f1[1], "QWRFS");
  fitresult[2] = byEv_ca5_gr->Fit(f1[2], "QWRFS");
  fitresult[3] = byEv_manff_gr->Fit(f1[3], "QWRFS");
  fitresult[4] = byEv_bgscl_gr->Fit(f1[4], "QWRFS");
  fitresult[5] = byEv_dismpishp_gr->Fit(f1[5], "QWRFS");
  fitresult[6] = byEv_pdd_gr->Fit(f1[6], "QWRFS");
  fitresult[7] = byEv_InelLow_gr->Fit(f1[7], "QWRFS");
  fitresult[8] = byEv_InelHigh_gr->Fit(f1[8], "QWRFS");
  fitresult[9] = byEv_PiProd_gr->Fit(f1[9], "QWRFS");
  fitresult[10] = byEv_Abs_gr->Fit(f1[10], "QWRFS");
  fitresult[11] = byEv_CExLow_gr->Fit(f1[11], "QWRFS");

  for (int j=0; j<12; ++j) {
    if ((int)fitresult[j]>0) {
      switch (j) {
      case 0:
	fitresult[0] = byEv_maqe_gr->Fit(f1[0], "QWRFS");
	break;
      case 1:
	fitresult[1] = byEv_pfo_gr->Fit(f1[1], "QWRFS");
	break;
      case 2:
	fitresult[2] = byEv_ca5_gr->Fit(f1[2], "QWRFS");
	break;
      case 3:
	fitresult[3] = byEv_manff_gr->Fit(f1[3], "QWRFS");
	break;
      case 4:
	fitresult[4] = byEv_bgscl_gr->Fit(f1[4], "QWRFS");
	break;
      case 5:
	fitresult[5] = byEv_dismpishp_gr->Fit(f1[5], "QWRFS");
	break;
      case 6:
	fitresult[6] = byEv_pdd_gr->Fit(f1[6], "QWRFS");
	break;
      case 7:
	fitresult[7] = byEv_InelLow_gr->Fit(f1[7], "QWRFS");
	break;
      case 8:
	fitresult[8] = byEv_InelHigh_gr->Fit(f1[8], "QWRFS");
	break;
      case 9:
	fitresult[9] = byEv_PiProd_gr->Fit(f1[9], "QWRFS");
	break;
      case 10:
	fitresult[10] = byEv_Abs_gr->Fit(f1[10], "QWRFS");
	break;
      case 11:
	fitresult[11] = byEv_CExLow_gr->Fit(f1[11], "QWRFS");
	break;
      }
    }
  }
  fitflag = 0;
  for (int i=0; i<12; ++i) fitflag += (Int_t)fitresult[i];
  
  if (abs(mode)==1) { // ccqe -- maqe, pfo
    for (int i=0; i<6; ++i) {
      coefmaqe[i] = (float)f1[0]->GetParameter(i);
      coefpfo[i] = (float)f1[1]->GetParameter(i);
    }
  } else if (abs(mode)==2) { // mec -- pdd
    for (int i=0; i<2; ++i)
      coefpdd[i] = (float)f1[6]->GetParameter(i);
  } else if ((abs(mode)>10 && abs(mode)<16) || (abs(mode)>30 && abs(mode)<36)) { // cc1pi or nc1pi -- ca5, manff, bgscl
    for (int i=0; i<3; ++i) {
      coefca5[i] = (float)f1[2]->GetParameter(i);
      coefmanff[i] = (float)f1[3]->GetParameter(i);
      coefbgscl[i] = (float)f1[4]->GetParameter(i);
    }
  } else if (abs(mode)>21 && abs(mode)<30) { // DIS -- dis
    for (int i=0; i<4; ++i)
      coefdis[i] = (float)f1[5]->GetParameter(i);
  }
  
  npions = 0;
  for (Int_t iParticle = 0; iParticle < numnu; iParticle++){
    if (TMath::Abs(ipnu[iParticle]) == 211 || ipnu[iParticle] == 111) npions++;
  }
  bool scndpion = false;
  for (Int_t iParticle = 0; iParticle < nscndprt; ++iParticle) {
    if (iParticle>250) break;
    if (iflgscnd[iParticle]==3 || iflgscnd[iParticle]==8 || iflgscnd[iParticle]==9 || iflgscnd[iParticle]==901 || iflgscnd[iParticle]==900 || iflgscnd[iParticle]==400) {
      scndpion = true;
      break;
    } else if (iflgscnd[iParticle]==4) {
      if (iprtscnd[iParticle]==iprntprt[iParticle] || iprtscnd[iParticle]==111) {
	scndpion = true;
	break;
      }
    }
  }
  if (npions>0 || scndpion) has_pion = 1;
  if (has_pion) {
    for (int i=0; i<5; ++i) {
      coefinelo[i] = (float)f1[7]->GetParameter(i);
      coefinehi[i] = (float)f1[8]->GetParameter(i);
      coefpiprod[i] = (float)f1[9]->GetParameter(i);
      coefpiabs[i] = (float)f1[10]->GetParameter(i);
      coefcexlo[i] = (float)f1[11]->GetParameter(i);
    }
  }
}

void MiniTupleInterface::MakeSpline()
{
  /*
  byEv_maqe_sp = new TSpline3("byEv_maqe_sp", byEv_maqe_gr);
  byEv_pfo_sp = new TSpline3("byEv_pfo_sp", byEv_pfo_gr);
  byEv_ca5_sp = new TSpline3("byEv_ca5_sp", byEv_ca5_gr);
  byEv_manff_sp = new TSpline3("byEv_manff_sp", byEv_manff_gr);
  byEv_bgscl_sp = new TSpline3("byEv_bgscl_sp", byEv_bgscl_gr);
  byEv_dismpishp_sp = new TSpline3("byEv_dismpishp_sp", byEv_dismpishp_gr);
  byEv_pdd_sp = new TSpline3("byEv_pdd_sp", byEv_pdd_gr);
  byEv_berpa_a_sp = new TSpline3("byEv_berpa_a_sp", byEv_berpa_a_gr);
  byEv_berpa_b_sp = new TSpline3("byEv_berpa_b_sp", byEv_berpa_b_gr);
  byEv_berpa_c_sp = new TSpline3("byEv_berpa_c_sp", byEv_berpa_c_gr);
  byEv_berpa_d_sp = new TSpline3("byEv_berpa_d_sp", byEv_berpa_d_gr);
  */
}

bool MiniTupleInterface::PassPreSelection()
{
  if ( nhitac<16 && (fqdwall_e>50 || fqdwall_mu>50) && (fqtowall_e>150 || fqdwall_mu>150) && evisible>30 && evisible<1500.) return true;
  return false;
}

void MiniTupleInterface::EnableCopyBranches( )
{
  parenttree->SetBranchStatus( "*" , 0 );
  for ( unsigned i = 0 ; i < nExportBranches; i++ ) {
     parenttree->SetBranchStatus( ExportBranches[i].c_str() , 1 );
  }
}

void MiniTupleInterface::DefineExtraBranches( )
{
  childtree->Branch( "wgtosc"         , &wgtosc       , "wgtosc/F"        );
  childtree->Branch( "wgtflx"         , &wgtflx       , "wgtflx/F"        );
  childtree->Branch( "wgtsfrfg"       , &wgtsfrfg     , "wgtsfrfg/F"      );
  childtree->Branch( "wgtncgm"        , &prefit_ncgamma      , "wgtncgm/F"      );
  childtree->Branch( "wgtcccoh"       , &prefit_cccoh        , "wgtcccoh/F"      );
  childtree->Branch( "coszenith"      , &coszenith    , "coszenith/F"     );
  childtree->Branch( "evisible"       , &evisible     , "evisible/F"      );
  childtree->Branch( "cosbm_e"        , &cosbm_e      , "cosbm_e/F"       );
  childtree->Branch( "cosbm_mu"       , &cosbm_mu     , "cosbm_mu/F"      );
  childtree->Branch( "erec_e"         , &erec_e       , "erec_e/F"        );
  childtree->Branch( "erec_mu"        , &erec_mu      , "erec_mu/F"       );
  childtree->Branch( "pid_emu"        , &pid_emu      , "pid_emu/F"       ); 
  childtree->Branch( "pid_mupip"      , &pid_mupip    , "pid_mupip/F"     );
  childtree->Branch( "pid_epi0"       , &pid_epi0     , "pid_epi0/F"      );
  childtree->Branch( "rcpar"          , &rcpar        , "rcpar/F"         );
  childtree->Branch( "fqnring"        , &fqnring      , "fqnring/S"       );
  childtree->Branch( "ndcye"          , &ndcye        , "ndcye/S"         );
  childtree->Branch( "fqdwall_e"      , &fqdwall_e    , "fqdwall_e/F"     );
  childtree->Branch( "fqdwall_mu"     , &fqdwall_mu   , "fqdwall_mu/F"    );
  childtree->Branch( "fqtowall_e"     , &fqtowall_e   , "fqtowall_e/F"    );
  childtree->Branch( "fqtowall_mu"    , &fqtowall_mu  , "fqtowall_mu/F"   );
  childtree->Branch( "idByNEUT"       , &idByNEUT     , "idByNEUT/S"      );
  childtree->Branch( "idByFS"         , &idByFS       , "idByFS/S"        );
  childtree->Branch( "idByFSI"        , &idByFSI      , "idByFSI/S"       );
  childtree->Branch( "has_pion"       , &has_pion     , "has_pion/I"      );
  childtree->Branch( "coefmaqe"       , &coefmaqe     , "coefmaqe[6]/F"   );
  childtree->Branch( "coefpfo"        , &coefpfo      , "coefpfo[6]/F"    );
  childtree->Branch( "coefca5"        , &coefca5      , "coefca5[3]/F"    );
  childtree->Branch( "coefmanff"      , &coefmanff    , "coefmanff[3]/F" );
  childtree->Branch( "coefbgscl"      , &coefbgscl    , "coefbgscl[3]/F"  );
  childtree->Branch( "coefdis"        , &coefdis      , "coefdis[4]/F"    );
  childtree->Branch( "coefpdd"        , &coefpdd      , "coefpdd[2]/F"    );
  childtree->Branch( "coefinelo"      , &coefinelo    , "coefinelo[5]/F"  );
  childtree->Branch( "coefinehi"      , &coefinehi    , "coefinehi[5]/F"  );
  childtree->Branch( "coefpiprod"     , &coefpiprod   , "coefpiprod[5]/F" );
  childtree->Branch( "coefpiabs"      , &coefpiabs    , "coefpiabs[5]/F"  );
  childtree->Branch( "coefcexlo"      , &coefcexlo    , "coefcexlo[5]/F"  );
  
  if (splinetree) {
    /*
    spchildtree->Branch("byEv_maqe_sp", "TSpline3", &byEv_maqe_sp, 1280000, 1);
    spchildtree->Branch("byEv_pfo_sp", "TSpline3", &byEv_pfo_sp, 1280000, 1);
    spchildtree->Branch("byEv_ca5_sp", "TSpline3", &byEv_ca5_sp, 1280000, 1);
    spchildtree->Branch("byEv_manff_sp", "TSpline3", &byEv_manff_sp, 1280000, 1);
    spchildtree->Branch("byEv_bgscl_sp", "TSpline3", &byEv_bgscl_sp, 1280000, 1);
    spchildtree->Branch("byEv_dismpishp_sp", "TSpline3", &byEv_dismpishp_sp, 1280000, 1);
    spchildtree->Branch("byEv_pdd_sp", "TSpline3", &byEv_pdd_sp, 1280000, 1);
    spchildtree->Branch("byEv_berpa_a_sp", "TSpline3", &byEv_berpa_a_sp, 1280000, 1);
    spchildtree->Branch("byEv_berpa_b_sp", "TSpline3", &byEv_berpa_b_sp, 1280000, 1);
    spchildtree->Branch("byEv_berpa_c_sp", "TSpline3", &byEv_berpa_c_sp, 1280000, 1);
    spchildtree->Branch("byEv_berpa_d_sp", "TSpline3", &byEv_berpa_d_sp, 1280000, 1);
    */
  }
  
}

float MiniTupleInterface::ComputeCosBeam( float dx, float dy, float dz )
{
  float cosb = beamdir[0]*dx + beamdir[1]*dy + beamdir[2]*dz;
  return cosb;
}

float MiniTupleInterface::ComputeErec( float LeptonP , float cos, int flag )
{
   static const float Vnuc  = 27.0        ; // MeV 
   static const float mn    = 939.565346  ; // MeV
   static const float mp    = 938.272013  ; // MeV

   static const float me    = 0.510998   ; // MeV
   static const float mm    = 105.65836  ; // MeV

   float mass ;
   if( flag == 1 ) mass = me ;
   else            mass = mm ;

   float E  = 0.;
   float LeptonE = sqrt( mass*mass + LeptonP*LeptonP );

   E  = ( mn - Vnuc)*LeptonE - mass*mass/2. ;
   E +=   mn*Vnuc  - Vnuc*Vnuc/2.;
   E += ( mp*mp - mn*mn)/2.;
   E /= ( mn - Vnuc - LeptonE + LeptonP*cos );

   // returns value in MeV 
   return E;
}

float MiniTupleInterface::GetCosZenith()
{
  // the multi-ring algorithm in fiTQun yields a slightly different single-ring PID compared with T2K event selection. Here the fiTQun multi-ring result is used for SK atmospheric event calculations
  // although the difference is quite small, this should be changed in the future such that T2K and SK uses the same single-ring PID
  TVector3 ptot(0,0,0);
  if (fqmrnring[ifQtoUse]==1) {
    int iPID = 0; // same as T2K selection
    if (fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1]) { // e-like
      iPID = 1;
    } else if (fq1rnll[0][2]-fq1rnll[0][3]<0.15*fq1rmom[0][2]) { // mu-like
      iPID = 2;	
    } else { // pi+ like
      iPID = 3;
    }
    TVector3 pring;
    if (iPID==1) {
      TVector3 dirring(fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2]);
      pring = fq1rmom[0][1] * dirring;
    } else if (iPID==2) {
      TVector3 dirring(fq1rdir[0][2][0], fq1rdir[0][2][1], fq1rdir[0][2][2]);
      pring = fq1rmom[0][2] * dirring;
    } else {
      TVector3 dirring(fq1rdir[0][3][0], fq1rdir[0][3][1], fq1rdir[0][3][2]);
      pring = fq1reloss[0][3] * dirring;
    }
    ptot += pring;
  } else {
    for (int i=0; i<fqmrnring[ifQtoUse]; ++i) {
      int iPID = fqmrpid[ifQtoUse][i];
      TVector3 pring(fqmrdir[ifQtoUse][i][0], fqmrdir[ifQtoUse][i][1], fqmrdir[ifQtoUse][i][2]);
      if (iPID==1 || iPID==2) {
	pring *= fqmrmom[ifQtoUse][i];
      } else {
	pring *= GetRingEvis(i);
      }
      ptot += pring;
    }
  }
  return -ptot.CosTheta();
}

float MiniTupleInterface::GetElossMax(int iPID, float mom)
{
  double pCherenkovThr = m0[iPID]/0.882925;
  return TMath::Max( 0., TMath::Sqrt( mom*mom + m0[iPID]*m0[iPID] ) - TMath::Sqrt( TMath::Power( pCherenkovThr, 2.) + m0[iPID]*m0[iPID] ) );
}

float MiniTupleInterface::GetEnergy(int iPID, float mom)
{
  return TMath::Sqrt( mom*mom + m0[iPID]*m0[iPID] ); 
}

float MiniTupleInterface::GetRingEvis(int iring)
{
  if (iring<0 || iring>=fqmrnring[ifQtoUse]) return 0.;  
  int iPID = fqmrpid[ifQtoUse][iring];
  if (iPID==1) {//electron ring
    return fqmrmom[ifQtoUse][iring];
  } else if (iPID==2) {//muon ring
    return GetElossMax(iPID,fqmrmom[ifQtoUse][iring]);
  } else {//upstream track
    return fqmreloss[ifQtoUse][iring];
  }
}

float MiniTupleInterface::GetEvisTot()
{
  double evistot = 0;
  if (fqmrnring[ifQtoUse]==1) {
    if (fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1]) {
      evistot = fq1rmom[0][1];      
    } else if (fq1rnll[0][2]-fq1rnll[0][3]<0.15*fq1rmom[0][2]) {
      evistot = GetElossMax(2, fq1rmom[0][2]);
    } else {
      evistot = fq1reloss[0][3];
    }
  } else {
    for (int iring=0; iring<fqmrnring[ifQtoUse]; ++iring) {
      evistot += GetRingEvis(iring);
    }
  }
  return (float) evistot;
}

int MiniTupleInterface::GetnMueDcy()
{
  int nMueDcy_fQ = 0;
  for (int ise=1; ise<fqnse; ise++) {
    double tDcy_fQ = fq1rt0[ise][ie]-fqmrt0[ifQtoUse][fqmrindex.at(0)];    
    //if (tDcy_fQ>100.) {
      nMueDcy_fQ++;
    //}
  }
  return nMueDcy_fQ;
}

void MiniTupleInterface::SortRings()
{
  fqmrindex.resize(0);
  std::list<float> ringmom;
  std::map<float, int> ringmom_index;
  if (fqmrnring[ifQtoUse]==1) {
    if (fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1]) {
      ringmom.push_back(fq1rmom[0][1]);
      ringmom_index[fq1rmom[0][1]] = 0;
    } else if (fq1rnll[0][2]-fq1rnll[0][3]<0.15*fq1rmom[0][2]) {
      ringmom.push_back(fq1rmom[0][2]);
      ringmom_index[fq1rmom[0][2]] = 0;
    } else {
      ringmom.push_back(fq1reloss[0][3]);
      ringmom_index[fq1reloss[0][3]] = 0;
    }
  } else {
    if (sortByMom) {
      for (int i=0; i<fqmrnring[ifQtoUse]; ++i) {
	if (fqmrpid[ifQtoUse][i]==1 && fqmrpid[ifQtoUse][i]==2) {
	  ringmom.push_back(fqmrmom[ifQtoUse][i]);
	  ringmom_index[fqmrmom[ifQtoUse][i]] = i;
	} else {
	  ringmom.push_back(fqmreloss[ifQtoUse][i]);
	  ringmom_index[fqmreloss[ifQtoUse][i]] = i;
	}
      }
    } else {
      for (int i=0; i<fqmrnring[ifQtoUse]; ++i) {
	float ringevis = GetRingEvis(i);
	ringmom.push_back(ringevis);
	ringmom_index[ringevis] = i;
      }
    }
  }
  ringmom.sort(std::greater<int>());
  for (std::list<float>::iterator it=ringmom.begin(); it!=ringmom.end(); ++it) {
    fqmrindex.push_back(ringmom_index[*it]);
  }
}

int MiniTupleInterface::GetByNEUTMode()
{
  int AbsMode = std::abs(mode);
  int iNEUTMode=0;
  if (AbsMode==1) { // CCQE
    iNEUTMode=0;
  }
  else if (AbsMode==2) { // CC MEC
    iNEUTMode=1;
  }
  else if ( AbsMode>=11 && AbsMode<=13 ) { // CC1pi
    iNEUTMode=2;
  }
  else if ( AbsMode==16 ) { // CCcoherent
    iNEUTMode=3; 
  }
  else if ( AbsMode==21 ) { // CCnpi
    iNEUTMode=4;
  }
  else if (AbsMode<30 ) { // CC DIS
    iNEUTMode=5;
  }
  else if (AbsMode==31 || AbsMode==32) { // NCpi0
    iNEUTMode=6;
  }
  else if (AbsMode==33 || AbsMode==34) { // NCpipm
    iNEUTMode=7;
  }
  else if (AbsMode==36) { // NCcohereht
    iNEUTMode=8;
  }
  else if (AbsMode==38 || AbsMode==39) { // NCGamma
    iNEUTMode=9;
  }
  else if (AbsMode>36 && AbsMode<53) { // NCother including NC elastic
    iNEUTMode=10;
  }
  else {
    if (OscChannel) std::cout << "NEUT mode unknown : " << mode << std::endl;
    iNEUTMode=-1;
  }  
  return iNEUTMode;
}

void MiniTupleInterface::CountVisPar()
{
  const Double_t pthres_e  = 0.57 ;
  const Double_t pthres_mu = 118. ;
  const Double_t pthres_pi = 156. ;
  const Double_t pthres_p  = 1051. ;
  const Double_t pthres_gm = 20.;
  Int_t npar_vect_vis[10] ;
  Int_t npar_vect2_vis[10] ;
  for (int i=0; i<10; i++) {
    npar_vect_vis[i]=0;
    npar_vect2_vis[i]=0;
  }  
  
  for ( Int_t i=0; i<Npvc; i++) {// VCWORK stack, based on PDG code    
    if (Ichvc[i]==0) continue;    
    Int_t idp = TMath::Abs(Ipvc[i]) ;// PDG code
    Double_t pmom = Abspvc[i];// momentum    
    if ( idp==111 ) {   // pi0
      npar_vect_vis[w_pi0]++ ;
    }
    else if ( idp==211 ) {  // pi+-
      if ( pmom>pthres_pi )   {
        npar_vect_vis[w_pip]++ ;
      }
    }
    else if ( idp==22 ) {   // gamma
      if ( pmom>pthres_gm )      {
        npar_vect_vis[w_gm]++ ;
      }
    }
    else if ( idp==2212 ) {   // proton
      if ( pmom>pthres_p ) {
        npar_vect_vis[w_p]++ ;
      }
    }
    else if ( idp==11 ) { // electron
      if ( pmom>pthres_e ) {
	npar_vect_vis[w_e]++;
      }
    }
    else if ( idp==13 ) { // muon
      if ( pmom>pthres_mu ) {
	npar_vect_vis[w_mu]++;
      }
    }
  }
  
  for ( Int_t i=0 ; i<nscndprt ; i++ ) {   // Based on PDG code
    switch (lmecscnd[i]) {      
    case 6: // pair production
    case 7: // compton
    case 8: // photoelectric
    case 9: // bremstralung      
      //    case 12: // hadronic interaction
      //    case 13: // coherent scattering
    case 30: // below threshold
      continue;
    }
    if (tscnd[i]>10.) continue; // decay near vertex    
    Int_t idp = TMath::Abs(iprtscnd[i]) ;
    if (idp==211 && iprtscnd[i]==iprntprt[i]) continue;
    Double_t pmom = TMath::Sqrt(pscnd[i][0]*pscnd[i][0]+pscnd[i][1]*pscnd[i][1]+pscnd[i][2]*pscnd[i][2]);
    if ( idp==211 ) {       // pi+-
      if ( pmom>pthres_pi ) {
        npar_vect2_vis[w_pip]++ ;
      }
    }
    else if ( idp==111 ) { // pi0
      npar_vect2_vis[w_pi0]++ ;
    }
    else if ( idp==22 ) {
      if ( pmom>pthres_gm ) {    // gamma
        npar_vect2_vis[w_gm]++ ;
      }
    }
    else if ( idp==2212 ) {    // proton
      if ( pmom>pthres_p ) {
        npar_vect2_vis[w_p]++ ;
      }
    }
    else if ( idp==11 ) {        // electron
      if ( pmom>pthres_e ) {
        npar_vect2_vis[w_e]++ ;
      }
    }
    else if ( idp==13 ) {        // muon
      if ( pmom>pthres_mu) {
        npar_vect2_vis[w_mu]++ ;
      }
    }
  }
  npar_vect_vis[w_gm] -= 2*npar_vect_vis[w_pi0] ; // is this correct ??????????????????
  npar_vect2_vis[w_gm] -= 2*npar_vect2_vis[w_pi0] ; //remove two gamma from pi0 decay
  
  fVisible[w_pi0] = npar_vect_vis[w_pi0] + npar_vect2_vis[w_pi0];
  fVisible[w_pip] = npar_vect_vis[w_pip] + npar_vect2_vis[w_pip];
  fVisible[w_p] = npar_vect_vis[w_p] + npar_vect2_vis[w_p];
  fVisible[w_gm] = npar_vect_vis[w_gm] + npar_vect2_vis[w_gm];
  fVisible[w_e] = npar_vect_vis[w_e] + npar_vect2_vis[w_e];
  fVisible[w_mu] = npar_vect_vis[w_mu] + npar_vect2_vis[w_mu];
}

int MiniTupleInterface::GetByFSMode() // FS category by visible particles
{
  if ( ((fVisible[w_e]==1 && fVisible[w_gm]==0) || (fVisible[w_e]==0 && fVisible[w_gm]==1))
       && fVisible[w_mu]==0 && fVisible[w_pi0]==0 && fVisible[w_pip]==0 && fVisible[w_p]==0 )
    return 0; // 1e or 1gamma
  else if ( fVisible[w_mu]==1 && fVisible[w_e]==0 && fVisible[w_pi0]==0
	    && fVisible[w_pip]==0 && fVisible[w_p]==0 && fVisible[w_gm]==0)
    return 1; // 1mu
  else if ( ((fVisible[w_e]==1  && fVisible[w_gm]==0) || (fVisible[w_e]==0 && fVisible[w_gm]==1))
	    && fVisible[w_mu]==0 && (fVisible[w_pi0]>0 || fVisible[w_pip]>0 || fVisible[w_p]>0 ) )
    return 2; // (1e or 1gamma) + other
  else if (fVisible[w_mu]==1 //&& fVisible[w_e]==0 // && fVisible[w_gm]==0 ??????
	   && (fVisible[w_pi0]>0 || fVisible[w_pip]>0 || fVisible[w_p]>0 || fVisible[w_gm]>0 ) )
    return 3; // 1mu + other (including gamma)
  else if (fVisible[w_mu]==0 && fVisible[w_e]==0 && fVisible[w_pi0]==1
	   && fVisible[w_pip]==0 && fVisible[w_p]==0 && fVisible[w_gm]==0 )
    return 4; // 1pi0
  else if (fVisible[w_mu]==0 && fVisible[w_e]==0 && fVisible[w_pi0]==0 && fVisible[w_gm]==0
	   && ((fVisible[w_pip]==1 && fVisible[w_p]==0) || (fVisible[w_pip]==0 && fVisible[w_p]==1)) )
    return 5; // single hadron
  else return 6; // others (multiple hadrons, e/gamma...)
}

int MiniTupleInterface::GetByFSIMode() // FS category by the number of pions
{
  int iFSPiMode;
  int iNuFlv = 0;
  if (ipnu[0]==14 || ipnu[0]==-14) iNuFlv = 1;
  if (abs(mode)<30) {//CC
    if (fVisible[w_pi0]==0) {//CC 0pi0
      if (fVisible[w_pip]==0) {//CC 0pi0 0pi+-
        iFSPiMode=5*iNuFlv+0;
      }
      else {//CC 0pi0 w/ pi+-
        iFSPiMode=5*iNuFlv+1;
      }
    }
    else if (fVisible[w_pi0]==1) {//CC 1pi0
      if (fVisible[w_pip]==0) {//CC 1pi0 0pi+-
        iFSPiMode=5*iNuFlv+2;
      }
      else {//CC 1pi0 w/ pi+-
        iFSPiMode=5*iNuFlv+3;
      }
    }
    else {//CC mPi0
      iFSPiMode=5*iNuFlv+4;
    }
  }
  else {// NC
    if (fVisible[w_pi0]==0) {//NC 0pi0
      if (fVisible[w_pip]==0) {//NC 0pi0 0pi+-
        iFSPiMode=10;
      }
      else {//NC 0pi0 w/ pi+-
        iFSPiMode=11;
      }
    }
    else if (fVisible[w_pi0]==1) {//NC 1pi0
      if (fVisible[w_pip]==0) {//NC 1pi0 0pi+-
        iFSPiMode=12;
      }
      else {
        iFSPiMode=13;
      }
    }
    else {//NC mPi0
      iFSPiMode=14;
    }
  }
  return iFSPiMode;
}

void MiniTupleInterface::CalcVtx()
{
  if (fqmrnring[ifQtoUse]==1) {
    if ( fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1] ) {
      posx = fq1rpos[0][1][0];
      posy = fq1rpos[0][1][1];
      posz = fq1rpos[0][1][2];
      dirx = fq1rdir[0][1][0];
      diry = fq1rdir[0][1][1];
      dirz = fq1rdir[0][1][2];
    } else {
      posx = fq1rpos[0][2][0];
      posy = fq1rpos[0][2][1];
      posz = fq1rpos[0][2][2];
      dirx = fq1rdir[0][2][0];
      diry = fq1rdir[0][2][1];
      dirz = fq1rdir[0][2][2];
    }
  } else {
    if ( fqmrnring[ifQtoUse]==2 && fqmrpid[ifQtoUse][0]==1 && fqmrpid[ifQtoUse][1]==1 ) {
      dirx = fqpi0dirtot[0][0];
      diry = fqpi0dirtot[0][1];
      dirz = fqpi0dirtot[0][2];
      posx = fqpi0pos[0][0];
      posy = fqpi0pos[0][1];
      posz = fqpi0pos[0][2];
    } else {
      posx = fqmrpos[ifQtoUse][fqmrindex.at(0)][0];
      posy = fqmrpos[ifQtoUse][fqmrindex.at(0)][1];
      posz = fqmrpos[ifQtoUse][fqmrindex.at(0)][2];
      dirx = fqmrdir[ifQtoUse][fqmrindex.at(0)][0];
      diry = fqmrdir[ifQtoUse][fqmrindex.at(0)][1];
      dirz = fqmrdir[ifQtoUse][fqmrindex.at(0)][2]; 
    }
  }  
}

float MiniTupleInterface::GetDwall(float x, float y, float z)
{
  float Rmax = 1690.;
  float Zmax = 1810.;
  float rr   = sqrt(x*x + y*y);
  float absz = TMath::Abs(z);
  //check if vertex is outside tank
  float signflg = 1.;
  if (absz>Zmax) signflg = -1.;
  if (rr>Rmax)   signflg = -1.;
  //find min distance to wall
  float distz = TMath::Abs(Zmax-absz);
  float distr = TMath::Abs(Rmax-rr);
  float wall = signflg*fmin(distz,distr);
  return wall;
}

float MiniTupleInterface::GetTowall(float x, float y, float z, float dx, float dy, float dz)
{
  Double_t const R(1690);
  Double_t l_b(100000.0), H;
  Double_t l_t(100000.0);
  Double_t A, B, C, RAD;
  if(dx!=0 || dy!=0){
    A = (dx*dx+dy*dy);
    B = 2*(x*dx+y*dy);
    C = (x*x+y*y-R*R);
    RAD = (B*B) - (4*A*C);
    l_b = ((-1*B) + sqrt(RAD))/(2*A);
  }
  if (dz==0){return l_b;}
  else if(dz>0){H=1810;}
  else if(dz<0){H=-1810;}
  l_t=(H - z)/dz;
  return  (float)(l_t > l_b ? l_b:l_t);
}

float MiniTupleInterface::GetRCPar()
{
  double ngLnLBest = 10000000.;
  int best2rindex = 0;
  for (int ifit=0; ifit<fqnmrfit; ++ifit){
    int fitID = TMath::Abs(fqmrifit[ifit]); //< fit fit ID code
    if ( TMath::Abs(fitID-20000000) < 40 ) {
      if (fqmrnll[ifit] < ngLnLBest){
	ngLnLBest = fqmrnll[ifit];
	best2rindex = ifit;
      }
    }
  }
  float best1Rnglnl = TMath::Min(fq1rnll[0][1],fq1rnll[0][2]);
  float ringmom = TMath::Min(fqmrmom[best2rindex][0], fqmrmom[best2rindex][1]);
  float deltaLnL = best1Rnglnl - fqmrnll[best2rindex];
  float cthresh = 150. - 0.6*ringmom;
  if (cthresh<0.) cthresh=0.;
  double rc = 50*TMath::Sqrt(TMath::Abs(deltaLnL-cthresh));
  if (deltaLnL-cthresh<0) rc *= -1;
  return rc;
}

short MiniTupleInterface::GetNring()
{
  return fqmrnring[ifQtoUse];
}

float MiniTupleInterface::GetPIDemu()
{
  return (-(fq1rnll[0][2]-fq1rnll[0][1]-0.2*fq1rmom[0][1])/1.0198);
}

float MiniTupleInterface::GetPIDepi0()
{
  return ((fq1rnll[0][1]-fqpi0nll[0]+0.875*fqpi0mass[0]-175)/1.32877);
}

float MiniTupleInterface::GetPIDmupip()
{
  return ((fq1rnll[0][2]-fq1rnll[0][3]-0.15*fq1rmom[0][2])/1.0112); 
}

void MiniTupleInterface::Fill()
{
   childtree->Fill();
   //if(splinetree) spchildtree->Fill();
   ClearSpline();
}

void MiniTupleInterface::ClearSpline()
{
  /*
   delete byEv_maqe_sp;
   delete byEv_pfo_sp;
   delete byEv_ca5_sp;
   delete byEv_manff_sp;
   delete byEv_bgscl_sp;
   delete byEv_dismpishp_sp;
   delete byEv_pdd_sp;
   delete byEv_berpa_a_sp;
   delete byEv_berpa_b_sp;
   delete byEv_berpa_c_sp;
   delete byEv_berpa_d_sp;
  */
}

void MiniTupleInterface::Finish()
{
   outfile->cd();
   childtree->Write(0 , TObject::kWriteDelete );
   //if (splinetree) spchildtree->Write(0, TObject::kWriteDelete);
   outfile->Close();
}

double MiniTupleInterface::GetExtraWeights()
{
  int mode_ = TMath::Abs( mode );
  double Epi = 0.; //MeV

  double weight = 1.;
  if(mode_ == 38 || mode_ == 39){ weight *= 2; } // NCgamma prefit weight

  if(mode_ == 16){ // CCCoh prefit weight

    int j=0;
    int find = 0;
    while(j<npar && find==0){
      if(ipv[j] == 8 || ipv[j] == 9){
        Epi = (pmomv[j]*pmomv[j])/(2.*139.57);
        find++;
      }
      j++;
    }
    if(Epi<250.){ weight *= 0.135; }
    else if(Epi<500.){ weight *= 0.4; }
    else if(Epi<750.){ weight *= 0.294; }
    else if(Epi<1000.){ weight *= 1.206; }
  }

  return weight;
}
