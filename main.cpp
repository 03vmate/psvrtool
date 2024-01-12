#include <iostream>
#include <vector>
#include <cstring>
#include <libusb-1.0/libusb.h>
#include <string>
#include "CLI11.hpp"

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

size_t genPacket(uint8_t *dst, uint8_t cmdId, uint8_t *params, uint8_t paramsLen)
{
    dst[0] = cmdId;
    dst[1] = 0x00;
    dst[2] = 0xaa;
    dst[3] = paramsLen;
    memcpy(&dst[4], params, paramsLen);
    return paramsLen + 4;
}

void transferPacket(libusb_device_handle *dev_handle, uint8_t *packet, size_t packetLen)
{
    int transferred;
    int resp = libusb_interrupt_transfer(dev_handle, 0x04, packet, packetLen, &transferred, 0);
    if (resp < 0)
    {
        std::cerr << "Could not send poweroff command: " << libusb_error_name(resp) << "\n";
        libusb_release_interface(dev_handle, 0);
        libusb_close(dev_handle);
        libusb_exit(NULL);
        exit(1);
    }
    std::cout << "Transferred: " << transferred << "\n";
}

int main(int argc, char **argv)
{
    CLI::App app{"psvrtool"};

    CLI::App* cinemaModeCmd = app.add_subcommand("cinema_mode", "Render HDMI input as a flat 2D screen, with tracking and morphing handled by the processor unit");
    int brightness = 35;
    int screen_size = 35;
    cinemaModeCmd->add_option("-b,--brightness", brightness, "Brightness of the screen (percentage, 0-100)")->required(false);
    cinemaModeCmd->add_option("-s,--screen-size", screen_size, "Size of the screen (percentage, 0-100)")->required(false);

    app.add_subcommand("poweroff", "Power off the PSVR headset and processor unit");

    CLI11_PARSE(app, argc, argv);

    libusb_init(NULL);
    libusb_device_handle *dev_handle = libusb_open_device_with_vid_pid(NULL, 0x054c, 0x09af);

    if (!dev_handle)
    {
        std::cerr << "Device not found or unable to open.\n";
        libusb_exit(NULL);
        return 1;
    }

    // Claim interface 5
    if (libusb_claim_interface(dev_handle, 5) < 0)
    {
        std::cerr << "Could not claim interface.\n";
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }

    uint8_t *packet = new uint8_t[256];

    if (app.got_subcommand("cinema_mode"))
    {
        std::cout << "Setting cinema mode\n";
        // Disable VR mode
        uint32_t vrModePayload = 0x00;
        size_t packetLen = genPacket(packet, 0x23, (uint8_t *)&vrModePayload, 4);
        transferPacket(dev_handle, packet, packetLen);

        // Set up cinema mode
        struct
        {
            uint8_t mask; // ??? 255 works though
            uint8_t screen_size; //26 to 100
            uint8_t screen_distance; // ???
            uint8_t interpupillary_distance; // ???
            uint8_t reserved[6];
            uint8_t brightness; //0 to 32
            uint8_t mic_volume;
            uint8_t reserved2[4];
        } cinemaPayload;
        cinemaPayload.mask = 255;
        cinemaPayload.screen_size = map(screen_size, 0, 100, 26, 100);
        cinemaPayload.screen_distance = 0;
        cinemaPayload.interpupillary_distance = 0;
        cinemaPayload.brightness = map(brightness, 0, 100, 0, 32);
        cinemaPayload.mic_volume = 127;
        packetLen = genPacket(packet, 0x21, (uint8_t *)&cinemaPayload, 16);
        transferPacket(dev_handle, packet, packetLen);
    }
    else if(app.got_subcommand("poweroff")) {
        std::cout << "Powering off\n";
        uint32_t poweroffPayload = 0x01;
        size_t packetLen = genPacket(packet, 0x13, (uint8_t*)&poweroffPayload, 4);
        transferPacket(dev_handle, packet, packetLen);
    }

    libusb_exit(NULL);
    return 0;
}