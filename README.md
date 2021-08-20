# DaumErgoAnt
Transmit power, speed and cadence data from a Daum Electronic ergo bike using an ANT+ USB stick. Enables sharing 
the data to any other ANT+ device. Currently supports the following Daum ergo bikes: 
- [Premium 8i](http://daum-electronic.de/index.php?article_id=48) 
- [8008 TRS](http://daum-electronic.de/index.php?article_id=43)


## Building DaumErgoAnt
### Prerequisites
Currently, the application is only supported on MacOS. 

To build and run the application the following tools are needed:

- [C++14](https://isocpp.org)
- [CMake](https://cmake.org)
- [Make](https://www.gnu.org/software/make/)
- [ANT Mac SDK](https://www.thisisant.com/developer/resources/downloads/)

C++ is easiest retrieved by installing [Xcode](https://developer.apple.com/xcode/), Cmake and Make are available at 
[Homebrew](https://brew.sh) and the ANT Mac SDK is retrieved at 
[thisisant](https://www.thisisant.com/developer/resources/downloads/).

### Building
(This can probably be done in a "better" way, tips are appreciated).

When the SDK folder is downloaded the ANT_LIB header files need to be copied to ``DaumErgoAnt/src/libs/includes`` and 
the static library to ``DaumErgoAnt/src/libs/``. Can be done with the following commands:

```
cp `find ./ANT-SDK_Mac.3-2.5/ANT_LIB/ -name '*.h*'` ./DaumErgoAnt/src/libs/includes/ 
cp ANT-SDK_Mac.3-2.5/Bin/libantbase.a ./DaumErgoAnt/src/libs/
```

To build the application the ANT+ network key need to be added to DaumErgoAnt/network_key.cfg inside
the curly brackets.
```
/***************** ANT+ Network key *****************/
#define NETWORK_KEY { /* ENTER NETWORK KEY HERE */  };
// Example
// #define NETWORK_KEY { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  };
/***************** ANT+ Network key *****************/
```

The network key can be found [here](https://www.thisisant.com/developer/ant-plus/ant-plus-basics/network-keys) and is a
8 hex value. This key is not allowed to be shared due to licensing.

When all ANT_LIB headers and the static library is copied and the network key is added, the application can be built 
with the following commands:
```
cd ./DaumErgoAnt
cmake .
make
```

## Usage
```
Usage: 
	daum-ergoant <option(s)> BIKETYPE [PARAMETERS] <ANT device number>
Options:
	-h                       Show this help message
	-v                       Verbose output
Biketypes:
	p8i [IP ADDRESS]         Premium 8i
	8k8trs [SERIAL_PORT]     8008trs:
ANT device number:       
	0 for first USB stick plugged etc.
```
When the application is run together with any of the supported ergo bikes and an ANT USB stick, the application will 
simulate an ANT+ power sensor with ID 49 and an ANT+ speed/cadence sensor with ID 50. When the application is started, 
these devices should be visible for other devices to connect and retrieve data.

### Premium 8i

To use the application together with a Premium 8i bike, an ANT+ USB stick needs to be connected and the bike needs 
to be connected to the local network either by using an ethernet cable or any WiFi solution. 
Retrieve the bikes IP address and run the application with the following command (example IP address and first connected 
USB stick is used):
```
./daum-ergoant p8i 192.168.0.2 0
```

#### Tested hardware
Works with the following hardware:
- Elite USB ANT+ Key
- Ethernet cable


### 8008 TRS
To use the application together with an 8008 TRS bike, an ANT+ USB stick needs to be connected and the bike needs to be 
connected by using the Daum Electronic RS232 update-cable together with an RS232-USB converter. A "normal" RS232 
cable is not supported, and the
update-cable can either be bought from Daum Electronics, or be build according to the instructions 
[here](https://bikeboard.at/Board/showthread.php?2050-Schnittstellenkabel-RS-232-an-PC). (Tips where any similar 
cable can be retrieved are appreciated). 

Retrieve the name of the connected serial port (by using for example 
[pySerials](https://pyserial.readthedocs.io/en/latest/tools.html#module-serial.tools.list_ports) tool list_ports) 
and run the application with the following command (example serial port and first connected USB stick is used):
```
./daum-ergoant 8k8trs '/dev/cu.usbserial-1430' 0
```

#### Tested hardware
Works with the following hardware:
- Elite USB ANT+ Key
- [DIY RS232 cable](https://bikeboard.at/Board/showthread.php?2050-Schnittstellenkabel-RS-232-an-PC)
- Goobay USB serial RS232 converter