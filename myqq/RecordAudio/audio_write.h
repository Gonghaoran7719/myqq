#ifndef AUDIO_WRITE_H
#define AUDIO_WRITE_H
 
#include"world.h"
 
class Audio_Write : public QObject
{
    Q_OBJECT
public:
    Audio_Write();
    ~Audio_Write();
signals:
 
public slots:
    void finishedPlaying(QAudio::State state);
 
    /*********************************************************************
    *                           网络接收数据包
    *参数:data:接收的数据
    **********************************************************************/
 
    void slot_net_rx(QByteArray data);
 
    void update2();
 
private:
 
    QAudioOutput* audio_out; // class member.
    QIODevice *myBuffer_out;
 
    QByteArray Buffer_Play;
    //降噪和静音检测
//    SpeexPreprocessState * m_st;
//    SpeexEchoState *echo_state;

    //SPEEX相关全局变量
    SpeexBits bits_dec;
    void *Dec_State;
 
    short output_frame[SPEEX_FRAME_BYTE / 2];           //speex解压输出存储区
 
};
 
#endif // AUDIO_WRITE_H
