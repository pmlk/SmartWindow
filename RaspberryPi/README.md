# Raspberry Pi
All source code to be compiled and run on the RasPi is located in this directory.

---
The RasPi must be correctly configured to be able to communicate with RIOT nodes. Run the `setupLowpanInterface` script before running the *window controller* programm. It's a good idea to run the script every time the RasPi boots. In `/etc/rc.local` add the following line anywhere before `exit 0`:

```
/<path>/<to>/<script>/setupLowpanInterface
```

The script sets up the lowpan device's **channel** to **26** and **pan id** to **0x23**. Note that these settings must match on all nodes for network communications to work properly.

For more information on setting up the Raspberry Pi see the wiki!

[//]: # (f)