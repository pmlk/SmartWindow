```
ifconfig  
ifconfig INTERFACE up/down  
iwpan phy0 info  
iwpan wpan0 info  
iwpan wpan0 set pan_id 0xbeef  
sudo iwpan phy0 set channel 0 17  
sudo ip link add link wpan0 name lowpan0 type lowpan  
modprobe  

ip -6 neighbor show  
ip link show  
ping6 ff02::1 -I lowpan0



```
