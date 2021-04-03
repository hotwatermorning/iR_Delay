//----------------------------------------------------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//----------------------------------------------------------------------------------------------------------------------
#ifndef IR_DELAY_H
#define IR_DELAY_H

#include "RingBuffer.h"
#include "iR_Filters.h"
#include "iR_Utility.h"
#include "iR_RMSDetector.h"

#include <cmath>

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// Enums
//----------------------------------------------------------------------------------------------------------------------
enum class DelayMode
{
  kNormal, kPingpong
};

enum class DelayChannel
{
  kLeftChannel, kRightChannel
};

enum class DelaySyncBeat
{
  k1_64T, k1_64, k1_32T, k1_64D, k1_32, k1_16T, k1_32D,
  k1_16, k1_8T, k1_16D, k1_8, k1_4T, k1_8D, k1_4,
  k1_2T, k1_4D, k1_2, k1_2D, k1_Bar, k2_Bar, k4_Bar
};

//----------------------------------------------------------------------------------------------------------------------
// Delay Class
//----------------------------------------------------------------------------------------------------------------------
class Delay
{
public:
  // constructor
  Delay() = delete;
  Delay(float samplerate, float max_time, DelayMode initial_mode = DelayMode::kNormal);

  // destructor
  ~Delay();

  // common method
  void calculate();
  void applyDelay(float* in_L, float* in_R = nullptr);
  void clearBuffer() { ringbuffer_L_->clear(); ringbuffer_R_->clear(); }

  // getter
  DelayMode getMode() const { return mode_; }
  float getTime(DelayChannel channel) { return isChannelLeft(channel) ? time_L_ : time_R_; }
  float getLPFFreq() const { return lpf_freq_; }
  float getHPFFreq() const { return hpf_freq_; }
  float getDumping() const { return dumping_; }
  bool getEnableBPMSync() const { return enable_bpm_sync; }
  DelaySyncBeat getSyncBeat(DelayChannel channel) { return isChannelLeft(channel) ? sync_beat_L_ : sync_beat_R_; }
  bool getEnableTimeLink() const { return enable_time_link_; }
  float getFeedback(DelayChannel channel) { return isChannelLeft(channel) ? feedback_L_ : feedback_R_; }
  bool getEnableFeedbackLink() const { return enable_feedback_link_; }
  bool getEnableSwapChannel() const { return enable_swap_channel; }
  float getStereoWidth() const { return stereo_width_; }
  float getDucking() const { return ducking_; }
  float getMix() const { return mix_; }

  // setter
  void setMode(DelayMode new_mode);
  void setTime(float new_time, DelayChannel channel);
  void setLPFFreq(float new_freq);
  void setHPFFreq(float new_freq);
  void setDumping(float new_dumping) { dumping_ = new_dumping; }
  void setTempo(float new_tempo) { tempo_ = new_tempo; }
  void setEnableBPMSync(bool on_off = true) { enable_bpm_sync = on_off; }
  void setSyncBeat(DelaySyncBeat new_beat, DelayChannel channel);
  void setEnableTimeLink(bool on_off = true) { enable_time_link_ = on_off; }
  void setFeedback(float new_feedback, DelayChannel channel);
  void setEnableFeedbackLink(bool on_off = true) { enable_feedback_link_ = on_off; }
  void setEnableSwapChannel(bool on_off = true) { enable_swap_channel = on_off; }
  void setStereoWidth(float new_width) { stereo_width_ = new_width; }
  void setDucking(float new_ducking) { ducking_ = new_ducking; }
  void setMix(float new_mix) { mix_ = new_mix; }

//----------------------------------------------------------------------------------------------------------------------
 private:
  // main process method
  void applyNormalDelay(float* in_L, float* in_R);
  void applyNormalDelayMono(float* in);
  void applyPingpongDelay(float* in_L, float* in_R);

  void output(float* in_L, float* in_R);

  // helper
  bool isChannelLeft(DelayChannel channel) { return channel == DelayChannel::kLeftChannel; }
  float getSyncTimeByBeat(float tempo, DelayChannel channel);

  // member variable
  RingBuffer<float>* ringbuffer_L_ = nullptr;
  RingBuffer<float>* ringbuffer_R_ = nullptr;
  DelayMode mode_ = DelayMode::kNormal;
  const float max_time_ = 4.0f;

  LowPassFilter* lpf_ = nullptr;
  HighPassFilter* hpf_ = nullptr;
  float lpf_freq_ = 1100.0f;
  float hpf_freq_ = 700.0f;
  float dumping_ = 0.5f;

  float samplerate_ = 44100.0f;
  float tempo_ = 120.0f;

  float time_L_ = 1.0f, time_R_ = 1.0f;
  bool enable_bpm_sync = true;
  DelaySyncBeat sync_beat_L_ = DelaySyncBeat::k1_4;
  DelaySyncBeat sync_beat_R_ = DelaySyncBeat::k1_4;
  float sync_time_L_, sync_time_R_;
  bool enable_time_link_ = true;

  float feedback_L_ = 0.5f, feedback_R_ = 0.5f;
  float feedback_signal_L_ = 0.0f;
  float feedback_signal_R_ = 0.0f;
  bool enable_feedback_link_ = true;

  bool enable_swap_channel = false;
  float stereo_width_ = 1.0f;
  float mix_ = 0.5f;

  RMSDetector* rms_detector_ = nullptr;
  float ducking_ = 0.0f;
};

//----------------------------------------------------------------------------------------------------------------------
// Delay Class Implementation
//----------------------------------------------------------------------------------------------------------------------
// constructor
inline Delay::Delay(float samplerate, float max_time, DelayMode initial_mode) :
  samplerate_(samplerate),
  ringbuffer_L_(new RingBuffer<float>((int)(samplerate * max_time + 100))),
  ringbuffer_R_(new RingBuffer<float>((int)(samplerate * max_time + 100))),
  mode_(initial_mode),
  max_time_(max_time),
  lpf_(new LowPassFilter(1, 1100.0f, 0.7f)),
  hpf_(new HighPassFilter(1, 700.0f, 0.7f)),
  rms_detector_(new RMSDetector(samplerate, 20, 0.9999f))
{}

// destructor
inline Delay::~Delay()
{
  if (lpf_) lpf_->release();
  if (hpf_) hpf_->release();
  delete ringbuffer_L_;
  delete ringbuffer_R_;
  delete rms_detector_;
}

// common method
inline void Delay::calculate()
{
  if (enable_bpm_sync) {
    sync_time_L_ = getSyncTimeByBeat(tempo_, DelayChannel::kLeftChannel);
    sync_time_R_ = getSyncTimeByBeat(tempo_, DelayChannel::kRightChannel);
  }
  lpf_->calculate(samplerate_);
  hpf_->calculate(samplerate_);
  if (ducking_ > 0.0f) rms_detector_->prepare(samplerate_);
}

inline void Delay::applyDelay(float* in_L, float* in_R)
{
  if (ducking_ > 0.0f) rms_detector_->apply(in_L, in_R);

  switch (mode_) {
    case DelayMode::kNormal:
      if (in_R) applyNormalDelay(in_L, in_R);
      else applyNormalDelayMono(in_L);
      break;
    case DelayMode::kPingpong:
      if (in_R) applyPingpongDelay(in_L, in_R);
      else applyNormalDelayMono(in_L);
      break;
  }
}

// setter
inline void Delay::setMode(DelayMode new_mode)
{
  clearBuffer();
  mode_ = new_mode;
}

inline void Delay::setTime(float new_time, DelayChannel channel)
{
  if (!enable_bpm_sync && !(!isChannelLeft(channel) && enable_time_link_))
    clearBuffer();
  (isChannelLeft(channel) ? time_L_ : time_R_) = new_time;
}

inline void Delay::setFeedback(float new_feedback, DelayChannel channel)
{ 
  (isChannelLeft(channel) ? feedback_L_ : feedback_R_) = new_feedback;
}

inline void Delay::setLPFFreq(float new_freq) 
{ 
  lpf_freq_ = new_freq; 
  lpf_->set_freq(new_freq);
}

inline void Delay::setHPFFreq(float new_freq)
{ 
  hpf_freq_ = new_freq; 
  hpf_->set_freq(new_freq);
}

inline void Delay::setSyncBeat(DelaySyncBeat new_beat, DelayChannel channel)
{
  if (enable_bpm_sync && !(!isChannelLeft(channel) && enable_time_link_))
    clearBuffer();
  (isChannelLeft(channel) ? sync_beat_L_ : sync_beat_R_) = new_beat;
}
                                                                                                                        
// main process method
inline void Delay::applyNormalDelay(float* in_L, float* in_R)
{
  ringbuffer_L_->push(std::tanh(*in_L + feedback_signal_L_ * feedback_L_));
  ringbuffer_R_->push(std::tanh(*in_R + feedback_signal_R_ * (enable_feedback_link_ ? feedback_L_ : feedback_R_)));

  float delayed_L = ringbuffer_L_->getBack(samplerate_ * (enable_bpm_sync ? sync_time_L_ : time_L_));
  float delayed_R = ringbuffer_R_->getBack(samplerate_ * (enable_bpm_sync ? 
                                                            (enable_time_link_ ? sync_time_L_ : sync_time_R_)
                                                          : (enable_time_link_ ? time_L_ : time_R_)));

  float filter_L = delayed_L;
  float filter_R = delayed_R;
  lpf_->applyFilter(&filter_L, &filter_R);
  hpf_->applyFilter(&filter_L, &filter_R);

  feedback_signal_L_ = delayed_L * (1.f - std::fabs(dumping_)) + filter_L * dumping_;
  feedback_signal_R_ = delayed_R * (1.f - std::fabs(dumping_)) + filter_R * dumping_;

  output(in_L, in_R);
}

inline void Delay::applyNormalDelayMono(float* in)
{
  ringbuffer_L_->push(std::tanh(*in + feedback_signal_L_ * feedback_L_));

  float delayed = ringbuffer_L_->getBack(samplerate_ * (enable_bpm_sync ? sync_time_L_ : time_L_));

  float filter = delayed;
  lpf_->applyFilter(&filter);
  hpf_->applyFilter(&filter);

  feedback_signal_L_ = delayed * (1.f - std::fabs(dumping_)) + filter * dumping_;

  *in = *in * (1.f - mix_) + feedback_signal_L_ * mix_;
}

inline void Delay::applyPingpongDelay(float* in_L, float* in_R)
{
  ringbuffer_L_->push(std::tanh(((*in_L + *in_R) / 2) + feedback_signal_R_ * feedback_L_));
  float ping = ringbuffer_L_->getBack(samplerate_ * (enable_bpm_sync ? sync_time_L_ : time_L_));
  ringbuffer_R_->push(ping);
  float pong = ringbuffer_R_->getBack(samplerate_ * (enable_bpm_sync ?
                                                       (enable_time_link_ ? sync_time_L_ : sync_time_R_)
                                                     : (enable_time_link_ ? time_L_ : time_R_)));

  float filter_ping = ping;
  float filter_pong = pong;
  lpf_->applyFilter(&filter_ping, &filter_pong);
  hpf_->applyFilter(&filter_ping, &filter_pong);

  feedback_signal_L_ = ping * (1.f - std::fabs(dumping_)) + filter_ping * dumping_;
  feedback_signal_R_ = pong * (1.f - std::fabs(dumping_)) + filter_pong * dumping_;
 
  output(in_L, in_R);
}

inline void Delay::output(float* in_L, float* in_R)
{
  bool can_swap = mode_ == DelayMode::kPingpong;
  float feedback_tmp_L = (enable_swap_channel && can_swap) ? feedback_signal_R_ : feedback_signal_L_;
  float feedback_tmp_R = (enable_swap_channel && can_swap) ? feedback_signal_L_ : feedback_signal_R_;
  setStereoEnhance(stereo_width_, &feedback_tmp_L, &feedback_tmp_R);
  float rms = rms_detector_->getRmsAve();
  *in_L = *in_L * (1.f - mix_) + feedback_tmp_L * mix_ * (1.0f - rms * ducking_ * 1.5f);
  *in_R = *in_R * (1.f - mix_) + feedback_tmp_R * mix_ * (1.0f - rms * ducking_ * 1.5f);
}

// helper
inline float Delay::getSyncTimeByBeat(float tempo, DelayChannel channel)
{
  switch (isChannelLeft(channel) ? sync_beat_L_ : sync_beat_R_) 
  {
  case DelaySyncBeat::k1_64T: return 2.5f / tempo;   break;
  case DelaySyncBeat::k1_64:  return 3.75f / tempo;  break;
  case DelaySyncBeat::k1_32T: return 5.0f / tempo;   break;
  case DelaySyncBeat::k1_64D: return 5.625f / tempo; break;
  case DelaySyncBeat::k1_32:  return 7.5f / tempo;   break;
  case DelaySyncBeat::k1_16T: return 10.0f / tempo;  break;
  case DelaySyncBeat::k1_32D: return 11.25f / tempo; break;
  case DelaySyncBeat::k1_16:  return 15.0f / tempo;  break;
  case DelaySyncBeat::k1_8T:  return 20.0f / tempo;  break;
  case DelaySyncBeat::k1_16D: return 22.5f / tempo;  break;
  case DelaySyncBeat::k1_8:   return 30.0f / tempo;  break;
  case DelaySyncBeat::k1_4T:  return 40.0f / tempo;  break;
  case DelaySyncBeat::k1_8D:  return 45.0f / tempo;  break;
  case DelaySyncBeat::k1_4:   return 60.0f / tempo;  break;
  case DelaySyncBeat::k1_2T:  return 80.0f / tempo;  break;
  case DelaySyncBeat::k1_4D:  return 90.0f / tempo;  break;
  case DelaySyncBeat::k1_2:   return 120.0f / tempo; break;
  case DelaySyncBeat::k1_2D:  return 180.0f / tempo; break;
  case DelaySyncBeat::k1_Bar: return 240.0f / tempo; break;
  case DelaySyncBeat::k2_Bar: return 480.0f / tempo; break;
  case DelaySyncBeat::k4_Bar: return 960.0f / tempo; break;
  default:     return 60.0f / tempo;  break; // default return 1/4
  }
}


} // namespace INVOXRecords
#endif // IR_DELAY_H