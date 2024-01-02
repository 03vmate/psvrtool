#include <iostream>
#include <libusb-1.0/libusb.h>

int main()
{
    libusb_init(NULL);
    std::cout << "Hello World!\n";
    libusb_exit(NULL);
    return 0;
}