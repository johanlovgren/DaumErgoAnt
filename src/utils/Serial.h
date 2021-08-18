//
// Created by Johan Lövgren on 2021-08-17.
//

#ifndef DAUMERGOANT_SERIAL_H
#define DAUMERGOANT_SERIAL_H
#include <string>

/**
 * Class for simple USB serial communication
 */
class Serial {
public:
    /**
     * Default constructor
     * @param port name of the serial port which to connect
     * @param baudRate what baud rate to be used
     */
    Serial(const char* port, int baudRate);
    ~Serial();
    /**
     * Opens the serial connection and set it to default (and currently only) settings
     * @return true if successful, otherwise false.
     * If unsuccessful, errno is set accordingly
     */
    bool Open();
    /**
     * Closes the serial connection
     * @return true if successful, otherwise false.
     * If unsuccessful, errno is set accordingly
     */
    bool Close();
    /**
     * Checks if the serial connection is open
     * @return true if open, otherwise false
     */
    bool IsOpen() const;
    /**
     * Write to the serial port
     * @param txBuffer buffer with data to be written
     * @param bytesToWrite how many bytes from txBuffer to write
     * @return number of bytes written. On failure -1 is returned and errno is set accordingly
     */
    int Write(unsigned char* txBuffer, int bytesToWrite) const;
    /**
     * Read from the serial port
     * @param rxBuffer buffer for the received data
     * @param bytesToRead how many bytes to read from the serial port
     * @return number of bytes read. On failure -1 is returned and errno is set accordingly
     */
    int Read(unsigned char* rxBuffer, int bytesToRead) const;
    /**
     * Used to retrieve the name of the serial port
     * @return the name of the serial port
     */
    const char* GetPortName();

private:
    std::string* serialPort;
    int fd, baudRate;
};


#endif //DAUMERGOANT_SERIAL_H
