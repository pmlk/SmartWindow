##Raspi setting up wpan interface

**issues / questions**  

* unable to set ip address of wpan/lowpan interface
* how do we listen for packages on the interface?
* step-by-step instructions after setting up interface (ip!)

command|description
---|---
`sudo kill <PID of ifplugd process>`| kill process
``sudo kill -s KILL `pgrep ifplugd` `` | kill all ifplugd processes
`sudo ip link set wpan0 down` | disable interface, otherwise not able to set pan_id
`sudo ip link set lowpan0 down`| disable interface
`sudo iwpan phy phy0 set channel 0 17`| set page and channel (0 17)
`sudo ip link add link wpan name lowpan0 type lowpan` | add lowpan interface link?
`sudo iwpan dev wpan0 set pan_id 0xbeef` | set wpan pan_id to 0xbeef
`sudo ip link set wpan0 up` | enable interface
`sudo ip link set lowpan0 up` | enable interface
`sudo ip address add fd2d:0388:6a7b:0002:0000:0000:0000:0001/120 dev lowpan0` | set device ip address (***Operation not supported***)


**some useful commands**  

```bash
ifconfig
ifconfig INTERFACE up/down  
iwpan phy0 info  
iwpan wpan0 info  
iwpan wpan0 set pan_id 0xbeef  
sudo iwpan phy0 set channel 0 17  
sudo ip link add link wpan0 name lowpan0 type lowpan  
modprobe
```



---

##RIOT setting up wpan interface

`cd` to folder `../RIOT/examples/gnrc_networking`  
set necessary environment variables `$PATH`and `$BOARD`  
`make && make flash && make term`

```bash
export PATH=/Users/Patrick/Documents/Programmieren/SmartUni/gcc-arm-none-eabi-4_9-2015q3/bin/:$PATH
export BOARD=samr21-xpro
sudo make
sudo make flash
sudo make term
```

in terminal on board

command | description
---|---
ifconfig | print current settings, look for Iface # (=\<if_id>), default is 7
ifconfig \<if_id> set \<key> \<value> | see ifconfig -h
ifconfig 7 set pan_id 0xbeef | set pan_id to 0xbeef
ifconfig 7 set addr \<value> | set ip (**own, or receiver??**)
ifconfig 7 set channel 17 | set channel to 17









