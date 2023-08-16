# Test RNG PRG260
This is a test for the PRG260 Random Number Generator Driver component in TRENTOS.


## Test Setup
In order to allow the setup to work the host system must be prepared.

1. PRG260 RNG must be connected to the host system via UART (USB).
2. /dev folder is mounted in docker container
3. SerialProxy component connects Qemu TCP uart server with the PRG260 uart device


### Udev Rule
To allow for the non root mount of /dev in docker a udev rule must be added to the system

open file `/etc/udev/rules.d/99-serial.rules`
```sh
KERNEL=="ttyUSB[0-9]*",MODE="0666"
```

After this reboot or apply udev rule with:
```sh
sudo udevadm control --reload-rules && sudo udevadm trigger
```

## Launching of the test
Docker requires some extra parameters in order to get the test working correctly.

Launch the test with the following command:
```sh
cd <seos_test root>
src/build.sh build-and-test test_rng_prg260 -d '-v /dev:/dev --privileged'
```

## Sources
How to use uart devices in docker: [Losant Serial Devices Docker](https://www.losant.com/blog/how-to-access-serial-devices-in-docker)
