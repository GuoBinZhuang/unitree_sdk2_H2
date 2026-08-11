#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "unitree/robot/h2/loco/h2_loco_api.hpp"
#include "unitree/robot/h2/loco/h2_loco_client.hpp"

std::vector<float> stringToFloatVector(const std::string &str) {
  std::vector<float> result;
  std::stringstream ss(str);
  float num;
  while (ss >> num) {
    result.push_back(num);
    // ignore any trailing whitespace
    ss.ignore();
  }
  return result;
}

// Report the error code of a client call. Returns true if the call succeeded.
static bool Succeeded(const std::string &method, int32_t ret) {
  if (ret != 0) {
    std::cerr << "Call " << method << " failed, error code: " << ret << std::endl;
    return false;
  }
  return true;
}

static bool stringToInt(const std::string &str, int &value) {
  try {
    size_t pos = 0;
    value = std::stoi(str, &pos);
    if (pos != str.size()) {
      throw std::invalid_argument(str);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "Invalid integer argument: [" << str << "]" << std::endl;
    return false;
  } catch (const std::out_of_range &) {
    std::cerr << "Integer argument out of range: [" << str << "]" << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char const *argv[]) {
  std::map<std::string, std::string> args = {{"network_interface", "lo"}};

  std::map<std::string, std::string> values;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.substr(0, 2) == "--") {
      size_t pos = arg.find("=");
      std::string key, value;
      if (pos != std::string::npos) {
        key = arg.substr(2, pos - 2);
        value = arg.substr(pos + 1);

        if (value.front() == '"' && value.back() == '"') {
          value = value.substr(1, value.length() - 2);
        }
      } else {
        key = arg.substr(2);
        value = "";
      }
      if (args.find(key) != args.end()) {
        args[key] = value;
      } else {
        args.insert({{key, value}});
      }
    }
  }
  std::string network_interface = args["network_interface"];

  // the client can only be created once the channel factory is initialized
  std::unique_ptr<unitree::robot::h2::LocoClient> client;

  try {
    unitree::robot::ChannelFactory::Instance()->Init(0, network_interface);
    client = std::make_unique<unitree::robot::h2::LocoClient>();
    client->Init();
  } catch (const unitree::common::Exception &e) {
    std::cerr << "Failed to initialize the loco client on interface [" << network_interface
              << "]: " << e.what() << std::endl;
    return 1;
  }
  client->SetTimeout(10.f);

  int exit_code = 0;

  for (const auto &arg_pair : args) {
    if (arg_pair.first == "network_interface") {
      continue;
    }

    std::cout << "Processing command: [" << arg_pair.first << "] with param: [" << arg_pair.second << "] ..."
              << std::endl;

    bool ok = true;

    if (arg_pair.first == "get_fsm_id") {
      int fsm_id;
      ok = Succeeded("GetFsmId", client->GetFsmId(fsm_id));
      if (ok) {
        std::cout << "current fsm_id: " << fsm_id << std::endl;
      }
    } else if (arg_pair.first == "get_fsm_mode") {
      int fsm_mode;
      ok = Succeeded("GetFsmMode", client->GetFsmMode(fsm_mode));
      if (ok) {
        std::cout << "current fsm_mode: " << fsm_mode << std::endl;
      }
    } else if (arg_pair.first == "set_fsm_id") {
      int fsm_id;
      ok = stringToInt(arg_pair.second, fsm_id) && Succeeded("SetFsmId", client->SetFsmId(fsm_id));
      if (ok) {
        std::cout << "set fsm_id to " << fsm_id << std::endl;
      }
    } else if (arg_pair.first == "set_velocity") {
      std::vector<float> param = stringToFloatVector(arg_pair.second);
      auto param_size = param.size();
      float vx, vy, omega, duration;
      if (param_size == 3 || param_size == 4) {
        vx = param.at(0);
        vy = param.at(1);
        omega = param.at(2);
        duration = param_size == 4 ? param.at(3) : 1.f;
        ok = Succeeded("SetVelocity", client->SetVelocity(vx, vy, omega, duration));
        if (ok) {
          std::cout << "set velocity to " << arg_pair.second << std::endl;
        }
      } else {
        std::cerr << "Invalid param size for method SetVelocity: " << param_size << std::endl;
        ok = false;
      }
    } else if (arg_pair.first == "damp") {
      ok = Succeeded("Damp", client->Damp());
    } else if (arg_pair.first == "start") {
      ok = Succeeded("Start", client->Start());
    } else if (arg_pair.first == "stand_up") {
      ok = Succeeded("StandUp", client->StandUp());
    } else if (arg_pair.first == "zero_torque") {
      ok = Succeeded("ZeroTorque", client->ZeroTorque());
    } else if (arg_pair.first == "stop_move") {
      ok = Succeeded("StopMove", client->StopMove());
    } else if (arg_pair.first == "switch_move_mode") {
      if (arg_pair.second == "true" || arg_pair.second == "false") {
        bool flag = arg_pair.second == "true";
        ok = Succeeded("SwitchMoveMode", client->SwitchMoveMode(flag));
      } else {
        std::cerr << "invalid argument: " << arg_pair.second << std::endl;
        ok = false;
      }
    } else if (arg_pair.first == "move") {
      std::vector<float> param = stringToFloatVector(arg_pair.second);
      auto param_size = param.size();
      if (param_size == 3) {
        ok = Succeeded("Move", client->Move(param.at(0), param.at(1), param.at(2)));
      } else {
        std::cerr << "Invalid param size for method Move: " << param_size << std::endl;
        ok = false;
      }
    } else if (arg_pair.first == "set_speed_mode") {
      int param;
      ok = stringToInt(arg_pair.second, param) && Succeeded("SetSpeedMode", client->SetSpeedMode(param));
      if (ok) {
        std::cout << "set speed mode to " << arg_pair.second << std::endl;
      }
    } else {
      std::cerr << "Unknown command: [" << arg_pair.first << "]" << std::endl;
      ok = false;
    }

    if (ok) {
      std::cout << "Done!" << std::endl;
    } else {
      exit_code = 1;
    }
  }

  return exit_code;
}
