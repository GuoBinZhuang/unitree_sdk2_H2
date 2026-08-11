#include <fstream>
#include <iostream>
#include <thread>
#include <unitree/common/time/time_tool.hpp>
#include <unitree/idl/ros2/String_.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <unitree/robot/r1/audio/audio_client.hpp>

#include "common/audio_example_utils.hpp"

#define AUDIO_FILE_PATH "../example/r1/audio/test.wav"
#define AUDIO_SUBSCRIBE_TOPIC "rt/audio_msg"

#define CHUNK_SIZE 96000  // 3 seconds

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: audio_client_example [NetWorkInterface(eth0)]"
              << std::endl;
    exit(0);
  }
  int32_t ret;
  /*
   * Initilaize ChannelFactory
   */
  unitree::robot::ChannelFactory::Instance()->Init(0, argv[1]);
  unitree::robot::r1::AudioClient client;
  client.Init();
  client.SetTimeout(10.0f);

  /*ASR message Example*/
  unitree::robot::ChannelSubscriber<std_msgs::msg::dds_::String_> subscriber(
      AUDIO_SUBSCRIBE_TOPIC);
  subscriber.InitChannel(unitree::common::AsrHandler);

  /*Volume Example*/
  uint8_t volume;
  ret = client.GetVolume(volume);
  std::cout << "GetVolume API ret:" << ret
            << "  volume = " << std::to_string(volume) << std::endl;
  ret = client.SetVolume(100);
  std::cout << "SetVolume to 100% , API ret:" << ret << std::endl;

  /*TTS Example*/
  ret = client.TtsMaker("你好。我是宇树科技的机器人。例程启动成功",
                        0);  // Auto play
  std::cout << "TtsMaker API ret:" << ret << std::endl;
  unitree::common::Sleep(5);

  ret = client.TtsMaker(
      "Hello. I'm a robot from Unitree Robotics. The example has started "
      "successfully. ",
      1);  // Engilsh TTS
  std::cout << "TtsMaker API ret:" << ret << std::endl;
  unitree::common::Sleep(8);

  /*Audio Play Example*/
  int32_t sample_rate = -1;
  int8_t num_channels = 0;
  bool filestate = false;
  std::vector<uint8_t> pcm =
      ReadWave(AUDIO_FILE_PATH, &sample_rate, &num_channels, &filestate);

  std::cout << "wav file sample_rate = " << sample_rate
            << " num_channels =  " << std::to_string(num_channels)
            << " filestate =" << filestate << "filesize = " << pcm.size()
            << std::endl;

  if (filestate && sample_rate == 16000 && num_channels == 1) {
    size_t total_size = pcm.size();
    size_t offset = 0;
    int chunk_index = 0;
    std::string stream_id =
        std::to_string(unitree::common::GetCurrentTimeMillisecond());

    while (offset < total_size) {
      size_t remaining = total_size - offset;
      size_t current_chunk_size =
          std::min(static_cast<size_t>(CHUNK_SIZE), remaining);
      std::vector<uint8_t> chunk(pcm.begin() + offset,
                                 pcm.begin() + offset + current_chunk_size);
      client.PlayStream("example", stream_id, chunk);
      unitree::common::Sleep(1);
      std::cout << "Playing size: " << offset << std::endl;
      offset += current_chunk_size;
    }

    ret = client.PlayStop(stream_id);  // stop playback after transmission ends

  } else {
    std::cout << "audio file format error, please check!" << std::endl;
  }

  /*LED Control Example*/
  client.LedControl(0, 255, 0);
  unitree::common::Sleep(1);
  client.LedControl(0, 0, 0);
  unitree::common::Sleep(1);
  client.LedControl(0, 0, 255);

  std::cout << "AudioClient api test finish , asr start..." << std::endl;

  std::thread mic_t(
      [] { unitree::common::RecordMulticastMic("record.wav"); });

  while (1) {
    sleep(1);  // wait for asr message
  }
  mic_t.join();
  return 0;
}
