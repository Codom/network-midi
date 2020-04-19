---
title: 'IOT Guitar Pedal Project Proposal'
author: Christopher Odom
output: 
  pdf_document:
    number_sections: true
---

# Proposal/Abstract

For my project, I will be making a digital effects pedal
designed for use in a guitar rig, that can interface with
other instruments via midi triggers over the internet in
order to perform advanced effects automation that could not
be done with an analogue setup. This project will have two
parts, the first part will be setting up all of the audio
processing, which is made easy by the fact that there is 
a rich library of open source plugins and libraries to use.
The second part will be setting up a web interface to JACK - 
which is the main audio service that connects everything
together.

Here is a list of parts that I have used for the 
project.
(*I will be buying/already have all parts for this project, it's
something that I want to continue after the class is over.*):

* Audio Injector Stereo Sound Card
* Akai MPK Mini MKII (Midi Keyboard, MIDI Knobs, MIDI Drum
pad, all in one)
* 1/4" TRS Cables
* 1/4" Instrument cable adapters
* RCA Cables
* Instrument Preamp
* Expression pedal
* Auxiliary footswitch

## Audio on the Raspberry Pi
A Pi with sufficient audio hardware (The pi
doesn't have any audio inputs), also needs sufficient audio software.
Luckily, we can heavily lean on free software to provide
a wide variety of software to use for this. I want
to point out two pieces of software:

* JACK audio connection kit: Allows us to control the
input/output flow of an audio stream ([website](https://jackaudio.org/))
* QJackCtl: The best graphical frontend for JACK, I will
be heavily taking inspiration from the graph editor for
my remote control interface ([website](https://qjackctl.sourceforge.io/))

## IOT Components
These two pieces of software are adequate enough with ssh to
use in a controlled environment, but aren't flexible enough
for more chaotic environments such as the stage. In order
to add flexibility to this setup, I will be making a
web interface that will allow 3 things.

1. Allow remote setup and control via a qjackctl style
frontend (Or perhaps more [guitarix](http://guitarix.org/) style
for mobile if I get that far)
1. Allow for pairing of wireless pedals over mqtt. I will be
making a wireless expression pedal using the ESP8266 and an
analogue expression pedal.
1. Allow for remote midi control over the network, for reference
I will use another ESP8266 and connect it to a midi keyboard.

And if time permitting, I will implement a cloud based
remote management service
so that musicians won't have to fuss around with IP and LAN
issues.

(Diagram on next page)

![IOT Network](./diagram.png)


# Dev. Journal:

## Making Raspbian More Realtime

Raspbian is designed as a drop-in, budget friendly, desktop
replacement, and as such it isn't that good at realtime stuff.
While the stock performance of the audio-injector is impressive,
I think that we can do a little better. With a stock Jack setup,
Jack runs as a realtime process with a high priority (as defined in
`/etc/security/limits.conf`).

## MIDI over the network


