# Micro-ROS Pico W

Basic template for micro-ROS with the Raspberry Pi Pico W (via udp4) and c++.

## Getting Started

### Dependencies

Install the [pico-sdk](https://github.com/raspberrypi/pico-sdk) and the [micro-ROS raspberrypi pico sdk](https://github.com/micro-ROS/micro_ros_raspberrypi_pico_sdk):

```bash
sudo apt install cmake g++ gcc-arm-none-eabi doxygen libnewlib-arm-none-eabi git python3
git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git $HOME/pico-sdk

git clone https://github.com/micro-ROS/micro_ros_raspberrypi_pico_sdk.git $HOME/micro_ros_raspberrypi_pico_sdk -b jazzy
mkdir $HOME/micro_ros_raspberrypi_pico_sdk/build
cd $HOME/micro_ros_raspberrypi_pico_sdk/build
cmake ..
make
```


### Installing

* Clone this project
* Export the path to the pico-sdk and the micro-ros-sdk
* Build the project

```bash
git clone https://github.com/nfriedrich00/micro_ros_pico_w.git

export PICO_SDK_PATH=$HOME/pico-sdk
export PICO_MICRO_ROS_SDK_PATH=$HOME/micro_ros_raspberrypi_pico_sdk

mkdir micro_ros_pico_w/build
cd micro_ros_pico_w/build
cmake ..
make
```


## Authors

Contributors names and contact info

[Nils Friedrich](mailto:nils-jonathan.friedrich@student.tu-freiberg.de)

## Version History

* 0.1
    * Initial Release

## Acknowledgments

* [PICO-MAZING for the pico_wifi_transport](https://github.com/PICO-MAZING/uros_pico_w)

