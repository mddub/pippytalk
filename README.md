# Pippytalk

A talking dog collar controlled by a Pebble watch.

Here's how it works:

[![](http://img.youtube.com/vi/r97SWx9W0MY/mqdefault.jpg)](https://youtu.be/r97SWx9W0MY)

Here's the collar in action:

[![](http://img.youtube.com/vi/PqoeVxEmAFg/mqdefault.jpg)](https://youtu.be/PqoeVxEmAFg)

## Build it

This project has 5 components:

* **Hardware and collar**: Follow Adafruit's amazing [Phone Activated Talking Dog Collar](https://learn.adafruit.com/phone-activated-talking-dog-collar/overview) project guide. Really, 95% of this project is following their instructions!

* **Code for the Adafruit Feather 32u4**: see `PippyCollar/PippyCollar.ino`. This almost exactly matches the sketch in Adafruit's guide, but with an even simpler UART protocol.

* **iPhone app**: see `Pippytalk.xcodeproj`. This is a hastily-modified version of [nRF8001-Swift](https://github.com/MichMich/nRF8001-Swift) by [Michael Teeuw](https://github.com/MichMich/nRF8001-Swift). It forwards messages from the Pebble watch to the Feather board.

* **Pebble app**: see `pebble/`. Use the [Pebble SDK](https://developer.pebble.com/sdk/) or [CloudPebble](https://cloudpebble.net/) to build and install.

* **Sounds**: see `sounds/`. My samples are released under the MIT license; the two from "Up" are used without permission.

## Lessons learned

I hadn't done much soldering or circuit work before this. Maybe my mistakes will save you some headaches!

* The output lines from the amp to the speaker must be a direct connection (as opposed to, say, attached by alligator clips). Something about high frequency signals [requiring a good connection](https://forums.adafruit.com/viewtopic.php?f=19&t=73428). Naturally this ended up being the last connection I chose to solder.

* These [grabber test clips](https://www.aliexpress.com/item/KSOL-12X-SMD-IC-6-Colors-Test-Hook-Clip-Grabbers-Test-Probe/32741602633.html) are amazing for prototyping.

* The "activity" pin on the Soundboard doesn’t work with super high bit rates. (The pin goes low when audio is playing, to sync the red LED to the sounds.) When I encoded my sounds with Ogg Vorbis at [quality 10](http://wiki.audacityteam.org/wiki/OGG#Encoding_and_quality_settings), the pin wouldn’t give feedback. I knew it must be the encoding since the LED worked with all of Adafruit’s samples. After I [re-encoded my samples](http://apple.stackexchange.com/questions/121081/convert-audio-to-ogg-format-via-terminal) at a lower quality (6-8), the ACT pin worked correctly for all of them.

* For two boards to communicate over UART, they must share a common ground, not be powered by two different USB batteries. If you think about voltage and signals for about 10 seconds, you'll understand why. Apparently I didn't have 10 seconds to spare during the few days I spent adding debug statements in Adafruit's Arduino libraries, cycling through boards, doubting my multimeter, and resoldering every joint.

* If you are trying to sense "taps" (shakes) on the Pebble, know that vibrating the watch (`vibe_short_pulse`, etc.) can also trigger a tap. So if you vibrate the watch to give the user feedback when they shake their wrist, you may end up giving him/her a lot more feedback than intended.

## Photos

[A few photos of the build](http://imgur.com/a/V8f1B)
