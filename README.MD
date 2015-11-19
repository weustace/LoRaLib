## RFM98W Library
### William Eustace–Team Impulse

_ This library is made available under the MIT licence; please see the repository root. _

The object of this library is to permit easy control of a Hope RF RFM98W radio module (which uses the rather
impressive LoRa modulation to extend range beyond what would normally be plausible in this frequency band.) from an Arduino, or similar microcontroller (it has been used extensively by Team Impulse on the Teensy 3.1).

These modules are 434MHz; although it has not been tested, I see no reason why this code should not work for
any Semtech SX1276 based system, since the interface (over SPI) is broadly standard. Some variables might need tweaking.

Bandwidths down to 7.8kHz are achievable, but not with the RFM98W, since this has an ordinary crystal oscillator rather than
the required Temperature-Compensated Crystal Oscillator (TCXO). If the crystal is towards the edge of the specified frequency,
the centre frequency is too far off to be received with a smaller bandwidth. In other words, lower
bandwidth operations are infeasible (20.8kHz works on some modules but not others, and below this has not
been successfully tested) without a more accurate crystal or a TCXO. This has caused me a lot of grief
during the development of the library, until it was determined to be the issue!

This system has been successfully tested for High Altitude Balloon (HAB) flight, with range significantly exceeding 100km laterally. Line of sight is the limit, rather than distance; the high link budget suggests that if you can 'see' the balloon, it would be surprising in the extreme if you could not receive packets from it. It has also been successfully tested on a high-power rocket launch (European CanSat competition) to 3200 feet (1000m).

If you require assistance with the use of this library, please email the address provided on my GitHub profile.
