# SpoutSenderGL

This is a FreeframeGL FFGL 2.2 plugin for sending an OpenGL texture from Resolume Arena
to a Spout receiver. Normally it's not necessary because Arena already has a Spout output option.
However, this plugin could help to trace errors that might occur with Arena Spout output.

Copy SpoutSenderGL.dll to "...\Documents\Resolume Arena\Extra Effects"
or to another folder that has been assigned for FreeframeGL plugins.
Find SpoutSenderGL in the list of Effects and drop on top of any cell that renders an image.
Scroll down the clip window and find SpoutSenderGL at the bottom.
Enter a name for the sender. You should receive successfuly with the 
"Spout Demo Receiver" application.

After running the plugin, a log file "SpoutSenderGL.log" is produced.
Find it with "Diagnostics -> Logs" for either "SpoutSettings" or "Spout Demo Receiver -> Help".
The log should report any warnings or errors.

Usually, errors are related to GPU selection for each application.
Both sender and receiver must use the same GPU. This can be influenced by either 
the driver control panel or Windows Graphics performance preference.
Both the sender and receiver must have the same profile and for laptops with dual graphics
that should be "High performance". Windows settings over-ride the driver.

The plugin source is only for reference. Integration into the Resolume FFL project is not included.
If you wish to re-build the plugin, you should follow the instructions from Resolume in their
[FFGL repository](https://github.com/resolume/ffgl). In addition, you have to create another Filter
in plugin project and include all the [SpoutGL](https://github.com/leadedge/Spout2/tree/master/SPOUTSDK/SpoutGL)
source files in that.




