#include "audio_read.h"
#include<QHostAddress>
#include<QMessageBox>




///////////////////////////////////////////////////////////////////////////////////////

Audio_Read::Audio_Read()
{
    //speex编码初始化
    speex_bits_init(&bits_enc);
    Enc_State = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
    //Enc_State = speex_encoder_init(&speex_nb_mode);
    //设置压缩质量
    //设置质量为8(15kbps)
    int tmp = SPEEX_QUALITY;
    speex_encoder_ctl(Enc_State,SPEEX_SET_QUALITY,&tmp);

    //声卡采样格式
    // set up the format you want, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    //format.setByteOrder(QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    //format.setSampleType(QAudioFormat::SignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
      // qWarning()<<"default format not supported try to use nearest";
       QMessageBox::information(NULL , "提示", "打开音频设备失败");
       format = info.nearestFormat(format);
    }
    audio_in = NULL;
    m_playState = state_stop;
    // Records audio for 3000ms
    //qDebug() <<"record begin!" ;

    //////////////webrtc 静音VAD和降噪NSX//////////////////
#if   DEF_USE_WEBRTC
    int nSample = 8000;//采样率
    int nMode = 2; //降噪等级 0 1 2 3 越大降噪效果越好 一般使用1和2 2效果更好 但是2声音太小
    if (0 != WebRtcNsx_Create(&pNS_inst))
    {
        qDebug()<<"Noise_Suppression WebRtcNs_Create err! ";
    }

    if (0 !=  WebRtcNsx_Init(pNS_inst,nSample))
    {
        qDebug()<<"Noise_Suppression WebRtcNs_Init err! ";

    }

    if (0 !=  WebRtcNsx_set_policy(pNS_inst,nMode))
    {
        qDebug()<<"Noise_Suppression WebRtcNs_set_policy err! ";

    }
    if( 0 != WebRtcVad_Create(&handle) )
    {
        qDebug()<<"WebRtcVad_Create is error";
    }
    if( 0 != WebRtcVad_Init(handle) )
    {
        qDebug()<<"WebRtcVad_Init is error";
    }
    int mode = 1;
 // 0 Quality mode 1 Low bitrate mode 2 Aggressive mode 3 Very aggressive mode
    if( 0 != WebRtcVad_set_mode(handle,mode) )
    {
       qDebug()<<"WebRtcVad_set_mode is error";
    }
    WebRtcAgc_Create(&agcHandle);
    int minLevel = 0;
    int maxLevel = 255;
    int agcMode  = kAgcModeFixedDigital;//fixed 模式
    WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, 8000);
    WebRtcAgc_config_t agcConfig;
    agcConfig.compressionGaindB = 20;//在Fixed模式下，越大声音越大
    agcConfig.limiterEnable     = 1;
    agcConfig.targetLevelDbfs   = 3;//dbfs表示相对于full scale的下降值，0表示full scale，越小声音越大
    WebRtcAgc_set_config(agcHandle, agcConfig);
#endif
    //////////////webrtc 静音和降噪//////////////////

    timer = NULL;
}

Audio_Read::~Audio_Read()
{

    WebRtcNsx_Free(pNS_inst);
    WebRtcVad_Free(handle);
    WebRtcAgc_Free(agcHandle);
    this->UnInit();
    delete audio_in;

}
void Audio_Read::readMore()
{
    char bytes[800] = {0};
    int i = 0;
    float input_frame1[320];
    QByteArray frame;

    int nbytes = 0;

    if (!audio_in)
        return;

    QByteArray m_buffer(2048,0);
    qint64 len = audio_in->bytesReady();
//    qint64 len = myBuffer_in->bytesAvailable();
    if (len < 640)
    {
        return;
    }

    qint64 l = myBuffer_in->read(m_buffer.data(), 640);
//    qDebug() << "len2 = " << l;

    frame.clear();

    //////////////webrtc 静音和降噪//////////////////
#if  DEF_USE_WEBRTC
    char* pInData = (char*)m_buffer.data() ; //m_buffer 共640字节 对于short是320个点
    //下面 每80个点处理一次
    short shBufferIn[80] = {0};
    short shBufferOut[80] = {0};
    char buf[640] = {0};
    // VAD
    int16_t * tmp = 0;
    int micLevelIn = 0;
    int micLevelOut = 0;
    int frameSize = 80;
    int16_t* pData;

    int16_t pOutData[80] = {0};
    for( int i = 0; i< 640 ; i +=320)
    {
        tmp = (int16_t *)(pInData +i) ;
       //错误是-1 静音是0 有声是1
        if( 0 == WebRtcVad_Process(handle,8000,tmp,160) )
        {
//            qDebug()<<"silence";
            memset(tmp,0,320);
        }
    }
    memcpy( buf , pInData , 640);
    /////////////// NSX  降噪
        for( int i=0; i<640 ; i+= 160 )
        {
            memcpy(shBufferIn,(char*)( buf +i ),80*sizeof(short));
            if (0 != WebRtcNsx_Process( pNS_inst ,shBufferIn  ,NULL ,shBufferOut , NULL)){
                qDebug()<<"Noise_Suppression WebRtcNs_Process err!";
            }
            memcpy( buf+i , shBufferOut,80*sizeof(short) );
        }

     /////////   AGC 再增益

//        for( int i = 0 ; i < 640 ; i+= 160)
//        {
//            int inMicLevel  = micLevelOut;
//            int outMicLevel = 0;
//            uint8_t saturationWarning;
//            pData = (int16_t*)(buf+ i) ;
//            int nAgcRet = WebRtcAgc_Process(agcHandle, pData, NULL, frameSize, pOutData,NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
//            if (nAgcRet != 0)
//            {
//                printf("failed in WebRtcAgc_Process\n");
//                break;
//            }
//            micLevelIn = outMicLevel;
//            memcpy( buf+i , pOutData,frameSize*sizeof(short) );
//        }

#endif
#if USE_SPEEX
    {
        short num = 0;
        for (i = 0;i < 160;i++)
        {
            num = (uint8_t)buf[2 * i] | (((uint8_t)buf[2 * i + 1]) << 8);
            input_frame1[i] = num;
            //num = m_buffer[2 * i] | ((short)(m_buffer[2 * i + 1]) << 8);
            //qDebug() << "float in" << num << input_frame1[i];
        }
        //压缩数据
        speex_bits_reset(&bits_enc);
        speex_encode(Enc_State,input_frame1,&bits_enc);
        nbytes = speex_bits_write(&bits_enc,bytes,800);

        frame.append(bytes,nbytes);
        //大端
        for (i = 0;i < 160;i++)
        {
            num = (uint8_t)buf[2 * i + 320] | (((uint8_t)buf[2 * i + 1 + 320]) << 8);
            input_frame1[i] = num;
        }
        //压缩数据
        speex_bits_reset(&bits_enc);
        speex_encode(Enc_State,input_frame1,&bits_enc);
        nbytes = speex_bits_write(&bits_enc,bytes,800);
        //  qDebug() << "nbytes = " << nbytes;
        frame.append(bytes,nbytes);

        emit sig_net_tx_frame(frame);
        return;
    }
#endif

    frame.append(buf, 640);
 //   memcpy( m_buffer.data() , buf, 640 );
    //    m_buffer = qCompress(m_buffer,5);
    //    qDebug()<<"qCompress:"<<m_buffer.size();
    emit sig_net_tx_frame(/*m_buffer*/frame);

}

void Audio_Read::UnInit()
{
    if(timer)
        timer->stop();
    if(audio_in)
        audio_in->stop();

}

void Audio_Read::ResumeAudio()
{
    if( m_playState == state_stop )
    {
        audio_in = new QAudioInput(format, this);
        myBuffer_in = audio_in->start();
//       connect(myBuffer_in, SIGNAL(readyRead()), this, SLOT(readMore())  );
    }
    else
    {
        if(audio_in)
        {
            delete audio_in;
            audio_in = new QAudioInput(format, this);
            myBuffer_in = audio_in->start();
        }
    }
    m_playState = state_play;

    timer = new QTimer(this);
    connect( timer , &QTimer::timeout , this , &Audio_Read::readMore );
    timer->start(1000/40);
}

void Audio_Read::PauseAudio()
{
    if(timer)
        timer->stop();
    if( m_playState == state_play )
    {
        m_playState = state_pause;
        if(audio_in)
        audio_in->stop();
    }
}


