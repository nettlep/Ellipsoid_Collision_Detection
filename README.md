# Legacy Code Resurrection Project

This file is part of a glorious rescue mission: migrating ancient code from the dusty vaults of the internet to the shiny halls of GitHub.

This code has been preserved as faithfully as possible, warts and all. All code contained herein is original code written by Paul Nettle unless otherwise noted.

See the end of this document for the original text `ReadMe.txt` file.

---

## Original description

These downloads are based on the document [Generic Collision Detection for Games Using Ellipsoids](https://nettlep.github.io/Ellipsoid_Collision_Detection/docs/Fluid_Studios_Generic_Collision_Detection_for_Games_Using_Ellipsoids.pdf). __IMPORTANT:__ This document has been deprecated. It might be useful as a general guide, but please refer to the demonstration/source code (below) for implementation details.

This repository contains source code to an implementation using software rendering and a [demo version that uses hardware rendering](https://nettlep.github.io/Ellipsoid_Collision_Detection/hwdemo/DX8-CollisionDetectionDemo.zip). Sorry, no source code for the hardware rendered version. Try this: set the mode to full-screen and enable vsync, then click OK and hit 't' on your keyboard to begin playback of a pre-recorded walkthrough demo that showcases some of the collision detection features.

---

## What's in here?

This repository includes:

- Original source code with minimal modifications  
- Historical comments and design choices, preserved for posterity  
- A fresh coat of licensing and documentation (hello `LICENSE` and `README.md`!)  
- Possibly some delightful quirks from a bygone programming era

---

## License

The code is now released under the [MIT License](LICENSE), unless stated otherwise. You are free to use, modify, and redistribute it.

Note: Original copyright notices from the author have been retained for historical context.

---

## Disclaimer

> This code is **vintage**. That means:
> 
> - Expect odd formatting, outdated conventions, and maybe even some nostalgia.
> - It might not compile or run without some TLC.
> - There's zero warranty, and it may bite. You’ve been warned.

---

## Why preserve this?

Because history matters. This code is a snapshot of how things were done back then—and in some cases, how they’re still done today. Think of it as open source time travel.

Happy coding!

— Paul

---

## Original ReadMe.txt

    -------------------------------------------------------------------------------
     _____                 _ __  __        _        _
    |  __ \               | |  \/  |      | |      | |
    | |__) | ___  __ _  __| | \  / | ___  | |___  __ |_
    |  _  / / _ \/ _` |/ _` | |\/| |/ _ \ | __\ \/ / __|
    | | \ \|  __/ (_| | (_| | |  | |  __/_| |_ >  <| |_
    |_|  \_\\___|\__,_|\__,_|_|  |_|\___(_)\__/_/\_\\__|

                               Fluid Studios, Inc.
                           HTTP://www.FluidStudios.com/

    -------------------------------------------------------------------------------

    Restrictions & freedoms pertaining to usage and redistribution of this software:

     * This software is 100% free
     * If you use this software (in part or in whole) you must credit the author.
     * This software may not be re-distributed (in part or in whole) in a modified
       form without clear documentation on how to obtain a copy of the original work.
     * You may not use this software to directly or indirectly cause harm to others.
     * This software is provided as-is and without warrantee. Use at your own risk.

    For more information, visit HTTP://www.FluidStudios.com

    -------------------------------------------------------------------------------
    What to expect from this demo:
    -------------------------------------------------------------------------------

    The viewer application is just a simplified 3D engine with the purpose of
    showing the results from the Fluid Studios collision detection algorithm.

    In this demo, your collision detection entity is a very all ellipsoid. The
    dimensions of the ellipsoid are shown on screen with next to "RADIUS". You can
    use the function keys to change the size and shape of this ellipsoid. Note that
    the viewpoint posision is near the top of the ellipsoid, which can cause it to
    act differently than you might expect.

    Also, the friction is set somewhat high, so you can only move at a normal
    walking pace. The reason for this is to highlight the smooth way in which the
    ellipsoid "rolls" around corners and over stairs.

    -------------------------------------------------------------------------------
    A few notes about running this demo:
    -------------------------------------------------------------------------------

    Some of the larger maps will run rather slowly (theruins.oct, for example) due
    to the lack of visibility and software rendering.

    Also, some of the maps will place the viewpoint below the ground, or embedded
    in geometry. If you find yourself falling immediately upon loading a map (such
    as house5.oct), press G to turn off gravity, then F to allow you to fly around.
    Once you're above stable ground, press G and F again to return to a normal
    running state.

    -------------------------------------------------------------------------------
    About this release
    -------------------------------------------------------------------------------

    This public release is intended as a demonstration of Fluid Studios' collision
    detection algorithm. Source code is available on the Fluid Studios' website at:

                           HTTP://www.FluidStudios.com/

    This is not a demonstration of high-performance graphics. On the contrary, it
    was designed so that it could be easily understood by those trying to implement
    the technique in their own work. Because of this, the graphics pipeline is
    simplified.

    This demonstration also provides a few other benefits. It makes use of 4x4
    homogenous perspective transforms, 3D clipping, software rendering of
    perpsective correct textured polygons, software font rendering, and a modified
    version of WU's anti-aliased line drawing algorithm that is not limited to
    lines that begin or end on integer coordinates.

    The font files (*.f) were created using the FontGen tool, also available on
    the Fluid Studios website. This demonstration serves as a guide on rendering
    fonts generated with that tool.

    -------------------------------------------------------------------------------
    External credits
    -------------------------------------------------------------------------------

    The OctMap.* source files were provided courtesy of Jason Zisk at nFusion
    Interactive.  These files were based on the bspconv tool by Nate Miller.  Visit
    http://nate.scuzzy.net for more information on bspconv.

    The *.OCT data files were provided courtesy of Jason Zisk at nFusion
    Interactive. Visit HTTP://www.n-fusion.com/

    -------------------------------------------------------------------------------
    ReadMe.txt - End of file
    -------------------------------------------------------------------------------

