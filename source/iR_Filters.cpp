//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "iR_Filters.h"

namespace INVOXRecords {
//------------------------------------------------------------------------
// Filter Component
//------------------------------------------------------------------------
void FilterComponent::applyComponent(float b0, float b1, float b2,
  float a0, float a1, float a2, float* in_L, float* in_R)
{
  float b0a0 = b0 / a0;
  float b1a0 = b1 / a0;
  float b2a0 = b2 / a0;
  float a1a0 = a1 / a0;
  float a2a0 = a2 / a0;
  float tmp_L = *in_L;

  *in_L = b0a0 * *in_L + b1a0 * zx_L[0] + b2a0 * zx_L[1]
    - a1a0 * zy_L[0] - a2a0 * zy_L[1];

  zx_L[1] = zx_L[0];
  zx_L[0] = tmp_L;
  zy_L[1] = zy_L[0];
  zy_L[0] = *in_L;

  if (in_R) {
    float tmp_R = *in_R;

    *in_R = b0a0 * *in_R + b1a0 * zx_R[0] + b2a0 * zx_R[1]
      - a1a0 * zy_R[0] - a2a0 * zy_R[1];

    zx_R[1] = zx_R[0];
    zx_R[0] = tmp_R;
    zy_R[1] = zy_R[0];
    zy_R[0] = *in_R;
  }
}

//------------------------------------------------------------------------
// Filter Base
//------------------------------------------------------------------------
FilterBase::FilterBase(int max_cascade, float freq)
{
  set_max_cascade(max_cascade);
  set_freq(freq);
  for (int i = 0; i < max_cascade; i++) componentList[i] = new FilterComponent();
}

void FilterBase::applyFilter(float* in_L, float* in_R)
{
  if (!bypass()) {
    for (int i = 0; i < cascade_; i++) {
      if (in_R) componentList[i]->applyComponent(b0_, b1_, b2_, a0_, a1_, a2_, in_L, in_R);
      else componentList[i]->applyComponent(b0_, b1_, b2_, a0_, a1_, a2_, in_L);
    }
  }
}

void FilterBase::release()
{
  for (int i = 0; i < max_cascade(); i++)
    delete componentList[i];
  delete this;
}

bool FilterBase::isChangedParameter()
{
  if (tmp_freq_ != freq_) {
    tmp_freq_ = freq_;
    return true;
  }
  return false;
}

void FilterBase::set_bypass(bool bypass) { bypass_ = bypass; }
bool FilterBase::bypass() { return bypass_; }

void FilterBase::set_freq(float freq) { freq_ = freq; }
float FilterBase::freq() { return freq_; }

void FilterBase::set_max_cascade(int max_cascade) { max_cascade_ = max_cascade; }
int FilterBase::max_cascade() { return max_cascade_; }

void FilterBase::set_cascade(int cascade) { cascade_ = (cascade > max_cascade_) ? max_cascade_ : cascade; }
int FilterBase::cascade() { return max_cascade_; }

//------------------------------------------------------------------------
// Filter Use Q
//------------------------------------------------------------------------
FilterUseQ::FilterUseQ(int max_cascade, float freq, float q)
  : FilterBase(max_cascade, freq)
{
  set_q(q);
}

bool FilterUseQ::isChangedParameter()
{
  if (tmp_freq_ != freq() || tmp_q_ != q_) {
    tmp_freq_ = freq();
    tmp_q_ = q_;
    return true;
  }
  return false;
}

void FilterUseQ::set_q(float q) { q_ = q; }
float FilterUseQ::q() { return q_; }

//------------------------------------------------------------------------
// Filter Use Gain
//------------------------------------------------------------------------
FilterUseGain::FilterUseGain(int max_cascade, float freq, float q, float gain)
  : FilterUseQ(max_cascade, freq, q)
{
  set_gain(gain);
}

bool FilterUseGain::isChangedParameter()
{
  if (tmp_freq_ != freq() || tmp_q_ != q() || tmp_gain_ != gain_) {
    tmp_freq_ = freq();
    tmp_q_ = q();
    tmp_gain_ = gain_;
    return true;
  }
  return false;
}

void FilterUseGain::set_gain(float gain) { gain_ = gain; }
float FilterUseGain::gain() { return gain_; }

//------------------------------------------------------------------------
// Low Pass Filter
//------------------------------------------------------------------------
LowPassFilter::LowPassFilter(int max_cascade, float freq, float q)
  : FilterUseQ(max_cascade, freq, q) {}

void LowPassFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    b0_ = (1 - std::cos(w0)) / 2;
    b1_ = 1 - std::cos(w0);
    b2_ = (1 - std::cos(w0)) / 2;
    a0_ = 1 + alpha;
    a1_ = -2 * std::cos(w0);
    a2_ = 1 - alpha;
  }
}

//------------------------------------------------------------------------
// High Pass Filter
//------------------------------------------------------------------------
HighPassFilter::HighPassFilter(int max_cascade, float freq, float q)
  : FilterUseQ(max_cascade, freq, q) {}

void HighPassFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    b0_ = (1 + cosw0) / 2;
    b1_ = -(1 + cosw0);
    b2_ = (1 + cosw0) / 2;
    a0_ = 1 + alpha;
    a1_ = -2 * cosw0;
    a2_ = 1 - alpha;
  }
}

//------------------------------------------------------------------------
// Band Pass Filter
//------------------------------------------------------------------------
BandPassFilter::BandPassFilter(int max_cascade, float freq, float band_width)
  : FilterBase(max_cascade, freq)
{
  set_band_width(band_width);
}

void BandPassFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 * std::sinh((log(2) / 2) * band_width_ * (w0 / std::sin(w0)));
    b0_ = alpha;
    b1_ = 0;
    b2_ = -alpha;
    a0_ = 1 + alpha;
    a1_ = -(2 * cosw0);
    a2_ = 1 - alpha;
  }
}

bool BandPassFilter::isChangedParameter()
{
  if (tmp_freq_ != freq() || tmp_band_width_ != band_width_) {
    tmp_freq_ = freq();
    tmp_band_width_ = band_width_;
    return true;
  }
  return false;
}

void BandPassFilter::set_band_width(float band_width) { band_width_ = band_width; }
float BandPassFilter::band_width() { return band_width_; }

//------------------------------------------------------------------------
// Notch Pass Filter
//------------------------------------------------------------------------
NotchFilter::NotchFilter(int max_cascade, float freq, float q)
  : FilterUseQ(max_cascade, freq, q) {}

void NotchFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    b0_ = 1;
    b1_ = -2 * cosw0;
    b2_ = 1;
    a0_ = 1 + alpha;
    a1_ = -2 * cosw0;
    a2_ = 1 - alpha;
  }
}

//------------------------------------------------------------------------
// All Pass Filter
//------------------------------------------------------------------------
AllPassFilter::AllPassFilter(int max_cascade, float freq, float q)
  : FilterUseQ(max_cascade, freq, q) {}

void AllPassFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    b0_ = 1 - alpha;
    b1_ = -2 * cosw0;
    b2_ = 1 + alpha;
    a0_ = 1 + alpha;
    a1_ = -2 * cosw0;
    a2_ = 1 - alpha;
  }
}

//------------------------------------------------------------------------
// Low Shelf Filter
//------------------------------------------------------------------------
LowShelfFilter::LowShelfFilter(int max_cascade, float freq, float q, float gain)
  : FilterUseGain(max_cascade, freq, q, gain) {}

void LowShelfFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    float A = std::pow(10, gain() / 40);
    float sqrtA = std::sqrtf(A);
    float Ap1 = A + 1;
    float Am1 = A - 1;
    b0_ = A * (Ap1 - Am1 * cosw0 + 2 * sqrtA * alpha);
    b1_ = 2 * A * (Am1 - Ap1 * cosw0);
    b2_ = A * (Ap1 - Am1 * cosw0 - 2 * sqrtA * alpha);
    a0_ = Ap1 + Am1 * cosw0 + 2 * sqrtA * alpha;
    a1_ = -2 * (Am1 + Ap1 * cosw0);
    a2_ = Ap1 + Am1 * cosw0 - 2 * sqrtA * alpha;
  }
}

//------------------------------------------------------------------------
// High Shelf Filter
//------------------------------------------------------------------------
HighShelfFilter::HighShelfFilter(int max_cascade, float freq, float q, float gain)
  : FilterUseGain(max_cascade, freq, q, gain) {}

void HighShelfFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    float A = std::pow(10, gain() / 40);
    float sqrtA = std::sqrtf(A);
    float Ap1 = A + 1;
    float Am1 = A - 1;
    b0_ = A * (Ap1 + Am1 * cosw0 + 2 * sqrtA * alpha);
    b1_ = -2 * A * (Am1 + Ap1 * cosw0);
    b2_ = A * (Ap1 + Am1 * cosw0 - 2 * sqrtA * alpha);
    a0_ = Ap1 - Am1 * cosw0 + 2 * sqrtA * alpha;
    a1_ = 2 * (Am1 - Ap1 * cosw0);
    a2_ = Ap1 - Am1 * cosw0 - 2 * sqrtA * alpha;
  }
}

//------------------------------------------------------------------------
// Peaking Filter
//------------------------------------------------------------------------
PeakingFilter::PeakingFilter(float freq, float q, float gain)
  : FilterUseGain(1, freq, q, gain) {}

void PeakingFilter::calculate(float samplerate)
{
  if (isChangedParameter()) {
    float w0 = 2 * pi * freq() / samplerate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2 * q());
    float A = std::pow(10, gain() / 40);
    b0_ = 1 + (alpha * A);
    b1_ = -(2 * cosw0);
    b2_ = 1 - (alpha * A);
    a0_ = 1 + (alpha / A);
    a1_ = -(2 * cosw0);
    a2_ = 1 - (alpha / A);
  }
}

} // namespace INVOXRecords