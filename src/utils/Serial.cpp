//
// Created by Johan Lövgren on 2021-08-17.
//

#include "Serial.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

Serial::Serial(const char* port, int baudRate) {
    serialPort = new std::string(port);
    this->baudRate = baudRate;
    fd = -1;
}

Serial::~Serial() {
    delete serialPort;
}

bool Serial::Open() {
    fd = open(serialPort->c_str(), O_RDWR);
    if (fd < 0) {
        return false;
    }
    tcflush(fd, TCIOFLUSH);
    struct termios settings{};
    if (tcgetattr(fd, &settings)) {
        close(fd);
        return false;
    }
    cfsetispeed(&settings, baudRate);
    cfsetospeed(&settings, baudRate);
    settings.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
    settings.c_iflag |= IGNPAR | IGNBRK;
    settings.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    settings.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE | CRTSCTS);
    settings.c_cflag |= CLOCAL | CREAD | CS8;
    settings.c_lflag &= ~(ICANON | ISIG | ECHO);
    settings.c_cc[VTIME] = 1;
    settings.c_cc[VMIN]  = 0;
    if (tcsetattr(fd, TCSANOW, &settings) < 0) {
        close(fd);
        return false;
    }

    return true;
}

bool Serial::Close() {
    if (!this->IsOpen()) {
        return true;
    }
    if (close(fd) < 0)
        return false;
    fd = -1;
    return true;
}

int Serial::Write(unsigned char* txBuffer, int bytesToWrite) const {
    return write(fd, txBuffer, bytesToWrite);
}

int Serial::Read(unsigned char* rxBuffer, int bytesToRead) const {
    return read(fd, rxBuffer, bytesToRead);
}

bool Serial::IsOpen() const {
    return fd > 0;
}

const char* Serial::GetPortName() {
    return serialPort->c_str();
}

