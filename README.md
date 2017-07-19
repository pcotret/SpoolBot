![spoolbot](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/spoolbot.png)
# How to create a filament counter for a 3D printer for a few bucks
### Vladimir Béraud-Peigné, Eric Gastineau, Marie Xérès
CentraleSupélec, Rennes campus, 2016/2017

This project has been realized by Vladimir Béraud-Peigné, Eric Gastineau and Marie Xérès (1st year students 16/17 at CentraleSupélec, Rennes campus) for a short-term project under the supervision of Dr. Pascal Cotret.

## Goals of this project
![k8400](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/k8400.jpg)
Well, the basic idea of this project came from a little problem. We have a small fablab room with a Velleman K8400 3D printer. Unfortunately, this printer does not have a filament counter:

* We cannot know exactly how much filament has been used (in terms of length).
* As a consequence, we do not how much filament remains on the spool.

By browsing the Internet, we found a cool solution: https://www.hackster.io/binsun148/smart-3d-printer-filament-counter-filamentbot-383ac8

* It is cheap (based on a good old PS/2 mouse).
* It is quite well documented.

However, as this is a student project, the idea was not only to follow a tutorial but to understand how such a project can be achieved. This tutorial will show how we built our own SpoolBot!

## How does a mechanical mouse work?
![souris](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/mymouse.png)

A good old computer mouse is based on a ball causing the rotation of a small cylinder.This cylinder has an encoder wheel (drilled with several holes) at one of its ends. This wheel is placed between a phototransistor and a LED: counting pulses received by the phototransistor can be used to measure the distance traveled by the mouse ball.

Detecting the ball direction is a bit more tricky... In fact, there are two photo transistors slightly shifted. When we get through a hole of the wheel encoder, we get two detections (one for each transistor). Depending on which one comes first, we can guess the direction of the mouse.

## PS2 protocol

A computer mouse is a device giving ```[X,Y]``` coordinates of a movement and information about ```[left/right]``` clicks. Such information are transmitted through a synchronous serial link with a 9600bd/s baudrate: each frame is based on a classic ```8p1``` layout (8 data bits, 1 parity bit and 1 stop bit).

By default, a PS/2 computer mouse works with a 200 CPI (*Counts Per Inch*) resolution leading to 8 counts/mm (step is 125µm). Movements are coded on 8 bits for a distance of 3.2cm. The mouse is able to transmit up to 40 movements per second which allows to encode movements up to 2m/s (we think that’s quite enough, even for a hardcore gamer!).

Transmitted data is based on 8-bit words (see diagram below), with a start bit (set to 0) as a preamble. Data words are ended by a parity bit and a stop bit (set to 1). The whole message is coded on 11 bits.

![ps2](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/timing1.png)

The mouse transmits information in 3 frames spaced by 350 µs: frames are 3.6ms long and are spaced by 6.4ms at least. As a consequence, the mouse is able to transmit up to 100 frames per second.

![full](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/ps2_frame.png)

The first frame word contains the following information:

* ```L```: left button (active at 1).
* ```R```: right button (active at 1).
* ```Xs```: direction of the horizontalmoving (1 for left, 0 for right).
* ```Ys```: direction of the verticalmoving (1 for low, 0 for high).
* ```Xv```: overflow in X.
* ```Yv```: overflow in Y.

The two other words transmit the moving value:
* ```X[0..7]```: moving along the horizontal axis (positive to the right, negative to the left).
* ```Y[0..7]```: moving along the vertical axis (positive to the bottom, negative to the top).

## PS/2 decoding on Arduino

We were quite lucky, some nice people have already implemented a PS/2 library: https://playground.arduino.cc/ComponentLib/Ps2mouse
The ```mouse.txt``` file contains an Arduino sketch helping us to create a sketch suiting our needs. Let’s focus on the ```mouse_read()``` method:

```C++
char mouse_read(void)
{
  char data=0x00;
  int i;
  char bit=0x01;
  // Set clk and data lines to high
  gohi(MCLK);
  gohi(MDATA);
  // Wait for the first clock cycle
  delayMicroseconds(50);
  while(digitalRead(MCLK)==HIGH);
  delayMicroseconds(5);
  while(digitalRead(MCLK)==LOW);
  // For each clock rising edge, get the data bit
  for(i=0;i<8;i++)
  {
	while(digitalRead(MCLK)==HIGH);
    	if(digitalRead(MDATA)==HIGH)
	{
	  data=data|bit;
    	}
    	while(digitalRead(MCLK)==LOW);
    	bit=bit<<1;
  }
  // Wait for two clock cycles (parity and stop bits)
  while(digitalRead(MCLK)==HIGH);
  while(digitalRead(MCLK)==LOW);
  while(digitalRead(MCLK)==HIGH);
  while(digitalRead(MCLK)==LOW);
  // Set clock line to low
  golo(MCLK);
  return data;
}
```

## SpoolBot solution
![full](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/full_stuff.png)

### Arduino code
See https://github.com/pcotret/SpoolBot/blob/master/docs/spoolbot_code/spoolbot_code.ino

### Wiring diagram
You can also find it at https://github.com/pcotret/SpoolBot/blob/master/docs/fritzing_spoolbot.fzz
![fritzing](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/wiring.png)

### 3D-printed case
To be completed, we need to fix a few things on 3D files.
![solidworks](https://raw.githubusercontent.com/pcotret/SpoolBot/master/img/boitier.png)

## Demonstration video
[![hello](https://img.youtube.com/vi/5DotnpfB1dM/0.jpg)](https://www.youtube.com/watch?v=5DotnpfB1dM)

## References
* http://jerome.hennecart.free.fr/principe_souris.htm
* http://www.rennes.supelec.fr/ren/perso/jweiss/docs/mouse_ps2/souris_ps2.php/
* https://playground.arduino.cc/ComponentLib/Ps2mouse
* https://www.hackster.io/binsun148/smart-3d-printer-filament-counter-filamentbot-383ac8
