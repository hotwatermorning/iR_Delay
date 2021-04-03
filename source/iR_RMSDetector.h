#ifndef IR_RMSDETECTOR_H_
#define IR_RMSDETECTOR_H_

#include <cmath>
#include <memory>

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// RMS Detector
//----------------------------------------------------------------------------------------------------------------------
class RMSDetector
{
public:
  // constructor
  RMSDetector() = delete;
  RMSDetector(float samplerate, int length_ms, float lpf_rate = 0.99995f);

  // destructor
  ~RMSDetector();

  // common method
  void prepare(float samplerate);
  void apply(float* in_L, float* in_R = nullptr);

  // getter
  float getRmsL() { return rms_L_; }
  float getRmsR() { return rms_R_; }
  float getRmsAve() { return rms_ave_; }

  // setter
  void setLength(float new_length_ms);
  void setLpfRate(float new_lpf_rate) { lpf_rate_ = new_lpf_rate; }

//----------------------------------------------------------------------------------------------------------------------
private:
 // member variable
  float samplerate_;
  std::unique_ptr<float[]> buffer_L_;
  std::unique_ptr<float[]> buffer_R_;
  int length_ms_;
  int length_;
  int index_ = 0;

  float rms_L_ = 0.0f;
  float rms_R_ = 0.0f;
  float rms_ave_ = 0.0f;

  float sum_L_ = 0.0f;
  float sum_R_ = 0.0f;
  float lpf_rate_;
};

//----------------------------------------------------------------------------------------------------------------------
// RMS Detector implementation
//----------------------------------------------------------------------------------------------------------------------
inline RMSDetector::RMSDetector(float samplerate, int length_ms, float lpf_rate) :
  samplerate_(samplerate),
  length_ms_(length_ms),
  lpf_rate_(lpf_rate)
{
  length_ = samplerate * length_ms_ / 1000.0;
  buffer_L_ = std::make_unique<float[]>(length_);
  buffer_R_ = std::make_unique<float[]>(length_);
}

inline RMSDetector::~RMSDetector()
{
  buffer_L_.reset();
  buffer_R_.reset();
}

inline void RMSDetector::prepare(float samplerate)
{
  if (samplerate != samplerate_) {
    samplerate_ = samplerate;
    setLength(length_ms_);
  }
}

inline void RMSDetector::apply(float* in_L, float* in_R)
{
  // update buffer and variable
  buffer_L_.get()[index_] = *in_L * *in_L;
  if (in_R) buffer_R_.get()[index_] = *in_R * *in_R;
  index_++;

  // calculate rms
  if (index_ >= length_) {
    index_ = 0;
    sum_L_ = 0.0f;
    sum_R_ = 0.0f;
    for (int i = 0; i < length_; i++) {
      sum_L_ += buffer_L_.get()[i];
      if (in_R) sum_R_ += buffer_R_.get()[i];
    }
  }
  rms_L_ = std::sqrtf(sum_L_ / length_) * (1.0f - lpf_rate_) + rms_L_ * lpf_rate_;
  rms_R_ = std::sqrtf(sum_R_ / length_) * (1.0f - lpf_rate_) + rms_R_ * lpf_rate_;
  rms_ave_ = in_R ? (rms_L_ + rms_R_) / 2.0f : rms_L_;
}

inline void RMSDetector::setLength(float new_length_ms)
{
  length_ms_ = new_length_ms;
  length_ = samplerate_ * new_length_ms / 1000.0f;
  buffer_L_.reset(new float[length_]);
  buffer_R_.reset(new float[length_]);
}

} // namespace INVOXRecords
#endif // IR_RMSDETECTOR_H_