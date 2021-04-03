//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "pluginterfaces/vst/vsttypes.h"

using namespace Steinberg;

namespace INVOXRecords {
//------------------------------------------------------------------------
//  iR_DelayParamID
//------------------------------------------------------------------------
enum iR_DelayParamID : Vst::ParamID
{
  kBypassID,

  // delay time
  kEnableBPMSyncID,
  kBeatSelectLeftID,
  kBeatSelectRightID,
  kDelayTimeLeftID,
  kDelayTimeRightID,
  kEnableTimeLinkID,

  // Feedback
  kFeedbackLeftID,
  kFeedbackRightID,
  kEnableFeedbackLinkID,

  // color
  kDumpingID,
  kHighCutID,
  kLowCutID,

  // Setting
  kEnablePingPongID,
  kEnableSwapChannelID,

  // output
  kStereoWidthID,
  kDuckingID,
  kMixID,
  kOutputGainID,

  // other
  kHeaderMenuID,
};
//------------------------------------------------------------------------
} // namespace INVOXRecords
