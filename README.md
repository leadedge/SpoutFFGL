# SpoutSenderGL

This repository contains FreeframeGL 2.2 plugins for sending from Resolume to a Spout receiver or receiving from a Spout sender. 
	
Normally they are not necessary because Resolume Arena already has a Spout output option
and individual Spout sources can be selected. However, these plugins produce log files which
could help to trace any errors that occur.
	
Copy SpoutSenderGL.dll and SpoutReceiverGL.dll to : "...\Documents\Resolume Arena\Extra Effects"
or another folder assigned for FreeframeGL plugins.
	
To send :
- Find SpoutSenderGL in the list of Effects.
- Drop on top of any cell that renders an image.
- Scroll down the clip window and find SpoutSenderGL at the bottom.
- Enter a name for the sender.
- Receive with the Spout Demo Receiver or any application with Spout support.
	
To receive :
- Find SpoutReceiverGL in the list of Sources.
- Drop onto any empty cell. The active sender is immediately detected.
- Other senders can be selected from the SpoutReceiverGL clip window.\
("SpoutPanel" or "SpoutSettings" must have been run at least once).
- Receive with the Spout Demo Receiver or any application with Spout support.

Log files "SpoutSenderGL.log" and "SpoutReceiverGL.log" are produced to help trace errors.
Find them from "SpoutSettings -> Diagnostics -> Logs".

Usually, errors are related to GPU selection for each application.
Both sender and receiver must use the same GPU. This can be influenced by either 
the driver control panel or Windows Graphics performance preference.
Both the sender and receiver must have the same profile and for laptops with dual graphics
that should be "High performance". Windows settings over-ride the driver.

The plugin source files are only for reference. Project files for integration into the Resolume 
FFGL project is not included. If you wish to re-build the plugin, you should follow the instructions
in the Resolume [FFGL repository](https://github.com/resolume/ffgl). In addition, you have to create another Filter
in each plugin project and include all the [SpoutGL](https://github.com/leadedge/Spout2/tree/master/SPOUTSDK/SpoutGL)
source files in them.




