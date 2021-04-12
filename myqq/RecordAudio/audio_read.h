#ifndef AUDIO_READ_H
#define AUDIO_READ_H

#include "world.h"
#include<QTimer>


class Audio_Read : public QObject
{
    Q_OBJECT
public:
    Audio_Read();
    ~Audio_Read();
signals:
    /*********************************************************************
    *                           发送网络帧
    *参数:frame:发送的报文
    **********************************************************************/

    void sig_net_tx_frame(QByteArray frame);

public slots:
    void readMore();
    void PauseAudio();
    void ResumeAudio();
    void UnInit();
private:

    QAudioInput* audio_in; // class member.
    QIODevice *myBuffer_in;

//    //降噪和静音检测
//    SpeexPreprocessState * m_st;
//    SpeexEchoState *echo_state;

    //webrtc 降噪NSX
    NsxHandle *pNS_inst  ;
    //webrtc VAD
    VadInst *handle;
    //webrtc AGC
    void *agcHandle ;

    //SPEEX相关全局变量
    SpeexBits bits_enc;
    void *Enc_State;
    QAudioFormat format;

    short input_frame[SPEEX_FRAME_BYTE / 2];            //speex压缩输入存储区
    short input_frame0[SPEEX_FRAME_BYTE / 2];            //speex压缩输入存储区
    char cbits[SPEEX_FRAME_BYTE];                       //压缩后数据存储区
    char buf[SPEEX_FRAME_BYTE];                         //读取声卡存储区
    enum ENUM_AUDIO_STATE{state_stop , state_play , state_pause };
    int m_playState;

    QTimer *timer;
};

#endif // AUDIO_READ_H
