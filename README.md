# libni
Library for Network Input
=========================

Open library and examples for building and receiving commands
from networked input devices.

In a Libni setup there are usually two type of devices:

1. Controllers: the devices which send network updates. Controllers usually have buttons and/or some 
   analog sensors (sliders, turn knobs, gyroscopes). These could also be
   modern, unsought of input devices to augment your VR or AR experience, like addons
   for your Oculus Rift or Google Cardboard.

2. Receivers: these devices receive the inputs from the controllers and
   act depending on them. These could be games with the libni receiver library,
   a drone or other remote controlled vehicle, or the nii (Network Input Injector),
   turning the received signals into keystrokes and mouse events.

Libni enabled controllers send about every 10-20ms packages with update on all sensors it
is responsible for. Therefore it can easily use UDP as some loss in
data is covered with the next package.

The package has two formats, signalled in its header (the header is constant):

Header:
- Byte 0-3: always "LBNI" - a magic header
- Byte 4-5: The protocol version, so far always 1 (in two bytes: 0, 1)
- Byte 6-7: Protocol Type: either 0 or 1 (there are these types:0 is a
bitblock, 1 is a number of controls + their values, description below)
- Byte 8-11: UID, uniuque id, just chose 4 random bytes
- Byte 12-15: a specific client id to distiguish diffrent controllers,
but have fixed ids

These 15 bytes are followed by the controller data.

If the protocol type is 0, the data looks like the following:
- 32 bytes (256bits): status of 256 buttons (bit=0 means unpressed, 1
means pressed). In byte 0 are buttons 0-7, in byte 1 buttons 6-15, ...
Basically if only button 0 is pressed the first byte is 1.
- 32 bytes with 2-byte analog values, meaning 16 values. The values
are from -32768 to 32767, thefore ie 255,255 means -1.

If the protocol type is 1, it is a slightly simpler but less compressed format:
It follows a list of events.

An Event starts with its type and is followed by a load of different
length depending on the type:
- type: 0    - no load
- type: 1    - load: 1 byte with button number
- type: 2    - load: 1 byte with button number
- type: 3    - load: 1 byte with analog number + 2 byte big endian value
- type: 4    - load: 2 byte with big endian analog number + 4 byte big endian value or 32 big endian
  floating point
- type: 5    - load: 4 byte id + 16 byte data

You can send the packages via udp, tcp, or mqtt (tcp does not yet work
in all projects).

This is the current structure of the libni-project:

```
/controller - examples for libni controllers (esp, libgdx, ...)
  /bash - a small socat script to just send udp-data somewhere
  /esp8266 - examples for esp8266 (including my touch library)
  /libgdx - a small virtual joystick for desktop or mobile phone
simulating a joystick and a fire button
  /python - a test desktop client similar to the libgdx client

/receiver - example for receivers
  /java - libgdx receiver library and nii - java network input injector
  /python - skeleton for a potential receiver in python
  /shell - a small socat script to just dump the udp-data on the screen
```
