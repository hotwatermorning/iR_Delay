//----------------------------------------------------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//----------------------------------------------------------------------------------------------------------------------
#ifndef IR_UTILITY_H_
#define IR_UTILITY_H_

#include <cmath>
#include <algorithm>

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// Gain
//----------------------------------------------------------------------------------------------------------------------
inline void setGain(float gain, float* signal)
{
  *signal *= std::powf(10, gain / 20);
}

//----------------------------------------------------------------------------------------------------------------------
// Circular Pan (angle => [-45.0 ~ 45.0])
//----------------------------------------------------------------------------------------------------------------------
inline void setCircularPan(float angle, float* in_L, float* in_R)
{
  float angle_cos = std::cos(angle * 3.14159f / 180.0f);
  float angle_sin = std::sin(angle * 3.14159f / 180.0f);
  *in_L = 0.70711f * angle_cos - angle_sin * *in_L;
  *in_R = 0.70711f * angle_cos + angle_sin * *in_R;
}

//----------------------------------------------------------------------------------------------------------------------
// Channel Integrated Pan (degree => [-1.0 ~ 1.0])
//----------------------------------------------------------------------------------------------------------------------
inline void setIntegratedPan(float degree, float* in_L, float* in_R)
{
  float tmp_L = *in_L;
  *in_L = degree <= 0.0f ? 
            *in_L * (1 - std::fabs(degree / 2.0f)) + *in_R * std::fabs(degree / 2.0f) 
          : *in_L * (1.0f - degree);
  *in_R = degree <= 0.0f ?
            *in_R * (1 + degree)
          : *in_R * (1.0f - std::fabs(degree / 2.0f)) + tmp_L * std::fabs(degree / 2.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// Stereo Enhance
//----------------------------------------------------------------------------------------------------------------------
inline void setStereoEnhance(float width, float* in_L, float* in_R)
{
  float tmp = 1.0f / std::fmax(1.0f + width, 2.0f);
  float mid = (*in_L + *in_R) * tmp;
  float side = (*in_R - *in_L) * width * tmp;
  *in_L = mid - side;
  *in_R = mid + side;
}

} // namespace INVOXRecords
#endif