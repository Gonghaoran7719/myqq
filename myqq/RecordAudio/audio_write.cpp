#include "audio_write.h" 

Audio_Write::Audio_Write()
{
    //speex初始化
    speex_bits_init(&bits_dec);
    Dec_State = speex_decoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
//    Dec_State = speex_decoder_init(&speex_nb_mode);
 
    QAudioFormat format;
    // set up the format you want, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    //format.setByteOrder(QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    //format.setSampleType(QAudioFormat::SignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();/*defaultInputDevice*/
    if (!info.isFormatSupported(format)) {
       qWarning()<<"default format not supported try to use nearest";
       format = info.nearestFormat(format);
    }

    audio_out = new QAudioOutput(format, this);
    connect(audio_out,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
    myBuffer_out = audio_out->start();
    qDebug() <<"play begin!" << endl;
 
//    QTimer *timer2 = new QTimer(this);
//   connect(timer2, SIGNAL(timeout()), this, SLOT(update2()));
    //timer2->start(10 * INTERVAL);
    //timer2->start(5);
}
Audio_Write::~Audio_Write()
{
//    audio_out->stop();
    if(audio_out)
        delete audio_out;
    audio_out = NULL;
}

void Audio_Write::finishedPlaying(QAudio::State state)
 {
//   if(state == QAudio::IdleState) {
//     audio_out->stop();
//     inputFile.close();
//     delete audio_out;
//   }
 //  qDebug() << "play end!" << endl;
 }
 
/*********************************************************************
*                               网络接收数据包
*参数:data:接收的数据
**********************************************************************/
 
void Audio_Write::slot_net_rx(QByteArray ba)
{
    char bytes[800] = {0};
    int i = 0;
    float output_frame1[320] = {0};
    char buf[800] = {0};
 
    //memcpy(bytes,data.data(),data.length());
 
  //  qDebug() << "lenght!!!!!!!!!!!!!!" << data.length();
#if USE_SPEEX
    memcpy(bytes,ba.data(),ba.length() / 2);
    //解压缩数据106 62
    //speex_bits_reset(&bits_dec);
    speex_bits_read_from(&bits_dec,bytes,ba.length() / 2);
    int error = speex_decode(Dec_State,&bits_dec,output_frame1);
    //qDebug() << "error1 = !!!!!!!!!!!!!!" << error;
 
    //将解压后数据转换为声卡识别格式
    //大端


    short num = 0;
    for (i = 0;i < 160;i++)
    {
        num = output_frame1[i];
        buf[2 * i] = num;
        buf[2 * i + 1] = num >> 8;
        //qDebug() << "float out" << num << output_frame1[i];
    }
 
    memcpy(bytes,ba.data() + ba.length() / 2,ba.length() / 2);
    //解压缩数据
    //speex_bits_reset(&bits_dec);
    speex_bits_read_from(&bits_dec,bytes,ba.length() / 2);
    error = speex_decode(Dec_State,&bits_dec,output_frame1);
  //  qDebug() << "error2 = !!!!!!!!!!!!!!" << error;
 
    //将解压后数据转换为声卡识别格式

    //大端
    for (i = 0;i < 160;i++)
    {
        num = output_frame1[i];
        buf[2 * i + 320] = num;
        buf[2 * i + 1 + 320] = num >> 8;
    }

    myBuffer_out->write(buf,640);
    return;
#endif
//    data = qUncompress( data );
    myBuffer_out->write( ba.data() , 640 );
//    myBuffer_out->write(buf,640);

}
 
void Audio_Write::update2()
{
    char bytes[800] = {0};
    int i = 0;
    QByteArray frame;
 
    //short input_short[L_FRAME] = {0};
    int j = 0;
 
    //检查是否有剩余空间
   // qDebug() << "aaaaaaaaa222222222222222:" << audio_out->bytesFree()
   //          << audio_out->periodSize() << Buffer_Play.length();
    if (audio_out && audio_out->state() != QAudio::StoppedState)
    {
        int chunks = audio_out->bytesFree()/audio_out->periodSize();
        while (chunks)
        {
            if (Buffer_Play.length() >= audio_out->periodSize())
            {
                myBuffer_out->write(Buffer_Play.data(),audio_out->periodSize());
                Buffer_Play = Buffer_Play.mid(audio_out->periodSize());
            }
            else
            {
                myBuffer_out->write(Buffer_Play);
                Buffer_Play.clear();
                break;
            }
 
            --chunks;
        }
    }
 

}
