# Nob
Wireless Custom Volume Control Puck with Integrated OLED display 

### This is a WIP personal project to address a personal need for a resonably priced and customizable Rotary encoder for audio control. 

After switching to a brand new mechanical keyboard, I lost my built in volume knob and missed the convience of having a physical dial to control the volume of my music or videos. 
I wanted to build a wireless device that looks production ready and have full functionality while maintaining a minimalistic approach and footprint on my small dorm room desk.
---
### Prototyping
During the prototyping process, the Sparkfun Electornics Kit was used to breadboard components to a RedBoard. At this stage, I do not have access to an ESP32 microcontroller and thus cannot integrate wireless functionality nor can I directly interface with bluetooth audio controls. As such, I designed this prototype using a python script, taking advantage of the Pycaw library for audio interfacing with Windows. Being unfamilar with this Pycaw library, I took advantage of Generative AI and various sources of doucmentation to aid in drafting a basic volume control system that maps the potentiometer's rotation to a 1-100 scale volume value.

The limitations of the included 330Ω and 10kΩ resistors proved to be inconvient given the relatively poor quality of the included 16x2 LCD display with the SparkFun kit: The contrast cointrol (Pin3) required a very specfic resistance to "optimize" visibility of the backlight and rendered pixels. Typically, this is done through a potentiometer to fine tune the contrast levels through an analog input (as suggested by the included documentation). However, to avoid sourcing any additional parts for this basic drafting phase, I only had access to a single potentiometer which was being occupied as the actual rotary encoder for the volume dial. As a work around, a constant resistance was provided to the pin using a series of 330Ω and 10kΩ resistors to create an equivalent resistance of ~620Ω, which provuced a resonably usable result: 

![Overview of the breadboard prototype](https://github.com/PotatoSlop/Nob/blob/main/docs/images/Circuit%20TinkerCAD.jpg)

If one attemps to create their own prototype or circuit project, the resistors can be replaced with an equivalent resistor of approximately 600-620Ω.



