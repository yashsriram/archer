## Light Play
* Light Play is a 2d simulator/game of light
* It simulates reflection in presence of different types of mirrors

## How to play?
* There is a fixed laser source which emits a straight beam
* You can control its the direction
* The goal is to hit the green balloon and pop it
* But Alas! The mirrors are in the way!
* Your mission (should you choose to accept it) is to find a way through them using reflections and hit the balloon

## How to install? [Ubuntu]
* Install dependencies
    * `sudo apt-get install libx11-dev`
* Open terminal in this directory
    * `make` to compile code [creates an executable **lightplay**]
    * `make play` or `./lightplay` to start playing
