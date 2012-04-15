Chicken Tournament:


***NOTE***: Since this game contains exaggerated, pointless violence, I recommend to keep it away from players under the age of 18, even though the game is unrealistic. This is up to your personal discretion. I take no responsibility whatsoever for any damage (psychological or material) caused by this game (especially concerning domestic chickens IRL). GardenClaws are for gardening only!!



1) Minimum Requirements:
- 18 years
- 300 MHz
- Microsoft Windows 98
- 32 MB RAM
- 3D graphics card with DirectX 7 support
- DirectX 7.0
- Sound card (optional)
- Network interface card (optional, for multiplayer games)
- DivX 4 Codec (for the intro movie)
- Mouse
- Keyboard



2) Installation
No installation necessary, the game will run directly from CD. You can, of course, copy it to your hard disk anyway if you like.



3) Game Rules
The game in itself is absolutely pointless and has no real objective. You can move around the 3D game world as either a Farmer or a Chicken. As a Farmer you can pick up one of 4 different weapons randomly placed around the game world (GardenClaw, Golf Club, Combine Harvester, Plasma Cannon). With one of these you can then set out to liquidate Chickens. Farmer vs. Farmer is not an option. As a Chicken your primary weapon are eggs. You'll have to stop and breed to replenish these. While breeding, Chickens are vulnerable. They also have the ability to fly. This way they move and turn faster, and can fly kamikaze attacks on Farmers. They can also crash if you're not careful. Chickens cannot keep flying forever: they have flight power that is slowly drained in flight. While on the ground this power regenerates. Chicken vs. Chicken fights aren't possible either.

As a Chicken your coop serves as a shelter. In one of these you are protected from hostile Farmers and your breeding and flight power regeneration speeds are greatly increased.

Farmers can pick up Medipacks that instantly replenish 50% of their health (up to a maximum of 125%). Chickens can pick up egg cartons, each of which contains 16 eggs (ammunition).
Farmers also regenerate health over time, but slowly.

The Farmers' Plasma Cannon uses ammunition. If empty, it cannot be used. However, the Plasma Cannon regenerates ammunition over time. Alternatively you can pick up another Cannon with more ammunition left in it.

The Chickens' frag counter is calculated in percent, which means your score will eventually increase even if the damage from your shots did not suffice to kill a Farmer.

While inside a Combine Harvester all attacks against a Farmer will only deal 1/5 of their regular damage, however he does not regenerate health.

You can play in multiplayer or solo mode. While the game is running bots can be added or removed. The chicken bots' fighting AI can also be deactivated (fun mode! ;)



4) Controls

Keyboard:
- Escape: Menu
- W/A/S/D, Arrow keys: Movement
- Space: Jump / Takeoff

- ^ (the key under escape): Open command console
- Tab: Show multiplayer stats
- H: Change hat. (Farmers only!)
- B: Look back (only as a Chicken in flight, or as a Farmer in a Harvester)
- Backspace: Open chat window
- C: Change camera view

- Print Screen: Save a .BMP screenshot to your desktop
- Enter: Exit Harvester
- F4: Wireframe on/off (for debugging, could be considered a "wall hack" (unfair?))
- F5: Sound on/off
- F6: Music on/off
- R: Toggle Radar on/off
- Alt+F4: Exit game instantly (surprisingly enough!)


Mouse (Farmer):
- Rotate view
- Left mouse button: Primary Attack (thrust, swing, shoot)
- Right mouse button: Secondary Attack (throw, drop, exit)

Mouse (Chicken):
- Rotate view
- Left mouse button: Throw eggs
- Right mouse button: Breed
- In flight:
  - W/S/Mouse wheel: Thrust control
  - A/D: Yaw
  - Up/Down: Pitch
  - Left/Right: Roll



5.1) Local Options
Press Escape while the game is running, then click Options. Most of them should be self-explanatory. ;) All options only affect your own computer. The resolution can be changed in-game.
These options are stored in a file named "config.dat" in your game directory (wherever the .exe file is). Simply delete the "config.dat" to revert to default settings.

5.2) Server Options
Press Escape, then Server Options. These options affect the whole game behaviour. From here you can spawn new bots, objects, etc.


6) Multiplayer
At startup you first select the protocol to be used, then either join a multiplayer game or start a new one. Single player doesn't have to be specially selected because it's no different from a multiplayer game without other players.
If the server leaves during a multiplayer game, the server role is automatically given to another client and the game continues seamlessly.
Only the current server can change Server Options (using either the menu or the console).
The additional program "Register4Lobby.exe" can be used to add Chicken Tournament to DirectPlay's list of "lobby-supported" games. When this is done CT can be launched automatically from a DirectPlay lobby. The Microsoft Messenger, for example, can then be used to start CT (look for "start CT" or similar in the chat window).


7) Ports
I am very lazy, so CT uses DirectPlay for networking. DirectPlay uses the following ports (for firewall issues):

Primary Connection
  47624 TCP Outbound
Secondary Connection
  2300-2400 UDP Send Receive
  2300-2400 TCP Inbound
  2300-2400 TCP Outbound


8) Console
Here's a list of all console commands (can be opened by pressing ^).

- HELP: Lists all commands
- FOG: Fog on/off
- INFO: Lists all objects
- SPAWN [1-6]: Spawns a new object in the game world (1=Chicken, 2=GardenClaw, 3=Golf Club, 4=Harvester, 5=Tree, 6=Chicken Coop)
- EXIT: Exits the game
- KICK [nr]: Deletes the object with ID=nr from the world
- SPECULAR: Enable / Disable specular highlights on objects (can drain performance)
- NEWWORLD: Generates a new landscape (objects remain)
- SAY [Text]: Sends a chat message to all players
- PRIV [UID] [Text]: Sends a private chat message to the one player with the specified UID
- SENSITIVITY: Sets mouse sensitivity (< 25: faster)
- GRAVITY: Change gravity
- GORELEVEL: Change gore level
- AIRCONTROL: Acceleration factor while in the air
- JUMPSPEED: Jump speed factor (relative to walking speed)
- ALPHA: Enable / Disable alpha blending (can drain performance)
- NAME: Change player name (capitals only).
- TYPE [UID]: Change player type (Farmer / Chicken), and (server only) sets type or remote player
- SITSPEED: Eggs per second while breeding (starting value)
- THROWTIME: Delay between egg throws
- REPLENISHPOWER: Regenerate %/sec flight power
- TAKEPOWER: Drain %/sec flight power while in the air
- DAMAGE: Damage one egg does to a Farmer (+/- 50%)
- PLASMAREGENERATION: Regenerate n / sec ammunition for Plasma Cannon
- PLASMARADIUS: Blast radius for Plasma shots in meters
- PLASMARATE: Firing delay for Plasma Cannon (in seconds per shot)
- KILL [UID]: Kill self, or kill specified player (server only)
- RESPAWNTIME: Delay until a dead player respawns (in seconds)
- HARVESTERSPEED: Multiplicator of the acceleration of the harvester
- HARVESTERROTSPEED: Multiplicator of the rotation speed of the harvester
- GRAS: Enable / Disable grass


9) Known Bugs
I'd like to ask you to acknowledge these errors and accept them. I'm responsible for these and am desperately trying to come up with solutions, but as of yet I haven't managed to find any for these problems. I hope you won't be affected by them.

- The multiplayer mode is rather unstable.
- Game performance can be extremely sluggish with older graphics drivers. Graphical errors may appear as well.
- Joining a network game can fail sometimes. The client will then hang or crash, slowing down game performance for all other players on the server.
- Since the game has no real objective and is pretty pointless, it tends to get boring rather quickly.




10) Contact

This game was devised, designed and coded by Sascha Hlusiak (from June 2002 until July 2003).

Email: Spam84@gmx.de
Homepage: http://www.saschahlusiak.de



Thank you, and I hope you like my game. =)


Regards,
	Sascha Hlusiak
