# My first simple linux kernel module
When I got my new laptop, the cpu wasn't able to maintain frequency specified in intel's datasheet. After doing some research I came across [ThrottleStop](https://github.com/agoose77/throttlestop) project and saw this as a great opportunity to learn how to code kernel modules. Currently it only supports undervolting but I plan to add power and temperature controlling features.

## Installing
1. Clone this repository into `/usr/src/cpuctl-0.1`
2. Run `sudo dkms install cpuctl/0.1`
3. If you have Secure Boot enabled, sign the compiled kernel module located in `/lib/modules/$(uname -r)/updates/dkms/` with your [MOK](https://wiki.debian.org/SecureBoot#MOK_-_Machine_Owner_Key) key or custom Secure Boot keys.
