# Bar graph for the Renogy Wanderer PWM solar charge controller
Camping off-grid and dragging your solar panels around trying to find the strongest sunshine in the gaps between the tree shadows? Tired of running back to the trailer, squinting at the tiny display in the sunshine, waiting for the solar current screen to come around? Me too. This handy display shows a bargraph of solar current, by querying the Renogy Wanderer on its modbus interface.

### Acknowledgements
This is not much of my own work - it's a collection of instructions and code found on the following excellent and comprehensive write-ups:
* [SolarThing Renogy](https://solarthing.readthedocs.io/en/latest/quickstart/serial-port/hardware/rover/rs232-port.html)
* [SolarShed Wanderer](https://github.com/BarkinSpider/SolarShed/blob/master/RenogyWanderer.py)

### How it works, construction notes
Max current: I set this so that the full bar graph would show at 6 amps, which is about the most my solar panels will provide.

Board: I chose an old Arduino Nano I had lying around. Cheap and cheerful. 5 volts so it works directly with the LED strip.

Regulator: The on-board 5 V regulator would probably have worked for just monitoring the Renogy, but for the LEDs at full brightness I might need 100 mA at 5 V. I opted for a tiny 12->5 V buck converter instead.

MAX232: I had one of these lying around too.

Wiring:  
![Connections](https://github.com/thomasabbott/wanderer/blob/main/renogybargraph.png?raw=true)

Mounting:
I could have designed and printed a fancy traffic light to mount where it could be seen from all around... instead I wrapped the boards in electrical tape and stuck the bargraph to the front of the Wanderer  
![Installed and working](https://github.com/thomasabbott/wanderer/blob/main/bargraphinstalled.jpg?raw=true)

### Wanderer notes
The Renogy Wanderer is small and cheap but it is a high-quality design inside. Unlike many products it has a true common negative bus, with high-side switching and high-side current monitoring of the solar and load connections. It includes a fuse for the battery connection. The board is conformal coated against moisture, though this did not help my first one as the test points are not coated and the corrosion crept from there into all of the sensitive parts.

If you don't have a MAX-232 board, I recommend just opening up the Wanderer and removing the internal MAX-232, routing the 3.3 v serial port directly the connector. You could also route 3.3 V to the port, instead of 12 V, and save some trouble there too. Hey just stick the Arduino inside the case and have the bargraph hanging out.

There is a lot of other good information available in the modbus registers, including total solar production in Wh or Ah. By querying this, and/or integrating the load and charge current, you could build a primative battery fuel gauge.

