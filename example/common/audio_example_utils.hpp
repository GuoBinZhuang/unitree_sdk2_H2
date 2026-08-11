#pragma once

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <unitree/idl/ros2/String_.hpp>

#include "common/wav.hpp"

namespace unitree::common {

constexpr int kAudioSampleRate = 16000;
constexpr char kAudioMulticastGroupIp[] = "239.168.123.161";
constexpr uint16_t kAudioMulticastPort = 5555;

// Prints the ASR results published by the robot on "rt/audio_msg".
inline void AsrHandler(const void *msg) {
  const std_msgs::msg::dds_::String_ *res_msg =
      static_cast<const std_msgs::msg::dds_::String_ *>(msg);
  std::cout << "Topic:\"rt/audio_msg\" recv: " << res_msg->data() << std::endl;
}

// Returns the address of the local interface connected to the robot network.
inline std::string GetLocalIpForMulticast(
    const std::string &prefix = "192.168.123.") {
  struct ifaddrs *ifaddr = nullptr;
  char host[NI_MAXHOST];
  std::string result;

  if (getifaddrs(&ifaddr) != 0) {
    return result;
  }
  for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
    getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST,
                NULL, 0, NI_NUMERICHOST);
    std::string ip(host);
    if (ip.find(prefix) == 0) {
      result = ip;
      break;
    }
  }
  freeifaddrs(ifaddr);
  return result;
}

// Joins the robot microphone multicast group and records `seconds` of 16 kHz
// mono PCM audio into `file_path`.
inline void RecordMulticastMic(const std::string &file_path, int seconds = 5) {
  const int total_bytes_target = kAudioSampleRate * 2 * seconds;
  const size_t chunk_bytes = kAudioSampleRate * 2 * 160 / 1000;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  sockaddr_in local_addr{};
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(kAudioMulticastPort);
  local_addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (sockaddr *)&local_addr, sizeof(local_addr));

  ip_mreq mreq{};
  inet_pton(AF_INET, kAudioMulticastGroupIp, &mreq.imr_multiaddr);
  std::string local_ip = GetLocalIpForMulticast();
  std::cout << "local ip: " << local_ip << std::endl;
  mreq.imr_interface.s_addr = inet_addr(local_ip.c_str());
  setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

  int total_bytes = 0;
  std::vector<int16_t> pcm_data;
  pcm_data.reserve(total_bytes_target / 2);
  std::cout << "start record!" << std::endl;
  while (total_bytes < total_bytes_target) {
    std::vector<char> buffer(chunk_bytes);
    ssize_t len = recvfrom(sock, buffer.data(), buffer.size(), 0, nullptr,
                           nullptr);
    if (len > 0) {
      const int16_t *samples = reinterpret_cast<const int16_t *>(buffer.data());
      pcm_data.insert(pcm_data.end(), samples, samples + len / 2);
      total_bytes += len;
    }
  }

  WriteWave(file_path, kAudioSampleRate, pcm_data.data(),
            static_cast<int32_t>(pcm_data.size()), 1);
  std::cout << "record finish! save to " << file_path << std::endl;
}

}  // namespace unitree::common
