#include <iostream>
#include <vector>
#include <cstring>
#include <libusb-1.0/libusb.h>


int main()
{
    libusb_init(NULL);
    libusb_device_handle* dev_handle = libusb_open_device_with_vid_pid(NULL, 0x054c, 0x09af);

    if (!dev_handle) {
        std::cerr << "Device not found or unable to open.\n";
        libusb_exit(NULL);
        return 1;
    }

    // Claim interface 5
    if (libusb_claim_interface(dev_handle, 5) < 0) {
        std::cerr << "Could not claim interface.\n";
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }


    struct {
        uint8_t cmd_id;
        uint8_t unused;
        uint8_t magic;
        uint8_t payload_len;
        uint32_t payload;
    } poweroffPacket;

    poweroffPacket.cmd_id = 0x13;
    poweroffPacket.unused = 0x00;
    poweroffPacket.magic = 0xaa;
    poweroffPacket.payload_len = 0x04;
    poweroffPacket.payload = 0x01;

    // Send poweroff command to 0x84 endpoint
    int transferred;
    int resp = libusb_interrupt_transfer(dev_handle, 0x04, (uint8_t*)&poweroffPacket, sizeof(poweroffPacket), &transferred, 0);
    if(resp < 0) {
        std::cerr << "Could not send poweroff command: " << libusb_error_name(resp) << "\n";
        std::cout << "Transferred: " << transferred << "\n";
        libusb_release_interface(dev_handle, 0);
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }


    std::cout << "Hello World!\n";
    libusb_exit(NULL);
    return 0;
}