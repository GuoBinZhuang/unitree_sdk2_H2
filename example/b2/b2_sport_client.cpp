#include <iostream>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <stdexcept>
#include <unitree/robot/b2/sport/sport_client.hpp>

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
        {"hand_stand", 8},
        {"auto_recovery_set", 9},
        {"free_walk", 11},
        {"classic_walk", 12},
        {"fast_walk", 13},
        {"euler", 14},
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

    unitree::robot::b2::SportClient sport_client;
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
            res = sport_client.HandStand(true);
        }
        else if (test_option.id == 9)
        {
            res = sport_client.AutoRecoverySet(true);
        }
        else if (test_option.id == 11)
        {
            res = sport_client.FreeWalk();
        }
        else if (test_option.id == 12)
        {
            res = sport_client.ClassicWalk(true);
        }
        else if (test_option.id == 13)
        {
            res = sport_client.FastWalk(true);
        }
        else if (test_option.id == 14)
        {
            res = sport_client.Euler(0.0, 0.2, 0.0);
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