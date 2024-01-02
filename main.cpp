#include <iostream>
#include <vector>
#include <cstring>
#include <libusb-1.0/libusb.h>

size_t genPacket(uint8_t* dst, uint8_t cmdId, uint8_t *params, uint8_t paramsLen)
{
    dst[0] = cmdId;
    dst[1] = 0x00;
    dst[2] = 0xaa;
    dst[3] = paramsLen;
    memcpy(&dst[4], params, paramsLen);
    return paramsLen + 4;
}

int main()
{
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

    uint32_t poweroffPayload = 0x01;
    uint8_t* poweroffPacket = new uint8_t[256];
    size_t poweroffPacketLen = genPacket(poweroffPacket, 0x13, (uint8_t*)&poweroffPayload, 4);

    // Send poweroff command to 0x84 endpoint
    int transferred;
    int resp = libusb_interrupt_transfer(dev_handle, 0x04, poweroffPacket, poweroffPacketLen, &transferred, 0);
    if (resp < 0)
    {
        std::cerr << "Could not send poweroff command: " << libusb_error_name(resp) << "\n";
        libusb_release_interface(dev_handle, 0);
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return 1;
    }
    std::cout << "Transferred: " << transferred << "\n";
    libusb_exit(NULL);
    return 0;
}