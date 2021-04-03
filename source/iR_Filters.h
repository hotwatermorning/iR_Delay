//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------
#ifndef IR_FILTERS_H
#define IR_FILTERS_H

#include <cmath>

namespace INVOXRecords {

constexpr float pi = 3.14159265358979;
constexpr float init_q = 1 / 1.41421356;

//------------------------------------------------------------------------
// FilterComponent
//------------------------------------------------------------------------
class FilterComponent
{
//------------------------------------------------------------------------
 public:
  void applyComponent(float b0, float b1, float b2, float a0, float a1, float a2,
    float* in_L, float* in_R = nullptr);

//------------------------------------------------------------------------
 private:
  float zx_L[2] = {};
  float zx_R[2] = {};
  float zy_L[2] = {};
  float zy_R[2] = {};
};

//------------------------------------------------------------------------
// FilterBase
//------------------------------------------------------------------------
class FilterBase
{
//------------------------------------------------------------------------
 public:
  FilterBase() = delete;
  FilterBase(int max_cascade, float freq);
  void release();
  void applyFilter(float* in_L, float* in_R = nullptr);
  virtual void calculate(float samplerate) = 0;
  virtual bool isChangedParameter();

  void set_bypass(bool bypass = true);
  bool bypass();

  void set_freq(float freq);
  float freq();

  void set_max_cascade(int max_cascade);
  int max_cascade();

  void set_cascade(int cascade);
  int cascade();

//------------------------------------------------------------------------
 protected:
  float a0_ = 0, a1_ = 0, a2_ = 0;
  float b0_ = 0, b1_ = 0, b2_ = 0;
  float tmp_freq_ = 0;

  FilterComponent* componentList[5];

//------------------------------------------------------------------------
 private:
  bool bypass_ = false;
  float freq_ = 500.f;
  int cascade_ = 1;
  int max_cascade_ = 1;
};

//------------------------------------------------------------------------
// FilterUseQ
//------------------------------------------------------------------------
class FilterUseQ : public FilterBase
{
//------------------------------------------------------------------------
 public:
  FilterUseQ() = delete;
  FilterUseQ(int max_cascade, float freq, float q = init_q);
  void calculate(float samplerate) override = 0;
  bool isChangedParameter() override;

  void set_q(float q);
  float q();

//------------------------------------------------------------------------
 protected:
  float tmp_q_ = 0;

//------------------------------------------------------------------------
 private:
  float q_ = init_q;
};

class FilterUseGain : public FilterUseQ
{
//------------------------------------------------------------------------
 public:
  FilterUseGain() = delete;
  FilterUseGain(int max_cascade, float freq, float q = init_q, float gain = 0.f);
  void calculate(float samplerate) override = 0;
  bool isChangedParameter() override;

  void set_gain(float gain);
  float gain();

//------------------------------------------------------------------------
 protected:
  float tmp_gain_ = 0;

//------------------------------------------------------------------------
 private:
  float gain_ = 0.f;
};

//------------------------------------------------------------------------
// LowPassFilter
//------------------------------------------------------------------------
class LowPassFilter : public FilterUseQ
{
//------------------------------------------------------------------------
 public:
  LowPassFilter(int max_cascade, float freq, float q = init_q);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// HighPassFilter
//------------------------------------------------------------------------
class HighPassFilter : public FilterUseQ
{
//------------------------------------------------------------------------
 public:
  HighPassFilter(int max_cascade, float freq, float q = init_q);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// BandPassFilter
//------------------------------------------------------------------------
class BandPassFilter : public FilterBase
{
//------------------------------------------------------------------------
 public:
  BandPassFilter(int max_cascade, float freq, float band_width);
  void calculate(float samplerate) override;
  bool isChangedParameter() override;

  void set_band_width(float band_width);
  float band_width();

//------------------------------------------------------------------------
 protected:
  float tmp_band_width_ = 1000.f;

//------------------------------------------------------------------------
 private:
  float band_width_ = 1000.f;
};

//------------------------------------------------------------------------
// NotchFilter
//------------------------------------------------------------------------
class NotchFilter : public FilterUseQ
{
//------------------------------------------------------------------------
 public:
  NotchFilter(int max_cascade, float freq, float q = init_q);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// AllPassFilter
//------------------------------------------------------------------------
class AllPassFilter : public FilterUseQ
{
//------------------------------------------------------------------------
 public:
  AllPassFilter(int max_cascade, float freq, float q = init_q);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// LowShelfFilter
//------------------------------------------------------------------------
class LowShelfFilter : public FilterUseGain
{
//------------------------------------------------------------------------
 public:
  LowShelfFilter(int max_cascade, float freq, float q = init_q, float gain = 0.f);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// HighShelfFilter
//------------------------------------------------------------------------
class HighShelfFilter : public FilterUseGain
{
//------------------------------------------------------------------------
 public:
  HighShelfFilter(int max_cascade, float freq, float q = init_q, float gain = 0.f);
  void calculate(float samplerate) override;
};

//------------------------------------------------------------------------
// PeakingFilter
//------------------------------------------------------------------------
class PeakingFilter : public FilterUseGain
{
//------------------------------------------------------------------------
 public:
  PeakingFilter(float freq, float q = init_q, float gain = 0.f);
  void calculate(float samplerate);
};
//------------------------------------------------------------------------
} // namespace INVOXRecords
#endif