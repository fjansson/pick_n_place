# Pick and place machine code

## Joystick handling

### pro_micro_joystick_hid_mouse/pro_micro_joystick_hid_mouse.ino
Analog gameport joystick -> USB HID joystick, using a [Pro Micro](https://www.sparkfun.com/products/12640) module.

#### References

* using [HID-Project library](https://github.com/NicoHood/HID) - need to install this in the Arduino IDE.
* parts from [pro micro tutorial](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard)
* [hat switches](https://davenunez.wordpress.com/2014/02/22/reading-hat-switches-on-pc-gameport-joysticks/)
* [hat switches](http://www.descent2.com/sickone/analogjoystick.html)
* [linearization](http://www.built-to-spec.com/blog/2009/09/10/using-a-pc-joystick-with-the-arduino/) of the response

#### Connections

| Game port | Function |  Pro Micro pin  |
|----|------|----------|-----------------|
| 1  | Vcc       |   Vcc                 |
| 2  | Button1   |    9  		 |
| 3  | X axis    |   A0   ---100K--- GND | 
| 4  | GND       |   GND		 |
| 5  |           | 			 |
| 6  | Y axis    |   A1   ---100K--- GND |
| 7  | Button2   |    8			 |
| 8  |           |			 |
| 9  |           |			 |
| 10 | Button4   |			 |
| 11 | X2 - unused?? |			 |
| 12 |               |			 |
| 13 | Y2 - for hat  | A2   ---100K---GND |
| 14 | Button3       |			 |
     					 |

### pro_micro_joystick/pro_micro_joystick.ino
Analog gameport joystick -> USB HID mouse. Mostly a test. Based on the [pro micro tutorial](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard).

