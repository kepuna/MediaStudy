
#include "playthread.h"
#include <QtDebug>
#include <SDL2/SDL.h>
#include <QFile>

PlayThread::PlayThread(QObject *parent) : QThread(parent)
{
    connect(this, &PlayThread::finished,
            this, &PlayThread::deleteLater);
}


PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "析构了";
}

// 一些基本宏定义
#define PCM_FILENAME  "/Users/momo/Desktop/Resource/009.pcm"
// 采样率 （每秒采样的次数）
#define SAMPLE_RATE 44100

// 采样格式
//  _S16LSB 有符号 小端模式
// _U16LSB 无符号
//  S16MSB 有符号 大端模式
// 注意？： 大小端模式必须写正确，否则播放会有问题
#define SAMPLE_FORMAT AUDIO_S16LSB
// 采样大小
#define SAMPLE_SIZE SDL_AUDIO_BITSIZE(SAMPLE_FORMAT)
// 声道数
#define CHANNELS 2
// 音频缓冲区的样本数量
#define SAMPLES 1024

// 每个样本占用多少个字节
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) / 8)

// 文件缓冲区的大小
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)

// 用于存储读取的音频数据和长度
typedef  struct {
    int len = 0;
    int pullLen = 0; // 每次拉多少到缓冲区
    Uint8 *data = nullptr;
}  AudioBuffer;

// 回调函数 (等待音频设备回调)， 会回调多次
//  stream是个指针，它一定指向一段内存空间，所以需要往stream中填充PCM数据
// len 是希望填充的大小 （SDL_AudioSpec.samples * 每个样本的大小）
// 详细解释可以查看 07- 播放PCM 01:40:00

void pull_audio_data(void *userdata, Uint8 * stream,
                     int len) {

    // 清空stream
    SDL_memset(stream, 0, len);

    // 取出缓冲信息
    AudioBuffer *buffer = (AudioBuffer *)userdata;
    if(buffer->len <= 0) {
        return;
    }

    // 取len、bufferLen的最小值（为了保证数据安全，防止指针越界）
    buffer->pullLen = (len > buffer->len) ? buffer->len : len;

    // 填充数据
    SDL_MixAudio(stream, buffer->data, buffer->pullLen, SDL_MIX_MAXVOLUME);

    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}

void PlayThread::run() {

    SDL_version version;
    SDL_VERSION(&version);

    qDebug() << version.major << version.minor << version.patch;

    // 初始化Audio 子系统
    // 可以初始化多个子系统 SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)
    if(SDL_Init(SDL_INIT_AUDIO )) {
        // 返回值不是0 代表失败
        qDebug() << "SDL_Init Error" << SDL_GetError();
         return;
    };

    // 打开设备
    //  SDL_OpenAudio 第一个参数是 音频相关描述信息、

    // 音频参数
    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = SAMPLE_FORMAT;
    spec.channels = CHANNELS;
    spec.samples = SAMPLES; // 音频缓冲区的样本数量（这个值必须是2的幂）
   // 回调
    spec.callback = pull_audio_data;

    // 传递给回调的参数 ✅
    AudioBuffer buffer;
    spec.userdata = &buffer;

    if(SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio Error" << SDL_GetError();
         SDL_Quit();
         return;
    };

    //  打开文件 ，读取PCM，
    QFile file(PCM_FILENAME);

    if( !file.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << PCM_FILENAME;
        //  关闭设备
        SDL_CloseAudio();
        SDL_Quit();
        return;
    }

    // 播放 (1 表示暂停 0 表示取消暂停)
    SDL_PauseAudio(0);

    // 存放文件数据
    Uint8 data[BUFFER_SIZE];

    // 不断推送数据到 pull_audio_data
    while ( !isInterruptionRequested()) {

        // 只要从文件中读取的音频数据，还没有填充完毕就跳过
        if (buffer.len > 0) continue;

        buffer.len = file.read((char *)data, BUFFER_SIZE);

        // 文件数据已经读取完毕
        // 最后一次数据读完之后 buffer.len 已经等于0 了，但是可能当前播放的len比较长，如果直接break就有问题
       // 给你一段PCM，这个数据播放多少秒，是可以推算出来的
        // SDL 并没有提供播放结束的通知或回调
        if (buffer.len <= 0) {
            // 剩余的样本数量
            int samples = buffer.pullLen / BYTES_PER_SAMPLE;
            // 毫秒
            int ms = samples * 1000 / SAMPLE_RATE;
            SDL_Delay(ms);
            break;
        }

        // 读取到了文件数据
        buffer.data = data;
    }
     qDebug() << "采样大小 =" << SAMPLE_SIZE;

    file.close();
    SDL_CloseAudio();
    // 清除所有初始化子系统
    SDL_Quit();
}


