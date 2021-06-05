**Description**
The goal of this set of applications is used to analyze data logs comming from
a robot. Testing of my robot yeilds gigabytes of data that must be waded
through to figure out what went wrong/right.

The basic design is a series of Linux programs. Some are just console programs
other use WxWidgets to make them GUIs.

*Hub*
The center piece of the programs is a "hub". It is just a simple
publish/subscribe router. The other applications connect to the hub to exchange
messages.

*Playback*
This module reads a log file previously recorded and plays back the individual
messages. It accepts a command message to load/start/stop/rewind the log file.
The log file must be a series of messages in a set format.

*Cmd*
This GUI allows you to send commands to the other modules. It accepts log
messages from the other messages to provide the basic interface.

*Tbl*
This GUI provides an "Excel" table view of the data

*Scr*
This GUI provides a strip chart recorder view of the data

**Other routines**

*Midget*
This program extracts the DWARF information from a program to get all the
variables names for the messages is the log files.

