#ifndef SBND_FLASHMATCH_FLASHPREDICT_HH
#define SBND_FLASHMATCH_FLASHPREDICT_HH

// save diagnostic state
#pragma GCC diagnostic push

// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wconversion"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art_root_io/TFileService.h"
//#include "art/Framework/Services/Optional/TFileService.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "larpandora/LArPandoraInterface/LArPandoraHelper.h"
//#include "larpandora/LArPandoraEventBuilding/LArPandoraSliceIdHelper.h"
//#include "larpandora/LArPandoraEventBuilding/SliceIdBaseTool.h"
//#include "larpandora/LArPandoraEventBuilding/Slice.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcorealg/Geometry/WireGeo.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "art/Persistency/Common/PtrMaker.h"

#include "TTree.h"
#include "TFile.h"
// #include "TGraph.h"
#include "TH1.h"

#include "sbndcode/OpDetSim/OpT0FinderTypes.h"
#include "sbndcode/OpDetSim/sbndPDMapAlg.hh"

// turn the warnings back on
#pragma GCC diagnostic pop


#include <memory>
#include <string>
#include <algorithm>

class FlashPredict;
class FlashPredict : public art::EDProducer {

public:
  explicit FlashPredict(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.
  // Plugins should not be copied or assigned.
  FlashPredict(FlashPredict const&) = delete;
  FlashPredict(FlashPredict&&) = delete;
  FlashPredict& operator=(FlashPredict const&) = delete;
  FlashPredict& operator=(FlashPredict&&) = delete;
  // Required functions.
  void produce(art::Event& e) override;
  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:
  // Declare member data here.
  //  ::flashana::FlashMatchManager m_flashMatchManager; ///< The flash match manager
  // art::InputTag fFlashProducer;
  // art::InputTag fT0Producer; // producer for ACPT in-time anab::T0 <-> recob::Track assocaition
  std::string fPandoraProducer, fSpacePointProducer, fOpHitProducer, fInputFilename, fCaloProducer, fTrackProducer;
  double fBeamWindowEnd, fBeamWindowStart;
  double fLightWindowEnd, fLightWindowStart;
  double fMinFlashPE;
  double fDriftDistance;
  double fPEscale;
  double fChargeToNPhotonsShower, fChargeToNPhotonsTrack;
  std::string fDetector; // SBND or ICARUS
  int fCryostat;  // =0 or =1 to match ICARUS reco chain selection
  bool fNoAvailableMetrics, fMakeTree, fSelectNeutrino, fUseUncoatedPMT, fUseCalo;
  double fTermThreshold;
  std::vector<double> fPMTChannelCorrection;
  // geometry service
  static const size_t nMaxTPCs = 2; // ICARUS has 4 TPCs, however they need to be run independently
  std::array<flashana::QCluster_t, nMaxTPCs> qClusterInTPC;

  void computeFlashMetrics(size_t idtpc, std::vector<recob::OpHit> const& OpHitSubset);
  ::flashana::Flash_t GetFlashPESpectrum(const recob::OpFlash& opflash);
  void CollectDownstreamPFParticles(const lar_pandora::PFParticleMap &pfParticleMap,
                                    const art::Ptr<recob::PFParticle> &particle,
                                    lar_pandora::PFParticleVector &downstreamPFParticles) const;
  void CollectDownstreamPFParticles(const lar_pandora::PFParticleMap &pfParticleMap,
                                    const lar_pandora::PFParticleVector &parentPFParticles,
                                    lar_pandora::PFParticleVector &downstreamPFParticles) const;
  void AddDaughters(const art::Ptr<recob::PFParticle>& pfp_ptr,
                    const art::ValidHandle<std::vector<recob::PFParticle> >& pfp_h,
                    std::vector<art::Ptr<recob::PFParticle> > &pfp_v);
  bool isPDInCryoTPC(double pd_x, int icryo, size_t itpc, std::string detector);
  bool isPDInCryoTPC(int pdChannel, int icryo, size_t itpc, std::string detector);
  bool isChargeInCryoTPC(double qp_x, int icryo, int itpc, std::string detector);

  int icountPE = 0;
  const art::ServiceHandle<geo::Geometry> geometry;
  opdet::sbndPDMapAlg pdMap; // SBND opdets map

  // root stuff
  TTree* _flashmatch_nuslice_tree;
  TH1D *ophittime;
  TH1D *ophittime2;

  // Tree variables
  std::vector<double> _pe_reco_v, _pe_hypo_v;
  Double_t _charge_x, _charge_y, _charge_z, _charge_q;
  Double_t _flash_x, _flash_y, _flash_z, _flash_r, _flash_pe, _flash_unpe;
  // TODO: why not charge_time?
  Double_t _flash_time;
  Double_t _score;
  Int_t _evt, _run, _sub;

  std::map<size_t, size_t> _pfpmap;

  std::vector<double> dy_means, dz_means, rr_means, pe_means;
  std::vector<double> dy_spreads, dz_spreads, rr_spreads, pe_spreads;
  int n_bins;

};


#endif //SBND_FLASHMATCH_FLASHPREDICT_HH
