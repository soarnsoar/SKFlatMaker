//-------------------------------------------------
//
//   Class: SKFlatMaker
//
//   Description: Flat Ntuple maker for SNUCMS group data analysis framework
//
//
//   Authors of DYNtupleMaker (https://github.com/KyeongPil-Lee/NtupleMaker)
//   H.D.Yoo           Purdue University
//   K.P.Lee           Seoul National University
//   D.M.Pai           Seoul National University (EGM corrections, emu variables, and Gen-level neutrinos)
//
//   Revised By
//   S.B. Oh           Seoul National University
//   J.S. Kim          Seoul National University
//
//--------------------------------------------------

#include "SKFlatMaker/SKFlatMaker/interface/SKFlatMaker.h"

//
// class decleration
//

using namespace std;
using namespace reco;
using namespace edm;
using namespace pat;
using namespace isodeposit;


// -- Constructor -- //
SKFlatMaker::SKFlatMaker(const edm::ParameterSet& iConfig):
// -- object tokens -- //
MuonToken                           ( consumes< std::vector<pat::Muon> >                    (iConfig.getUntrackedParameter<edm::InputTag>("Muon")) ),
ElectronToken                       ( consumes< edm::View<pat::Electron> >                  (iConfig.getUntrackedParameter<edm::InputTag>("Electron")) ),
PhotonToken                         ( consumes< edm::View<pat::Photon> >                    (iConfig.getUntrackedParameter<edm::InputTag>("Photon")) ),
JetToken                            ( consumes< std::vector<pat::Jet> >                     (iConfig.getUntrackedParameter<edm::InputTag>("Jet")) ),
genJetToken                         ( consumes< reco::GenJetCollection >                    (iConfig.getUntrackedParameter<edm::InputTag>("GenJet")) ),
FatJetToken                         ( consumes< std::vector<pat::Jet> >                     (iConfig.getUntrackedParameter<edm::InputTag>("FatJet")) ),
genFatJetToken                      ( consumes< reco::GenJetCollection >                    (iConfig.getUntrackedParameter<edm::InputTag>("GenFatJet")) ),
MetToken                            ( consumes< std::vector<pat::MET> >                     (iConfig.getParameter<edm::InputTag>("MET")) ),
//MetToken                            ( consumes< pat::METCollection>                               (iConfig.getParameter<edm::InputTag>("MET")) ),

LHEEventProductToken                ( consumes< LHEEventProduct >                           (iConfig.getUntrackedParameter<edm::InputTag>("LHEEventProduct")) ),
LHERunInfoProductToken              ( consumes< LHERunInfoProduct,edm::InRun >              (iConfig.getUntrackedParameter<edm::InputTag>("LHERunInfoProduct")) ),
mcLabel_                            ( consumes< reco::GenParticleCollection>                (iConfig.getUntrackedParameter<edm::InputTag>("GenParticle"))  ),
pcToken_                            ( consumes< pat::PackedCandidateCollection >            (iConfig.getUntrackedParameter<edm::InputTag>("pfCandsForMiniIso"))   ),

// -- MET Filter tokens -- //
METFilterResultsToken_PAT           ( consumes<edm::TriggerResults>                         (iConfig.getParameter<edm::InputTag>("METFilterResults_PAT"))),
METFilterResultsToken_RECO          ( consumes<edm::TriggerResults>                         (iConfig.getParameter<edm::InputTag>("METFilterResults_RECO"))),

// -- Electron tokens -- //
RhoToken                            ( consumes< double >                                    (iConfig.getUntrackedParameter<edm::InputTag>("rho")) ),
ConversionsToken                    ( consumes< std::vector<reco::Conversion> >             (iConfig.getUntrackedParameter<edm::InputTag>("conversionsInputTag")) ),
GsfTrackToken                       ( consumes< std::vector< reco::GsfTrack > >             (iConfig.getUntrackedParameter<edm::InputTag>("GsfTrack")) ),

// -- Trigger Token -- //
TriggerToken                        ( consumes< edm::TriggerResults >                       (iConfig.getUntrackedParameter<edm::InputTag>("TriggerResults")) ),
TriggerTokenPAT                     ( consumes< edm::TriggerResults >                       (iConfig.getUntrackedParameter<edm::InputTag>("TriggerResultsPAT")) ),
TriggerObjectToken                  ( consumes< std::vector<pat::TriggerObjectStandAlone> > (iConfig.getUntrackedParameter<edm::InputTag>("TriggerObject")) ),
// -- Else -- //
GenEventInfoToken                   ( consumes< GenEventInfoProduct >                       (iConfig.getUntrackedParameter<edm::InputTag>("GenEventInfo")) ),
BeamSpotToken                       ( consumes< reco::BeamSpot >                            (iConfig.getUntrackedParameter<edm::InputTag>("BeamSpot")) ),
PrimaryVertexToken                  ( consumes< reco::VertexCollection >                    (iConfig.getUntrackedParameter<edm::InputTag>("PrimaryVertex")) ),
TrackToken                          ( consumes< edm::View<reco::Track> >                    (iConfig.getUntrackedParameter<edm::InputTag>("Track")) ),
PileUpInfoToken                     ( consumes< std::vector< PileupSummaryInfo > >          (iConfig.getUntrackedParameter<edm::InputTag>("PileUpInfo")) )
{

  theDebugLevel                     = iConfig.getUntrackedParameter<int>("DebugLevel", 0);

  if(theDebugLevel) cout << "[SKFlatMaker::SKFlatMaker] Constructor called" << endl;

  nEvt = 0;
  
  processName                       = iConfig.getUntrackedParameter<string>("processName", "HLT");

  electron_EA_NHandPh_file          = iConfig.getUntrackedParameter<edm::FileInPath>( "electron_EA_NHandPh_file" );
  photon_EA_CH_file                 = iConfig.getUntrackedParameter<edm::FileInPath>( "photon_EA_CH_file" );
  photon_EA_HN_file                 = iConfig.getUntrackedParameter<edm::FileInPath>( "photon_EA_HN_file" );
  photon_EA_Ph_file                 = iConfig.getUntrackedParameter<edm::FileInPath>( "photon_EA_Ph_file" );

  miniIsoParams_                    = iConfig.getParameter< std::vector<double> >("miniIsoParams");
  miniIsoParamsE_                   = iConfig.getParameter< std::vector<double> >("miniIsoParamsE");
  miniIsoParamsB_                   = iConfig.getParameter< std::vector<double> >("miniIsoParamsB");

  jet_payloadName_                  = iConfig.getParameter<std::string>("AK4Jet_payloadName");
  fatjet_payloadName_               = iConfig.getParameter<std::string>("AK8Jet_payloadName");

  jet_jecUnc = NULL;
  fatjet_jecUnc = NULL;
  
  // -- Store Flags -- //
  theStorePriVtxFlag                = iConfig.getUntrackedParameter<bool>("StorePriVtxFlag", true);
  theStoreJetFlag                   = iConfig.getUntrackedParameter<bool>("StoreJetFlag", true);
  theStoreFatJetFlag                = iConfig.getUntrackedParameter<bool>("StoreFatJetFlag", true);
  theStoreMETFlag                   = iConfig.getUntrackedParameter<bool>("StoreMETFlag", true);
  theStoreHLTReportFlag             = iConfig.getUntrackedParameter<bool>("StoreHLTReportFlag", true);
  theStoreHLTObjectFlag             = iConfig.getUntrackedParameter<bool>("StoreHLTObjectFlag", true);
  theStoreMuonFlag                  = iConfig.getUntrackedParameter<bool>("StoreMuonFlag", true);
  theStoreElectronFlag              = iConfig.getUntrackedParameter<bool>("StoreElectronFlag", true);
  theStoreLHEFlag                   = iConfig.getUntrackedParameter<bool>("StoreLHEFlag", false);
  theStoreGENFlag                   = iConfig.getUntrackedParameter<bool>("StoreGENFlag", true);
  theKeepAllGen                     = iConfig.getUntrackedParameter<bool>("KeepAllGen", true);
  theStoreTTFlag                    = iConfig.getUntrackedParameter<bool>("StoreTTFlag", false);
  theStorePhotonFlag                = iConfig.getUntrackedParameter<bool>("StorePhotonFlag", true);

  rc.init(edm::FileInPath( iConfig.getParameter<std::string>("roccorPath") ).fullPath());

  //==================
  //==== Prepare JER
  //==================

  //==== this method uses Spring16_25nsV6 (80X, 2016, ICHEP dataset) DATA/MC SFs ...
  //jet_resolution = JME::JetResolution::get(iSetup, "AK4PFchs_pt");
  //jet_resolution_sf = JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");

  jet_resolution = JME::JetResolution( edm::FileInPath( iConfig.getParameter<std::string>("AK4Jet_JER_PtRes_filepath") ).fullPath()  );
  jet_resolution_sf = JME::JetResolutionScaleFactor( edm::FileInPath( iConfig.getParameter<std::string>("AK4Jet_JER_SF_filepath") ).fullPath() );
  fatjet_resolution = JME::JetResolution( edm::FileInPath( iConfig.getParameter<std::string>("AK8Jet_JER_PtRes_filepath") ).fullPath()  );
  fatjet_resolution_sf = JME::JetResolutionScaleFactor( edm::FileInPath( iConfig.getParameter<std::string>("AK8Jet_JER_SF_filepath") ).fullPath() );

  //==== PDF

  PDFOrder_ = iConfig.getParameter<string>("PDFOrder");
  PDFIDShift_ = iConfig.getParameter<int>("PDFIDShift");
  PDFType_ = iConfig.getParameter<string>("PDFType");

  if(PDFType_=="powheg") int_PDFType_=0;
  else if(PDFType_=="madgraph0") int_PDFType_=1;
  else if(PDFType_=="madgraph1000") int_PDFType_=2;
  else{
    int_PDFType_=-1;
  }

  // -- Filters -- //
  DoPileUp = iConfig.getUntrackedParameter<bool>("DoPileUp");
  // if( DoPileUp )
  // {
  //   PileUpRD_ = iConfig.getParameter< std::vector<double> >("PileUpRD");
  //   PileUpRDMuonPhys_ = iConfig.getParameter< std::vector<double> >("PileUpRDMuonPhys");
  //   PileUpMC_ = iConfig.getParameter< std::vector<double> >("PileUpMC");
  // }
  
  if(theDebugLevel) cout << "[SKFlatMaker::SKFlatMaker] Constructor finished" << endl;

}

SKFlatMaker::~SKFlatMaker() { }

//
// member functions
//

// ------------ method called to for each event  ------------ //
void SKFlatMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] called" << endl;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theTTBuilder);
  
  ///////////////////////////////////////////
  // -- initialize for ntuple variables -- //
  ///////////////////////////////////////////
  
  Flag_goodVertices = false;
  Flag_globalTightHalo2016Filter = false;
  Flag_globalSuperTightHalo2016Filter = false;
  Flag_HBHENoiseFilter = false;
  Flag_HBHENoiseIsoFilter = false;
  Flag_EcalDeadCellTriggerPrimitiveFilter = false;
  Flag_BadPFMuonFilter = false;
  Flag_BadChargedCandidateFilter = false;
  Flag_eeBadScFilter = false;
  Flag_ecalBadCalibFilter = false;

  nPV = -1;
  PVtrackSize = -1;
  PVchi2 = -1;
  PVndof = -1;
  PVnormalizedChi2 = -1;
  PVx = -1000;
  PVy = -1000;
  PVz = -1000;
  PVprob = -1;
  
  // -- PF iso deposits -- // 
  sumEt = 0;
  photonEt = 0;
  chargedHadronEt = 0;
  neutralHadronEt = 0;
  Rho = 0;

  //==== MET

  pfMET_pt=-999;
  pfMET_phi=-999;
  pfMET_Px=-999;
  pfMET_Py=-999;
  pfMET_SumEt=-999;
  pfMET_Type1_pt=-999;
  pfMET_Type1_phi=-999;
  pfMET_Type1_Px=-999;
  pfMET_Type1_Py=-999;
  pfMET_Type1_SumEt=-999;
  pfMET_Type1_PhiCor_pt=-999;
  pfMET_Type1_PhiCor_phi=-999;
  pfMET_Type1_PhiCor_Px=-999;
  pfMET_Type1_PhiCor_Py=-999;
  pfMET_Type1_PhiCor_SumEt=-999;
  pfMET_pt_shifts.clear();
  pfMET_phi_shifts.clear();
  pfMET_Px_shifts.clear();
  pfMET_Py_shifts.clear();
  pfMET_SumEt_shifts.clear();
  pfMET_Type1_pt_shifts.clear();
  pfMET_Type1_phi_shifts.clear();
  pfMET_Type1_Px_shifts.clear();
  pfMET_Type1_Py_shifts.clear();
  pfMET_Type1_SumEt_shifts.clear();
  pfMET_Type1_PhiCor_pt_shifts.clear();
  pfMET_Type1_PhiCor_phi_shifts.clear();
  pfMET_Type1_PhiCor_Px_shifts.clear();
  pfMET_Type1_PhiCor_Py_shifts.clear();
  pfMET_Type1_PhiCor_SumEt_shifts.clear();

  //==== Tracker Track

  TrackerTrack_dxy.clear();
  TrackerTrack_dxyErr.clear();
  TrackerTrack_d0.clear();
  TrackerTrack_d0Err.clear();
  TrackerTrack_dsz.clear();
  TrackerTrack_dszErr.clear();
  TrackerTrack_dz.clear();
  TrackerTrack_dzErr.clear();
  TrackerTrack_dxyBS.clear();
  TrackerTrack_dszBS.clear();
  TrackerTrack_dzBS.clear();
  TrackerTrack_pt.clear();
  TrackerTrack_Px.clear();
  TrackerTrack_Py.clear();
  TrackerTrack_Pz.clear();
  TrackerTrack_eta.clear();
  TrackerTrack_phi.clear();
  TrackerTrack_charge.clear();

  //==== Trigger (object)

  HLT_TriggerName.clear();
  HLT_TriggerFilterName.clear();
  HLT_TriggerFired.clear();
  HLT_TriggerPrescale.clear();
  HLTObject_pt.clear();
  HLTObject_eta.clear();
  HLTObject_phi.clear();
  HLTObject_FiredFilters.clear();
  HLTObject_FiredPaths.clear();

  //==== LHE

  PDFWeights_Scale.clear();
  PDFWeights_Error.clear();
  PDFWeights_AlphaS.clear();

  //==== GEN

  gen_phi.clear();
  gen_eta.clear();
  gen_pt.clear();
  gen_Px.clear();
  gen_Py.clear();
  gen_Pz.clear();
  gen_E.clear();
  gen_mother_PID.clear();
  gen_mother_pt.clear();
  gen_mother_index.clear();
  gen_charge.clear();
  gen_status.clear();
  gen_PID.clear();
  gen_isPrompt.clear();
  gen_isPromptFinalState.clear();
  gen_isTauDecayProduct.clear();
  gen_isPromptTauDecayProduct.clear();
  gen_isDirectPromptTauDecayProductFinalState.clear();
  gen_isHardProcess.clear();
  gen_isLastCopy.clear();
  gen_isLastCopyBeforeFSR.clear();
  gen_isPromptDecayed.clear();
  gen_isDecayedLeptonHadron.clear();
  gen_fromHardProcessBeforeFSR.clear();
  gen_fromHardProcessDecayed.clear();
  gen_fromHardProcessFinalState.clear();
  gen_isMostlyLikePythia6Status3.clear();
  gen_weight=-999;
  genWeight_Q=-999;
  genWeight_X1=-999;
  genWeight_X2=-999;
  genWeight_id1=-999;
  genWeight_id2=-999;
  genWeight_alphaQCD=-999;
  genWeight_alphaQED=-999;

  // -- PU reweight -- //
  PUweight = -1;
  pileUpReweightIn = pileUpReweight = 1.0;
  pileUpReweightPlus = pileUpReweightMinus = 0.0;
  pileUpReweightInMuonPhys = pileUpReweightMuonPhys = 1.0;
  pileUpReweightPlusMuonPhys = pileUpReweightMinusMuonPhys = 0.0;
  
  //==== Electron
  electron_MVAIso.clear();
  electron_MVANoIso.clear();
  electron_et.clear();
  electron_caloEnergy.clear();
  electron_Energy.clear();
  electron_Energy_Scale_Up.clear();
  electron_Energy_Scale_Down.clear();
  electron_Energy_Smear_Up.clear();
  electron_Energy_Smear_Down.clear();
  electron_pt.clear();
  electron_pt_Scale_Up.clear();
  electron_pt_Scale_Down.clear();
  electron_pt_Smear_Up.clear();
  electron_pt_Smear_Down.clear();
  electron_Px.clear();
  electron_Py.clear();
  electron_Pz.clear();
  electron_eta.clear();
  electron_phi.clear();
  electron_charge.clear();
  electron_gsfpt.clear();
  electron_gsfPx.clear();
  electron_gsfPy.clear();
  electron_gsfPz.clear();
  electron_gsfEta.clear();
  electron_gsfPhi.clear();
  electron_gsfCharge.clear();
  electron_scEta.clear();
  electron_scPhi.clear();
  electron_etaWidth.clear();
  electron_phiWidth.clear();
  electron_dEtaIn.clear();
  electron_dEtaInSeed.clear();
  electron_dPhiIn.clear();
  electron_sigmaIEtaIEta.clear();
  electron_Full5x5_SigmaIEtaIEta.clear();
  electron_HoverE.clear();
  electron_fbrem.clear();
  electron_eOverP.clear();
  electron_energyEC.clear();
  electron_Pnorm.clear();
  electron_InvEminusInvP.clear();
  electron_dxyVTX.clear();
  electron_dxyerrVTX.clear();
  electron_dzVTX.clear();
  electron_dzerrVTX.clear();
  electron_3DIPVTX.clear();
  electron_3DIPerrVTX.clear();
  electron_dxy.clear();
  electron_sigdxy.clear();
  electron_dz.clear();
  electron_dxyBS.clear();
  electron_dzBS.clear();
  electron_AEff03.clear();
  electron_chIso03.clear();
  electron_nhIso03.clear();
  electron_phIso03.clear();
  electron_pcIso03.clear();
  electron_puChIso03.clear();
  electron_chIso04.clear();
  electron_nhIso04.clear();
  electron_phIso04.clear();
  electron_pcIso04.clear();
  electron_puChIso04.clear();
  electron_relIsoCom03.clear();
  electron_relIsoCom04.clear();
  electron_relIsoBeta03.clear();
  electron_relIsoBeta04.clear();
  electron_relIsoRho03.clear();
  electron_passConversionVeto.clear();
  electron_isGsfCtfScPixChargeConsistent.clear();
  electron_isGsfScPixChargeConsistent.clear();
  electron_isGsfCtfChargeConsistent.clear();
  electron_mHits.clear();
  electron_crack.clear();
  electron_ecalDriven.clear();
  electron_isoEMHADDepth1.clear();
  electron_25over55.clear();
  electron_15over55.clear();
  electron_isoHADDepth2.clear();
  electron_isoptTrks.clear();
  electron_modIsoEMHADDepth1.clear();
  electron_modIsoptTrks.clear();
  electron_modIsoEMHADDepth1Orig.clear();
  electron_modIsoptTrksOrig.clear();
  electron_ambGsf0pt.clear();
  electron_ambGsf0Eta.clear();
  electron_ambGsf0Phi.clear();
  electron_ambGsf0Charge.clear();
  electron_ambGsf1pt.clear();
  electron_ambGsf1Eta.clear();
  electron_ambGsf1Phi.clear();
  electron_ambGsf1Charge.clear();
  electron_ambGsf2pt.clear();
  electron_ambGsf2Eta.clear();
  electron_ambGsf2Phi.clear();
  electron_ambGsf2Charge.clear();
  electron_ambGsf3pt.clear();
  electron_ambGsf3Eta.clear();
  electron_ambGsf3Phi.clear();
  electron_ambGsf3Charge.clear();
  electron_r9.clear();
  electron_scEnergy.clear();
  electron_scPreEnergy.clear();
  electron_scRawEnergy.clear();
  electron_scEt.clear();
  electron_E15.clear();
  electron_E25.clear();
  electron_E55.clear();
  electron_RelPFIso_dBeta.clear();
  electron_RelPFIso_Rho.clear();
  electron_passVetoID.clear();
  electron_passLooseID.clear();
  electron_passMediumID.clear();
  electron_passTightID.clear();
  electron_passMVAID_noIso_WP80.clear();
  electron_passMVAID_noIso_WP90.clear();
  electron_passMVAID_iso_WP80.clear();
  electron_passMVAID_iso_WP90.clear();
  electron_passHEEPID.clear();
  electron_EnergyUnCorr.clear();
  electron_mva.clear();
  electron_zzmva.clear();
  electron_chMiniIso.clear();
  electron_nhMiniIso.clear();
  electron_phMiniIso.clear();
  electron_puChMiniIso.clear();

  //==== Muon
  muon_PfChargedHadronIsoR05.clear();
  muon_PfNeutralHadronIsoR05.clear();
  muon_PfGammaIsoR05.clear();
  muon_PfChargedHadronIsoR04.clear();
  muon_PfNeutralHadronIsoR04.clear();
  muon_PfGammaIsoR04.clear();
  muon_PFSumPUIsoR04.clear();
  muon_PfChargedHadronIsoR03.clear();
  muon_PfNeutralHadronIsoR03.clear();
  muon_PfGammaIsoR03.clear();
  muon_PFSumPUIsoR03.clear();
  muon_isPF.clear();
  muon_isGlobal.clear();
  muon_isTracker.clear();
  muon_isStandAlone.clear();
  muon_isTight.clear();
  muon_isMedium.clear();
  muon_isLoose.clear();
  muon_isSoft.clear();
  muon_isHighPt.clear();
  muon_dB.clear();
  muon_phi.clear();
  muon_eta.clear();
  muon_pt.clear();
  muon_mass.clear();
  muon_cktpt.clear();
  muon_cktPx.clear();
  muon_cktPy.clear();
  muon_cktPz.clear();
  muon_cktptError.clear();
  muon_Px.clear();
  muon_Py.clear();
  muon_Pz.clear();
  muon_sumtrkpt.clear();
  muon_trkiso.clear();
  muon_hcaliso.clear();
  muon_ecaliso.clear();
  muon_trkisoR05.clear();
  muon_hcalisoR05.clear();
  muon_ecalisoR05.clear();
  muon_charge.clear();
  muon_nChambers.clear();
  muon_matchedstations.clear();
  muon_stationMask.clear();
  muon_nSegments.clear();
  muon_normchi.clear();
  muon_validhits.clear();
  muon_trackerHits.clear();
  muon_pixelHits.clear();
  muon_validmuonhits.clear();
  muon_trackerLayers.clear();
  muon_qoverp.clear();
  muon_theta.clear();
  muon_lambda.clear();
  muon_dxy.clear();
  muon_d0.clear();
  muon_dsz.clear();
  muon_dz.clear();
  muon_dxyBS.clear();
  muon_dzBS.clear();
  muon_dszBS.clear();
  muon_dxyVTX.clear();
  muon_dxyerrVTX.clear();
  muon_dzVTX.clear();
  muon_dzerrVTX.clear();
  muon_3DIPVTX.clear();
  muon_3DIPerrVTX.clear();
  muon_dszVTX.clear();
  muon_dxycktVTX.clear();
  muon_dzcktVTX.clear();
  muon_dszcktVTX.clear();
  muon_vx.clear();
  muon_vy.clear();
  muon_vz.clear();
  muon_Best_pt.clear();
  muon_Best_ptError.clear();
  muon_Best_Px.clear();
  muon_Best_Py.clear();
  muon_Best_Pz.clear();
  muon_Best_eta.clear();
  muon_Best_phi.clear();
  muon_Inner_pt.clear();
  muon_Inner_ptError.clear();
  muon_Inner_Px.clear();
  muon_Inner_Py.clear();
  muon_Inner_Pz.clear();
  muon_Inner_eta.clear();
  muon_Inner_phi.clear();
  muon_Outer_pt.clear();
  muon_Outer_ptError.clear();
  muon_Outer_Px.clear();
  muon_Outer_Py.clear();
  muon_Outer_Pz.clear();
  muon_Outer_eta.clear();
  muon_Outer_phi.clear();
  muon_GLB_pt.clear();
  muon_GLB_ptError.clear();
  muon_GLB_Px.clear();
  muon_GLB_Py.clear();
  muon_GLB_Pz.clear();
  muon_GLB_eta.clear();
  muon_GLB_phi.clear();
  muon_TuneP_pt.clear();
  muon_TuneP_ptError.clear();
  muon_TuneP_Px.clear();
  muon_TuneP_Py.clear();
  muon_TuneP_Pz.clear();
  muon_TuneP_eta.clear();
  muon_TuneP_phi.clear();
  muon_roch_sf.clear();
  muon_roch_sf_up.clear();
  muon_PfChargedHadronMiniIso.clear();
  muon_PfNeutralHadronMiniIso.clear();
  muon_PfGammaMiniIso.clear();
  muon_PFSumPUMiniIso.clear();

  //==== Jet
  jet_pt.clear();
  jet_eta.clear();
  jet_phi.clear();
  jet_charge.clear();
  jet_area.clear();
  jet_partonFlavour.clear();
  jet_hadronFlavour.clear();
  jet_CSVv2.clear();
  jet_DeepCSV.clear();
  jet_CvsL.clear();
  jet_CvsB.clear();
  jet_DeepFlavour_b.clear();
  jet_DeepFlavour_bb.clear();
  jet_DeepFlavour_lepb.clear();
  jet_DeepFlavour_c.clear();
  jet_DeepFlavour_uds.clear();
  jet_DeepFlavour_g.clear();
  jet_DeepCvsL.clear();
  jet_DeepCvsB.clear();
  jet_chargedHadronEnergyFraction.clear();
  jet_neutralHadronEnergyFraction.clear();
  jet_neutralEmEnergyFraction.clear();
  jet_chargedEmEnergyFraction.clear();
  jet_chargedMultiplicity.clear();
  jet_neutralMultiplicity.clear();
  jet_tightJetID.clear();
  jet_tightLepVetoJetID.clear();
  jet_partonPdgId.clear();
  jet_vtxNtracks.clear();
  jet_m.clear();
  jet_energy.clear();
  jet_PileupJetId.clear();
  jet_shiftedEnUp.clear();
  jet_shiftedEnDown.clear();
  jet_smearedRes.clear();
  jet_smearedResUp.clear();
  jet_smearedResDown.clear();
  jet_JECL1FastJet.clear();
  jet_JECFull.clear();

  //==== FatJet
  fatjet_pt.clear();
  fatjet_eta.clear();
  fatjet_phi.clear();
  fatjet_charge.clear();
  fatjet_area.clear();
  fatjet_partonFlavour.clear();
  fatjet_hadronFlavour.clear();
  fatjet_CSVv2.clear();
  fatjet_DeepCSV.clear();
  fatjet_CvsL.clear();
  fatjet_CvsB.clear();
  fatjet_DeepFlavour_b.clear();
  fatjet_DeepFlavour_bb.clear();
  fatjet_DeepFlavour_lepb.clear();
  fatjet_DeepFlavour_c.clear();
  fatjet_DeepFlavour_uds.clear();
  fatjet_DeepFlavour_g.clear();
  fatjet_DeepCvsL.clear();
  fatjet_DeepCvsB.clear();
  fatjet_tightJetID.clear();
  fatjet_tightLepVetoJetID.clear();
  fatjet_partonPdgId.clear();
  fatjet_vtxNtracks.clear();
  fatjet_m.clear();
  fatjet_energy.clear();
  fatjet_puppi_tau1.clear();
  fatjet_puppi_tau2.clear();
  fatjet_puppi_tau3.clear();
  fatjet_puppi_tau4.clear();
  fatjet_softdropmass.clear();
  fatjet_chargedHadronEnergyFraction.clear();
  fatjet_neutralHadronEnergyFraction.clear();
  fatjet_neutralEmEnergyFraction.clear();
  fatjet_chargedEmEnergyFraction.clear();
  fatjet_chargedMultiplicity.clear();
  fatjet_neutralMultiplicity.clear();
  fatjet_shiftedEnUp.clear();
  fatjet_shiftedEnDown.clear();
  fatjet_smearedRes.clear();
  fatjet_smearedResUp.clear();
  fatjet_smearedResDown.clear();

  //==== Photon
  photon_pt.clear();
  photon_eta.clear();
  photon_phi.clear();
  photon_scEta.clear();
  photon_scPhi.clear();
  photon_HoverE.clear();
  photon_hasPixelSeed.clear();
  photon_Full5x5_SigmaIEtaIEta.clear();
  photon_ChIso.clear();
  photon_NhIso.clear();
  photon_PhIso.clear();
  photon_ChIsoWithEA.clear();
  photon_NhIsoWithEA.clear();
  photon_PhIsoWithEA.clear();
  photon_passMVAID_WP80.clear();
  photon_passMVAID_WP90.clear();
  photon_passLooseID.clear();
  photon_passMediumID.clear();
  photon_passTightID.clear();
  photon_ptUnCorr.clear();

  // cout << "[SKFlatMaker::analyze] Varialbe intilization done" << endl;
  
  nEvt++;
  // -- run number & event number -- //
  runNum = iEvent.id().run();
  evtNum = iEvent.id().event();
  lumiBlock = iEvent.id().luminosityBlock();
  
  // edm::Handle<double> weight_;
  // iEvent.getByLabel("PUweight", weight_);
  
  // if(weight_.isValid())
  //   PUweight = *weight_;
  // else
  //   PUweight = 1.0;
  
  //get the geometry
  edm::ESHandle<GlobalTrackingGeometry> glbTrackingGeometry;
  iSetup.get<GlobalTrackingGeometryRecord>().get(glbTrackingGeometry);
  
  // -- PileUp Reweighting -- //
  IsData = iEvent.isRealData();
  if( !IsData && DoPileUp ){
    edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
    iEvent.getByToken(PileUpInfoToken, PupInfo);
    std::vector<PileupSummaryInfo>::const_iterator PVI;

    int npv = -1;
    //int npvin = -1;
      
    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
      int BX = PVI->getBunchCrossing();
    
      if( BX == 0 ){
        // npvin = PVI->getPU_NumInteractions(); // in time only
        npv = PVI->getTrueNumInteractions(); // in and out of time
        continue;
      }
    
    }
      
    nPileUp = npv;
      
  }

  //==================
  //==== Prepare JEC
  //==================

  //==== 1) AK4

  edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
  iSetup.get<JetCorrectionsRecord>().get(jet_payloadName_,JetCorParColl);
  JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];
  if(jet_jecUnc) delete jet_jecUnc;
  jet_jecUnc = new JetCorrectionUncertainty(JetCorPar);

  //==== For cross check
  //==== https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
  //if(jet_jecUnc_methodB) delete jet_jecUnc_methodB;
  //jet_jecUnc_methodB = new JetCorrectionUncertainty("/u/user/jskim/scratch/CMSSW_9_4_2/src/SKFlatMaker/SKFlatMaker/data/JEC/MC/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_Uncertainty_AK4PFchs.txt");

  //==== 2) AK8

  edm::ESHandle<JetCorrectorParametersCollection> FatJetCorParColl;
  iSetup.get<JetCorrectionsRecord>().get(fatjet_payloadName_,FatJetCorParColl);
  JetCorrectorParameters const & FatJetCorPar = (*FatJetCorParColl)["Uncertainty"];
  if(fatjet_jecUnc) delete fatjet_jecUnc;
  fatjet_jecUnc = new JetCorrectionUncertainty(FatJetCorPar);

  //==== For JER, get genJets
  iEvent.getByToken(genJetToken, m_genJets);
  iEvent.getByToken(genFatJetToken, m_genFatJets);

  //==== Event varialbes
  edm::Handle< double > rhoHandle;
  iEvent.getByToken(RhoToken,rhoHandle);
  Rho = *rhoHandle;

  // fills
  
  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreHLTReportFlag" << endl;
  if( theStoreHLTReportFlag ) hltReport(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStorePriVtxFlag" << endl;
  if( theStorePriVtxFlag ) fillPrimaryVertex(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreJetFlag" << endl;
  if( theStoreJetFlag ) fillJet(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreFatJetFlag" << endl;
  if( theStoreFatJetFlag ) fillFatJet(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreMETFlag" << endl;
  if( theStoreMETFlag ) fillMET(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreLHEFlag" << endl;
  //if( !IsData && theStoreLHEFlag ) fillLHEInfo(iEvent);
  if(theStoreLHEFlag ) fillLHEInfo(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreGENFlag" << endl;
  //if( !IsData && theStoreGENFlag ) fillGENInfo(iEvent);
  if(theStoreGENFlag ) fillGENInfo(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStorePhotonFlag" << endl;
  if( theStorePhotonFlag ) fillPhotons(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreMuonFlag" << endl;
  if( theStoreMuonFlag ) fillMuons(iEvent, iSetup);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreElectronFlag" << endl;
  if( theStoreElectronFlag ) fillElectrons(iEvent, iSetup);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] theStoreTTFlag" << endl;
  if( theStoreTTFlag ) fillTT(iEvent);

  if(theDebugLevel) cout << "[SKFlatMaker::analyze] Tree Fill" << endl;
  DYTree->Fill();
  if(theDebugLevel) cout << "[SKFlatMaker::analyze] Tree Fill finished" << endl;

}

// ------------ method called once each job just before starting event loop  ------------ //
void SKFlatMaker::beginJob()
{

  if(theDebugLevel) cout << "[SKFlatMaker::beginJob] called" << endl;
  // if( DoPileUp )
  // {
  // // Pileup Reweight: 2012, Summer12_S10
  // std::vector< float > _PUreweightRun2012 ;
  // std::vector< float > _PUreweightRun2012MuonPhys ;
  // std::vector< float > _MC2012;

  // for( int i = 0; i < 100; ++i)
  // {
  // _PUreweightRun2012.push_back((float)PileUpRD_[i]);
  // _PUreweightRun2012MuonPhys.push_back((float)PileUpRDMuonPhys_[i]);
  // _MC2012.push_back((float)PileUpMC_[i]);
  // }

  // LumiWeights_ = edm::LumiReWeighting(_MC2012, _PUreweightRun2012);
  // PShiftDown_ = reweight::PoissonMeanShifter(-0.5);
  // PShiftUp_ = reweight::PoissonMeanShifter(0.5);

  // LumiWeightsMuonPhys_ = edm::LumiReWeighting(_MC2012, _PUreweightRun2012MuonPhys);
  // PShiftDownMuonPhys_ = reweight::PoissonMeanShifter(-0.5);
  // PShiftUpMuonPhys_ = reweight::PoissonMeanShifter(0.5);
  // }

  edm::Service<TFileService> fs;
  DYTree = fs->make<TTree>("SKFlat","SKFlat");

  // -- global event variables -- //
  DYTree->Branch("IsData",&IsData,"IsData/O");
  DYTree->Branch("nTotal",&nEvt,"nTotal/I");
  DYTree->Branch("run",&runNum,"runNum/I");
  DYTree->Branch("event",&evtNum,"evtNum/l");
  DYTree->Branch("lumi",&lumiBlock,"lumiBlock/I");
  DYTree->Branch("PUweight",&PUweight,"PUweight/D");
  // DYTree->Branch("sumEt",&sumEt,"sumEt/D");
  // DYTree->Branch("photonEt",&photonEt,"photonEt/D");
  // DYTree->Branch("chargedHadronEt",&chargedHadronEt,"chargedHadronEt/D");
  // DYTree->Branch("neutralHadronEt",&neutralHadronEt,"neutralHadronEt/D");
  DYTree->Branch("Rho",&Rho,"Rho/D");
  DYTree->Branch("nPV",&nPV,"nPV/I");
  
  //MET Filters 2017
  DYTree->Branch("Flag_goodVertices",&Flag_goodVertices,"Flag_goodVertices/O");
  DYTree->Branch("Flag_globalTightHalo2016Filter",&Flag_globalTightHalo2016Filter,"Flag_globalTightHalo2016Filter/O");
  DYTree->Branch("Flag_globalSuperTightHalo2016Filter",&Flag_globalSuperTightHalo2016Filter,"Flag_globalSuperTightHalo2016Filter/O");
  DYTree->Branch("Flag_HBHENoiseFilter",&Flag_HBHENoiseFilter,"Flag_HBHENoiseFilter/O");
  DYTree->Branch("Flag_HBHENoiseIsoFilter",&Flag_HBHENoiseIsoFilter,"Flag_HBHENoiseIsoFilter/O");
  DYTree->Branch("Flag_EcalDeadCellTriggerPrimitiveFilter",&Flag_EcalDeadCellTriggerPrimitiveFilter,"Flag_EcalDeadCellTriggerPrimitiveFilter/O");
  DYTree->Branch("Flag_BadPFMuonFilter",&Flag_BadPFMuonFilter,"Flag_BadPFMuonFilter/O");
  DYTree->Branch("Flag_BadChargedCandidateFilter",&Flag_BadChargedCandidateFilter,"Flag_BadChargedCandidateFilter/O");
  DYTree->Branch("Flag_eeBadScFilter",&Flag_eeBadScFilter,"Flag_eeBadScFilter/O");
  DYTree->Branch("Flag_ecalBadCalibFilter",&Flag_ecalBadCalibFilter,"Flag_ecalBadCalibFilter/O");

  
  
  if(theStorePriVtxFlag){
    DYTree->Branch("PVtrackSize", &PVtrackSize,"PVtrackSize/I");
    DYTree->Branch("PVchi2", &PVchi2,"PVchi2/D");
    DYTree->Branch("PVndof", &PVndof,"PVndof/D");
    DYTree->Branch("PVnormalizedChi2", &PVnormalizedChi2,"PVnormalizedChi2/D");
    DYTree->Branch("vertex_X", &PVx,"PVx/D");
    DYTree->Branch("vertex_Y", &PVy,"PVy/D");
    DYTree->Branch("vertex_Z", &PVz,"PVz/D");
  }

  if(theStoreHLTReportFlag){

    DYTree->Branch("HLT_TriggerName", "vector<string>", &HLT_TriggerName);
    DYTree->Branch("HLT_TriggerFired", "vector<bool>", &HLT_TriggerFired);
    DYTree->Branch("HLT_TriggerPrescale", "vector<int>", &HLT_TriggerPrescale);

    if(theStoreHLTObjectFlag){
      DYTree->Branch("HLT_TriggerFilterName", "vector<string>", &HLT_TriggerFilterName);
      DYTree->Branch("HLTObject_pt", "vector<double>", &HLTObject_pt);
      DYTree->Branch("HLTObject_eta", "vector<double>", &HLTObject_eta);
      DYTree->Branch("HLTObject_phi", "vector<double>", &HLTObject_phi);
      DYTree->Branch("HLTObject_FiredFilters", "vector<string>", &HLTObject_FiredFilters);
      DYTree->Branch("HLTObject_FiredPaths", "vector<string>", &HLTObject_FiredPaths);
    }

  }

  if(theStoreJetFlag){

    DYTree->Branch("jet_pt", "vector<double>", &jet_pt);
    DYTree->Branch("jet_eta", "vector<double>", &jet_eta);
    DYTree->Branch("jet_phi", "vector<double>", &jet_phi);
    DYTree->Branch("jet_charge", "vector<double>", &jet_charge);
    DYTree->Branch("jet_area", "vector<double>", &jet_area);
    DYTree->Branch("jet_partonFlavour", "vector<int>", &jet_partonFlavour);
    DYTree->Branch("jet_hadronFlavour", "vector<int>", &jet_hadronFlavour);
    DYTree->Branch("jet_CSVv2", "vector<double>", &jet_CSVv2);
    DYTree->Branch("jet_DeepCSV", "vector<double>", &jet_DeepCSV);
    DYTree->Branch("jet_CvsL", "vector<double>", &jet_CvsL);
    DYTree->Branch("jet_CvsB", "vector<double>", &jet_CvsB);
    DYTree->Branch("jet_DeepFlavour_b", "vector<double>", &jet_DeepFlavour_b);
    DYTree->Branch("jet_DeepFlavour_bb", "vector<double>", &jet_DeepFlavour_bb);
    DYTree->Branch("jet_DeepFlavour_lepb", "vector<double>", &jet_DeepFlavour_lepb);
    DYTree->Branch("jet_DeepFlavour_c", "vector<double>", &jet_DeepFlavour_c);
    DYTree->Branch("jet_DeepFlavour_uds", "vector<double>", &jet_DeepFlavour_uds);
    DYTree->Branch("jet_DeepFlavour_g", "vector<double>", &jet_DeepFlavour_g);
    DYTree->Branch("jet_DeepCvsL", "vector<double>", &jet_DeepCvsL);
    DYTree->Branch("jet_DeepCvsB", "vector<double>", &jet_DeepCvsB);
    DYTree->Branch("jet_chargedHadronEnergyFraction", "vector<double>", &jet_chargedHadronEnergyFraction);
    DYTree->Branch("jet_neutralHadronEnergyFraction", "vector<double>", &jet_neutralHadronEnergyFraction);
    DYTree->Branch("jet_neutralEmEnergyFraction", "vector<double>", &jet_neutralEmEnergyFraction);
    DYTree->Branch("jet_chargedEmEnergyFraction", "vector<double>", &jet_chargedEmEnergyFraction);
    DYTree->Branch("jet_chargedMultiplicity", "vector<int>", &jet_chargedMultiplicity);
    DYTree->Branch("jet_neutralMultiplicity", "vector<int>", &jet_neutralMultiplicity);
    DYTree->Branch("jet_tightJetID", "vector<bool>", &jet_tightJetID);
    DYTree->Branch("jet_tightLepVetoJetID", "vector<bool>", &jet_tightLepVetoJetID);
    DYTree->Branch("jet_partonPdgId", "vector<int>", &jet_partonPdgId);
    DYTree->Branch("jet_vtxNtracks", "vector<int>", &jet_vtxNtracks);
    DYTree->Branch("jet_m", "vector<double>", &jet_m);
    DYTree->Branch("jet_energy", "vector<double>", &jet_energy);
    DYTree->Branch("jet_PileupJetId", "vector<double>", &jet_PileupJetId);
    DYTree->Branch("jet_shiftedEnUp", "vector<double>", &jet_shiftedEnUp);
    DYTree->Branch("jet_shiftedEnDown", "vector<double>", &jet_shiftedEnDown);
    DYTree->Branch("jet_smearedRes", "vector<double>", &jet_smearedRes);
    DYTree->Branch("jet_smearedResUp", "vector<double>", &jet_smearedResUp);
    DYTree->Branch("jet_smearedResDown", "vector<double>", &jet_smearedResDown);
    DYTree->Branch("jet_JECL1FastJet", "vector<double>", &jet_JECL1FastJet);
    DYTree->Branch("jet_JECFull", "vector<double>", &jet_JECFull);

  }
  
  if(theStoreFatJetFlag){
    DYTree->Branch("fatjet_pt", "vector<double>", &fatjet_pt);
    DYTree->Branch("fatjet_eta", "vector<double>", &fatjet_eta);
    DYTree->Branch("fatjet_phi", "vector<double>", &fatjet_phi);
    DYTree->Branch("fatjet_charge", "vector<double>", &fatjet_charge);
    DYTree->Branch("fatjet_area", "vector<double>", &fatjet_area);
    DYTree->Branch("fatjet_partonFlavour", "vector<int>", &fatjet_partonFlavour);
    DYTree->Branch("fatjet_hadronFlavour", "vector<int>", &fatjet_hadronFlavour);
    DYTree->Branch("fatjet_CSVv2", "vector<double>", &fatjet_CSVv2);
    DYTree->Branch("fatjet_DeepCSV", "vector<double>", &fatjet_DeepCSV);
    DYTree->Branch("fatjet_DeepFlavour_b", "vector<double>", &fatjet_DeepFlavour_b);
    DYTree->Branch("fatjet_DeepFlavour_bb", "vector<double>", &fatjet_DeepFlavour_bb);
    DYTree->Branch("fatjet_DeepFlavour_lepb", "vector<double>", &fatjet_DeepFlavour_lepb);
    DYTree->Branch("fatjet_DeepFlavour_c", "vector<double>", &fatjet_DeepFlavour_c);
    DYTree->Branch("fatjet_DeepFlavour_uds", "vector<double>", &fatjet_DeepFlavour_uds);
    DYTree->Branch("fatjet_DeepFlavour_g", "vector<double>", &fatjet_DeepFlavour_g);
    DYTree->Branch("fatjet_CvsL", "vector<double>", &fatjet_CvsL);
    DYTree->Branch("fatjet_CvsB", "vector<double>", &fatjet_CvsB);
    DYTree->Branch("fatjet_DeepCvsL", "vector<double>", &fatjet_DeepCvsL);
    DYTree->Branch("fatjet_DeepCvsB", "vector<double>", &fatjet_DeepCvsB);
    DYTree->Branch("fatjet_tightJetID", "vector<bool>", &fatjet_tightJetID);
    DYTree->Branch("fatjet_tightLepVetoJetID", "vector<bool>", &fatjet_tightLepVetoJetID);
    DYTree->Branch("fatjet_partonPdgId", "vector<int>", &fatjet_partonPdgId);
    DYTree->Branch("fatjet_vtxNtracks", "vector<int>", &fatjet_vtxNtracks);
    DYTree->Branch("fatjet_m", "vector<double>", &fatjet_m);
    DYTree->Branch("fatjet_energy", "vector<double>", &fatjet_energy);
    DYTree->Branch("fatjet_puppi_tau1", "vector<double>", &fatjet_puppi_tau1);
    DYTree->Branch("fatjet_puppi_tau2", "vector<double>", &fatjet_puppi_tau2);
    DYTree->Branch("fatjet_puppi_tau3", "vector<double>", &fatjet_puppi_tau3);
    DYTree->Branch("fatjet_puppi_tau4", "vector<double>", &fatjet_puppi_tau4);
    DYTree->Branch("fatjet_softdropmass", "vector<double>", &fatjet_softdropmass);
    DYTree->Branch("fatjet_chargedHadronEnergyFraction", "vector<double>", &fatjet_chargedHadronEnergyFraction);
    DYTree->Branch("fatjet_neutralHadronEnergyFraction", "vector<double>", &fatjet_neutralHadronEnergyFraction);
    DYTree->Branch("fatjet_neutralEmEnergyFraction", "vector<double>", &fatjet_neutralEmEnergyFraction);
    DYTree->Branch("fatjet_chargedEmEnergyFraction", "vector<double>", &fatjet_chargedEmEnergyFraction);
    DYTree->Branch("fatjet_chargedMultiplicity", "vector<int>", &fatjet_chargedMultiplicity);
    DYTree->Branch("fatjet_neutralMultiplicity", "vector<int>", &fatjet_neutralMultiplicity);
    DYTree->Branch("fatjet_shiftedEnUp", "vector<double>", &fatjet_shiftedEnUp);
    DYTree->Branch("fatjet_shiftedEnDown", "vector<double>", &fatjet_shiftedEnDown);
    DYTree->Branch("fatjet_smearedRes", "vector<double>", &fatjet_smearedRes);
    DYTree->Branch("fatjet_smearedResUp", "vector<double>", &fatjet_smearedResUp);
    DYTree->Branch("fatjet_smearedResDown", "vector<double>", &fatjet_smearedResDown);
  }

  // Electron
  if( theStoreElectronFlag ){
    DYTree->Branch("electron_MVAIso", "vector<double>", &electron_MVAIso);
    DYTree->Branch("electron_MVANoIso", "vector<double>", &electron_MVANoIso);
    DYTree->Branch("electron_et", "vector<double>", &electron_et);
    DYTree->Branch("electron_caloEnergy", "vector<double>", &electron_caloEnergy);
    DYTree->Branch("electron_Energy", "vector<double>", &electron_Energy);
    DYTree->Branch("electron_Energy_Scale_Up", "vector<double>", &electron_Energy_Scale_Up);
    DYTree->Branch("electron_Energy_Scale_Down", "vector<double>", &electron_Energy_Scale_Down);
    DYTree->Branch("electron_Energy_Smear_Up", "vector<double>", &electron_Energy_Smear_Up);
    DYTree->Branch("electron_Energy_Smear_Down", "vector<double>", &electron_Energy_Smear_Down);
    DYTree->Branch("electron_pt", "vector<double>", &electron_pt);
    DYTree->Branch("electron_pt_Scale_Up", "vector<double>", &electron_pt_Scale_Up);
    DYTree->Branch("electron_pt_Scale_Down", "vector<double>", &electron_pt_Scale_Down);
    DYTree->Branch("electron_pt_Smear_Up", "vector<double>", &electron_pt_Smear_Up);
    DYTree->Branch("electron_pt_Smear_Down", "vector<double>", &electron_pt_Smear_Down);
    DYTree->Branch("electron_Px", "vector<double>", &electron_Px);
    DYTree->Branch("electron_Py", "vector<double>", &electron_Py);
    DYTree->Branch("electron_Pz", "vector<double>", &electron_Pz);
    DYTree->Branch("electron_eta", "vector<double>", &electron_eta);
    DYTree->Branch("electron_phi", "vector<double>", &electron_phi);
    DYTree->Branch("electron_charge", "vector<int>", &electron_charge);
    DYTree->Branch("electron_gsfpt", "vector<double>", &electron_gsfpt);
    DYTree->Branch("electron_gsfPx", "vector<double>", &electron_gsfPx);
    DYTree->Branch("electron_gsfPy", "vector<double>", &electron_gsfPy);
    DYTree->Branch("electron_gsfPz", "vector<double>", &electron_gsfPz);
    DYTree->Branch("electron_gsfEta", "vector<double>", &electron_gsfEta);
    DYTree->Branch("electron_gsfPhi", "vector<double>", &electron_gsfPhi);
    DYTree->Branch("electron_gsfCharge", "vector<int>", &electron_gsfCharge);
    DYTree->Branch("electron_scEta", "vector<double>", &electron_scEta);
    DYTree->Branch("electron_scPhi", "vector<double>", &electron_scPhi);
    DYTree->Branch("electron_etaWidth", "vector<double>", &electron_etaWidth);
    DYTree->Branch("electron_phiWidth", "vector<double>", &electron_phiWidth);
    DYTree->Branch("electron_dEtaIn", "vector<double>", &electron_dEtaIn);
    DYTree->Branch("electron_dEtaInSeed", "vector<double>", &electron_dEtaInSeed);
    DYTree->Branch("electron_dPhiIn", "vector<double>", &electron_dPhiIn);
    DYTree->Branch("electron_sigmaIEtaIEta", "vector<double>", &electron_sigmaIEtaIEta);
    DYTree->Branch("electron_Full5x5_SigmaIEtaIEta", "vector<double>", &electron_Full5x5_SigmaIEtaIEta);
    DYTree->Branch("electron_HoverE", "vector<double>", &electron_HoverE);
    DYTree->Branch("electron_fbrem", "vector<double>", &electron_fbrem);
    DYTree->Branch("electron_eOverP", "vector<double>", &electron_eOverP);
    DYTree->Branch("electron_energyEC", "vector<double>", &electron_energyEC);
    DYTree->Branch("electron_Pnorm", "vector<double>", &electron_Pnorm);
    DYTree->Branch("electron_InvEminusInvP", "vector<double>", &electron_InvEminusInvP);
    DYTree->Branch("electron_dxyVTX", "vector<double>", &electron_dxyVTX);
    DYTree->Branch("electron_dxyerrVTX", "vector<double>", &electron_dxyerrVTX);
    DYTree->Branch("electron_dzVTX", "vector<double>", &electron_dzVTX);
    DYTree->Branch("electron_dzerrVTX", "vector<double>", &electron_dzerrVTX);
    DYTree->Branch("electron_3DIPVTX", "vector<double>", &electron_3DIPVTX);
    DYTree->Branch("electron_3DIPerrVTX", "vector<double>", &electron_3DIPerrVTX);
    DYTree->Branch("electron_dxy", "vector<double>", &electron_dxy);
    DYTree->Branch("electron_sigdxy", "vector<double>", &electron_sigdxy);
    DYTree->Branch("electron_dz", "vector<double>", &electron_dz);
    DYTree->Branch("electron_dxyBS", "vector<double>", &electron_dxyBS);
    DYTree->Branch("electron_dzBS", "vector<double>", &electron_dzBS);
    DYTree->Branch("electron_AEff03", "vector<double>", &electron_AEff03);
    DYTree->Branch("electron_chIso03", "vector<double>", &electron_chIso03);
    DYTree->Branch("electron_nhIso03", "vector<double>", &electron_nhIso03);
    DYTree->Branch("electron_phIso03", "vector<double>", &electron_phIso03);
    DYTree->Branch("electron_pcIso03", "vector<double>", &electron_pcIso03);
    DYTree->Branch("electron_puChIso03", "vector<double>", &electron_puChIso03);
    DYTree->Branch("electron_chIso04", "vector<double>", &electron_chIso04);
    DYTree->Branch("electron_nhIso04", "vector<double>", &electron_nhIso04);
    DYTree->Branch("electron_phIso04", "vector<double>", &electron_phIso04);
    DYTree->Branch("electron_pcIso04", "vector<double>", &electron_pcIso04);
    DYTree->Branch("electron_puChIso04", "vector<double>", &electron_puChIso04);
    DYTree->Branch("electron_relIsoCom03", "vector<double>", &electron_relIsoCom03);
    DYTree->Branch("electron_relIsoCom04", "vector<double>", &electron_relIsoCom04);
    DYTree->Branch("electron_relIsoBeta03", "vector<double>", &electron_relIsoBeta03);
    DYTree->Branch("electron_relIsoBeta04", "vector<double>", &electron_relIsoBeta04);
    DYTree->Branch("electron_relIsoRho03", "vector<double>", &electron_relIsoRho03);
    DYTree->Branch("electron_passConversionVeto", "vector<bool>", &electron_passConversionVeto);
    DYTree->Branch("electron_isGsfCtfScPixChargeConsistent", "vector<bool>", &electron_isGsfCtfScPixChargeConsistent);
    DYTree->Branch("electron_isGsfScPixChargeConsistent", "vector<bool>", &electron_isGsfScPixChargeConsistent);
    DYTree->Branch("electron_isGsfCtfChargeConsistent", "vector<bool>", &electron_isGsfCtfChargeConsistent);
    DYTree->Branch("electron_mHits", "vector<int>", &electron_mHits);
    DYTree->Branch("electron_crack", "vector<int>", &electron_crack);
    DYTree->Branch("electron_ecalDriven", "vector<int>", &electron_ecalDriven);
    DYTree->Branch("electron_isoEMHADDepth1", "vector<double>", &electron_isoEMHADDepth1);
    DYTree->Branch("electron_25over55", "vector<double>", &electron_25over55);
    DYTree->Branch("electron_15over55", "vector<double>", &electron_15over55);
    DYTree->Branch("electron_isoHADDepth2", "vector<double>", &electron_isoHADDepth2);
    DYTree->Branch("electron_isoptTrks", "vector<double>", &electron_isoptTrks);
    DYTree->Branch("electron_modIsoEMHADDepth1", "vector<double>", &electron_modIsoEMHADDepth1);
    DYTree->Branch("electron_modIsoptTrks", "vector<double>", &electron_modIsoptTrks);
    DYTree->Branch("electron_modIsoEMHADDepth1Orig", "vector<double>", &electron_modIsoEMHADDepth1Orig);
    DYTree->Branch("electron_modIsoptTrksOrig", "vector<double>", &electron_modIsoptTrksOrig);
    DYTree->Branch("electron_ambGsf0pt", "vector<double>", &electron_ambGsf0pt);
    DYTree->Branch("electron_ambGsf0Eta", "vector<double>", &electron_ambGsf0Eta);
    DYTree->Branch("electron_ambGsf0Phi", "vector<double>", &electron_ambGsf0Phi);
    DYTree->Branch("electron_ambGsf0Charge", "vector<double>", &electron_ambGsf0Charge);
    DYTree->Branch("electron_ambGsf1pt", "vector<double>", &electron_ambGsf1pt);
    DYTree->Branch("electron_ambGsf1Eta", "vector<double>", &electron_ambGsf1Eta);
    DYTree->Branch("electron_ambGsf1Phi", "vector<double>", &electron_ambGsf1Phi);
    DYTree->Branch("electron_ambGsf1Charge", "vector<double>", &electron_ambGsf1Charge);
    DYTree->Branch("electron_ambGsf2pt", "vector<double>", &electron_ambGsf2pt);
    DYTree->Branch("electron_ambGsf2Eta", "vector<double>", &electron_ambGsf2Eta);
    DYTree->Branch("electron_ambGsf2Phi", "vector<double>", &electron_ambGsf2Phi);
    DYTree->Branch("electron_ambGsf2Charge", "vector<double>", &electron_ambGsf2Charge);
    DYTree->Branch("electron_ambGsf3pt", "vector<double>", &electron_ambGsf3pt);
    DYTree->Branch("electron_ambGsf3Eta", "vector<double>", &electron_ambGsf3Eta);
    DYTree->Branch("electron_ambGsf3Phi", "vector<double>", &electron_ambGsf3Phi);
    DYTree->Branch("electron_ambGsf3Charge", "vector<double>", &electron_ambGsf3Charge);
    DYTree->Branch("electron_r9", "vector<double>", &electron_r9);
    DYTree->Branch("electron_scEnergy", "vector<double>", &electron_scEnergy);
    DYTree->Branch("electron_scPreEnergy", "vector<double>", &electron_scPreEnergy);
    DYTree->Branch("electron_scRawEnergy", "vector<double>", &electron_scRawEnergy);
    DYTree->Branch("electron_scEt", "vector<double>", &electron_scEt);
    DYTree->Branch("electron_E15", "vector<double>", &electron_E15);
    DYTree->Branch("electron_E25", "vector<double>", &electron_E25);
    DYTree->Branch("electron_E55", "vector<double>", &electron_E55);
    DYTree->Branch("electron_RelPFIso_dBeta", "vector<double>", &electron_RelPFIso_dBeta);
    DYTree->Branch("electron_RelPFIso_Rho", "vector<double>", &electron_RelPFIso_Rho);
    DYTree->Branch("electron_passVetoID", "vector<bool>", &electron_passVetoID);
    DYTree->Branch("electron_passLooseID", "vector<bool>", &electron_passLooseID);
    DYTree->Branch("electron_passMediumID", "vector<bool>", &electron_passMediumID);
    DYTree->Branch("electron_passTightID", "vector<bool>", &electron_passTightID);
    DYTree->Branch("electron_passMVAID_noIso_WP80", "vector<bool>", &electron_passMVAID_noIso_WP80);
    DYTree->Branch("electron_passMVAID_noIso_WP90", "vector<bool>", &electron_passMVAID_noIso_WP90);
    DYTree->Branch("electron_passMVAID_iso_WP80", "vector<bool>", &electron_passMVAID_iso_WP80);
    DYTree->Branch("electron_passMVAID_iso_WP90", "vector<bool>", &electron_passMVAID_iso_WP90);
    DYTree->Branch("electron_passHEEPID", "vector<bool>", &electron_passHEEPID);
    DYTree->Branch("electron_EnergyUnCorr", "vector<double>", &electron_EnergyUnCorr);
    DYTree->Branch("electron_mva", "vector<double>", &electron_mva);
    DYTree->Branch("electron_zzmva", "vector<double>", &electron_zzmva);
    DYTree->Branch("electron_chMiniIso", "vector<double>", &electron_chMiniIso);
    DYTree->Branch("electron_nhMiniIso", "vector<double>", &electron_nhMiniIso);
    DYTree->Branch("electron_phMiniIso", "vector<double>", &electron_phMiniIso);
    DYTree->Branch("electron_puChMiniIso", "vector<double>", &electron_puChMiniIso);
  }
  
  // -- muon variables -- //
  if( theStoreMuonFlag ){

    DYTree->Branch("muon_PfChargedHadronIsoR05", "vector<double>", &muon_PfChargedHadronIsoR05);
    DYTree->Branch("muon_PfNeutralHadronIsoR05", "vector<double>", &muon_PfNeutralHadronIsoR05);
    DYTree->Branch("muon_PfGammaIsoR05", "vector<double>", &muon_PfGammaIsoR05);
    DYTree->Branch("muon_PfChargedHadronIsoR04", "vector<double>", &muon_PfChargedHadronIsoR04);
    DYTree->Branch("muon_PfNeutralHadronIsoR04", "vector<double>", &muon_PfNeutralHadronIsoR04);
    DYTree->Branch("muon_PfGammaIsoR04", "vector<double>", &muon_PfGammaIsoR04);
    DYTree->Branch("muon_PFSumPUIsoR04", "vector<double>", &muon_PFSumPUIsoR04);
    DYTree->Branch("muon_PfChargedHadronIsoR03", "vector<double>", &muon_PfChargedHadronIsoR03);
    DYTree->Branch("muon_PfNeutralHadronIsoR03", "vector<double>", &muon_PfNeutralHadronIsoR03);
    DYTree->Branch("muon_PfGammaIsoR03", "vector<double>", &muon_PfGammaIsoR03);
    DYTree->Branch("muon_PFSumPUIsoR03", "vector<double>", &muon_PFSumPUIsoR03);
    DYTree->Branch("muon_isPF", "vector<bool>", &muon_isPF);
    DYTree->Branch("muon_isGlobal", "vector<bool>", &muon_isGlobal);
    DYTree->Branch("muon_isTracker", "vector<bool>", &muon_isTracker);
    DYTree->Branch("muon_isStandAlone", "vector<bool>", &muon_isStandAlone);
    DYTree->Branch("muon_isTight", "vector<bool>", &muon_isTight);
    DYTree->Branch("muon_isMedium", "vector<bool>", &muon_isMedium);
    DYTree->Branch("muon_isLoose", "vector<bool>", &muon_isLoose);
    DYTree->Branch("muon_isSoft", "vector<bool>", &muon_isSoft);
    DYTree->Branch("muon_isHighPt", "vector<bool>", &muon_isHighPt);
    DYTree->Branch("muon_dB", "vector<double>", &muon_dB);
    DYTree->Branch("muon_phi", "vector<double>", &muon_phi);
    DYTree->Branch("muon_eta", "vector<double>", &muon_eta);
    DYTree->Branch("muon_pt", "vector<double>", &muon_pt);
    DYTree->Branch("muon_mass", "vector<double>", &muon_mass);
    DYTree->Branch("muon_cktpt", "vector<double>", &muon_cktpt);
    DYTree->Branch("muon_cktPx", "vector<double>", &muon_cktPx);
    DYTree->Branch("muon_cktPy", "vector<double>", &muon_cktPy);
    DYTree->Branch("muon_cktPz", "vector<double>", &muon_cktPz);
    DYTree->Branch("muon_cktptError", "vector<double>", &muon_cktptError);
    DYTree->Branch("muon_Px", "vector<double>", &muon_Px);
    DYTree->Branch("muon_Py", "vector<double>", &muon_Py);
    DYTree->Branch("muon_Pz", "vector<double>", &muon_Pz);
    DYTree->Branch("muon_sumtrkpt", "vector<double>", &muon_sumtrkpt);
    DYTree->Branch("muon_trkiso", "vector<double>", &muon_trkiso);
    DYTree->Branch("muon_hcaliso", "vector<double>", &muon_hcaliso);
    DYTree->Branch("muon_ecaliso", "vector<double>", &muon_ecaliso);
    DYTree->Branch("muon_trkisoR05", "vector<double>", &muon_trkisoR05);
    DYTree->Branch("muon_hcalisoR05", "vector<double>", &muon_hcalisoR05);
    DYTree->Branch("muon_ecalisoR05", "vector<double>", &muon_ecalisoR05);
    DYTree->Branch("muon_charge", "vector<int>", &muon_charge);
    DYTree->Branch("muon_nChambers", "vector<int>", &muon_nChambers);
    DYTree->Branch("muon_matchedstations", "vector<int>", &muon_matchedstations);
    DYTree->Branch("muon_stationMask", "vector<int>", &muon_stationMask);
    DYTree->Branch("muon_nSegments", "vector<int>", &muon_nSegments);
    DYTree->Branch("muon_normchi", "vector<double>", &muon_normchi);
    DYTree->Branch("muon_validhits", "vector<int>", &muon_validhits);
    DYTree->Branch("muon_trackerHits", "vector<int>", &muon_trackerHits);
    DYTree->Branch("muon_pixelHits", "vector<int>", &muon_pixelHits);
    DYTree->Branch("muon_validmuonhits", "vector<int>", &muon_validmuonhits);
    DYTree->Branch("muon_trackerLayers", "vector<int>", &muon_trackerLayers);
    DYTree->Branch("muon_qoverp", "vector<double>", &muon_qoverp);
    DYTree->Branch("muon_theta", "vector<double>", &muon_theta);
    DYTree->Branch("muon_lambda", "vector<double>", &muon_lambda);
    DYTree->Branch("muon_dxy", "vector<double>", &muon_dxy);
    DYTree->Branch("muon_d0", "vector<double>", &muon_d0);
    DYTree->Branch("muon_dsz", "vector<double>", &muon_dsz);
    DYTree->Branch("muon_dz", "vector<double>", &muon_dz);
    DYTree->Branch("muon_dxyBS", "vector<double>", &muon_dxyBS);
    DYTree->Branch("muon_dzBS", "vector<double>", &muon_dzBS);
    DYTree->Branch("muon_dszBS", "vector<double>", &muon_dszBS);
    DYTree->Branch("muon_dxyVTX", "vector<double>", &muon_dxyVTX);
    DYTree->Branch("muon_dxyerrVTX", "vector<double>", &muon_dxyerrVTX);
    DYTree->Branch("muon_dzVTX", "vector<double>", &muon_dzVTX);
    DYTree->Branch("muon_dzerrVTX", "vector<double>", &muon_dzerrVTX);
    DYTree->Branch("muon_3DIPVTX", "vector<double>", &muon_3DIPVTX);
    DYTree->Branch("muon_3DIPerrVTX", "vector<double>", &muon_3DIPerrVTX);
    DYTree->Branch("muon_dszVTX", "vector<double>", &muon_dszVTX);
    DYTree->Branch("muon_dxycktVTX", "vector<double>", &muon_dxycktVTX);
    DYTree->Branch("muon_dzcktVTX", "vector<double>", &muon_dzcktVTX);
    DYTree->Branch("muon_dszcktVTX", "vector<double>", &muon_dszcktVTX);
    DYTree->Branch("muon_vx", "vector<double>", &muon_vx);
    DYTree->Branch("muon_vy", "vector<double>", &muon_vy);
    DYTree->Branch("muon_vz", "vector<double>", &muon_vz);
    DYTree->Branch("muon_Best_pt", "vector<double>", &muon_Best_pt);
    DYTree->Branch("muon_Best_ptError", "vector<double>", &muon_Best_ptError);
    DYTree->Branch("muon_Best_Px", "vector<double>", &muon_Best_Px);
    DYTree->Branch("muon_Best_Py", "vector<double>", &muon_Best_Py);
    DYTree->Branch("muon_Best_Pz", "vector<double>", &muon_Best_Pz);
    DYTree->Branch("muon_Best_eta", "vector<double>", &muon_Best_eta);
    DYTree->Branch("muon_Best_phi", "vector<double>", &muon_Best_phi);
    DYTree->Branch("muon_Inner_pt", "vector<double>", &muon_Inner_pt);
    DYTree->Branch("muon_Inner_ptError", "vector<double>", &muon_Inner_ptError);
    DYTree->Branch("muon_Inner_Px", "vector<double>", &muon_Inner_Px);
    DYTree->Branch("muon_Inner_Py", "vector<double>", &muon_Inner_Py);
    DYTree->Branch("muon_Inner_Pz", "vector<double>", &muon_Inner_Pz);
    DYTree->Branch("muon_Inner_eta", "vector<double>", &muon_Inner_eta);
    DYTree->Branch("muon_Inner_phi", "vector<double>", &muon_Inner_phi);
    DYTree->Branch("muon_Outer_pt", "vector<double>", &muon_Outer_pt);
    DYTree->Branch("muon_Outer_ptError", "vector<double>", &muon_Outer_ptError);
    DYTree->Branch("muon_Outer_Px", "vector<double>", &muon_Outer_Px);
    DYTree->Branch("muon_Outer_Py", "vector<double>", &muon_Outer_Py);
    DYTree->Branch("muon_Outer_Pz", "vector<double>", &muon_Outer_Pz);
    DYTree->Branch("muon_Outer_eta", "vector<double>", &muon_Outer_eta);
    DYTree->Branch("muon_Outer_phi", "vector<double>", &muon_Outer_phi);
    DYTree->Branch("muon_GLB_pt", "vector<double>", &muon_GLB_pt);
    DYTree->Branch("muon_GLB_ptError", "vector<double>", &muon_GLB_ptError);
    DYTree->Branch("muon_GLB_Px", "vector<double>", &muon_GLB_Px);
    DYTree->Branch("muon_GLB_Py", "vector<double>", &muon_GLB_Py);
    DYTree->Branch("muon_GLB_Pz", "vector<double>", &muon_GLB_Pz);
    DYTree->Branch("muon_GLB_eta", "vector<double>", &muon_GLB_eta);
    DYTree->Branch("muon_GLB_phi", "vector<double>", &muon_GLB_phi);
    DYTree->Branch("muon_TuneP_pt", "vector<double>", &muon_TuneP_pt);
    DYTree->Branch("muon_TuneP_ptError", "vector<double>", &muon_TuneP_ptError);
    DYTree->Branch("muon_TuneP_Px", "vector<double>", &muon_TuneP_Px);
    DYTree->Branch("muon_TuneP_Py", "vector<double>", &muon_TuneP_Py);
    DYTree->Branch("muon_TuneP_Pz", "vector<double>", &muon_TuneP_Pz);
    DYTree->Branch("muon_TuneP_eta", "vector<double>", &muon_TuneP_eta);
    DYTree->Branch("muon_TuneP_phi", "vector<double>", &muon_TuneP_phi);
    DYTree->Branch("muon_roch_sf", "vector<double>", &muon_roch_sf);
    DYTree->Branch("muon_roch_sf_up", "vector<double>", &muon_roch_sf_up);
    DYTree->Branch("muon_PfChargedHadronMiniIso", "vector<double>", &muon_PfChargedHadronMiniIso);
    DYTree->Branch("muon_PfNeutralHadronMiniIso", "vector<double>", &muon_PfNeutralHadronMiniIso);
    DYTree->Branch("muon_PfGammaMiniIso", "vector<double>", &muon_PfGammaMiniIso);
    DYTree->Branch("muon_PFSumPUMiniIso", "vector<double>", &muon_PFSumPUMiniIso);

  }
  
  // -- LHE info -- //
  if( theStoreLHEFlag ){
    DYTree->Branch("PDFWeights_Scale", "vector<double>", &PDFWeights_Scale);
    DYTree->Branch("PDFWeights_Error", "vector<double>", &PDFWeights_Error);
    DYTree->Branch("PDFWeights_AlphaS", "vector<double>", &PDFWeights_AlphaS);
  }
  
  // GEN info
  if( theStoreGENFlag ){
    DYTree->Branch("gen_phi", "vector<double>", &gen_phi);
    DYTree->Branch("gen_eta", "vector<double>", &gen_eta);
    DYTree->Branch("gen_pt", "vector<double>", &gen_pt);
    DYTree->Branch("gen_Px", "vector<double>", &gen_Px);
    DYTree->Branch("gen_Py", "vector<double>", &gen_Py);
    DYTree->Branch("gen_Pz", "vector<double>", &gen_Pz);
    DYTree->Branch("gen_E", "vector<double>", &gen_E);
    DYTree->Branch("gen_mother_PID", "vector<int>", &gen_mother_PID);
    DYTree->Branch("gen_mother_pt", "vector<double>", &gen_mother_pt);
    DYTree->Branch("gen_mother_index", "vector<int>", &gen_mother_index);
    DYTree->Branch("gen_charge", "vector<int>", &gen_charge);
    DYTree->Branch("gen_status", "vector<int>", &gen_status);
    DYTree->Branch("gen_PID", "vector<int>", &gen_PID);
    DYTree->Branch("gen_isPrompt", "vector<int>", &gen_isPrompt);
    DYTree->Branch("gen_isPromptFinalState", "vector<int>", &gen_isPromptFinalState);
    DYTree->Branch("gen_isTauDecayProduct", "vector<int>", &gen_isTauDecayProduct);
    DYTree->Branch("gen_isPromptTauDecayProduct", "vector<int>", &gen_isPromptTauDecayProduct);
    DYTree->Branch("gen_isDirectPromptTauDecayProductFinalState", "vector<int>", &gen_isDirectPromptTauDecayProductFinalState);
    DYTree->Branch("gen_isHardProcess", "vector<int>", &gen_isHardProcess);
    DYTree->Branch("gen_isLastCopy", "vector<int>", &gen_isLastCopy);
    DYTree->Branch("gen_isLastCopyBeforeFSR", "vector<int>", &gen_isLastCopyBeforeFSR);
    DYTree->Branch("gen_isPromptDecayed", "vector<int>", &gen_isPromptDecayed);
    DYTree->Branch("gen_isDecayedLeptonHadron", "vector<int>", &gen_isDecayedLeptonHadron);
    DYTree->Branch("gen_fromHardProcessBeforeFSR", "vector<int>", &gen_fromHardProcessBeforeFSR);
    DYTree->Branch("gen_fromHardProcessDecayed", "vector<int>", &gen_fromHardProcessDecayed);
    DYTree->Branch("gen_fromHardProcessFinalState", "vector<int>", &gen_fromHardProcessFinalState);
    DYTree->Branch("gen_isMostlyLikePythia6Status3", "vector<int>", &gen_isMostlyLikePythia6Status3);
    DYTree->Branch("gen_weight", &gen_weight, "gen_weight/D");
    DYTree->Branch("genWeight_Q", &genWeight_Q, "genWeight_Q/D");
    DYTree->Branch("genWeight_X1", &genWeight_X1, "genWeight_X1/D");
    DYTree->Branch("genWeight_X2", &genWeight_X2, "genWeight_X2/D");
    DYTree->Branch("genWeight_id1", &genWeight_id1, "genWeight_id1/I");
    DYTree->Branch("genWeight_id2", &genWeight_id2, "genWeight_id2/I");
    DYTree->Branch("genWeight_alphaQCD", &genWeight_alphaQCD, "genWeight_alphaQCD/D");
    DYTree->Branch("genWeight_alphaQED", &genWeight_alphaQED, "genWeight_alphaQED/D");
  }
  
  if( theStorePhotonFlag ){
    DYTree->Branch("photon_pt", "vector<double>", &photon_pt);
    DYTree->Branch("photon_eta", "vector<double>", &photon_eta);
    DYTree->Branch("photon_phi", "vector<double>", &photon_phi);
    DYTree->Branch("photon_scEta", "vector<double>", &photon_scEta);
    DYTree->Branch("photon_scPhi", "vector<double>", &photon_scPhi);
    DYTree->Branch("photon_HoverE", "vector<double>", &photon_HoverE);
    DYTree->Branch("photon_hasPixelSeed", "vector<int>", &photon_hasPixelSeed);
    DYTree->Branch("photon_Full5x5_SigmaIEtaIEta", "vector<double>", &photon_Full5x5_SigmaIEtaIEta);
    DYTree->Branch("photon_ChIso", "vector<double>", &photon_ChIso);
    DYTree->Branch("photon_NhIso", "vector<double>", &photon_NhIso);
    DYTree->Branch("photon_PhIso", "vector<double>", &photon_PhIso);
    DYTree->Branch("photon_ChIsoWithEA", "vector<double>", &photon_ChIsoWithEA);
    DYTree->Branch("photon_NhIsoWithEA", "vector<double>", &photon_NhIsoWithEA);
    DYTree->Branch("photon_PhIsoWithEA", "vector<double>", &photon_PhIsoWithEA);
    DYTree->Branch("photon_passMVAID_WP80", "vector<bool>", &photon_passMVAID_WP80);
    DYTree->Branch("photon_passMVAID_WP90", "vector<bool>", &photon_passMVAID_WP90);
    DYTree->Branch("photon_passLooseID", "vector<bool>", &photon_passLooseID);
    DYTree->Branch("photon_passMediumID", "vector<bool>", &photon_passMediumID);
    DYTree->Branch("photon_passTightID", "vector<bool>", &photon_passTightID);
    DYTree->Branch("photon_ptUnCorr", "vector<double>", &photon_ptUnCorr);
  }
  
  
  
  // Pile-up Reweight
  DYTree->Branch("nPileUp",&nPileUp,"nPileUp/I");
  DYTree->Branch("pileUpReweightIn",&pileUpReweightIn,"pileUpReweightIn/D");
  DYTree->Branch("pileUpReweight",&pileUpReweight,"pileUpReweight/D");
  DYTree->Branch("pileUpReweightPlus",&pileUpReweightPlus,"pileUpReweightPlus/D");
  DYTree->Branch("pileUpReweightMinus",&pileUpReweightMinus,"pileUpReweightMinus/D");
  DYTree->Branch("pileUpReweightInMuonPhys",&pileUpReweightInMuonPhys,"pileUpReweightInMuonPhys/D");
  DYTree->Branch("pileUpReweightMuonPhys",&pileUpReweightMuonPhys,"pileUpReweightMuonPhys/D");
  DYTree->Branch("pileUpReweightPlusMuonPhys",&pileUpReweightPlusMuonPhys,"pileUpReweightPlusMuonPhys/D");
  DYTree->Branch("pileUpReweightMinusMuonPhys",&pileUpReweightMinusMuonPhys,"pileUpReweightMinusMuonPhys/D");
  
  if( theStoreTTFlag ){
    DYTree->Branch("TrackerTrack_dxy", "vector<double>", &TrackerTrack_dxy);
    DYTree->Branch("TrackerTrack_dxyErr", "vector<double>", &TrackerTrack_dxyErr);
    DYTree->Branch("TrackerTrack_d0", "vector<double>", &TrackerTrack_d0);
    DYTree->Branch("TrackerTrack_d0Err", "vector<double>", &TrackerTrack_d0Err);
    DYTree->Branch("TrackerTrack_dsz", "vector<double>", &TrackerTrack_dsz);
    DYTree->Branch("TrackerTrack_dszErr", "vector<double>", &TrackerTrack_dszErr);
    DYTree->Branch("TrackerTrack_dz", "vector<double>", &TrackerTrack_dz);
    DYTree->Branch("TrackerTrack_dzErr", "vector<double>", &TrackerTrack_dzErr);
    DYTree->Branch("TrackerTrack_dxyBS", "vector<double>", &TrackerTrack_dxyBS);
    DYTree->Branch("TrackerTrack_dszBS", "vector<double>", &TrackerTrack_dszBS);
    DYTree->Branch("TrackerTrack_dzBS", "vector<double>", &TrackerTrack_dzBS);
    DYTree->Branch("TrackerTrack_pt", "vector<double>", &TrackerTrack_pt);
    DYTree->Branch("TrackerTrack_Px", "vector<double>", &TrackerTrack_Px);
    DYTree->Branch("TrackerTrack_Py", "vector<double>", &TrackerTrack_Py);
    DYTree->Branch("TrackerTrack_Pz", "vector<double>", &TrackerTrack_Pz);
    DYTree->Branch("TrackerTrack_eta", "vector<double>", &TrackerTrack_eta);
    DYTree->Branch("TrackerTrack_phi", "vector<double>", &TrackerTrack_phi);
    DYTree->Branch("TrackerTrack_charge", "vector<double>", &TrackerTrack_charge);
  }
  
  if( theStoreMETFlag ){
    DYTree->Branch("pfMET_pt", &pfMET_pt, "pfMET_pt/D");
    DYTree->Branch("pfMET_phi", &pfMET_phi, "pfMET_phi/D");
    DYTree->Branch("pfMET_Px", &pfMET_Px, "pfMET_Px/D");
    DYTree->Branch("pfMET_Py", &pfMET_Py, "pfMET_Py/D");
    DYTree->Branch("pfMET_SumEt", &pfMET_SumEt, "pfMET_SumEt/D");
    DYTree->Branch("pfMET_Type1_pt", &pfMET_Type1_pt, "pfMET_Type1_pt/D");
    DYTree->Branch("pfMET_Type1_phi", &pfMET_Type1_phi, "pfMET_Type1_phi/D");
    DYTree->Branch("pfMET_Type1_Px", &pfMET_Type1_Px, "pfMET_Type1_Px/D");
    DYTree->Branch("pfMET_Type1_Py", &pfMET_Type1_Py, "pfMET_Type1_Py/D");
    DYTree->Branch("pfMET_Type1_SumEt", &pfMET_Type1_SumEt, "pfMET_Type1_SumEt/D");
    DYTree->Branch("pfMET_Type1_PhiCor_pt", &pfMET_Type1_PhiCor_pt, "pfMET_Type1_PhiCor_pt/D");
    DYTree->Branch("pfMET_Type1_PhiCor_phi", &pfMET_Type1_PhiCor_phi, "pfMET_Type1_PhiCor_phi/D");
    DYTree->Branch("pfMET_Type1_PhiCor_Px", &pfMET_Type1_PhiCor_Px, "pfMET_Type1_PhiCor_Px/D");
    DYTree->Branch("pfMET_Type1_PhiCor_Py", &pfMET_Type1_PhiCor_Py, "pfMET_Type1_PhiCor_Py/D");
    DYTree->Branch("pfMET_Type1_PhiCor_SumEt", &pfMET_Type1_PhiCor_SumEt, "pfMET_Type1_PhiCor_SumEt/D");
    DYTree->Branch("pfMET_pt_shifts", "vector<double>", &pfMET_pt_shifts);
    DYTree->Branch("pfMET_phi_shifts", "vector<double>", &pfMET_phi_shifts);
    DYTree->Branch("pfMET_Px_shifts", "vector<double>", &pfMET_Px_shifts);
    DYTree->Branch("pfMET_Py_shifts", "vector<double>", &pfMET_Py_shifts);
    DYTree->Branch("pfMET_SumEt_shifts", "vector<double>", &pfMET_SumEt_shifts);
    DYTree->Branch("pfMET_Type1_pt_shifts", "vector<double>", &pfMET_Type1_pt_shifts);
    DYTree->Branch("pfMET_Type1_phi_shifts", "vector<double>", &pfMET_Type1_phi_shifts);
    DYTree->Branch("pfMET_Type1_Px_shifts", "vector<double>", &pfMET_Type1_Px_shifts);
    DYTree->Branch("pfMET_Type1_Py_shifts", "vector<double>", &pfMET_Type1_Py_shifts);
    DYTree->Branch("pfMET_Type1_SumEt_shifts", "vector<double>", &pfMET_Type1_SumEt_shifts);
    DYTree->Branch("pfMET_Type1_PhiCor_pt_shifts", "vector<double>", &pfMET_Type1_PhiCor_pt_shifts);
    DYTree->Branch("pfMET_Type1_PhiCor_phi_shifts", "vector<double>", &pfMET_Type1_PhiCor_phi_shifts);
    DYTree->Branch("pfMET_Type1_PhiCor_Px_shifts", "vector<double>", &pfMET_Type1_PhiCor_Px_shifts);
    DYTree->Branch("pfMET_Type1_PhiCor_Py_shifts", "vector<double>", &pfMET_Type1_PhiCor_Py_shifts);
    DYTree->Branch("pfMET_Type1_PhiCor_SumEt_shifts", "vector<double>", &pfMET_Type1_PhiCor_SumEt_shifts);
  }

  if(theDebugLevel) cout << "[SKFlatMaker::beginJob] finished" << endl;

}

void SKFlatMaker::beginRun(const Run & iRun, const EventSetup & iSetup)
{
  
  if(theDebugLevel) cout << "[SKFlatMaker::beginRun] called" << endl;
  
  vector<string> temp_trigs = {
      "HLT_Mu*", "HLT_Ele*", "HLT_DoubleEle*", "HLT_DoublePhoton*", "HLT_IsoMu*", "HLT_Photon*",
      "HLT_oldMu100_v*", "HLT_TkMu100_v*",

/*
      //==== single muon triggers
      "HLT_IsoMu27_v*",
      "HLT_Mu50_v*",
            
      //==== double muon triggers
      "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v*",
      
      //==== Single Electron
      "HLT_Ele35_WPTight_Gsf_v*",
      
      //==== Double Electron
      "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v*", // low pt, loose ID & iso, unprescaled 
      "HLT_DoubleEle33_CaloIdL_MW_v*", // loose ID, no isolation
      
      //==== Double Photon
      "HLT_DoublePhoton33_CaloIdL_v*"
*/

  };
  //==== copy this to member variable, HLTName_WildCard
  HLTName_WildCard.clear();
  for(unsigned int i=0; i<temp_trigs.size(); i++ ) HLTName_WildCard.push_back(temp_trigs.at(i));

  bool changedConfig;
  if(!hltConfig_.init(iRun, iSetup, processName, changedConfig)){
    LogError("HLTMuonVal") << "Initialization of HLTConfigProvider failed!!";
    return;
  }

  if(theDebugLevel) cout << "[SKFlatMaker::beginRun] finished" << endl;

}

// ------------ method called once each job just after ending the event loop  ------------ //
void SKFlatMaker::endJob()
{
  if(theDebugLevel) cout << "[SKFlatMaker::endJob] endJob" << endl;
  std::cout <<"[SKFlatMaker::endJob] ++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout <<"[SKFlatMaker::endJob] analyzed " << nEvt << " events: " << std::endl;
  std::cout <<"[SKFlatMaker::endJob] ++++++++++++++++++++++++++++++++++++++" << std::endl;
}

///////////////////////////////////////////////////////
// -- makes hlt report and fills it to the ntuple -- //
///////////////////////////////////////////////////////
void SKFlatMaker::hltReport(const edm::Event &iEvent)
{

  if(theDebugLevel) cout << "[SKFlatMaker::hltReport] called" << endl;

  Handle<TriggerResults> trigResult;
  iEvent.getByToken(TriggerToken, trigResult);

  if( !trigResult.failedToGet() ){

    //==== All trigger paths in this event setup
    const edm::TriggerNames trigName = iEvent.triggerNames(*trigResult);

    if(theDebugLevel>=2){
      cout << "[SKFlatMaker::hltReport] trigger names in trigger result (HLT)" << endl;
      cout << "[SKFlatMaker::hltReport] trigName.size() = " << trigName.size() << endl;
      for(int itrig=0; itrig<(int)trigName.size(); itrig++)
        cout << "[SKFlatMaker::hltReport] trigName = " << trigName.triggerName(itrig) << " " << itrig << endl;
    }


    //==== iteration for each Trigger Name WildCards
    //==== e.g., {"HLT_Mu*", "HLT_Ele*"}
    for(unsigned int it_HLTWC = 0; it_HLTWC < HLTName_WildCard.size(); it_HLTWC++ ){
      if(theDebugLevel) cout << "[SKFlatMaker::hltReport] [" << it_HLTWC << "th Input Trigger Name WildCard = " << HLTName_WildCard[it_HLTWC] << "]" << endl;

      //==== find triggers in HLT matched to this WildCard
      std::vector<std::vector<std::string>::const_iterator> matches = edm::regexMatch(trigName.triggerNames(), HLTName_WildCard[it_HLTWC]);

      if( !matches.empty() ){

        //==== iteration for each wildcard-matched triggers
        //==== e.g., {"HLT_Mu8", "HLT_Mu17"}
        BOOST_FOREACH(std::vector<std::string>::const_iterator match, matches){

          if(theDebugLevel) cout << "[SKFlatMaker::hltReport]   [matched trigger = " << *match << "]" << endl;
          HLT_TriggerName.push_back(*match); //save HLT list as a vector

          //==== find prescale value
          int _preScaleValue = hltConfig_.prescaleValue(0, *match);
          HLT_TriggerPrescale.push_back(_preScaleValue);

          //==== Check if this trigger is fired
          if( trigResult->accept(trigName.triggerIndex(*match)) ){
            HLT_TriggerFired.push_back(true);
          }
          else{
            HLT_TriggerFired.push_back(false);
          }

          //==== Save Filter if theStoreHLTObjectFlag
          if(theStoreHLTObjectFlag){
            //==== find modules corresponding to a trigger in HLT configuration
            std::vector<std::string> moduleNames = hltConfig_.moduleLabels( *match );
            if(theDebugLevel){
              cout << "[SKFlatMaker::hltReport]   moduleNames.size() = " << moduleNames.size() << endl;
              for(unsigned int it_md=0; it_md<moduleNames.size(); it_md++){
                if(theDebugLevel) cout << "[SKFlatMaker::hltReport]     " << moduleNames.at(it_md) << endl;
              }
            }
            string this_modulename = "";
            //==== Last module = moduleNames[nmodules-1] is always "hltBoolEnd"
            int nmodules = moduleNames.size();
            if( nmodules-2 >= 0 ){
              this_modulename = moduleNames[nmodules-2];
            }
            HLT_TriggerFilterName.push_back( this_modulename );
          }


        } // END Loop over matches

      } // END matches not empty

    } // END Loop over Trigger Wildcards

    //==== Now, all trigers we are interested in are saved

    if(theStoreHLTObjectFlag){

      //==== HLT Object for lepton matching
      edm::Handle< std::vector<pat::TriggerObjectStandAlone> > triggerObject;
      iEvent.getByToken(TriggerObjectToken, triggerObject);

      for(pat::TriggerObjectStandAlone obj : *triggerObject){

        obj.unpackPathNames(trigName);
        obj.unpackFilterLabels(iEvent, *trigResult);  //added Suoh

        HLTObject_pt.push_back( obj.pt() );
        HLTObject_eta.push_back( obj.eta() );
        HLTObject_phi.push_back( obj.phi() );

        string FiredFilters = "", FiredPaths = "";

        //==== Path we are interested in are saved in : std::vector<std::string > HLT_TriggerName;
        //==== Filter we are interested in are svaed in : std::vector<std::string > HLT_TriggerFilterName;

        //==== Loop over filters
        //cout << "This HLT Object : " << endl;
        for( size_t i_filter = 0; i_filter < obj.filterLabels().size(); ++i_filter ){
          string this_filter = obj.filterLabels().at(i_filter);
          //cout << "  this_filter = " << this_filter << endl;
          if(std::find( HLT_TriggerFilterName.begin(), HLT_TriggerFilterName.end(), this_filter) != HLT_TriggerFilterName.end() ){
            FiredFilters += this_filter+":";
          }
        } //==== END Filter Loop

        //==== Loop over path
        std::vector<std::string> pathNamesAll  = obj.pathNames(true); // Get path whose last filter is fired
        for( size_t i_filter = 0; i_filter < pathNamesAll.size(); ++i_filter ){
          string this_path = pathNamesAll.at(i_filter);
          //cout << "  this_path = " << this_path << endl;
          if(std::find( HLT_TriggerName.begin(), HLT_TriggerName.end(), this_path ) != HLT_TriggerName.end()){
            FiredPaths += pathNamesAll.at(i_filter)+":";
          }
        } //==== END Filter Loop

        if(theDebugLevel>=2){
          cout << "  ==> FiredFilters = " << FiredFilters << endl;
          cout << "  ==> FiredPaths = " << FiredPaths << endl;
        }

        HLTObject_FiredFilters.push_back( FiredFilters );
        HLTObject_FiredPaths.push_back( FiredPaths );

      } //==== END HLT Object loop

    }

  } // -- end of if( !trigResult.failedToGet() ) -- //
  
  if( IsData ){
    Handle<TriggerResults> trigResultPAT;
    iEvent.getByToken(TriggerTokenPAT, trigResultPAT);
      
    if( !trigResultPAT.failedToGet() ){
      const edm::TriggerNames trigName = iEvent.triggerNames(*trigResultPAT);
    
      // cout << "trigger names in trigger result (PAT)" << endl;
      // for(int itrig=0; itrig<(int)trigName.size(); itrig++)
      //   cout << "trigName = " << trigName.triggerName(itrig) << " " << itrig << endl;

      if( trigResultPAT->accept(trigName.triggerIndex("Flag_goodVertices")) ) Flag_goodVertices = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_globalTightHalo2016Filter")) ) Flag_globalTightHalo2016Filter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_globalSuperTightHalo2016Filter")) ) Flag_globalSuperTightHalo2016Filter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_HBHENoiseFilter")) ) Flag_HBHENoiseFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_HBHENoiseIsoFilter")) ) Flag_HBHENoiseIsoFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_EcalDeadCellTriggerPrimitiveFilter")) ) Flag_EcalDeadCellTriggerPrimitiveFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_BadPFMuonFilter")) ) Flag_BadPFMuonFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_BadChargedCandidateFilter")) ) Flag_BadChargedCandidateFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_eeBadScFilter")) ) Flag_eeBadScFilter = true;
      if( trigResultPAT->accept(trigName.triggerIndex("Flag_ecalBadCalibFilter")) ) Flag_ecalBadCalibFilter = true;
 
    }
  }
  
  
  //==============
  //==== MiniAOD
  //==============
  
  //save event filter infomation
  if (!iEvent.getByToken(METFilterResultsToken_PAT, METFilterResults)){
    iEvent.getByToken(METFilterResultsToken_RECO, METFilterResults);
  }
  
  const edm::TriggerNames &metNames = iEvent.triggerNames(*METFilterResults);
  
  //*******************************************************************************
  //For Debug printout
  //*******************************************************************************
  //cout << "----------------------------" << endl;
  //for (unsigned int i = 0, n = METFilterResults->size(); i < n; ++i) {
  //  std::cout << "MET Filter " << metNames.triggerName(i).c_str() << "\n";
  //}
    
  for(unsigned int i = 0, n = METFilterResults->size(); i < n; ++i){
    if(strcmp(metNames.triggerName(i).c_str(), "Flag_goodVertices") == 0) Flag_goodVertices = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_globalTightHalo2016Filter") == 0) Flag_globalTightHalo2016Filter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_globalSuperTightHalo2016Filter") == 0) Flag_globalSuperTightHalo2016Filter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_HBHENoiseFilter") == 0) Flag_HBHENoiseFilter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_HBHENoiseIsoFilter") == 0) Flag_HBHENoiseIsoFilter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_EcalDeadCellTriggerPrimitiveFilter") == 0) Flag_EcalDeadCellTriggerPrimitiveFilter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_BadPFMuonFilter") == 0) Flag_BadPFMuonFilter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_BadChargedCandidateFilter") == 0) Flag_BadChargedCandidateFilter = METFilterResults -> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_eeBadScFilter") == 0) Flag_eeBadScFilter = METFilterResults-> accept(i);
    else if(strcmp(metNames.triggerName(i).c_str(), "Flag_ecalBadCalibFilter") == 0) Flag_ecalBadCalibFilter = METFilterResults -> accept(i);
  }

}

///////////////////////////////////
// -- Get Primary vertex info -- //
///////////////////////////////////
void SKFlatMaker::fillPrimaryVertex(const edm::Event &iEvent)
{
  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByToken(PrimaryVertexToken, pvHandle);
  const reco::VertexCollection vtx = *(pvHandle.product());
  nPV = pvHandle->size();

  if( vtx.size() > 2 && theDebugLevel > 0) cout << "[SKFlatMaker::fillPrimaryVertex] Reconstructed "<< vtx.size() << " vertices" << endl;
  if (vtx.size() > 0 ){
    PVtrackSize = vtx.front().tracksSize();
    PVchi2 = vtx.front().chi2();
    PVndof = vtx.front().ndof();
    PVnormalizedChi2 = vtx.front().normalizedChi2();
    PVx = vtx.front().x();
    PVy = vtx.front().y();
    PVz = vtx.front().z();
    PVprob = TMath::Prob(PVchi2,(int)PVndof);
   }
  
}

//////////////////////////////
// -- Get Muons info -- //
//////////////////////////////
void SKFlatMaker::fillMuons(const edm::Event &iEvent, const edm::EventSetup& iSetup)
{
  // -- BeamSpot -- //
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(BeamSpotToken, beamSpotHandle);
  reco::BeamSpot beamSpot = (*beamSpotHandle);
  
  // -- Vertex -- //
  math::XYZPoint RefVtx;
  RefVtx.SetXYZ(0, 0, 0);
  
  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByToken(PrimaryVertexToken, pvHandle);
  const reco::VertexCollection &vertices = *pvHandle.product();

  // -- What is the purpose of below line? -- //
  for(reco::VertexCollection::const_iterator it=vertices.begin() ; it!=vertices.end() ; ++it){
    RefVtx = it->position();
    break;
  }
  const reco::Vertex &vtx = pvHandle->front();
  
  // muons
  ESHandle<MagneticField> B;
  iSetup.get<IdealMagneticFieldRecord>().get(B);
  
  // -- Call PAT muons -- //
  edm::Handle< std::vector<pat::Muon> > muonHandle;
  iEvent.getByToken(MuonToken, muonHandle);
  using reco::MuonCollection;
  MuonCollection::const_iterator imuon;

  //==== Prepare PF for miniiso
  edm::Handle<pat::PackedCandidateCollection> pc;
  iEvent.getByToken(pcToken_, pc);

  edm::Handle<reco::GenParticleCollection> genParticles;
  if(!IsData){
    iEvent.getByToken(mcLabel_,genParticles);
  }

  for( unsigned int i = 0; i != muonHandle->size(); i++ ){
    // cout << "##### Analyze:Start the loop for the muon #####" << endl;
    const pat::Muon imuon = muonHandle->at(i);
    
    if( imuon.isStandAloneMuon() ) muon_isStandAlone.push_back( true );
    else muon_isStandAlone.push_back( false );

    if( imuon.isGlobalMuon() ) muon_isGlobal.push_back( true );
    else muon_isGlobal.push_back( false );

    if( imuon.isTrackerMuon() ) muon_isTracker.push_back( true );
    else muon_isTracker.push_back( false );

    if( imuon.isPFMuon() ) muon_isPF.push_back( true );
    else muon_isPF.push_back( false );

    if( imuon.isTightMuon(vtx) ) muon_isTight.push_back( true );
    else muon_isTight.push_back( false );

    if( imuon.isMediumMuon() ) muon_isMedium.push_back( true );
    else muon_isMedium.push_back( false );

    if( imuon.isLooseMuon() ) muon_isLoose.push_back( true );
    else muon_isLoose.push_back( false );

    if( imuon.isSoftMuon(vtx) ) muon_isSoft.push_back( true );
    else muon_isSoft.push_back( false );

    if( imuon.isHighPtMuon(vtx) ) muon_isHighPt.push_back( true );
    else muon_isHighPt.push_back( false );
    
    // -- bits 0-1-2-3 = DT stations 1-2-3-4 -- //
    // -- bits 4-5-6-7 = CSC stations 1-2-3-4 -- //
    int _segments = 0;
      
    for( int idet = 1; idet < 4; idet++ ){
      // -- DT (1), CSC (2), RPC (3) -- //
      for( int istation = 1; istation < 5; istation++ ){
        // -- station 1, 2, 3, 4 -- //
        _segments += imuon.numberOfSegments(istation, idet);
      }
    }
    muon_nSegments.push_back( _segments );
      
    // cout << "##### Analyze:Muon Type #####" << endl;
      
      
    // -- reco track information -- //
    reco::TrackRef trackerTrack = imuon.innerTrack();
    reco::TrackRef muonTrack    = imuon.outerTrack();
    reco::TrackRef glbTrack     = imuon.globalTrack();
    // reco::TrackRef cktTrack   = (muon::tevOptimized(imuon, 200, 17., 40., 0.25)).first;
    
    // cout << "##### Analyze:Muon Tracks #####" << endl;

    muon_validhits.push_back( imuon.numberOfValidHits() );
    

    //==== Global track
    if( glbTrack.isNonnull() ){

      const reco::HitPattern & glbhit = glbTrack->hitPattern();

      muon_normchi.push_back( glbTrack->normalizedChi2() );
      muon_validmuonhits.push_back( glbhit.numberOfValidMuonHits() );

      muon_qoverp.push_back( glbTrack->qoverp() );
      muon_theta.push_back( glbTrack->theta() );
      muon_lambda.push_back( glbTrack->lambda() );
      muon_dxy.push_back( glbTrack->dxy() );
      muon_d0.push_back( glbTrack->d0() );
      muon_dsz.push_back( glbTrack->dsz() );
      muon_dz.push_back( glbTrack->dz() );
      muon_dxyBS.push_back( glbTrack->dxy(beamSpot.position()) );
      muon_dszBS.push_back( glbTrack->dsz(beamSpot.position()) );
      muon_dzBS.push_back( glbTrack->dz(beamSpot.position()) );
      
      muon_vx.push_back( glbTrack->vx() );
      muon_vy.push_back( glbTrack->vy() );
      muon_vz.push_back( glbTrack->vz() );
      
    }
    //==== No Global track, but innertrack
    else if( trackerTrack.isNonnull() ){

      const reco::HitPattern & trkhit = trackerTrack->hitPattern();

      muon_normchi.push_back( trackerTrack->normalizedChi2()  );
      muon_validmuonhits.push_back( trkhit.numberOfValidMuonHits() );
      
      muon_qoverp.push_back( trackerTrack->qoverp() );
      muon_theta.push_back( trackerTrack->theta() );
      muon_lambda.push_back( trackerTrack->lambda() );
      muon_dxy.push_back( trackerTrack->dxy() );
      muon_d0.push_back( trackerTrack->d0() );
      muon_dsz.push_back( trackerTrack->dsz() );
      muon_dz.push_back( trackerTrack->dz() );
      muon_dxyBS.push_back( trackerTrack->dxy(beamSpot.position()) );
      muon_dszBS.push_back( trackerTrack->dsz(beamSpot.position()) );
      muon_dzBS.push_back( trackerTrack->dz(beamSpot.position()) );
      
      muon_vx.push_back( trackerTrack->vx() );
      muon_vy.push_back( trackerTrack->vy() );
      muon_vz.push_back( trackerTrack->vz() );
      
    }
    //==== No Global, No Tracker -> StandAlone
    else{

      const reco::HitPattern & muonhit = muonTrack->hitPattern();

      muon_normchi.push_back( muonTrack->normalizedChi2()  );
      muon_validmuonhits.push_back( muonhit.numberOfValidMuonHits() );

      muon_qoverp.push_back( muonTrack->qoverp() );
      muon_theta.push_back( muonTrack->theta() );
      muon_lambda.push_back( muonTrack->lambda() );
      muon_dxy.push_back( muonTrack->dxy() );
      muon_d0.push_back( muonTrack->d0() );
      muon_dsz.push_back( muonTrack->dsz() );
      muon_dz.push_back( muonTrack->dz() );
      muon_dxyBS.push_back( muonTrack->dxy(beamSpot.position()) );
      muon_dszBS.push_back( muonTrack->dsz(beamSpot.position()) );
      muon_dzBS.push_back( muonTrack->dz(beamSpot.position()) );

      muon_vx.push_back( muonTrack->vx() );
      muon_vy.push_back( muonTrack->vy() );
      muon_vz.push_back( muonTrack->vz() );

    }

    if( trackerTrack.isNonnull() ){
      const reco::HitPattern & inhit = trackerTrack->hitPattern();
      
      muon_trackerHits.push_back( inhit.numberOfValidTrackerHits() );
      muon_pixelHits.push_back( inhit.numberOfValidPixelHits() );
      muon_trackerLayers.push_back( inhit.trackerLayersWithMeasurement() );
    }
    else{
      muon_trackerHits.push_back( 0 );
      muon_pixelHits.push_back( 0 );
      muon_trackerLayers.push_back( 0 );
    }
      
    if( !pvHandle->empty() && !pvHandle->front().isFake() ){
/*
      muon_dxyVTX.push_back( imuon.muonBestTrack()->dxy(vtx.position()) );
      muon_dzVTX.push_back( imuon.muonBestTrack()->dz(vtx.position()) );
      muon_dszVTX.push_back( imuon.muonBestTrack()->dsz(vtx.position()) );
*/

      //==== https://github.com/cms-sw/cmssw/blob/3e032f6f8d88a5a63769f68fe525b4f06d72b55f/PhysicsTools/PatAlgos/plugins/PATMuonProducer.cc#L322-L345
      //==== PAT dB are set with BestTrack

      muon_dxyVTX.push_back( imuon.dB(pat::Muon::PV2D) );
      muon_dxyerrVTX.push_back( imuon.edB(pat::Muon::PV2D) );
      muon_dzVTX.push_back( imuon.dB(pat::Muon::PVDZ) );
      muon_dzerrVTX.push_back( imuon.edB(pat::Muon::PVDZ) );
      muon_3DIPVTX.push_back( imuon.dB(pat::Muon::PV3D) );
      muon_3DIPerrVTX.push_back( imuon.edB(pat::Muon::PV3D) );

      muon_dszVTX.push_back( imuon.muonBestTrack()->dsz(vtx.position()) );

      // muon_dxycktVTX.push_back( cktTrack->dxy(vtx.position()) );
      // muon_dszcktVTX.push_back( cktTrack->dsz(vtx.position()) );
      // muon_dzcktVTX.push_back( cktTrack->dz(vtx.position()) );
    }
    else{

      muon_dxyVTX.push_back( 9999 );
      muon_dxyerrVTX.push_back( 9999 );
      muon_dzVTX.push_back( 9999 );
      muon_dzerrVTX.push_back( 9999 );
      muon_3DIPVTX.push_back( 9999 );
      muon_3DIPerrVTX.push_back( 9999 );

      muon_dszVTX.push_back( 9999 );
    }
      
    // muon1 kinematics
    // muon_cktpT.push_back( cktTrack->pt() );
    // muon_cktPx.push_back( cktTrack->px() );
    // muon_cktPy.push_back( cktTrack->py() );
    // muon_cktPz.push_back( cktTrack->pz() );
    // muon_cktpTError.push_back( cktTrack->ptError() );
    
    muon_pt.push_back( imuon.pt() );
    muon_mass.push_back( imuon.mass() );
    muon_Px.push_back( imuon.px() );
    muon_Py.push_back( imuon.py() );
    muon_Pz.push_back( imuon.pz() );
    muon_eta.push_back( imuon.eta() );
    muon_phi.push_back( imuon.phi() );
    
    muon_dB.push_back( imuon.dB() );
      
    // -- Various track informations -- //
    // -- MuonBestTrack -- //
    if( imuon.muonBestTrack().isNonnull() ){
      muon_Best_pt.push_back( imuon.muonBestTrack()->pt() );
      muon_Best_ptError.push_back( imuon.muonBestTrack()->ptError() );
      muon_Best_Px.push_back( imuon.muonBestTrack()->px() );
      muon_Best_Py.push_back( imuon.muonBestTrack()->py() );
      muon_Best_Pz.push_back( imuon.muonBestTrack()->pz() );
      muon_Best_eta.push_back( imuon.muonBestTrack()->eta() );
      muon_Best_phi.push_back( imuon.muonBestTrack()->phi() );
    }
    else{
      muon_Best_pt.push_back( -999 );
      muon_Best_ptError.push_back( -999 );
      muon_Best_Px.push_back( -999 );
      muon_Best_Py.push_back( -999 );
      muon_Best_Pz.push_back( -999 );
      muon_Best_eta.push_back( -999 );
      muon_Best_phi.push_back( -999 );
    }
      
      
    // -- Inner Track -- //
    if( imuon.innerTrack().isNonnull() ){
      muon_Inner_pt.push_back( imuon.innerTrack()->pt() );
      muon_Inner_ptError.push_back( imuon.innerTrack()->ptError() );
      muon_Inner_Px.push_back( imuon.innerTrack()->px() );
      muon_Inner_Py.push_back( imuon.innerTrack()->py() );
      muon_Inner_Pz.push_back( imuon.innerTrack()->pz() );
      muon_Inner_eta.push_back( imuon.innerTrack()->eta() );
      muon_Inner_phi.push_back( imuon.innerTrack()->phi() );
    }
    else{
      muon_Inner_pt.push_back( -999 );
      muon_Inner_ptError.push_back( -999 );
      muon_Inner_Px.push_back( -999 );
      muon_Inner_Py.push_back( -999 );
      muon_Inner_Pz.push_back( -999 );
      muon_Inner_eta.push_back( -999 );
      muon_Inner_phi.push_back( -999 );
    }
      
    // -- Outer Track -- //
    if( imuon.outerTrack().isNonnull() ){
      muon_Outer_pt.push_back( imuon.outerTrack()->pt() );
      muon_Outer_ptError.push_back( imuon.outerTrack()->ptError() );
      muon_Outer_Px.push_back( imuon.outerTrack()->px() );
      muon_Outer_Py.push_back( imuon.outerTrack()->py() );
      muon_Outer_Pz.push_back( imuon.outerTrack()->pz() );
      muon_Outer_eta.push_back( imuon.outerTrack()->eta() );
      muon_Outer_phi.push_back( imuon.outerTrack()->phi() );
    }
    else{
      muon_Outer_pt.push_back( -999 );
      muon_Outer_ptError.push_back( -999 );
      muon_Outer_Px.push_back( -999 );
      muon_Outer_Py.push_back( -999 );
      muon_Outer_Pz.push_back( -999 );
      muon_Outer_eta.push_back( -999 );
      muon_Outer_phi.push_back( -999 );
    }
      
    // -- Global Track -- //
    if( imuon.globalTrack().isNonnull() ){
      muon_GLB_pt.push_back( imuon.globalTrack()->pt() );
      muon_GLB_ptError.push_back( imuon.globalTrack()->ptError() );
      muon_GLB_Px.push_back( imuon.globalTrack()->px() );
      muon_GLB_Py.push_back( imuon.globalTrack()->py() );
      muon_GLB_Pz.push_back( imuon.globalTrack()->pz() );
      muon_GLB_eta.push_back( imuon.globalTrack()->eta() );
      muon_GLB_phi.push_back( imuon.globalTrack()->phi() );
    }
    else{
      muon_GLB_pt.push_back( -999 );
      muon_GLB_ptError.push_back( -999 );
      muon_GLB_Px.push_back( -999 );
      muon_GLB_Py.push_back( -999 );
      muon_GLB_Pz.push_back( -999 );
      muon_GLB_eta.push_back( -999 );
      muon_GLB_phi.push_back( -999 );
    }
      
    // -- tuneP MuonBestTrack -- //
    if( imuon.tunePMuonBestTrack().isNonnull() ){
      muon_TuneP_pt.push_back( imuon.tunePMuonBestTrack()->pt() );
      muon_TuneP_ptError.push_back( imuon.tunePMuonBestTrack()->ptError() );
      muon_TuneP_Px.push_back( imuon.tunePMuonBestTrack()->px() );
      muon_TuneP_Py.push_back( imuon.tunePMuonBestTrack()->py() );
      muon_TuneP_Pz.push_back( imuon.tunePMuonBestTrack()->pz() );
      muon_TuneP_eta.push_back( imuon.tunePMuonBestTrack()->eta() );
      muon_TuneP_phi.push_back( imuon.tunePMuonBestTrack()->phi() );
    }
    else{
      muon_TuneP_pt.push_back( -999 );
      muon_TuneP_ptError.push_back( -999 );
      muon_TuneP_Px.push_back( -999 );
      muon_TuneP_Py.push_back( -999 );
      muon_TuneP_Pz.push_back( -999 );
      muon_TuneP_eta.push_back( -999 );
      muon_TuneP_phi.push_back( -999 );
    }
      
    //-- ISOLATIONS GO HERE -- //
    // -- detector based -- //
    muon_trkiso.push_back( imuon.isolationR03().sumPt );
    muon_hcaliso.push_back( imuon.isolationR03().hadEt );
    muon_ecaliso.push_back( imuon.isolationR03().emEt );
    muon_trkisoR05.push_back( imuon.isolationR05().sumPt );
    muon_hcalisoR05.push_back( imuon.isolationR05().hadEt );
    muon_ecalisoR05.push_back( imuon.isolationR05().emEt ); 
    
    // -- pf isolation -- // 
    muon_PfChargedHadronIsoR04.push_back( imuon.pfIsolationR04().sumChargedHadronPt );
    muon_PfNeutralHadronIsoR04.push_back( imuon.pfIsolationR04().sumNeutralHadronEt );
    muon_PfGammaIsoR04.push_back( imuon.pfIsolationR04().sumPhotonEt );
    muon_PFSumPUIsoR04.push_back( imuon.pfIsolationR04().sumPUPt );
    
    muon_PfChargedHadronIsoR03.push_back( imuon.pfIsolationR03().sumChargedHadronPt );
    muon_PfNeutralHadronIsoR03.push_back( imuon.pfIsolationR03().sumNeutralHadronEt );
    muon_PfGammaIsoR03.push_back( imuon.pfIsolationR03().sumPhotonEt );
    muon_PFSumPUIsoR03.push_back( imuon.pfIsolationR03().sumPUPt );

/*
    //==== MiniIso
    PFIsolation this_miniiso = GetMiniIso(pc, imuon.p4(),
                                          miniIsoParams_[0], miniIsoParams_[1], miniIsoParams_[2],
                                          miniIsoParams_[3], miniIsoParams_[4], miniIsoParams_[5],
                                          miniIsoParams_[6], miniIsoParams_[7], miniIsoParams_[8]);

    cout << "=======================" << endl;
    cout << "Pt = " << imuon.pt() << endl;
    cout << "---- R04 ----" << endl;
    cout << "CH = " << imuon.pfIsolationR04().sumChargedHadronPt << endl;
    cout << "NH = " << imuon.pfIsolationR04().sumNeutralHadronEt << endl;
    cout << "Ph = " << imuon.pfIsolationR04().sumPhotonEt << endl;
    cout << "PU = " << imuon.pfIsolationR04().sumPUPt << endl;
    cout << "---- Mini ----" << endl;
    cout << "CH = " << this_miniiso.chargedHadronIso() << endl;
    cout << "HN = " << this_miniiso.neutralHadronIso() << endl;
    cout << "Ph = " << this_miniiso.photonIso() << endl;
    cout << "PU = " << this_miniiso.puChargedHadronIso() << endl;
    cout << "---- MiniAOD MiniIso ----" << endl;
    cout << "CH = " << imuon.miniPFIsolation().chargedHadronIso() << endl;
    cout << "HN = " << imuon.miniPFIsolation().neutralHadronIso() << endl;
    cout << "Ph = " << imuon.miniPFIsolation().photonIso() << endl;
    cout << "PU = " << imuon.miniPFIsolation().puChargedHadronIso() << endl;
    cout << "---- Diff ----" << endl;
    cout << "dCH = " << this_miniiso.chargedHadronIso() - imuon.miniPFIsolation().chargedHadronIso() << endl;
    cout << "dNH = " << this_miniiso.neutralHadronIso() - imuon.miniPFIsolation().neutralHadronIso() << endl;
    cout << "dPh = " << this_miniiso.photonIso() - imuon.miniPFIsolation().photonIso() << endl;
    cout << "dPU = " << this_miniiso.puChargedHadronIso() - imuon.miniPFIsolation().puChargedHadronIso() << endl;

    muon_PfChargedHadronMiniIso.push_back( this_miniiso.chargedHadronIso() );
    muon_PfNeutralHadronMiniIso.push_back( this_miniiso.neutralHadronIso() );
    muon_PfGammaMiniIso.push_back( this_miniiso.photonIso() );
    muon_PFSumPUMiniIso.push_back( this_miniiso.puChargedHadronIso() );
*/

    muon_PfChargedHadronMiniIso.push_back( imuon.miniPFIsolation().chargedHadronIso() );
    muon_PfNeutralHadronMiniIso.push_back( imuon.miniPFIsolation().neutralHadronIso() );
    muon_PfGammaMiniIso.push_back( imuon.miniPFIsolation().photonIso() );
    muon_PFSumPUMiniIso.push_back( imuon.miniPFIsolation().puChargedHadronIso() );

    // -- Else -- //
    muon_charge.push_back( imuon.charge() );
    muon_nChambers.push_back( imuon.numberOfChambers() ); // -- # of chambers -- //
    muon_matchedstations.push_back( imuon.numberOfMatchedStations() ); // -- # of chambers with matched segments -- //
    muon_stationMask.push_back( imuon.stationMask() ); // -- bit map of stations with matched segments -- //

    //==== Rochestor

    double this_roccor = 1.;
    double this_roccor_err = 0.;

    //==== https://indico.cern.ch/event/460734/contributions/1131590/attachments/1183866/1715221/rochcor_run2_MuonPOG_110815.pdf
    //==== Recommended pt range is pt>20 GeV
    //==== JH asked to the authors, and they said pt 10~20 GeV MIGHT be okay..
    //==== HOWEVER, few GeVs of muon shuold not use this correction,
    //==== because the authors did not consider the radiations of low pt muons inside detectors
    //==== So for now, if pt<10 GeV, no correction applied
    if(imuon.pt()>10.){

      //==== Data
      if(IsData){
        this_roccor = rc.kScaleDT(imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), 0, 0); //data
        this_roccor_err = rc.kScaleDTerror(imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi());
      }
      //==== MC
      else{

        gRandom->SetSeed( evtNum ); // to make the seed always same
        double u = gRandom->Rndm();

        int n_trackerLayersWithMeasurement = 0;
        if(trackerTrack.isNonnull()) n_trackerLayersWithMeasurement = imuon.innerTrack()->hitPattern().trackerLayersWithMeasurement();


        //==== Using GenPt

        //cout << "[This Reco Muon] pt = " << imuon.pt() << ", eta = " << imuon.eta() << ", phi = " << imuon.phi() << endl;
        //cout << "isTight = " << imuon.isTightMuon(vtx) << endl;
        //cout << "isMediumMuon = " << imuon.isMediumMuon() << endl;
        //cout << "isLoose = " << imuon.isLooseMuon() << endl;
        //cout << "isPF = " << muon_isPF.at(i) << endl;
        //cout << "isGlobal = " << muon_isGlobal.at(i) << endl;
        //cout << "chi2 = " << muon_normchi.at(i) << endl;
        //cout << "isTracker = " << muon_isTracker.at(i) << endl;
        //cout << "isStandAlone = " << muon_isStandAlone.at(i) << endl;
        //cout << "dxy = " << muon_dxyVTX.at(i) << endl;
        //cout << "dz = " << muon_dzVTX.at(i) << endl;

        int counter=0;
        double this_genpt=-999.;
        double mindr = 0.2;
        for( reco::GenParticleCollection::const_iterator genptl = genParticles->begin(); genptl != genParticles->end(); ++genptl, ++counter) {

          //int idx = -1;
          //for( reco::GenParticleCollection::const_iterator mit = genParticles->begin(); mit != genParticles->end(); ++mit ){
          //  if( genptl->mother()==&(*mit) ){
          //    idx = std::distance(genParticles->begin(),mit);
          //    break;
          //  }
          //}
          //cout << counter << "\t" << genptl->pdgId() << "\t" << idx << "\t" << "gen pt = " << genptl->pt() << ", eta = " << genptl->eta() << ", phi = " << genptl->phi() << endl;

          if( fabs(genptl->pdgId()) != 13 ) continue;
          if( genptl->status() != 1 ) continue;

          double this_dr = reco::deltaR( imuon.eta(), imuon.phi(), genptl->eta(), genptl->phi() );
          if( this_dr < mindr ){
            this_genpt = genptl->pt();
            mindr = this_dr;
          }
        }
        //cout << this_genpt << endl;

        if(this_genpt>0){
          this_roccor     = rc.kSpreadMC     (imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), this_genpt, 0, 0);
          this_roccor_err = rc.kSpreadMCerror(imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), this_genpt);
        }
        else{
          this_roccor     = rc.kSmearMC     (imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), n_trackerLayersWithMeasurement, u, 0, 0);
          this_roccor_err = rc.kSmearMCerror(imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), n_trackerLayersWithMeasurement, u);
        }

/*
        //==== TODO Now I'm not using genpt
        this_roccor     = rc.kSmearMC     (imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), n_trackerLayersWithMeasurement, u, 0, 0);
        this_roccor_err = rc.kSmearMCerror(imuon.charge(), imuon.pt(), imuon.eta(), imuon.phi(), n_trackerLayersWithMeasurement, u);
*/

      }

    } // END IF pt>10 GeV
/*
    cout << "--------------------" << endl;
    cout << "pt = " << imuon.pt() << endl;
    cout << "this_roccor = " << this_roccor << endl;
    cout << "this_roccor_err = " << this_roccor_err << endl;
*/

/*
    //==== Debugging for large correction muons
    if(this_roccor<-20){

      cout << "#### RECO ####" << endl;
      cout << "this_roccor = " << this_roccor << endl;
      cout << "pt = " << imuon.pt() << endl;
      cout << "eta = " << imuon.eta() << endl;
      cout << "phi = " << imuon.phi() << endl;
      cout << "muon_normchi = " << muon_normchi.at(i) << endl;
      cout << "isLooseMuon = " << imuon.isLooseMuon() << endl;
      cout << "#### GEN ####" << endl;
      cout << "Index\tPID\tMotherIndex\tpt\teta\tphi" << endl;
      int counter = 0;
      for( reco::GenParticleCollection::const_iterator genptl = genParticles->begin(); genptl != genParticles->end(); ++genptl, ++counter) {

        int idx = -1;
        for( reco::GenParticleCollection::const_iterator mit = genParticles->begin(); mit != genParticles->end(); ++mit ){
          if( genptl->mother()==&(*mit) ){
            idx = std::distance(genParticles->begin(),mit);
            break;
          }
        }

        cout << counter << "\t" << genptl->pdgId() << "\t" << idx << "\t" << "gen pt = " << genptl->pt() << ", eta = " << genptl->eta() << ", phi = " << genptl->phi() << endl;

      }

    }
*/

    muon_roch_sf.push_back( this_roccor );
    muon_roch_sf_up.push_back( this_roccor+this_roccor_err );

  } // -- End of imuon iteration -- //
  
}

//////////////////////////////
// -- Get Electrons info -- //
//////////////////////////////
void SKFlatMaker::fillElectrons(const edm::Event &iEvent, const edm::EventSetup& iSetup)
{
  
  if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] called" << endl;
  // -- BeamSpot -- //
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(BeamSpotToken, beamSpotHandle);
  reco::BeamSpot beamSpot = (*beamSpotHandle);
  
  // -- Primary vertex -- //
  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByToken(PrimaryVertexToken, pvHandle);
  const reco::VertexCollection vtx = *(pvHandle.product());
  
  // -- electron -- //
  edm::Handle< edm::View<pat::Electron> > ElecHandle;
  iEvent.getByToken(ElectronToken, ElecHandle);

  edm::Handle< std::vector<reco::Conversion> > conversions;
  iEvent.getByToken(ConversionsToken, conversions);
  
  // -- B-field for vertex variables (ee, emu) -- //
  ESHandle<MagneticField> B;
  iSetup.get<IdealMagneticFieldRecord>().get(B);
  
  // -- muon for emu vertex -- //
  edm::Handle< std::vector<pat::Muon> > muonHandle;
  iEvent.getByToken(MuonToken, muonHandle);

  //==== Prepare PF for miniiso
  edm::Handle<pat::PackedCandidateCollection> pc;
  iEvent.getByToken(pcToken_, pc);

  edm::FileInPath eaConstantsFile(electron_EA_NHandPh_file);
  EffectiveAreas effectiveAreas(eaConstantsFile.fullPath());

  std::vector< double > _ambGsfTrkPt;
  if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] for ElecHandle starts, ElecHandle->size() : " << ElecHandle->size() << endl;
  
  for(int i=0; i< (int)ElecHandle->size(); i++){
    const auto el = ElecHandle->ptrAt(i);
    
    electron_MVAIso.push_back( el -> userFloat("ElectronMVAEstimatorRun2Fall17IsoV1Values") );
    electron_MVANoIso.push_back( el -> userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV1Values") );
    double elec_theta = el -> theta();
    double sin_theta = sin(elec_theta);

    electron_pt.push_back( el->userFloat("ecalTrkEnergyPostCorr") * sin_theta );
    electron_pt_Scale_Up.push_back( el->userFloat("energyScaleUp") * sin_theta );
    electron_pt_Scale_Down.push_back( el->userFloat("energyScaleDown") * sin_theta );
    electron_pt_Smear_Up.push_back( el->userFloat("energySigmaUp") * sin_theta );
    electron_pt_Smear_Down.push_back( el->userFloat("energySigmaDown") * sin_theta );
    electron_eta.push_back( el->eta() );
    electron_phi.push_back( el->phi() );
    electron_Px.push_back( el->px() );
    electron_Py.push_back( el->py() );
    electron_Pz.push_back( el->pz() );

/*
    //==== Debugging lines for egamma correction
    cout << "==== Electron ====" << endl;
    cout << "el->energy() = " << el->energy() << endl;
    cout << "el->userFloat(\"ecalTrkEnergyPreCorr\") = " << el->userFloat("ecalTrkEnergyPreCorr") << endl;
    cout << "el->userFloat(\"ecalTrkEnergyPostCorr\") = " << el->userFloat("ecalTrkEnergyPostCorr") << endl;
*/

    //==== UnCorrected
    electron_EnergyUnCorr.push_back( el->userFloat("ecalTrkEnergyPreCorr") );

    electron_Energy.push_back( el->userFloat("ecalTrkEnergyPostCorr") );
    electron_Energy_Scale_Up.push_back( el->userFloat("energyScaleUp") );
    electron_Energy_Scale_Down.push_back( el->userFloat("energyScaleDown") );
    electron_Energy_Smear_Up.push_back( el->userFloat("energySigmaUp") );
    electron_Energy_Smear_Down.push_back( el->userFloat("energySigmaDown") );
    
    electron_charge.push_back( el->charge() );
    electron_fbrem.push_back( el->fbrem() );
    electron_eOverP.push_back( el->eSuperClusterOverP() );
    electron_ecalDriven.push_back( el->ecalDrivenSeed() );
    if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] basic info. input" << endl;
    
    
    // -- Information from SuperCluster -- //
    electron_scEnergy.push_back( el->superCluster()->energy() );
    electron_scPreEnergy.push_back( el->superCluster()->preshowerEnergy() );
    electron_scRawEnergy.push_back( el->superCluster()->rawEnergy() );
    electron_scEta.push_back( el->superCluster()->eta() );
    electron_scPhi.push_back( el->superCluster()->phi() );
    double R = sqrt(el->superCluster()->x()*el->superCluster()->x() + el->superCluster()->y()*el->superCluster()->y() +el->superCluster()->z()*el->superCluster()->z());
    double Rt = sqrt(el->superCluster()->x()*el->superCluster()->x() + el->superCluster()->y()*el->superCluster()->y());
    electron_scEt.push_back( el->superCluster()->energy()*(Rt/R) );
    
    if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] supercluster" << endl;

    
    // -- Information from ECAL  -- //
    electron_sigmaIEtaIEta.push_back( el->full5x5_sigmaIetaIeta() );
    electron_Full5x5_SigmaIEtaIEta.push_back( el->full5x5_sigmaIetaIeta() );
    electron_E15.push_back( el->e1x5() );
    electron_E25.push_back( el->e2x5Max() );
    electron_E55.push_back( el->e5x5() );
    // electron_HoverE.push_back( el->hcalOverEcal() );
    electron_HoverE.push_back( el->hadronicOverEm() ); // -- https://github.com/ikrav/cmssw/blob/egm_id_80X_v1/RecoEgamma/ElectronIdentification/plugins/cuts/GsfEleHadronicOverEMCut.cc#L40 -- //
    electron_etaWidth.push_back( el->superCluster()->etaWidth() );
    electron_phiWidth.push_back( el->superCluster()->phiWidth() );
    electron_r9.push_back( el->r9() );
    
    if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] ECAL" << endl;
    
    
    // -- Information from ECAL & Track -- //
    electron_dEtaIn.push_back( el->deltaEtaSuperClusterTrackAtVtx() );
    electron_dPhiIn.push_back( el->deltaPhiSuperClusterTrackAtVtx() );
    
    // -- https://github.com/ikrav/cmssw/blob/egm_id_80X_v1/RecoEgamma/ElectronIdentification/plugins/cuts/GsfEleDEtaInSeedCut.cc#L30-L33 -- //
    electron_dEtaInSeed.push_back( el->superCluster().isNonnull() && el->superCluster()->seed().isNonnull() ?
el->deltaEtaSuperClusterTrackAtVtx() - el->superCluster()->eta() + el->superCluster()->seed()->eta() : std::numeric_limits<float>::max() );
    
    // -- |1/E-1/p| = |1/E - EoverPinner/E| is computed below. The if protects against ecalEnergy == inf or zero -- //
    if( el->ecalEnergy() == 0 ) electron_InvEminusInvP.push_back( 1e30 );
    else if(  !std::isfinite( el->ecalEnergy() )  ) electron_InvEminusInvP.push_back( 1e30 );
    else electron_InvEminusInvP.push_back( fabs( 1.0/el->ecalEnergy() - el->eSuperClusterOverP()/el->ecalEnergy() ) );
    
    if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] ECAL & track" << endl;
    
    // -- Isolation -- //
    double pfCharged = el->pfIsolationVariables().sumChargedHadronPt;
    double pfNeutral = el->pfIsolationVariables().sumNeutralHadronEt;
    double pfPhoton = el->pfIsolationVariables().sumPhotonEt;
    double pfChargedFromPU = el->pfIsolationVariables().sumPUPt;
    electron_chIso03.push_back( pfCharged );
    electron_nhIso03.push_back( pfNeutral );
    electron_phIso03.push_back( pfPhoton );
    electron_puChIso03.push_back( pfChargedFromPU );
    electron_RelPFIso_dBeta.push_back( (pfCharged + max<float>( 0.0, pfNeutral + pfPhoton - 0.5 * pfChargedFromPU))/(el->pt()) );
    
    float abseta = fabs(el->superCluster()->eta());
    float eA = effectiveAreas.getEffectiveArea(abseta);
    electron_RelPFIso_Rho.push_back( (pfCharged + max<float>( 0.0, pfNeutral + pfPhoton - Rho * eA))/(el->pt()) );

    //==== MiniIso
/*
    PFIsolation this_miniiso;

    if(el->isEE()){

      this_miniiso = GetMiniIso(pc, el->p4(),
                                miniIsoParamsE_[0], miniIsoParamsE_[1], miniIsoParamsE_[2],
                                miniIsoParamsE_[3], miniIsoParamsE_[4], miniIsoParamsE_[5],
                                miniIsoParamsE_[6], miniIsoParamsE_[7], miniIsoParamsE_[8]);

    }
    else{

      this_miniiso = GetMiniIso(pc, el->p4(),
                                miniIsoParamsB_[0], miniIsoParamsB_[1], miniIsoParamsB_[2],
                                miniIsoParamsB_[3], miniIsoParamsB_[4], miniIsoParamsB_[5],
                                miniIsoParamsB_[6], miniIsoParamsB_[7], miniIsoParamsB_[8]);

    }

    cout << "=======================" << endl;
    cout << "Pt = " << el->pt() << endl;
    cout << "isEE = " << el->isEE() << endl;
    cout << "---- R03 ----" << endl;
    cout << "CH = " << el->pfIsolationVariables().sumChargedHadronPt << endl;
    cout << "NH = " << el->pfIsolationVariables().sumNeutralHadronEt << endl;
    cout << "Ph = " << el->pfIsolationVariables().sumPhotonEt << endl;
    cout << "PU = " << el->pfIsolationVariables().sumPUPt << endl;
    cout << "---- Mini ----" << endl;
    cout << "CH = " << this_miniiso.chargedHadronIso() << endl;
    cout << "HN = " << this_miniiso.neutralHadronIso() << endl;
    cout << "Ph = " << this_miniiso.photonIso() << endl;
    cout << "PU = " << this_miniiso.puChargedHadronIso() << endl;
    cout << "---- MiniAOD MiniIso ----" << endl;
    cout << "CH = " << el->miniPFIsolation().chargedHadronIso() << endl;
    cout << "HN = " << el->miniPFIsolation().neutralHadronIso() << endl;
    cout << "Ph = " << el->miniPFIsolation().photonIso() << endl;
    cout << "PU = " << el->miniPFIsolation().puChargedHadronIso() << endl;
    cout << "---- Diff ----" << endl;
    cout << "dCH = " << this_miniiso.chargedHadronIso() - el->miniPFIsolation().chargedHadronIso() << endl;
    cout << "dNH = " << this_miniiso.neutralHadronIso() - el->miniPFIsolation().neutralHadronIso() << endl;
    cout << "dPh = " << this_miniiso.photonIso() - el->miniPFIsolation().photonIso() << endl;
    cout << "dPU = " << this_miniiso.puChargedHadronIso() - el->miniPFIsolation().puChargedHadronIso() << endl;

    electron_chMiniIso.push_back( this_miniiso.chargedHadronIso() );
    electron_nhMiniIso.push_back( this_miniiso.neutralHadronIso() );
    electron_phMiniIso.push_back( this_miniiso.photonIso() );
    electron_puChMiniIso.push_back( this_miniiso.puChargedHadronIso() );
*/

    electron_chMiniIso.push_back( el->miniPFIsolation().chargedHadronIso() );
    electron_nhMiniIso.push_back( el->miniPFIsolation().neutralHadronIso() );
    electron_phMiniIso.push_back( el->miniPFIsolation().photonIso() );
    electron_puChMiniIso.push_back( el->miniPFIsolation().puChargedHadronIso() );

    // cout << "##### fillElectrons: Before elecTrk #####" << endl;
    
    if(theDebugLevel) cout << "[SKFlatMaker::fillElectrons] Isolation" << endl;
    
    // -- Track - Impact Parameter, Conversion rejection, Converted -- //
    reco::GsfTrackRef elecTrk = el->gsfTrack();

    electron_passConversionVeto.push_back( el->passConversionVeto() );
    electron_isGsfCtfScPixChargeConsistent.push_back( el->isGsfCtfScPixChargeConsistent() );
    electron_isGsfScPixChargeConsistent.push_back( el->isGsfScPixChargeConsistent() );
    electron_isGsfCtfChargeConsistent.push_back( el->isGsfCtfChargeConsistent() );

    //==== https://github.com/ikrav/cmssw/blob/egm_id_80X_v1/RecoEgamma/ElectronIdentification/plugins/cuts/GsfEleMissingHitsCut.cc#L34-L41
    constexpr auto missingHitType = reco::HitPattern::MISSING_INNER_HITS;
    electron_mHits.push_back( elecTrk->hitPattern().numberOfLostHits(missingHitType) );

    electron_sigdxy.push_back( elecTrk->dxy() / elecTrk->dxyError() );
    electron_dxy.push_back( elecTrk->dxy() );
    electron_dz.push_back( elecTrk->dz() );

    electron_dxyBS.push_back( elecTrk->dxy(beamSpot.position()) );
    electron_dzBS.push_back( elecTrk->dz(beamSpot.position()) );

    if( !pvHandle->empty() && !pvHandle->front().isFake() ){

/*
      electron_dxyVTX.push_back( elecTrk->dxy(vtx.position()) );
      electron_dzVTX.push_back( elecTrk->dz(vtx.position()) );
*/

      electron_dxyVTX.push_back( el->dB(pat::Electron::PV2D) );
      electron_dxyerrVTX.push_back( el->edB(pat::Electron::PV2D) );
      electron_dzVTX.push_back( el->dB(pat::Electron::PVDZ) );
      electron_dzerrVTX.push_back( el->edB(pat::Electron::PVDZ) );
      electron_3DIPVTX.push_back( el->dB(pat::Electron::PV3D) );
      electron_3DIPerrVTX.push_back( el->edB(pat::Electron::PV3D) );

    }
    else{

      electron_dxyVTX.push_back( -999 );
      electron_dxyerrVTX.push_back( -999 );
      electron_dzVTX.push_back( -999 );
      electron_dzerrVTX.push_back( -999 );
      electron_3DIPVTX.push_back( -999 );
      electron_3DIPerrVTX.push_back( -999 );

    }

    if( elecTrk.isNonnull() ){
      electron_gsfpt.push_back( elecTrk->pt() );
      electron_gsfPx.push_back( elecTrk->px() );
      electron_gsfPy.push_back( elecTrk->py() );
      electron_gsfPz.push_back( elecTrk->pz() );
      electron_gsfEta.push_back( elecTrk->eta() );
      electron_gsfPhi.push_back( elecTrk->phi() );
      electron_gsfCharge.push_back( elecTrk->charge() );
    }
    else{
      electron_gsfpt.push_back( -999 );
      electron_gsfPx.push_back( -999 );
      electron_gsfPy.push_back( -999 );
      electron_gsfPz.push_back( -999 );
      electron_gsfEta.push_back( -999 );
      electron_gsfPhi.push_back( -999 );
      electron_gsfCharge.push_back( -999 );
    }
      
    //==== ID Booleans

    //==== Cut Based
    //==== 94x-V2 : https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Recipe_for_regular_users_formats
    bool isPassVeto  = el -> electronID("cutBasedElectronID-Fall17-94X-V2-veto");
    bool isPassLoose  = el -> electronID("cutBasedElectronID-Fall17-94X-V2-loose");
    bool isPassMedium = el -> electronID("cutBasedElectronID-Fall17-94X-V2-medium");
    bool isPassTight  = el -> electronID("cutBasedElectronID-Fall17-94X-V2-tight");
    //==== MVA Based
    //==== Fall17 : https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2#VID_based_recipe_provides_pass_f
    bool isPassMVA_noIso_WP80 = el -> electronID("mvaEleID-Fall17-noIso-V1-wp80");
    bool isPassMVA_noIso_WP90 = el -> electronID("mvaEleID-Fall17-noIso-V1-wp90");
    bool isPassMVA_iso_WP80 = el -> electronID("mvaEleID-Fall17-iso-V1-wp80");
    bool isPassMVA_iso_WP90 = el -> electronID("mvaEleID-Fall17-iso-V1-wp90");
    //==== HEEP ID
    //==== V7 : https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronIdentificationRun2#Configuring_and_Running_VID_in_a
    bool isPassHEEP = el -> electronID("heepElectronID-HEEPV70");

/*
    //==== checking pogboolean and cut-by-hand
    if(isPassLoose){

      double this_hovere = el->hadronicOverEm();
      double corrE = el->userFloat("ecalTrkEnergyPostCorr");
      double uncorrE = el->userFloat("ecalTrkEnergyPreCorr");
      double scE = el->superCluster()->energy();

      if(abseta<1.479){

        double this_loosecut_corrE   = 0.05 + 1.12/corrE   + 0.0368*Rho/corrE;
        double this_loosecut_uncorrE = 0.05 + 1.12/uncorrE + 0.0368*Rho/uncorrE;

        if( !(this_hovere < this_loosecut_uncorrE) ){
          cout << "---- |scEta| = " << abseta << endl;
          cout << "hadronicOverEm() = " << this_hovere << endl;
          cout << "this_loosecut_corrE = " << this_loosecut_corrE << endl;
          cout << "this_loosecut_uncorrE = " << this_loosecut_uncorrE << endl;
        }

      }
      else{

        double this_loosecut_corrE   = 0.0414 + 0.5/corrE   + 0.201*Rho/corrE;
        double this_loosecut_uncorrE = 0.0414 + 0.5/uncorrE + 0.201*Rho/uncorrE;
        double this_loosecut_scE     = 0.0414 + 0.5/scE   + 0.201*Rho/scE;

        if( !(this_hovere < this_loosecut_uncorrE) ){
          cout << "fabs(el->superCluster()->eta()) = " << fabs(el->superCluster()->eta()) << endl;
          cout << "fabs(el->eta()) = " << fabs(el->eta()) << endl;
          cout << "el->electronID(\"cutBasedElectronID-Fall17-94X-V1-loose\") = " << el -> electronID("cutBasedElectronID-Fall17-94X-V1-loose") << endl;
          cout << "el->hadronicOverEm() = " << el->hadronicOverEm() << endl;
          cout << "el->userFloat(\"ecalTrkEnergyPreCorr\") = " << el->userFloat("ecalTrkEnergyPreCorr") << endl;
          cout << "el->userFloat(\"ecalTrkEnergyPostCorr\") = " << el->userFloat("ecalTrkEnergyPostCorr") << endl;
          cout << "el->superCluster()->energy() = " << el->superCluster()->energy() << endl;
          cout << "Rho = " << Rho << endl;
          cout << "this_loosecut_corrE = " << this_loosecut_corrE << endl;
          cout << "this_loosecut_uncorrE = " << this_loosecut_uncorrE << endl;
          cout << "this_loosecut_scE = " << this_loosecut_scE << endl;
        }

      }
    }
*/

    // cout << "isPassVeto: " << isPassVeto << ", isPassLoose: " << isPassLoose << ", isPassMedium: " << isPassMedium << ", isPassTight: " << isPassTight << endl;
    electron_passVetoID.push_back( isPassVeto );
    electron_passLooseID.push_back( isPassLoose );
    electron_passMediumID.push_back( isPassMedium );
    electron_passTightID.push_back( isPassTight );
    electron_passMVAID_noIso_WP80.push_back( isPassMVA_noIso_WP80 );
    electron_passMVAID_noIso_WP90.push_back( isPassMVA_noIso_WP90 );
    electron_passMVAID_iso_WP80.push_back( isPassMVA_iso_WP80 );
    electron_passMVAID_iso_WP90.push_back( isPassMVA_iso_WP90 );
    electron_passHEEPID.push_back( isPassHEEP );


  } // -- end of for(int i=0; i< (int)ElecHandle->size(); i++): 1st electron iteration -- //
  
  // cout << "##### End of fillElectrons #####" << endl;
}

////////////////////////
// -- Get LHE info -- //
////////////////////////
void SKFlatMaker::fillLHEInfo(const edm::Event &iEvent)
{
  Handle<LHEEventProduct> LHEInfo;
  iEvent.getByToken(LHEEventProductToken, LHEInfo);
  if(!LHEInfo.isValid()) return;
  
/*
  //==== FIXME do we want this?
  const lhef::HEPEUP& lheEvent = LHEInfo->hepeup();
  std::vector<lhef::HEPEUP::FiveVector> lheParticles = lheEvent.PUP;
  
  for( size_t idxParticle = 0; idxParticle < lheParticles.size(); ++idxParticle ){
    Int_t id = lheEvent.IDUP[idxParticle];
    
    if( fabs(id) == 13 || fabs(id) == 11 || fabs(id) == 15 ){
      Double_t Px = lheParticles[idxParticle][0];
      Double_t Py = lheParticles[idxParticle][1];
      Double_t Pz = lheParticles[idxParticle][2];
      Double_t E = lheParticles[idxParticle][3];
      // Double_t M = lheParticles[idxParticle][4];    
      Int_t status = lheEvent.ISTUP[idxParticle];
      
    }
  }
*/

  // -- PDf weights for theoretical uncertainties: scale, PDF replica and alphaS variation -- //
  // -- ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW -- //
  int nWeight = (int)LHEInfo->weights().size();

  //==== https://indico.cern.ch/event/690726/contributions/2890915/attachments/1598277/2532794/2017_MC.pdf
  //==== Check which convention

  //==== if PDFType_ was wrong (i.e., int_PDFType_=-1), we find the type
  //==== if not, it was found in the constructor
  //==== find int_PDFType_ from the first event
  if(int_PDFType_<0){

    int_PDFType_ = 0; // powheg (RED)
    for(int i=0; i<nWeight; i++){
      if( stoi(LHEInfo->weights()[i].id.c_str())==1 ){
        int_PDFType_ = 1; // madgraph0 (BLUE)
        break;
      }
      if( stoi(LHEInfo->weights()[i].id.c_str())==1010 ){
        int_PDFType_ = 2; // madgraph1000 (GREEN)
        break;
      }
    }

  }

  //==== Save id-weight as a map
  map<int,double> map_id_to_weight;
  for(int i=0; i<nWeight; i++){
    int this_id = stoi(LHEInfo->weights()[i].id.c_str())+PDFIDShift_;
    map_id_to_weight[this_id] = LHEInfo->weights()[i].wgt;
    if(theDebugLevel) cout << "[SKFlatMaker::fillLHEInfo] map_id_to_weight["<<this_id<<"] = " << map_id_to_weight[this_id] << endl;
  }

  //==== Get Nominal
  double w_def = LHEInfo->originalXWGTUP(); // w_def is not always same as map_id_to_weight[DEFAULT_ID]; if POWHEG+JHUGen, BR(m4l) is applied to default w_def, but not in map_id_to_weight[DEFAULT_ID]
  w_def = 1.; // FIXME we want reweight w.r.t w_def, so we can set it as 1
  double w_nominal(1.);
  if(PDFOrder_=="NLO"){
    //==== NNPDF31_nlo_hessian_pdfas, LHAPDF = 305800
    if(int_PDFType_==0) w_nominal = map_id_to_weight[3000]*w_def/map_id_to_weight[1001];
    else if(int_PDFType_==1) w_nominal = map_id_to_weight[121]*w_def/map_id_to_weight[1];
    else if(int_PDFType_==2) w_nominal = map_id_to_weight[1121]*w_def/map_id_to_weight[1001];
    else w_nominal = 1.;
  }
  //==== FIXME
  else{
    //==== Assuming LO
    //==== NNPDF31_lo_as_0130, LHAPDF = 315200
    if(int_PDFType_==0) w_nominal = map_id_to_weight[1850]*w_def/map_id_to_weight[1001];
    else if(int_PDFType_==1) w_nominal = map_id_to_weight[1078]*w_def/map_id_to_weight[1];
    else if(int_PDFType_==2) w_nominal = map_id_to_weight[2078]*w_def/map_id_to_weight[1001];
    else w_nominal = 1.;
  }

  //==== QCD Scale variation
  //==== PDFWeights_Scale.at(0) should be applied as nominal reweighting
  if(int_PDFType_==1){
    for(int i=0;i<9;i++){
      PDFWeights_Scale.push_back( w_nominal*map_id_to_weight[1+i]/map_id_to_weight[1] );
    }
  }
  else{
    for(int i=0;i<9;i++){
      PDFWeights_Scale.push_back( w_nominal*map_id_to_weight[1001+i]/map_id_to_weight[1001] );
    }
  }

  if(PDFOrder_=="NLO"){

    //==== PDF Error (Hessian)

    int index_b = 3000;
    if(int_PDFType_==1) index_b = 121;
    if(int_PDFType_==2) index_b = 1121;

    double temp_Error(0.);
    for(int i=1;i<=100;i++){
      double this_diff = map_id_to_weight[index_b+i]-map_id_to_weight[index_b];
      temp_Error += this_diff*this_diff;
    }
    double this_hessian_up = w_nominal*(1.+sqrt(temp_Error)/map_id_to_weight[index_b]);
    double this_hessian_dn = w_nominal*(1.-sqrt(temp_Error)/map_id_to_weight[index_b]);
    PDFWeights_Error.push_back(this_hessian_up);
    PDFWeights_Error.push_back(this_hessian_dn);

    //==== AlphaS

    double this_as_up = w_nominal*(map_id_to_weight[index_b+101]-map_id_to_weight[index_b])/map_id_to_weight[index_b]*0.75;
    double this_as_dn = w_nominal*(map_id_to_weight[index_b+102]-map_id_to_weight[index_b])/map_id_to_weight[index_b]*0.75;
    PDFWeights_AlphaS.push_back(1.+this_as_up);
    PDFWeights_AlphaS.push_back(1.-this_as_dn);

  }

  if(theDebugLevel){
    cout << "[SKFlatMaker::fillLHEInfo] PDFType_ = " << PDFType_ << endl;
    cout << "[SKFlatMaker::fillLHEInfo] int_PDFType_ = " << int_PDFType_ << endl;
    cout << "[SKFlatMaker::fillLHEInfo] PDFOrder_ = " << PDFOrder_ << endl;
    cout << "[SKFlatMaker::fillLHEInfo] LHEInfo->originalXWGTUP() = " << LHEInfo->originalXWGTUP() << endl;
    cout << "[SKFlatMaker::fillLHEInfo] PDFIDShift_ = " << PDFIDShift_ << " : how much the weight id is shifted " << endl;
    cout << "[SKFlatMaker::fillLHEInfo] [PDFWeights_Scale]" << endl;
    for(unsigned int i=0;i<PDFWeights_Scale.size();i++) cout << "[SKFlatMaker::fillLHEInfo] " << PDFWeights_Scale.at(i) << endl;
    cout << "[SKFlatMaker::fillLHEInfo] [PDFWeights_Error]" << endl;
    for(unsigned int i=0;i<PDFWeights_Error.size();i++) cout << "[SKFlatMaker::fillLHEInfo] " << PDFWeights_Error.at(i) << endl;
    cout << "[SKFlatMaker::fillLHEInfo] [PDFWeights_AlphaS]" << endl;
    for(unsigned int i=0;i<PDFWeights_AlphaS.size();i++) cout << "[SKFlatMaker::fillLHEInfo] " << PDFWeights_AlphaS.at(i) << endl;
  }


/*
  for(int i=0; i<nWeight; i++){
    double weight = LHEInfo->weights()[i].wgt;
    double ratio = weight / w_def;

    PDFWeights.push_back( ratio );

    //std::cout << i << "th weight = " << weight << "(ID=" << LHEInfo->weights()[i].id <<"), ratio w.r.t. original: " << ratio << endl;
  }
*/
}

////////////////////////
// -- Get GEN info -- //
////////////////////////
void SKFlatMaker::fillGENInfo(const edm::Event &iEvent)
{
  //cout << "fill pdf info" << endl;
  
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(mcLabel_,genParticles);
  
  
  int counter=0;
  for( reco::GenParticleCollection::const_iterator it = genParticles->begin(); it != genParticles->end(); ++it , ++counter) {      

    if(!theKeepAllGen && counter > 30) continue;
    
    gen_PID.push_back( it->pdgId() );
    gen_pt.push_back( it->pt() );
    gen_Px.push_back( it->px() );
    gen_Py.push_back( it->py() );
    gen_Pz.push_back( it->pz() );
    gen_E.push_back( it->energy() );
    gen_eta.push_back( it->eta() );
    gen_phi.push_back( it->phi() );
    gen_charge.push_back( it->charge() );
    gen_status.push_back( it->status() );
    
    //Flags (Ref: https://indico.cern.ch/event/402279/contribution/5/attachments/805964/1104514/mcaod-Jun17-2015.pdf)
    gen_isPrompt.push_back( it->statusFlags().isPrompt() ); //not from hadron, muon or tau decay 
    gen_isPromptFinalState.push_back( it->isPromptFinalState() ); //isPrompt && final state (status==1)
    gen_isTauDecayProduct.push_back( it->statusFlags().isTauDecayProduct() ); //is directly or indirectly from a tau decay
    gen_isPromptTauDecayProduct.push_back( it->statusFlags().isPromptTauDecayProduct() ); //is directly or indirectly from a tau decay, where the tau did not come from a hadron decay
    gen_isDirectPromptTauDecayProductFinalState.push_back( it->isDirectPromptTauDecayProductFinalState() ); // is the direct decay product from a tau decay (ie no intermediate hadron), where the tau did not come from a hadron decay && final state
    gen_isHardProcess.push_back( it->isHardProcess() );
    gen_isLastCopy.push_back( it->isLastCopy() );
    gen_isLastCopyBeforeFSR.push_back( it->isLastCopyBeforeFSR() );
    gen_isPromptDecayed.push_back( it->isPromptDecayed() );
    gen_isDecayedLeptonHadron.push_back( it->statusFlags().isDecayedLeptonHadron() );
    gen_fromHardProcessBeforeFSR.push_back( it->fromHardProcessBeforeFSR() );
    gen_fromHardProcessDecayed.push_back( it->fromHardProcessDecayed() );
    gen_fromHardProcessFinalState.push_back( it->fromHardProcessFinalState() );
    gen_isMostlyLikePythia6Status3.push_back( it->fromHardProcessBeforeFSR() );
    
    if(it->numberOfMothers() > 0){
      gen_mother_PID.push_back( it->mother(0)->pdgId() );
      gen_mother_pt.push_back( it->mother(0)->pt() );
    }
    else{
      gen_mother_PID.push_back( -999 );
      gen_mother_pt.push_back( -999 );
    }
    
    int idx = -1;
    for( reco::GenParticleCollection::const_iterator mit = genParticles->begin(); mit != genParticles->end(); ++mit ){
      if( it->mother()==&(*mit) ){
        idx = std::distance(genParticles->begin(),mit);
        break;
      }
    }
    
    gen_mother_index.push_back( idx );
    
  }
   
  edm::Handle<GenEventInfoProduct> genEvtInfo;
  iEvent.getByToken(GenEventInfoToken, genEvtInfo);
  gen_weight = genEvtInfo->weight();

  genWeight_Q = genEvtInfo->pdf()->scalePDF;

  //==== I think they are same..
  //cout << "[JSKIM] genEvtInfo->pdf()->scalePDF = " << genEvtInfo->pdf()->scalePDF << endl;
  //cout << "[JSKIM] genEvtInfo->qScale() = " << genEvtInfo->qScale() << endl << endl;

  genWeight_X1 = genEvtInfo->pdf()->x.first;
  genWeight_X2 = genEvtInfo->pdf()->x.second;
  genWeight_id1 = genEvtInfo->pdf()->id.first;
  genWeight_id2 = genEvtInfo->pdf()->id.second;
  genWeight_alphaQCD = genEvtInfo->alphaQCD();
  genWeight_alphaQED = genEvtInfo->alphaQED();
  
}


/////////////////////////
// Get Photons info -- // 
/////////////////////////
void SKFlatMaker::fillPhotons(const edm::Event &iEvent)
{
  
  edm::Handle< edm::View<pat::Photon> > PhotonHandle;
  iEvent.getByToken(PhotonToken, PhotonHandle);
  
  EffectiveAreas photon_EA_CH( photon_EA_CH_file.fullPath() );
  EffectiveAreas photon_EA_HN( photon_EA_HN_file.fullPath() );
  EffectiveAreas photon_EA_Ph( photon_EA_Ph_file.fullPath() );

  for(size_t i=0; i< PhotonHandle->size(); ++i){
    const auto pho = PhotonHandle->ptrAt(i);
    
    double sin_theta = sin(pho->theta());
    photon_ptUnCorr.push_back( pho -> userFloat("ecalEnergyPreCorr") * sin_theta );
    photon_pt.push_back( pho -> userFloat("ecalEnergyPostCorr") * sin_theta );
    //double pho_pt_noncor = pho -> userFloat("ecalEnergyPreCorr") * sin_theta;
    //cout << "pho->pt() : " << pho->pt() << ", pho_pt_noncor : " << pho_pt_noncor << ", Pt(cor) : " << photon_pt.at(i) << endl;
    //photon_pt.push_back( pho->pt() * ratio_E );
    photon_eta.push_back( pho->eta() );
    photon_phi.push_back( pho->phi() );
    
    photon_scEta.push_back( pho->superCluster()->eta() );
    photon_scPhi.push_back( pho->superCluster()->phi() );
    
    photon_HoverE.push_back( pho->hadTowOverEm() );
    photon_hasPixelSeed.push_back( (Int_t)pho->hasPixelSeed() );
    //photon_Full5x5_SigmaIEtaIEta.push_back( (*full5x5SigmaIEtaIEtaMap)[ pho ] );
    photon_Full5x5_SigmaIEtaIEta.push_back( pho -> full5x5_sigmaIetaIeta() );
    
    
    float chIso = pho -> chargedHadronIso();
    float nhIso = pho -> neutralHadronIso();
    float phIso = pho -> photonIso();
    
    photon_ChIso.push_back( chIso );
    photon_NhIso.push_back( nhIso );
    photon_PhIso.push_back( phIso );
    
    float abseta = fabs( pho->superCluster()->eta());
    photon_ChIsoWithEA.push_back( std::max( 0.0, chIso - Rho*photon_EA_CH.getEffectiveArea(abseta) ) );
    photon_NhIsoWithEA.push_back( std::max( 0.0, nhIso - Rho*photon_EA_HN.getEffectiveArea(abseta) ) );
    photon_PhIsoWithEA.push_back( std::max( 0.0, phIso - Rho*photon_EA_Ph.getEffectiveArea(abseta) ) );
    
    bool isPassLoose  = pho -> photonID("cutBasedPhotonID-Fall17-94X-V1-loose");
    bool isPassMedium  = pho -> photonID("cutBasedPhotonID-Fall17-94X-V1-medium");
    bool isPassTight  = pho  -> photonID("cutBasedPhotonID-Fall17-94X-V1-tight");
    bool isPassMVA_WP80 = pho -> photonID("mvaPhoID-RunIIFall17-v1-wp80");
    bool isPassMVA_WP90 = pho -> photonID("mvaPhoID-RunIIFall17-v1-wp90");
  
    photon_passMVAID_WP80.push_back( isPassMVA_WP80 );
    photon_passMVAID_WP90.push_back( isPassMVA_WP90 );
    photon_passLooseID.push_back( isPassLoose );
    photon_passMediumID.push_back( isPassMedium );
    photon_passTightID.push_back( isPassTight );




  }
  
}


/////////////////////////
// -- Get METs info -- // 
/////////////////////////
void SKFlatMaker::fillMET(const edm::Event &iEvent)
{
  edm::Handle< std::vector<pat::MET> > metHandle;
  iEvent.getByToken(MetToken,metHandle);
  
  if( (metHandle->size() > 1) && (theDebugLevel > 0)) cout << "[SKFlatMaker::fillMET] # of METs = " << metHandle->size() << endl;
  
  pfMET_pt = metHandle->front().uncorPt();
  pfMET_phi = metHandle->front().uncorPhi();
  pfMET_Px = metHandle->front().uncorPx();
  pfMET_Py = metHandle->front().uncorPy();
  pfMET_SumEt = metHandle->front().uncorSumEt();
  
  pfMET_Type1_pt = metHandle->front().pt();
  pfMET_Type1_phi = metHandle->front().phi();
  pfMET_Type1_Px = metHandle->front().px();
  pfMET_Type1_Py = metHandle->front().py();
  pfMET_Type1_SumEt = metHandle->front().sumEt();
  
  pfMET_Type1_PhiCor_pt = metHandle->front().corPt(pat::MET::Type1XY);
  pfMET_Type1_PhiCor_phi = metHandle->front().corPhi(pat::MET::Type1XY);
  pfMET_Type1_PhiCor_Px = metHandle->front().corPx(pat::MET::Type1XY);
  pfMET_Type1_PhiCor_Py = metHandle->front().corPy(pat::MET::Type1XY);
  pfMET_Type1_PhiCor_SumEt = metHandle->front().corSumEt(pat::MET::Type1XY);


  //cout << "[MET Check]" << endl;
  //cout << "Uncorrected\t" << pfMET_pt << "\t" << pfMET_phi << endl;
  //cout << "Cor(Raw)\t\t" <<  metHandle->front().corPt(pat::MET::Raw) << "\t" << metHandle->front().corPhi(pat::MET::Raw) << endl;
  //cout << "Default(Type1)\t"     << pfMET_Type1_pt << "\t" << pfMET_Type1_phi << endl;
  //cout << "Cor(Type1)\t\t\t" <<  metHandle->front().corPt(pat::MET::Type1) << "\t" << metHandle->front().corPhi(pat::MET::Type1) << endl;


  //==== Uncertainties
  //==== https://github.com/cms-sw/cmssw/blob/4dbb008c8f4473dc9beb26171d7dde863880d02e/DataFormats/PatCandidates/interface/MET.h#L151-L157

  for(int i=0; i<pat::MET::METUncertaintySize; i++){

    pfMET_pt_shifts.push_back( metHandle->front().shiftedPt(pat::MET::METUncertainty(i), pat::MET::Raw) );
    pfMET_phi_shifts.push_back( metHandle->front().shiftedPhi(pat::MET::METUncertainty(i), pat::MET::Raw) );
    pfMET_Px_shifts.push_back( metHandle->front().shiftedPx(pat::MET::METUncertainty(i), pat::MET::Raw) );
    pfMET_Py_shifts.push_back( metHandle->front().shiftedPy(pat::MET::METUncertainty(i), pat::MET::Raw) );
    pfMET_SumEt_shifts.push_back( metHandle->front().shiftedSumEt(pat::MET::METUncertainty(i), pat::MET::Raw) );

    pfMET_Type1_pt_shifts.push_back( metHandle->front().shiftedPt(pat::MET::METUncertainty(i), pat::MET::Type1) );
    pfMET_Type1_phi_shifts.push_back( metHandle->front().shiftedPhi(pat::MET::METUncertainty(i), pat::MET::Type1) );
    pfMET_Type1_Px_shifts.push_back( metHandle->front().shiftedPx(pat::MET::METUncertainty(i), pat::MET::Type1) );
    pfMET_Type1_Py_shifts.push_back( metHandle->front().shiftedPy(pat::MET::METUncertainty(i), pat::MET::Type1) );
    pfMET_Type1_SumEt_shifts.push_back( metHandle->front().shiftedSumEt(pat::MET::METUncertainty(i), pat::MET::Type1) );

    pfMET_Type1_PhiCor_pt_shifts.push_back( metHandle->front().shiftedPt(pat::MET::METUncertainty(i), pat::MET::Type1XY) );
    pfMET_Type1_PhiCor_phi_shifts.push_back( metHandle->front().shiftedPhi(pat::MET::METUncertainty(i), pat::MET::Type1XY) );
    pfMET_Type1_PhiCor_Px_shifts.push_back( metHandle->front().shiftedPx(pat::MET::METUncertainty(i), pat::MET::Type1XY) );
    pfMET_Type1_PhiCor_Py_shifts.push_back( metHandle->front().shiftedPy(pat::MET::METUncertainty(i), pat::MET::Type1XY) );
    pfMET_Type1_PhiCor_SumEt_shifts.push_back( metHandle->front().shiftedSumEt(pat::MET::METUncertainty(i), pat::MET::Type1XY) );

  }


}

/////////////////////////
// -- Get Jets info -- // 
/////////////////////////
void SKFlatMaker::fillJet(const edm::Event &iEvent)
{

  // edm::Handle<edm::View<pat::Jet> > jetHandle;
  edm::Handle< std::vector<pat::Jet> > jetHandle;
  iEvent.getByToken(JetToken,jetHandle);
  
  if( jetHandle->size() > 0 && theDebugLevel > 0) 
    cout << "[SKFlatMaker::fillJet] # of Jets = " << jetHandle->size() << endl;
  
  if(jetHandle->size() == 0) return;

  const JetCollection& ForJER_jets = *jetHandle;
  unsigned int runNum_uint = static_cast <unsigned int> (iEvent.id().run());
  unsigned int lumiNum_uint = static_cast <unsigned int> (iEvent.id().luminosityBlock());
  unsigned int evNum_uint = static_cast <unsigned int> (iEvent.id().event());
  unsigned int jet0eta = uint32_t(ForJER_jets.empty() ? 0 : ForJER_jets[0].eta()/0.01);
  int m_nomVar=1;
  std::uint32_t seed = jet0eta + m_nomVar + (lumiNum_uint<<10) + (runNum_uint<<20) + evNum_uint;
  m_random_generator.seed(seed);

  for (vector<pat::Jet>::const_iterator jets_iter = jetHandle->begin(); jets_iter != jetHandle->end(); ++jets_iter){

    jet_pt.push_back( jets_iter->pt() );
    jet_eta.push_back( jets_iter->eta() );
    jet_phi.push_back( jets_iter->phi() );
    jet_charge.push_back( jets_iter->jetCharge() );
    jet_area.push_back( jets_iter->jetArea() );
    jet_partonFlavour.push_back( jets_iter->partonFlavour() );
    jet_hadronFlavour.push_back( jets_iter->hadronFlavour() );

    //=========================================================================
    //==== Taggers
    //==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
    //=========================================================================

    //=== CSVv2

    jet_CSVv2.push_back( jets_iter->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );

    //==== DeepCSV (B)
/*
    cout << "==============================" << endl;
    cout << "---- BvsAll ----" << endl;
    cout << "methodA = " << jets_iter->bDiscriminator("pfDeepCSVJetTags:probb")+jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb") << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") << endl;
*/
    //==== methodA
    //jet_DeepCSV.push_back( jets_iter->bDiscriminator("pfDeepCSVJetTags:probb")+jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb") );
    //==== methodB
    jet_DeepCSV.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") );

    //==== DeepFlavour

    jet_DeepFlavour_b.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probb"));
    jet_DeepFlavour_bb.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probbb"));
    jet_DeepFlavour_lepb.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:problepb"));
    jet_DeepFlavour_c.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probc"));
    jet_DeepFlavour_uds.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probuds"));
    jet_DeepFlavour_g.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probg"));

    //==== Old Charm Tagger

    jet_CvsL.push_back( jets_iter->bDiscriminator("pfCombinedCvsLJetTags") );
    jet_CvsB.push_back( jets_iter->bDiscriminator("pfCombinedCvsBJetTags") );

    //==== DeepCSV charm tagger

/*
    cout << "---- CvsL ----" << endl;
    cout << "methodA = " << deepcharm_c/(deepcharm_c+deepcharm_udsg) << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsL") << endl;
    cout << "---- CvsB ----" << endl;
    cout << "methodA = " << deepcharm_c/(deepcharm_c+deepcharm_b+deepcharm_bb) << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsB") << endl;
*/
    //==== methodA : it gives value = 0.5 or 0.333 when these are -1.. 
    //double deepcharm_c = jets_iter->bDiscriminator("pfDeepCSVJetTags:probc");
    //double deepcharm_udsg = jets_iter->bDiscriminator("pfDeepCSVJetTags:probudsg");
    //double deepcharm_b = jets_iter->bDiscriminator("pfDeepCSVJetTags:probb");
    //double deepcharm_bb = jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb");
    //jet_DeepCvsL.push_back( deepcharm_c/(deepcharm_c+deepcharm_udsg) );
    //jet_DeepCvsB.push_back( deepcharm_c/(deepcharm_c+deepcharm_b+deepcharm_bb) );
    //==== methodB
    jet_DeepCvsL.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsL") );
    jet_DeepCvsB.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsB") );


    jet_chargedHadronEnergyFraction.push_back( jets_iter->chargedHadronEnergyFraction() );
    jet_neutralHadronEnergyFraction.push_back( jets_iter->neutralHadronEnergyFraction() );
    jet_neutralEmEnergyFraction.push_back( jets_iter->neutralEmEnergyFraction() );
    jet_chargedEmEnergyFraction.push_back( jets_iter->chargedEmEnergyFraction() );
    jet_chargedMultiplicity.push_back( jets_iter->chargedMultiplicity() );
    jet_neutralMultiplicity.push_back( jets_iter->neutralMultiplicity() );

    //==== https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2017
    double NHF  = jets_iter->neutralHadronEnergyFraction();
    double NEMF = jets_iter->neutralEmEnergyFraction();
    double CHF  = jets_iter->chargedHadronEnergyFraction();
    double MUF  = jets_iter->muonEnergyFraction();
    double CEMF = jets_iter->chargedEmEnergyFraction();
    double NumConst = jets_iter->chargedMultiplicity()+jets_iter->neutralMultiplicity();
    double NumNeutralParticles =jets_iter->neutralMultiplicity();
    double CHM      = jets_iter->chargedMultiplicity();
    double eta = jets_iter->eta();

    bool tightJetID        = (NHF<0.90 && NEMF<0.90 && NumConst>1) &&            ((fabs(eta)<=2.4 && CHF>0 && CHM>0)              || fabs(eta)>2.4) && fabs(eta)<=2.7;
    bool tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.80) || fabs(eta)>2.4) && fabs(eta)<=2.7;
    if(fabs(eta)>3.0){
      tightJetID = (NEMF<0.90 && NHF>0.02 && NumNeutralParticles>10) && fabs(eta)>3.0;
      tightLepVetoJetID = false; //FIXME default is false or true? following CAT2016
    }
    else if(fabs(eta)>2.7){
      tightJetID = (NEMF>0.02 && NEMF<0.99) && (NumNeutralParticles>2) && abs(eta)>2.7 && abs(eta)<=3.0;
      tightLepVetoJetID = false; //FIXME default is false or true? following CAT2016
    }

    jet_tightJetID.push_back(tightJetID);
    jet_tightLepVetoJetID.push_back(tightLepVetoJetID);

    int partonPdgId = jets_iter->genParton() ? jets_iter->genParton()->pdgId() : 0;
    jet_partonPdgId.push_back( partonPdgId );

    if( jets_iter->hasUserFloat("vtxNtracks") ) jet_vtxNtracks.push_back( jets_iter->userFloat("vtxNtracks") );
    jet_m.push_back( jets_iter->mass() );
    jet_energy.push_back( jets_iter->energy() );
    if( jets_iter->hasUserFloat("pileupJetId:fullDiscriminant") ) jet_PileupJetId.push_back( jets_iter->userFloat("pileupJetId:fullDiscriminant") );

    //==========
    //==== JEC
    //==========

    //==== https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
    jet_jecUnc->setJetEta( jets_iter->eta() );
    jet_jecUnc->setJetPt( jets_iter->pt() ); // here you must use the CORRECTED jet pt
    double unc = jet_jecUnc->getUncertainty(true);
    jet_shiftedEnUp.push_back( 1.+unc );
    jet_shiftedEnDown.push_back( 1.-unc ); // I found jet_jecUnc->getUncertainty(true) = jet_jecUnc->getUncertainty(false)

    //==== For cross check
    //==== methodB
    //jet_jecUnc_methodB->setJetEta( jets_iter->eta() );
    //jet_jecUnc_methodB->setJetPt( jets_iter->pt() ); 
    //double unc_methodB = jet_jecUnc_methodB->getUncertainty(true);
    //cout << "jec unc methodA = " << unc << endl;
    //cout << "jec unc methodB = " << unc_methodB << endl << endl;

    //=======================================================================================================================
    //==== Save JEC variables for Lepton-aware JEC
    //==== https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/PhysicsTools/NanoAOD/plugins/LeptonJetVarProducer.cc#L154-L184
    //=======================================================================================================================

    //==== Save L1 Correction
    //==== auto L1Correctedjet = jets_iter->correctedP4("L1FastJet"); : "JET corrected up to L1FastJet"
    //==== jets_iter->jecFactor("L1FastJet") = L1Correctedjet.pt()/jets_iter->pt()
    //==== i.e., "FINAL JET" times "jecFactor(L1FastJet)" = "JET corrected up to L1FastJet"
    //==== So, to get L1 correction alone,
    //==== jecFactor("L1FastJet")/jecFactor("Uncorrected")

    jet_JECL1FastJet.push_back( jets_iter->jecFactor("L1FastJet")/jets_iter->jecFactor("Uncorrected") );

    //==== Save Full Correction
    //==== auto uncorjet =  jets_iter->correctedP4("Uncorrected");
    //==== jets_iter->jecFactor("Uncorrected") = uncorjet.pt()/jets_iter->pt()
    auto uncorjet =  jets_iter->correctedP4("Uncorrected");
    //cout << "uncorjet.pt()/jets_iter->pt() = " << uncorjet.pt()/jets_iter->pt() << endl;
    //cout << ( jets_iter->jecFactor("Uncorrected") ) / ( uncorjet.pt()/jets_iter->pt() ) << endl;
    jet_JECFull.push_back( jets_iter->pt()/uncorjet.pt() );

    if(!IsData){

      //==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution#Smearing_procedures
      //==== https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
      //==== https://github.com/cms-sw/cmssw/blob/aa14a75177e9f010b3b8d0af0440598735c49311/PhysicsTools/PatUtils/python/patPFMETCorrections_cff.py

      //==========
      //==== JER
      //==========

      double jer = jet_resolution.getResolution({{JME::Binning::JetPt, jets_iter->pt()}, {JME::Binning::JetEta, jets_iter->eta()}, {JME::Binning::Rho, Rho}});
      double jer_sf = jet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::NOMINAL);
      double jer_sf_UP = jet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::UP);
      double jer_sf_DOWN = jet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::DOWN);

      if(theDebugLevel){

        cout << "#### Jet ####" << endl;
        cout << "pt = " << jets_iter->pt() << endl;
        cout << "eta = " << jets_iter->eta() << endl;
        cout << "Rho = " << Rho << endl;
        cout << "jer = " << jer << endl;
        cout << "jer_sf = " << jer_sf << endl;
        cout << "jer_sf_UP = " << jer_sf_UP << endl;

      }

      const reco::GenJet* genJet = nullptr;
      genJet = match(*jets_iter, jets_iter->pt() * jer, "AK4");
      double smearFactor = 1., smearFactor_UP = 1., smearFactor_DOWN = 1.;
      if(genJet){

        //==== Case 1: we have a "good" gen jet matched to the reco jet

        double dPt = jets_iter->pt() - genJet->pt();
        smearFactor = 1 + (jer_sf - 1.) * dPt / jets_iter->pt();
        smearFactor_UP = 1 + (jer_sf_UP - 1.) * dPt / jets_iter->pt();
        smearFactor_DOWN = 1 + (jer_sf_DOWN - 1.) * dPt / jets_iter->pt();

      }
      else if (jer_sf > 1) {

        //==== Case 2: we don't have a gen jet. Smear jet pt using a random gaussian variation

        double sigma = jer * std::sqrt(jer_sf * jer_sf - 1);
        std::normal_distribution<> d(0, sigma);
        smearFactor = 1. + d(m_random_generator);

        double sigma_UP = jer * std::sqrt(jer_sf_UP * jer_sf_UP - 1);
        std::normal_distribution<> d_UP(0, sigma_UP);
        smearFactor_UP = 1. + d_UP(m_random_generator);

        double sigma_DOWN = jer * std::sqrt(jer_sf_DOWN * jer_sf_DOWN - 1);
        std::normal_distribution<> d_DOWN(0, sigma_DOWN);
        smearFactor_DOWN = 1. + d_DOWN(m_random_generator);  

      }
      else{

      }

      //==== Negative or too small smearFactor. We would change direction of the jet
      //==== and this is not what we want.
      //==== Recompute the smearing factor in order to have jet.energy() == MIN_JET_ENERGY
      double newSmearFactor = MIN_JET_ENERGY / jets_iter->energy();

      smearFactor = std::max(smearFactor,newSmearFactor);
      smearFactor_UP = std::max(smearFactor_UP,newSmearFactor);
      smearFactor_DOWN = std::max(smearFactor_DOWN,newSmearFactor);

      if(theDebugLevel){

        cout << "--->" << endl;
        cout << "smearFactor = " << smearFactor << endl;
        cout << "smearFactor_UP = " << smearFactor_UP << endl;
        cout << "smearFactor_DOWN = " << smearFactor_DOWN << endl;

      }

      jet_smearedRes.push_back(smearFactor);
      jet_smearedResUp.push_back(smearFactor_UP);
      jet_smearedResDown.push_back(smearFactor_DOWN);

    }

    //cout << "QGTagger:qgLikelihood = " << jets_iter->userFloat("QGTagger:qgLikelihood") << endl;


  } 
  
}

/////////////////////////////
// -- Get FatJets info -- // 
////////////////////////////
void SKFlatMaker::fillFatJet(const edm::Event &iEvent)
{

  // edm::Handle<edm::View<pat::Jet> > jetHandle;
  edm::Handle< std::vector<pat::Jet> > jetHandle;
  iEvent.getByToken(FatJetToken,jetHandle);

  if( jetHandle->size() > 0 && theDebugLevel > 0)
    cout << "[SKFlatMaker::fillFatJet] # of FatJets = " << jetHandle->size() << endl;

  if(jetHandle->size() == 0) return;

  const JetCollection& ForJER_jets = *jetHandle;
  unsigned int runNum_uint = static_cast <unsigned int> (iEvent.id().run());
  unsigned int lumiNum_uint = static_cast <unsigned int> (iEvent.id().luminosityBlock());
  unsigned int evNum_uint = static_cast <unsigned int> (iEvent.id().event());
  unsigned int jet0eta = uint32_t(ForJER_jets.empty() ? 0 : ForJER_jets[0].eta()/0.01);
  int m_nomVar=1;
  std::uint32_t seed = jet0eta + m_nomVar + (lumiNum_uint<<10) + (runNum_uint<<20) + evNum_uint;
  m_random_generator.seed(seed);

  for (vector<pat::Jet>::const_iterator jets_iter = jetHandle->begin(); jets_iter != jetHandle->end(); ++jets_iter){

    //==== https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1785/1/1.html
    //==== FatJet with pt 30~170 GeV are only for SM jet analysis
    //==== We can apply pt cut here

    if(jets_iter->pt()<=170.) continue;

    //==== https://hypernews.cern.ch/HyperNews/CMS/get/jet-algorithms/443/2.html
    fatjet_pt.push_back( jets_iter->pt() );
    fatjet_eta.push_back( jets_iter->eta() );
    fatjet_phi.push_back( jets_iter->phi() );
    fatjet_charge.push_back( jets_iter->jetCharge() );
    fatjet_area.push_back( jets_iter->jetArea() );
    fatjet_partonFlavour.push_back( jets_iter->partonFlavour() );
    fatjet_hadronFlavour.push_back( jets_iter->hadronFlavour() );

    //=========================================================================
    //==== Taggers
    //==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
    //=========================================================================

    //=== CSVv2

    fatjet_CSVv2.push_back( jets_iter->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );

    //==== DeepCSV (B)
/*
    cout << "==============================" << endl;
    cout << "---- BvsAll ----" << endl;
    cout << "methodA = " << jets_iter->bDiscriminator("pfDeepCSVJetTags:probb")+jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb") << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") << endl;
*/
    //==== methodA
    //fatjet_DeepCSV.push_back( jets_iter->bDiscriminator("pfDeepCSVJetTags:probb")+jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb") );
    //==== methodB
    fatjet_DeepCSV.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") );

    //==== DeepFlavour

    fatjet_DeepFlavour_b.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probb"));
    fatjet_DeepFlavour_bb.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probbb"));
    fatjet_DeepFlavour_lepb.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:problepb"));
    fatjet_DeepFlavour_c.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probc"));
    fatjet_DeepFlavour_uds.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probuds"));
    fatjet_DeepFlavour_g.push_back( jets_iter->bDiscriminator("pfDeepFlavourJetTags:probg"));

    //==== Old Charm Tagger

    fatjet_CvsL.push_back( jets_iter->bDiscriminator("pfCombinedCvsLJetTags") );
    fatjet_CvsB.push_back( jets_iter->bDiscriminator("pfCombinedCvsBJetTags") );

    //==== DeepCSV charm tagger

/*
    cout << "---- CvsL ----" << endl;
    cout << "methodA = " << deepcharm_c/(deepcharm_c+deepcharm_udsg) << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsL") << endl;
    cout << "---- CvsB ----" << endl;
    cout << "methodA = " << deepcharm_c/(deepcharm_c+deepcharm_b+deepcharm_bb) << endl;
    cout << "methodB = " << jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsB") << endl;
*/
    //==== methodA
    //double deepcharm_c = jets_iter->bDiscriminator("pfDeepCSVJetTags:probc");
    //double deepcharm_udsg = jets_iter->bDiscriminator("pfDeepCSVJetTags:probudsg");
    //double deepcharm_b = jets_iter->bDiscriminator("pfDeepCSVJetTags:probb");
    //double deepcharm_bb = jets_iter->bDiscriminator("pfDeepCSVJetTags:probbb");
    //fatjet_DeepCvsL.push_back( deepcharm_c/(deepcharm_c+deepcharm_udsg) );
    //fatjet_DeepCvsB.push_back( deepcharm_c/(deepcharm_c+deepcharm_b+deepcharm_bb) );
    //==== methodB
    fatjet_DeepCvsL.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsL") );
    fatjet_DeepCvsB.push_back( jets_iter->bDiscriminator("pfDeepCSVDiscriminatorsJetTags:CvsB") );


    fatjet_chargedHadronEnergyFraction.push_back( jets_iter->chargedHadronEnergyFraction() );
    fatjet_neutralHadronEnergyFraction.push_back( jets_iter->neutralHadronEnergyFraction() );
    fatjet_neutralEmEnergyFraction.push_back( jets_iter->neutralEmEnergyFraction() );
    fatjet_chargedEmEnergyFraction.push_back( jets_iter->chargedEmEnergyFraction() );
    fatjet_chargedMultiplicity.push_back( jets_iter->chargedMultiplicity() );
    fatjet_neutralMultiplicity.push_back( jets_iter->neutralMultiplicity() );

    //==== https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2017
    //==== TODO now, only AK4 ID are provided.. But I just put AK4 jet requirements here
    double NHF  = jets_iter->neutralHadronEnergyFraction();
    double NEMF = jets_iter->neutralEmEnergyFraction();
    double CHF  = jets_iter->chargedHadronEnergyFraction();
    double MUF  = jets_iter->muonEnergyFraction();
    double CEMF = jets_iter->chargedEmEnergyFraction();
    double NumConst = jets_iter->chargedMultiplicity()+jets_iter->neutralMultiplicity();
    double NumNeutralParticles =jets_iter->neutralMultiplicity();
    double CHM      = jets_iter->chargedMultiplicity();
    double eta = jets_iter->eta();

    bool tightJetID        = (NHF<0.90 && NEMF<0.90 && NumConst>1) &&            ((fabs(eta)<=2.4 && CHF>0 && CHM>0)              || fabs(eta)>2.4) && fabs(eta)<=2.7;
    bool tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.80) || fabs(eta)>2.4) && fabs(eta)<=2.7;
    if(fabs(eta)>3.0){
      tightJetID = (NEMF<0.90 && NHF>0.02 && NumNeutralParticles>10) && fabs(eta)>3.0;
      tightLepVetoJetID = false; //FIXME default is false or true? following CAT2016
    }
    else if(fabs(eta)>2.7){
      tightJetID = (NEMF>0.02 && NEMF<0.99) && (NumNeutralParticles>2) && abs(eta)>2.7 && abs(eta)<=3.0;
      tightLepVetoJetID = false; //FIXME default is false or true? following CAT2016
    }

    fatjet_tightJetID.push_back(tightJetID);
    fatjet_tightLepVetoJetID.push_back(tightLepVetoJetID);

    int partonPdgId = jets_iter->genParton() ? jets_iter->genParton()->pdgId() : 0;
    fatjet_partonPdgId.push_back( partonPdgId );

    if( jets_iter->hasUserFloat("vtxNtracks") ) fatjet_vtxNtracks.push_back( jets_iter->userFloat("vtxNtracks") );
    fatjet_m.push_back( jets_iter->mass() );
    fatjet_energy.push_back( jets_iter->energy() );

    fatjet_puppi_tau1.push_back( jets_iter->userFloat("NjettinessAK8Puppi:tau1") );
    fatjet_puppi_tau2.push_back( jets_iter->userFloat("NjettinessAK8Puppi:tau2") );
    fatjet_puppi_tau3.push_back( jets_iter->userFloat("NjettinessAK8Puppi:tau3") );
    fatjet_puppi_tau4.push_back( jets_iter->userFloat("NjettinessAK8Puppi:tau4") );
    fatjet_softdropmass.push_back( jets_iter->userFloat("ak8PFJetsPuppiSoftDropMass") );

    //==== https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
    fatjet_jecUnc->setJetEta( jets_iter->eta() );
    fatjet_jecUnc->setJetPt( jets_iter->pt() ); // here you must use the CORRECTED jet pt
    double unc = fatjet_jecUnc->getUncertainty(true);
    fatjet_shiftedEnUp.push_back( 1.+unc );
    fatjet_shiftedEnDown.push_back( 1.-unc ); // I found fatjet_jecUnc->getUncertainty(true) = fatjet_jecUnc->getUncertainty(false)

    if(!IsData){

      //==========
      //==== JER
      //==========

      double jer = fatjet_resolution.getResolution({{JME::Binning::JetPt, jets_iter->pt()}, {JME::Binning::JetEta, jets_iter->eta()}, {JME::Binning::Rho, Rho}});
      double jer_sf = fatjet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::NOMINAL);
      double jer_sf_UP = fatjet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::UP);
      double jer_sf_DOWN = fatjet_resolution_sf.getScaleFactor({{JME::Binning::JetEta, jets_iter->eta()}}, Variation::DOWN);

      if(theDebugLevel){

        cout << "#### FatJet ####" << endl;
        cout << "pt = " << jets_iter->pt() << endl;
        cout << "eta = " << jets_iter->eta() << endl;
        cout << "Rho = " << Rho << endl;
        cout << "jer = " << jer << endl;
        cout << "jer_sf = " << jer_sf << endl;
        cout << "jer_sf_UP = " << jer_sf_UP << endl;

      }

      const reco::GenJet* genJet = nullptr;
      genJet = match(*jets_iter, jets_iter->pt() * jer, "AK8");
      double smearFactor = 1., smearFactor_UP = 1., smearFactor_DOWN = 1.;
      if(genJet){

        //==== Case 1: we have a "good" gen jet matched to the reco jet

        double dPt = jets_iter->pt() - genJet->pt();
        smearFactor = 1 + (jer_sf - 1.) * dPt / jets_iter->pt();
        smearFactor_UP = 1 + (jer_sf_UP - 1.) * dPt / jets_iter->pt();
        smearFactor_DOWN = 1 + (jer_sf_DOWN - 1.) * dPt / jets_iter->pt();

      }
      else if (jer_sf > 1) {

        //==== Case 2: we don't have a gen jet. Smear jet pt using a random gaussian variation

        double sigma = jer * std::sqrt(jer_sf * jer_sf - 1);
        std::normal_distribution<> d(0, sigma);
        smearFactor = 1. + d(m_random_generator);

        double sigma_UP = jer * std::sqrt(jer_sf_UP * jer_sf_UP - 1);
        std::normal_distribution<> d_UP(0, sigma_UP);
        smearFactor_UP = 1. + d_UP(m_random_generator);

        double sigma_DOWN = jer * std::sqrt(jer_sf_DOWN * jer_sf_DOWN - 1);
        std::normal_distribution<> d_DOWN(0, sigma_DOWN);
        smearFactor_DOWN = 1. + d_DOWN(m_random_generator);

      }
      else{

      }

      //==== Negative or too small smearFactor. We would change direction of the jet
      //==== and this is not what we want.
      //==== Recompute the smearing factor in order to have jet.energy() == MIN_JET_ENERGY
      double newSmearFactor = MIN_JET_ENERGY / jets_iter->energy();

      smearFactor = std::max(smearFactor,newSmearFactor);
      smearFactor_UP = std::max(smearFactor_UP,newSmearFactor);
      smearFactor_DOWN = std::max(smearFactor_DOWN,newSmearFactor);

      if(theDebugLevel){

        cout << "--->" << endl;
        cout << "smearFactor = " << smearFactor << endl;
        cout << "smearFactor_UP = " << smearFactor_UP << endl;
        cout << "smearFactor_DOWN = " << smearFactor_DOWN << endl;

      }

      fatjet_smearedRes.push_back(smearFactor);
      fatjet_smearedResUp.push_back(smearFactor_UP);
      fatjet_smearedResDown.push_back(smearFactor_DOWN);

    }


  }

}

//////////////////////////////////////////////////////
// -- Get Tracker track info (all single tracks) -- //
//////////////////////////////////////////////////////
void SKFlatMaker::fillTT(const edm::Event &iEvent)
{
  edm::Handle<edm::View<reco::Track> > trackHandle;
  iEvent.getByToken(TrackToken, trackHandle);
  
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(BeamSpotToken, beamSpotHandle);
  reco::BeamSpot beamSpot = (*beamSpotHandle);
  
  edm::Handle<reco::VertexCollection> _pvHandle;
  iEvent.getByToken(PrimaryVertexToken, _pvHandle);
  const reco::Vertex &vtx = _pvHandle->front();
  
  // to store gsftracks close to electrons
  edm::Handle< std::vector< reco::GsfTrack > > gsfTracks; 
  iEvent.getByToken(GsfTrackToken, gsfTracks);
  
  for(unsigned int igsf = 0; igsf < gsfTracks->size(); igsf++ ){
    GsfTrackRef iTT(gsfTracks, igsf);

    //if( iTT->pt() < 1.0 || iTT->pt() > 100000 ) continue;
    bool _isMatch = false;
    for( int i = 0; i < Nelectrons; i++ ){
      double dpT = fabs(iTT->pt() - electron_gsfpt[i]);
      double dR = deltaR(iTT->eta(), iTT->phi(), electron_gsfEta[i], electron_gsfPhi[i]);
      //cout << "elec = " << i << " " << electron_gsfpt[i] << " " << electron_gsfEta[i] << " " << electron_gsfPhi[i] << " " << dR << endl;
      if( dR < 0.001 && dpT < 1.0 ) _isMatch = true;
    }
    if( _isMatch ) continue;
    
    TrackerTrack_dxy.push_back( iTT->dxy(vtx.position()) );
    TrackerTrack_dxyErr.push_back( iTT->dxyError() );
    TrackerTrack_d0.push_back( iTT->d0() );
    TrackerTrack_d0Err.push_back( iTT->d0Error() );
    TrackerTrack_dsz.push_back( iTT->dsz(vtx.position()) );
    TrackerTrack_dszErr.push_back( iTT->dszError() );
    TrackerTrack_dz.push_back( iTT->dz(vtx.position()) );
    TrackerTrack_dzErr.push_back( iTT->dzError() );
    TrackerTrack_dxyBS.push_back( iTT->dxy(beamSpot.position()) );
    TrackerTrack_dszBS.push_back( iTT->dsz(beamSpot.position()) );
    TrackerTrack_dzBS.push_back( iTT->dz(beamSpot.position()) );
    TrackerTrack_pt.push_back( iTT->pt() );
    TrackerTrack_Px.push_back( iTT->px() );
    TrackerTrack_Py.push_back( iTT->py() );
    TrackerTrack_Pz.push_back( iTT->pz() );
    TrackerTrack_eta.push_back( iTT->eta() );
    TrackerTrack_phi.push_back( iTT->phi() );
    TrackerTrack_charge.push_back( iTT->charge() );
  } // -- end of for(unsigned igsf = 0; igsf < gsfTracks->size(); igsf++ ): GSFTrack iteration -- //
  
}

void SKFlatMaker::endRun(const Run & iRun, const EventSetup & iSetup)
{
  if(theDebugLevel) cout << "[SKFlatMaker::endRun] called" << endl;

  // -- only when LHE information is available (ex> aMC@NLO, Powheg) case. Samples generated by pythia8 doesn't work! -- //
  if( this->theStoreLHEFlag ){
    // -- LHE information -- //
    // -- ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW#Retrieving_the_weights -- //
    edm::Handle<LHERunInfoProduct> LHERunInfo;
    iRun.getByToken(LHERunInfoProductToken, LHERunInfo);
    if(!LHERunInfo.isValid()) return;

    cout << "[SKFlatMaker::endRun] ##### Information about PDF weights #####" << endl;
    LHERunInfoProduct myLHERunInfoProduct = *(LHERunInfo.product());
    typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
    for(headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
      std::cout << "[SKFlatMaker::endRun,tag] " << iter->tag() << std::endl;
      std::vector<std::string> lines = iter->lines();
      for (unsigned int iLine = 0; iLine<lines.size(); iLine++){
        std::cout << "[SKFlatMaker::endRun,lines] " << lines.at(iLine);
      }
    }
    cout << "[SKFlatMaker::endRun] ##### End of information about PDF weights #####" << endl;
  }

}

float SKFlatMaker::miniIsoDr(const math::XYZTLorentzVector &p4, float mindr, float maxdr, float kt_scale){
  return std::max(mindr, std::min(maxdr, float(kt_scale/p4.pt())));
}

PFIsolation SKFlatMaker::GetMiniIso(edm::Handle<pat::PackedCandidateCollection> pfcands,
                                 const math::XYZTLorentzVector &p4,
                                 float mindr, float maxdr, float kt_scale,
                                 float ptthresh, float deadcone_ch, float deadcone_pu,
                                 float deadcone_ph, float deadcone_nh, float dZ_cut){

/*
  cout << "[SKFlatMaker::GetMiniIso] mindr = " << mindr << endl;
  cout << "[SKFlatMaker::GetMiniIso] maxdr = " << maxdr << endl;
  cout << "[SKFlatMaker::GetMiniIso] kt_scale = " << kt_scale << endl;
  cout << "[SKFlatMaker::GetMiniIso] ptthresh = " << ptthresh << endl;
  cout << "[SKFlatMaker::GetMiniIso] deadcone_ch = " << deadcone_ch << endl;
  cout << "[SKFlatMaker::GetMiniIso] deadcone_pu = " << deadcone_pu << endl;
  cout << "[SKFlatMaker::GetMiniIso] deadcone_ph = " << deadcone_ph << endl;
  cout << "[SKFlatMaker::GetMiniIso] deadcone_nh = " << deadcone_nh << endl;
  cout << "[SKFlatMaker::GetMiniIso] dZ_cut = " << dZ_cut << endl;
*/

  float chiso=0, nhiso=0, phiso=0, puiso=0;
  float drcut = miniIsoDr(p4,mindr,maxdr,kt_scale);
  for(const pat::PackedCandidate &pc : *pfcands){
    float dr = deltaR(p4, pc.p4());
    if(dr>drcut)  continue;
    float pt = pc.p4().pt();
    int id = pc.pdgId();
    if(std::abs(id)==211){
      bool fromPV = (pc.fromPV()>1 || fabs(pc.dz()) < dZ_cut);
      if(fromPV && dr > deadcone_ch){
        //==== if charged hadron and from primary vertex, add to charged hadron isolation
        chiso += pt;
      }
      else if(!fromPV && pt > ptthresh && dr > deadcone_pu){
        //==== if charged hadron and NOT from primary vertex, add to pileup isolation
        puiso += pt;
      }
    }
    //==== if neutral hadron, add to neutral hadron isolation
    if(std::abs(id)==130 && pt>ptthresh && dr>deadcone_nh)
      nhiso += pt;
    //==== if photon, add to photon isolation
    if(std::abs(id)==22 && pt>ptthresh && dr>deadcone_ph)
      phiso += pt;

  }

  return pat::PFIsolation(chiso, nhiso, phiso, puiso);

}

template<class T>
const reco::GenJet* SKFlatMaker::match(const T& jet, double resolution, TString whichjet){

  edm::Handle<reco::GenJetCollection> this_genjets;

  double m_dR_max = 0.4/2.;
  if(whichjet=="AK4"){
    this_genjets = m_genJets;
    m_dR_max = 0.4/2.;
  }
  else if(whichjet=="AK8"){
    this_genjets = m_genFatJets;
    m_dR_max = 0.8/2.;
  }
  else{
    cout << "[const reco::GenJet* SKFlatMaker::match] whichjet = " << whichjet << endl;
  }

  const reco::GenJetCollection& genJets = *this_genjets;

  //==== Try to find a gen jet matching
  //==== dR < m_dR_max
  //==== dPt < m_dPt_max_factor * resolution

  double m_dPt_max_factor = 3.;

  double min_dR = std::numeric_limits<double>::infinity();
  const reco::GenJet* matched_genJet = nullptr;

  for (const auto& genJet: genJets) {
      double dR = deltaR(genJet, jet);

      if (dR > min_dR)
          continue;

      if (dR < m_dR_max ) {
          double dPt = std::abs(genJet.pt() - jet.pt());
          if (dPt > m_dPt_max_factor * resolution)
              continue;

          min_dR = dR;
          matched_genJet = &genJet;
      }
  }

  return matched_genJet;
}

//define this as a plug-in
DEFINE_FWK_MODULE(SKFlatMaker);
