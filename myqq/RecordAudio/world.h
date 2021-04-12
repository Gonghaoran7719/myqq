#ifndef WORLD_H
#define WORLD_H


#include<QIODevice>
#include<QAudioInput>
#include<QAudioOutput>
#include <speex/include/speex.h>
 
#include<qDebug>
#include<QTimer>

#include "./WebRtcMoudle/signal_processing_library.h"
#include "./WebRtcMoudle/noise_suppression_x.h"
#include "./WebRtcMoudle/noise_suppression.h"
#include "./WebRtcMoudle/gain_control.h"
#include "./WebRtc_Vad/webrtc_vad.h"

//NS  降噪
//vad 静音检测

#define SPEEX_FRAME_BYTE (320)
#define SPEEX_QUALITY    (8)

#define DEF_READ_DENOISE 0
#define DEF_WRITE_DENOISE 0
#define DEF_USE_WEBRTC 1
#define USE_SPEEX 0

#define DEF_TEL_RQ      (100)
#define DEF_TEL_RS      (101)
#define DEF_TEL_DATA    (102)
#define DEF_TEL_END     (103)

#endif // WORLD_H
