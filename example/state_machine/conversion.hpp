#pragma once

#include <array>

#include "comm.h"
#include "unitree/idl/go2/LowCmd_.hpp"

#include "common/crc32.hpp"

namespace unitree::common
{

    void motorCmd2Dds(UNITREE_LEGGED_SDK::MotorCmd *raw, std::array<unitree_go::msg::dds_::MotorCmd_, 20> &dds)
    {
        for (int i = 0; i < 20; i++)
        {
            dds[i].mode(raw[i].mode);
            dds[i].q(raw[i].q);
            dds[i].dq(raw[i].dq);
            dds[i].tau(raw[i].tau);
            dds[i].kp(raw[i].Kp);
            dds[i].kd(raw[i].Kd);

            memcpy(&dds[i].reserve()[0], &raw[i].reserve[0], 12);
        }
    };

    void bmsCmd2Dds(UNITREE_LEGGED_SDK::BmsCmd &raw, unitree_go::msg::dds_::BmsCmd_ &dds)
    {
        dds.off(raw.off);

        memcpy(&dds.reserve()[0], &raw.reserve[0], 3);
    };

    void lowCmd2Dds(UNITREE_LEGGED_SDK::LowCmd &raw, unitree_go::msg::dds_::LowCmd_ &dds)
    {
        // with crc
        memcpy(&dds.head()[0], &raw.head[0], 2);

        dds.level_flag(raw.levelFlag);
        dds.frame_reserve(raw.frameReserve);

        memcpy(&dds.sn()[0], &raw.SN[0], 8);
        memcpy(&dds.version()[0], &raw.version[0], 8);

        dds.bandwidth(raw.bandWidth);

        motorCmd2Dds(&raw.motorCmd[0], dds.motor_cmd());
        bmsCmd2Dds(raw.bms, dds.bms_cmd());

        memcpy(&dds.wireless_remote()[0], &raw.wirelessRemote[0], 40);

        memcpy(&dds.led()[0], &raw.led[0], 12); // go2
        memcpy(&dds.fan()[0], &raw.fan[0], 2);
        dds.gpio(raw.gpio); // go2

        dds.reserve(raw.reserve);

        raw.crc = unitree::common::Crc32Core((uint32_t *)&raw, (sizeof(raw) >> 2) - 1);

        dds.crc(raw.crc);
    };
}