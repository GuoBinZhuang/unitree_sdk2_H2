#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <stdexcept>
#include <unitree/robot/as2/sport/sport_client.hpp>

#include "common/sport_test_option.hpp"

using namespace std;

const std::vector<unitree::common::TestOption> option_list =
    {
        {"damp", 0},
        {"balance_stand", 1},
        {"stop_move", 2},
        {"stand_down", 3},
        {"recovery_stand", 4},
        {"move", 5},
        {"switch_gait", 6},
        {"speed_level", 7},
        {"get_state", 8},
        {"recovery_switch", 9},
        {"body_height", 10},
        {"stand_up", 11},
        
        // Caution:  test in open area
        {"enter_leftside_gait", 12},
        {"exit_leftside_gait", 13},
        {"enter_handstand", 14},
        {"exit_handstand", 15},
        {"front_flip", 16},
        {"back_flip", 17},
        {"pose", 18},
        {"euler", 19},

        {"switch_joystick", 20},

        {"enter_biped_stand", 21},
        {"exit_biped_stand", 22},
        {"enter_rightside_gait", 23},
        {"exit_rightside_gait", 24},
        {"greeting", 25},
        {"heart", 26},
        {"content", 27},
        {"dance1", 28},
        {"dance2", 29},
        {"handshake", 30},
        {"stretch", 31},
        {"enter_sit", 32},
        {"exit_sit", 33},
        {"front_jump", 34},
        {"push_up", 35},
        {"up_jump", 36},

};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " networkInterface" << std::endl;
        exit(-1);
    }
    unitree::robot::ChannelFactory::Instance()->Init(0, argv[1]);

    unitree::common::TestOption test_option;
    test_option.id = 1;

    unitree::robot::as2::SportClient sport_client;
    sport_client.SetTimeout(25.0f);
    sport_client.Init();

    unitree::common::UserInterface user_interface(option_list, &test_option);

    std::cout << "Input \"list \" to list all test option ..." << std::endl;
    long res_count = 0;
    while (1)
    {
        auto time_start_trick = std::chrono::high_resolution_clock::now();
        static const constexpr auto dt = std::chrono::microseconds(20000); // 50Hz

        user_interface.terminalHandle();

        int res = 1;
        if (test_option.id == 0)
        {
            res = sport_client.Damp();
        }
        else if (test_option.id == 1)
        {
            res = sport_client.BalanceStand();
        }
        else if (test_option.id == 2)
        {
            res = sport_client.StopMove();
        }
        else if (test_option.id == 3)
        {
            res = sport_client.StandDown();
        }
        else if (test_option.id == 4)
        {
            res = sport_client.RecoveryStand();
        }
        else if (test_option.id == 5)
        {
            res = sport_client.Move(0.0, 0.0, 0.5);
        }
        else if (test_option.id == 6)
        {
            res = sport_client.SwitchGait(0);
        }
        else if (test_option.id == 7)
        {
            res = sport_client.SpeedLevel(1);
        }
        else if (test_option.id == 8)
        {
            std::map<std::string, std::string> state_map;
            res = sport_client.GetState(state_map);
            std::cout << "fsm_id: " <<  state_map["fsm_id"] << std::endl;
            std::cout << "fsm_name: " <<  state_map["fsm_name"] << std::endl;
            std::cout << "speed_level: " << state_map["speed_level"] << std::endl;
            std::cout << "auto_recovery_switch: " << state_map["auto_recovery_switch"] << std::endl;
            std::cout << "process_state: " << state_map["process_state"] << std::endl;
        }
        else if (test_option.id == 9)
        {
            res = sport_client.SetAutoRecovery(0);
        }
        else if (test_option.id == 10)
        {
            res = sport_client.BodyHeight(0.3f);
        }
        else if (test_option.id == 11)
        {
            res = sport_client.StandUp();
        }
        else if (test_option.id == 12)
        {
            res = sport_client.LeftSideGait(1);
        }
        else if (test_option.id == 13)
        {
            res = sport_client.LeftSideGait(0);
        }
        else if (test_option.id == 14)
        {
            res = sport_client.HandStand(1);
        }
        else if (test_option.id == 15)
        {
            res = sport_client.HandStand(0);
        }
        else if (test_option.id == 16)
        {
            res = sport_client.FrontFlip();
        }
        else if (test_option.id == 17)
        {
            res = sport_client.BackFlip();
        }
        else if (test_option.id == 18)
        {
            res = sport_client.BodyPosition(0.2f, 0.2f, -0.2f, 0.2f);
        }
        else if (test_option.id == 19)
        {
            res = sport_client.Euler(0.2f, 0.3f, 0.3f);
        }
        else if (test_option.id == 20)
        {
            res = sport_client.SwitchJoystick(0);
        }
        else if (test_option.id == 21)
        {
            res = sport_client.BipedStand(1);
        }
        else if (test_option.id == 22)
        {
            res = sport_client.BipedStand(0);
        }
        else if (test_option.id == 23)
        {
            res = sport_client.RightSideGait(1);
        }
        else if (test_option.id == 24)
        {
            res = sport_client.RightSideGait(0);
        }
        else if (test_option.id == 25)
        {
            res = sport_client.Greeting();
        }
        else if (test_option.id == 26)
        {
            res = sport_client.Heart();
        }
        else if (test_option.id == 27)
        {
            res = sport_client.Content();
        }
        else if (test_option.id == 28)
        {
            res = sport_client.Dance1();
        }
        else if (test_option.id == 29)
        {
            res = sport_client.Dance2();
        }
        else if (test_option.id == 30)
        {
            res = sport_client.Handshake();
        }
        else if (test_option.id == 31)
        {
            res = sport_client.Stretch();
        }
        else if (test_option.id == 32)
        {
            res = sport_client.Sit(1);
        }
        else if (test_option.id == 33)
        {
            res = sport_client.Sit(0);
        }
        else if (test_option.id == 34)
        {
            res = sport_client.FrontJump();
        }
        else if (test_option.id == 35)
        {
            res = sport_client.PushUp();
        }
        else if (test_option.id == 36)
        {
            res = sport_client.UpJump();
        }

        if (res < 0)
        {
            res_count += 1;
            std::cout << "Request error for: " << option_list[test_option.id].name << ", code: " << res << ", count: " << res_count << std::endl;
        }
        else
        {
            res_count = 0;
            std::cout << "Request successed: " << option_list[test_option.id].name << ", code: " << res << std::endl;
        }
        std::this_thread::sleep_until(time_start_trick + dt);
    }
    return 0;
}