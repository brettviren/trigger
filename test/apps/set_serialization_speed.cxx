#include "trigger/TPSet.hpp"
#include "trigger/TASet.hpp"
#include "serialization/Serialization.hpp"
#include "triggeralgs/TriggerPrimitive.hpp"
#include "logging/Logging.hpp"

#include <chrono>
#include <iostream>
#include <random>

// Return the current steady clock in microseconds
inline uint64_t // NOLINT(build/unsigned)
now_us()
{
  using namespace std::chrono;
  // std::chrono is the worst
  return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

void
time_serialization(int tps_per_set)
{
  const int N = 100000;
  int total = 0;

  std::default_random_engine generator;
  std::uniform_int_distribution<int> uniform(0, 1000);

  std::vector<dunedaq::trigger::TPSet> sets;
  for(int i=0; i<N; ++i){
    dunedaq::trigger::TPSet set;
    set.seqno=i+1;
    set.start_time = (i+1)*5000;
    set.end_time = (i+2)*5000-1;
    set.from_detids={1,2,3};
    for(int j=0; j<tps_per_set; ++j){
      triggeralgs::TriggerPrimitive tp;
      tp.time_start = 1234963454;
      tp.time_peak = tp.time_start+10000;
      tp.time_over_threshold = uniform(generator);
      tp.channel = uniform(generator);
      tp.adc_integral = uniform(generator);
      tp.adc_peak = uniform(generator);
      tp.detid = 1;
      tp.type = 1;
      tp.algorithm = 1;
      tp.version = 1;
      tp.flag = 1;

      set.objects.push_back(tp);
    }
    sets.push_back(set);
  }

  uint64_t start_time = now_us(); // NOLINT(build/unsigned)

  for (int i = 0; i < N; ++i) {
    std::vector<uint8_t> bytes = dunedaq::serialization::serialize(sets[i], dunedaq::serialization::kMsgPack); // NOLINT(build/unsigned)
    dunedaq::trigger::TPSet set_recv = dunedaq::serialization::deserialize<dunedaq::trigger::TPSet>(bytes);
    total += set_recv.seqno;
  }
  uint64_t end_time = now_us(); // NOLINT(build/unsigned)
  
  double time_taken_s = 1e-6 * (end_time - start_time);
  double msg_kHz = 1e-3 * N / time_taken_s;
  double tp_kHz = 1e-3 * tps_per_set * N / time_taken_s;
  std::cout << "Sent " << N << " messages in " << time_taken_s << " (" << msg_kHz << " kHz of msgs, " << tp_kHz << " kHz of TPs) " << total << std::endl; // NOLINT

}

int main()
{
  std::vector<int> n_tps{0, 1, 10, 100, 1000};
  for(auto n: n_tps){
    std::cout << n << " TPs per set: " << std::flush;
    time_serialization(n);
  }

  dunedaq::trigger::TASet taset;
  std::vector<uint8_t> bytes = dunedaq::serialization::serialize(taset, dunedaq::serialization::kMsgPack); // NOLINT(build/unsigned)
  dunedaq::trigger::TASet set_recv = dunedaq::serialization::deserialize<dunedaq::trigger::TASet>(bytes);
  
}
