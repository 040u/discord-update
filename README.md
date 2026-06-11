How to install:

```
git clone https://github.com/040u/discord-update
cd discord-update
sudo make install
```

How to use:

```
sudo discord-update
```

Then start discord again to update it.
<br />
<br />

This script works by grabbing the version number from the forward link in the HTML code of 
<br />

```
https://discord.com/api/download?platform=linux&format=tar.gz
```

It then edits your /opt/discord/resources/build_info.json as sudo and kills the "discord" process. Restarting Discord has to be done manually as you should not execute
Discord as a privileged user.

<br />
<br />
limitations: only works if discord is installed, a /opt/discord/resources/build_info.json exists and a version number is inside of it.
