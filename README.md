# WheelBarrow

Developed by **Dr_J0nes**

---

This tool introduces **map-wide persistence** for the game **SnowRunner** without modifying your original game files.

I suggest using my other mod called ChainSaw together with this tool. The ChainSaw Mod makes all trees and stumps destructable and was developed to be used with the WheelBarrow tool. Have a look at the *My Mods* Section below.

This tool utilizes the **[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader)**

## My Vision

You remember last time when you drove around your current map and your tires reformed the mud pits?
While you were playing you likely saw how mud turned into deeper mud and how your tires drew deep marks. Maybe you even destroyed some trees or bushes. Maybe you even turned a whole lawn into a new mud road by driving over it, again and again.
I guess every player experienced this and was excited about these awesome outstanding features of the game.

And then you reload the map and everything is gone...

---

The game only saves the map state in a radius around each vehicle and trailer that you own. To be precise that range is only 24 meters.
The developers did this on purpose. The game was developed with consoles in mind. So it must be as efficient as possible. Saving the map state only in a small radius around your vehicles and trailers makes sure that your savegame files will be smaller and fewer resources are needed to run the game.

But what about gaming PCs? They have enough storage capacities and are performant enough. So these limitations on the savegame are kind of useless for us. And even worse: They kill immersion. At least for me.

---

Imagine you could drive around the map and wherever you go you leave your marks. You continue playing in a few days and everything is still there. Everywhere!
Exactly that was my dream. Fortunately I was able to achieve this dream with some patches I wrote. And here they are! Released for everyone to use.

But be warned! Persistent mud on the whole map means that mud will get deeper and deeper whenever you use that route. No more mud resets mean the game will be harder. But on the other hand we can now run over trees and plants and they will not respawn. This allows us to find and make completely alternate routes. Especially when you use the ChainSaw mod together with the WheelBarrow Tool. This can make things easier again. I think these patches keep the balance well. In my opinion the patches just change the experience you will have in a more immersive way and expand the gameplay oppertunities.

---

## Features

*   **Extended Terrain Persistence Range:** The game will no longer reset mud, snow or ice. But you need to own at least one vehicle or trailer on this map! Otherwise the whole map will reset again! Be warned!

*   **Extended World Destruction Range:** Smashed fences, knocked-down trees, and destroyed bushes remain exactly as you left them throughout your entire playthrough. When you destroy something it will never come back! Be warned! Rock and stone positions will reset just as in vanilla gameplay.

*   **Grass Persistence:** Normally grass will get reset everywhere where the map did not place mud in the map's terrain block. Mud you made yourself does not count. When you make a mud pit on your own on a grassy field for example, the game will respawn the grass when you reload the map. This looks odd. I do not know why this decision was made by the developers of the game, but it does not save any resources. So I made grass removal persistent. This improves the experience of world persistence a lot.

---

## Installation & Usage

1. Download the .zip file from the [Releases](https://github.com/DoktorJ0nes/DRJNS_WheelBarrow/releases/latest) and unpack it.

2. Copy the `dinput8.dll` file and `scripts` folder into the same folder where your SnowRunner.exe is located.

3. Launch the game just as you always do. The patches will be loaded automatically.

4. (Optional) You can disable the welcome message window you see when starting the game with WheelBarrow. You can also disable/enable certain patches by editing the `WheelBarrow.cfg` in the `scripts` folder. I do not suggest disabling the welcome message, because it shows you if everything is working as it should.

#### File Structure Example:

```text
├── SnowRunner.exe
├── dinput8.dll
└── scripts/
    ├── WheelBarrow.asi
    └── WheelBarrow.cfg
```

#### Notes for older Savegames

When your savegames are very old, the game will treat them a bit differently. The persistent grass patch needs another additional patch in this case. You can enable that patch by adding a # character in the config file in front of that patch's name.

#### Notes for Map updates

Sometimes the developers will make changes to the maps. I have in mind that this for example happened to Michigan before. So when the game detects that a map you load got changed in a game update it will reset all the persistent features this tool provided. To prevent that from happening this tool provides a map CRC check skip patch. This is enabled by default, but it could break things on your map when the developers changed plants on the map. I guess the worst case is that plants that you broke before are now there again and instead other random plants are removed then. But it can be even worse. No guarantees on that from my side! Because you are smart and always make backups of your savegames, you can revert to them if something broke and disable the map CRC check skipper patch in the config file of this tool.

---

## My Mods

### SnowRunner

*   **[WheelBarrow](https://github.com/DoktorJ0nes/DRJNS_WheelBarrow/releases/latest/):** Map-wide persistence for the game SnowRunner.

*   **ChainSaw:** *[UNRELEASED]* Makes all trees and stumps destructable in the game SnowRunner.

*   **FuelPump:** *[UNRELEASED]* Changes the fuel consumption of all your trucks by a multiplier of your choice in the game SnowRunner.

### RoadCraft

*   **[JackHammer](https://github.com/DoktorJ0nes/DRJNS_JackHammer/releases/latest/):** Map-wide persistence for the game RoadCraft.

*   **JerryCan:** *[UNRELEASED]* Changes the fuel consumption of all your trucks by a multiplier of your choice in the game RoadCraft.

### DayZ

*   **[Trader](https://steamcommunity.com/workshop/filedetails/?id=1590841260):** Brings AI Trader, Tradercitys and Safezones to the game DayZ.

---

## Legal Disclaimer & Notice

> **IMPORTANT:** This is an unofficial, community-made fan patch. It is not affiliated with, endorsed by, or connected to Saber Interactive or Focus Entertainment. This Legal Disclaimer is only valid for the main repository made and uploaded by Dr_J0nes (User DoktorJ0nes on GitHub). The Legal Disclaimer is not valid on forks or on other third party repositories.

*   **Although these patches were written with safety in mind and should survive any game update, they can break. And be warned that they can corrupt your savegame! So always make backups! When you disable this tool the persistency will no longer work. When you now save your game all the persistency features of this tool will be gone from your savegame! The maps will be reset just as in vanilla gameplay. Apart from this it is always safe to disable this tool. ALWAYS MAKE BACKUPS!**

*   **No DRM Circumvention:** This software does **NOT** bypass, alter, disable, or modify any Digital Rights Management (DRM), copyright protection, or anti-cheat mechanisms. It does **NOT** unlock unowned DLCs or paid content.

*   **Strictly Quality of Life (QoL):** The modifications made by this tool are purely intended to improve single-player immersion and custom gameplay balance.

*   **EULA Notice:** Modifying software memory technically violates standard End User License Agreements (EULA). This tool is provided "as-is" without any warranties. Use at your own risk. It is always recommended to back up your save games before applying patches.

---

## Transparency & Antivirus False Positives

Because this tool utilizes **Memory Hooking / DLL Injection** to dynamically find engine offsets at runtime, certain Antivirus programs or Windows Defender can inevitably flag the binaries as a "False Positive".

*   **100% Open Source:** To maintain absolute transparency and trust within the community, the entire source code is hosted publicly in this repository. Feel free to audit, review, or compile the binaries yourself.

*   **Reputation:** Developed by Dr_J0nes. Known for delivering secure community mods (such as the DayZ Standalone Trader Mod with millions of unique users). This tool contains no malicious code, tracking, or telemetry. Do not use this tool if you have any concerns. Make sure that this repository is hosted by the user DoktorJ0nes on GitHub.

---

## Multiplayer & Co-op Compatibility

*   **Status:** **EXPERIMENTAL / Singleplayer Focused**

*   This tool was designed and optimized for Singleplayer. Running these patches in Multiplayer may cause visual and physical desynchronization (desync) between the host and clients unless all players in the session are utilizing the exact same modifications. It could even ruin your savegame. I did not test it for multiplayer! Use at your own risk!

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.