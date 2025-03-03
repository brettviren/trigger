#include "TriggerCandidateMaker.hpp"

#include "trigger/AlgorithmPlugins.hpp"
#include "trigger/triggercandidatemaker/Nljs.hpp"

namespace dunedaq::trigger {

  std::shared_ptr<triggeralgs::TriggerCandidateMaker>
  TriggerCandidateMaker::make_maker(const nlohmann::json& obj)
  {
    auto params = obj.get<triggercandidatemaker::Conf>();
    return make_tc_maker(params.candidate_maker);
  }

} // namespace dunedaq::trigger

DEFINE_DUNE_DAQ_MODULE(dunedaq::trigger::TriggerCandidateMaker)
