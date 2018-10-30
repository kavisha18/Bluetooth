BLUETOOTH and PULSEAUDIO Setup

1. Install the "bluez" package, providing the bluetooth protocol stack

2. Install the "bluez-utils" package, providong the bluetoothctl utility. 

3. The generic Bluetooth driver is the "btusb" kernel Module. Check if that module is loaded, if it's not, then load the module.
	3.1 to load a module: "# modprobe module_name"

4. Start/Enable "bluetooth.service"
** By default, your bluetooth adapter will not power on after a reboot. In order to have auto power-on after a reboot add "AutoEnable=true" in /etc/bluetooth/main.conf in the [Policy} section:
[Policy]
AutoEnable=true

5.In order to enable your system to be detected as A2DP sink, create a audio.conf file , /etc/bluetooth/audio.conf
add the following lines
[General]
Enable=Source,Sink,Media,Socket

6. Install following packages for pulseadio sett up
	5.1 "pulseaudio" 
	5.2 "pulseaudio-alsa" 
	5.3 "pulseaudio-bluetooth"
	5.4 "pavucontrol"
7. PulseAudio will first look for configuration files in the home directory ~/.config/pulse, then system-wide /etc/pulse.
It is strongly suggested not to edit system-wide configuration files, but rather edit user ones. Create the ~/.config/pulse directory, then copy the system configuration files into it and edit according to your need. 

OFONO Setup

1. In order to compile telephony stack you need the following packages:
	1.1 GCC Compiler
	1.2 GLib Library
	1.3 D-bus Library

2. Download tar file for ofono from: https://kernel.googlesource.com/pub/scm/network/ofono/ofono/+/master

3. in the ofono-master directory run ./bootstrap ; ./bootstrap-configure ; ./configure

4. run sudo make && make install

5. Enable and start ofono service. 
	systemctl enable ofono
	systemctl start ofono


Configure automatically searches for all required components and packages.
To compile and install run : make && make install

HOW TO GET OFONO, PULSEAUDIO and BLUETOOTH COMPLIED TO EACH OTHER
1. In '/etc/pulse/default.pa' find the line 'load-module module-bluetooth-discover' and change it in 'load-module module-bluetooth-discover headset=ofono' 

2. Add the user "pulse" to group "bluetooth" to grant the permission: "sudo useradd -g bluetooth pulse"

3. To grant the permission, add this to /etc/dbus-1/system.d/ofono.conf (before /busconfig> ):

<policy user="pulse">
 <allow send_destination="org.ofono"/>
</policy>

4. Run "pavuconrol", under configuration tab make sure that yor connected device has all profile available i.e A2DP and HSP/HFP

5. Run "pactl lists" and make sure the sinks are not SUSPENDED. If they are, disable suspend-on-idle in /etc/pulse/default.pa
	comment out "load-module module-suspend-on-idle"
	and then "killall pulseaudio" to restart it

	
ADDITIONAL PACKAGES TO BUILD PACKAGES....
automake
autoconf
libtool
c-ares

** 
