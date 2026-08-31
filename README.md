# Watch for The Droplet

- **Author**: Yuchen Zhou
- **Description**: Imagine you are a droplet. But wait, not a normal droplet in the sink, but a invincible droplet-shaped alien spacecraft in the great universe and it is ready to hunt human ships! Try your best to catch the escaping troops and something lying in the universe may help you. **LET'S HUNT!**

![Game Preview](preview.jpg)

# Asset Pipeline
## Assets
Here are all the assets I draw with Asprite:
1. [Droplet (player)](assets/droplet-16-16.png) - ![Droplet](assets/droplet-16-16.png)
2. [Spacecraft (target)](assets/human-spacecraft-48-24.png) - ![Spacecraft](assets/human-spacecraft-48-24.png)
3. Buffs: [small star](assets/small-star-8-8.png) and [bright starts](assets/bright-star-16-16.png) - ![small star](assets/small-star-8-8.png) ![bright starts](assets/bright-star-16-16.png)
4. Debuffs: [small meteorites](assets/meteorite-small-16-16.png) and [large meteorites](assets/meteorite-large-24-24.png) - ![small meteorites](assets/meteorite-small-16-16.png) ![large meteorites](assets/meteorite-large-24-24.png)

## How it works
# How To Play:

(TODO: describe the controls and (if needed) goals/strategy.)

## Other Sprites:
1. Stars: 
- [Small stars](assets/small-star-8-8.png) can boost you speed one step from Normal -> Accelerated -> Lightspeed.
- [Bright stars](assets/bright-star-16-16.png) can boost you directly to Lightspeed, not matter whatever the current speed you are. 
- You can not go beyond Lightspeed, that's the maximum buff you can get.
- The above logic applies to the spacecraft as well.
2. Meteorites:
- [Small meteorite](assets/meteorite-small-16-16.png) will slow down your speed one step back: Lightspeed -> Accelerated -> Normal.
- [Large meteorite](assets/meteorite-large-24-24.png) will slow down your speed two steps directly back to Normal. 
- If you are in Normal speed and you hit a meteorite (small or large), it will not effect your speed. 
- The above logic applies to the spacecraft as well.

# Acknowledgements
This game is inspired by [Three Body Problem II - The Dark Forest](https://www.3body.com/) from [Cixin Liu](https://en.wikipedia.org/wiki/Liu_Cixin).


This game was built with [NEST](NEST.md).
