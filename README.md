# IS1200-Final-Project-Game
Working with the Microchip Pic32 Uno on a low level to create an advanced game project.

Project by Dean Tsankov and Benedek Boldizsár

To run the game on the ChipKit, compile the contents of this zip file in '. /opt/mcb32tools/environment' mcb32tools-environment
with command '$make' and then install it onto the ChipKit with the command '$make install'.

**Title:	Tunnelbana Trouble Teens**

2024-03-01

Advance project proposal

**Objective and Requirements.** 
The purpose of this project is to develop the notorious
“Subway surfers” mobile game on an embedded platform with limited resources. In this game, a teenager character is running on the screen, while dodging various obstacles (moving left or right, jumping or rolling) trying not to get hit by trains or smash into barriers and get as far away as possible (the highest score). The game is implemented with 2 lanes for running as opposed to 3 ones in the original, The must have requirements for the game are as follow:

  • Must support high score lists.
  
  • The game should be drawn in a vertical orientation as opposed to the standard horizontal one
  
  • It must support an increasing difficulty level.
  
  • Sprite based graphics
  
  • Animated character actions
  
  • The user must be able to control the game with the buttons on the ChipKIT board.
  
  • There must be different game states for menu, viewing highscores and playing the game
  
  • Dynamic obstacle movement and lane change of the player
  
  • Obstacle generation should be done using a seed number which could allow for the same scenarios in a potential multiplayer mode.


Optional features that may be included, if time allows:

  • Enable multiplayer mode where two players with different ChipKIT boards can
  
  play with each other. The UART will be used for serial communication. To decide who dies first.
  
  • To use the A/D converter together with an external speaker to generate sounds for
  the game.
  
  • To control the game using an external control scheme. The controls are communicating with the main board using SPI or I2C.
  
  • The game could support a local multiplayer mode and define a winner.


**Solution.** 
We developed our project on the ChipKIT Uno32 board together with
the Basic I/O shield. We used the small display on the Basic I/O shield to display the
game and use the slide switches to move between the 2 lanes and two push buttons to
control jumping and rolling, another button would be used as a select input. We used interrupts triggered by the built in timer to update the screen and control the increasing speed of the game. All the development was done using the MCB32tools.

**Verification.** 
We performed extensive testing on the current version of the game so as to bring it to as playable state as we could within the given time restraints. We
focused on system testing, and testing the proper displaying of objects on the screen. We had to use numerous test functions to verify the output of each action we wanted to perform as well as the game's behaviour according to them. These functions used different screen indicators to help us detect any wrong return values.

**Contributions.**

-did backgrounds --Benedek 

-did font bitmaps --Dean

-did main menu screen and gamestates --Benedek

-did interrupts, decide on interrupt priorities --Benedek

-did an update_display() – together

-did screen drawing overhaul and sprite display related functions –Benedek 

-did player movements – Dean

-did generating trains --Dean

-did collision-detection – Benedek

-did sprites and visual elements --Dean

**Reflections.** We thoroughly enjoyed working on this mini project despite the numerous issues we had to conquer in order to implement all of the features we planned to have. As an end result we are very proud of our accomplishment, but do not dismiss the fact that there is a lot more that could be done to improve our project. We think we managed to divide our time well on all of the various tasks and be as productive as we could, and yet we still have some unimplemented features. For future reference what we can take away from this experience is mainly that during the planning stage of the project we should perhaps consider a bit more the hierarchy and structure of our code so as to have clearly defined boundaries as to which functionality goes where and relates to what part of the project. Another thing we would definitely consider on any future venture is using C structures as opposed to passing unbounded variable parameters around. This implementation would undoubtedly help with organising our code and making it more easily interpretable.

https://github.com/kexana/IS1200-Final-Project-Game/assets/24894488/4c584dd7-a7b6-4f05-a2c7-513b64f4b1c6

Here we present a mockup we made of what we want the game too look like on the left next to a sample view of the game running on hardware on the right:
	  		
![image](https://github.com/kexana/IS1200-Final-Project-Game/assets/24894488/9aee3c12-194c-48fc-bca6-4a6b0b65d1f7)

