-- Create game entities using the exposed C++ functions and enums



   print("This is a start up Lua function.")
   LoadTexture("room", "assets/dormroom.png")
   LoadTexture("fox", "assets/fox.png")

   --BED TEXTURES--
   LoadTexture("boringbed", "assets/bed.png")
   LoadTexture("patriotbed", "assets/patriotbed.png")
   LoadTexture("patriotbed2", "assets/patriotbed2.png") --size 206% on the website or save the json file and modify the sprite but DONT move it
   LoadTexture("richsnitchbed", "assets/richsnitchbed.png")
   LoadTexture("gamerbed", "assets/gamerbed.png")
   LoadTexture("gamerbed2", "assets/gamerbed2.png")
   LoadTexture("stonerbed", "assets/stonerbed.png")
   LoadTexture("gothbed", "assets/gothbed.png")
   LoadTexture("gothbed2", "assets/gothbed2.png")
   LoadTexture("halloweenbed", "assets/halloweenbed.png")
   LoadTexture("christmasbed", "assets/christmasbed.png")
   LoadTexture("gymratbed", "assets/gymratbed.png")
   LoadTexture("animebed", "assets/animebed.png")
   
   --LAMP TEXTURES--
   LoadTexture("boringlamp", "assets/boringlamp.png")
   LoadTexture("gamerlamp", "assets/gamerlamp.png")
   LoadTexture("patriotlamp", "assets/patriotlamp.png")
   LoadTexture("richlamp", "assets/richlamp.png")
   LoadTexture("christmaslamp", "assets/christmaslamp.png")
   LoadTexture("gymratlamp", "assets/gymratlamp.png")
   LoadTexture("animelamp", "assets/animelamp.png")

   --------DESK TEXTURES-------
   LoadTexture("boringdesk","assets/boringdesk.png")
   LoadTexture("deskpc","assets/deskpc.png")
   LoadTexture("deskconsole","assets/deskconsole.png")
   LoadTexture("desktv","assets/desktv.png")
   LoadTexture("christmasdesk", "assets/christmasdesk.png")
   LoadTexture("gymratdesk", "assets/gymratdesk.png")
   LoadTexture("animedesk", "assets/animedesk.png")


   
  ---------DRESSER TEXTURES-------------
  LoadTexture("boringdresser","assets/boringdresser.png")
  LoadTexture("gamingdresser","assets/gamingdresser.png")
  LoadTexture("patriotdresser","assets/patriotdresser.png")
  LoadTexture("christmasdresser", "assets/christmasdresser.png")
  LoadTexture("gymratdresser", "assets/gymratdresser.png")
  LoadTexture("animedresser", "assets/animedresser.png")

   ---------FRIDGE TEXTURES---------------
   
   LoadTexture("fridgefood", "assets/fridgefood.png")
   LoadTexture("fridgemicrowave", "assets/microwave.png")
   LoadTexture("fridgetowel", "assets/boringtowel.png")
   
   --------FLOOR TEXTURES---------------
   LoadTexture("boringfloor", "assets/boringfloor.png")
   LoadTexture("gamefloor", "assets/gamefloor.png")
   LoadTexture("patriotfloor", "assets/patriotfloor.png")
   LoadTexture("christmasfloor", "assets/christmasfloor.png")
   LoadTexture("gymratfloor", "assets/gymratfloor.png")
   LoadTexture("animefloor", "assets/animefloor.png")

   --Load Sounds-------
   LoadSound("twinkle", "assets/sounds/twinkle.wav")
   LoadSound("chime", "assets/sounds/windchime.wav")
   --PlaySound("twinkle") -- play sound

   e1 = CreateEntity() --EID = 0
   e0 = CreateEntity() --EID =1

   bed = CreateEntity() --EID = 2
   lamp = CreateEntity() -- EID = 3
   desk = CreateEntity()--EID = 4
   dresser = CreateEntity() --EID = 5
   fridge = CreateEntity()-- EID= 6
   Rug = CreateEntity() --9
   window = CreateEntity() -- EID= 7
   Wall = CreateEntity() --8
 
   
   GetSprite(e0).image_name = "room"
   GetSprite(e0).position.x= 10
   GetSprite(e0).position.y = 0
   GetSprite(e0).position.z = 0
   GetSprite(e0).scale.x = 200
   GetSprite(e0).scale.y = 200
   GetScript(e0).name = "room"
   --print("e0:",e0)

  
   GetSprite(bed).image_name = "boringbed"
   GetSprite(bed).position.x= 65
   GetSprite(bed).position.y = -41--up = higher value
   GetSprite(bed).position.z = 0
   GetSprite(bed).scale.x = 145
   GetSprite(bed).scale.y = 145
   GetScript(bed).name = "bed"--print("bed:",bed)

   GetSprite(desk).image_name = "boringdesk"
   GetSprite(desk).position.x= -45
   GetSprite(desk).position.y =-3
   GetSprite(desk).position.z = 0
   GetSprite(desk).scale.x = 48
   GetSprite(desk).scale.y = 48
   GetScript(desk).name = "desk"

   GetSprite(lamp).image_name = "boringlamp"
   GetSprite(lamp).position.x= -84
   GetSprite(lamp).position.y =-2
   GetSprite(lamp).position.z = 0
   GetSprite(lamp).scale.x = 30
   GetSprite(lamp).scale.y = 30
   GetScript(lamp).name = "lamp"


   GetSprite(dresser).image_name = "boringdresser"
   GetSprite(dresser).position.x= -32
   GetSprite(dresser).position.y = 15
   GetSprite(dresser).position.z = 0
   GetSprite(dresser).scale.x = 38
   GetSprite(dresser).scale.y = 38
   GetScript(dresser).name = "dresser"


   GetSprite(Rug).image_name = "boringfloor"
   GetSprite(Rug).position.x= 7
   GetSprite(Rug).position.y = -52
   GetSprite(Rug).position.z = 0
   GetSprite(Rug).scale.x = 115
   GetSprite(Rug).scale.y = 115
   GetScript(Rug).name = "floor"

   
   GetSprite(fridge).image_name = "fridgetowel"
   GetSprite(fridge).position.x= -9
   GetSprite(fridge).position.y = 25
   GetSprite(fridge).position.z = 0
   GetSprite(fridge).scale.x = 23
   GetSprite(fridge).scale.y = 23
   GetScript(fridge).name = "fridge"
   
   --OUR FOX ENTITY WILL BE THE GAMER SO ANY COMPONENTS REVOLVING THE USER JUST ATTACH TO THE FOX ENTITY
   GetSprite(e1).image_name = "fox"
   GetSprite(e1).position.x= -90
   GetSprite(e1).position.y = -80
   GetSprite(e1).position.z = 0
   GetSprite(e1).scale.x = 25
   GetSprite(e1).scale.y = 25
   GetScript(e1).name = "fox"
   GetMoney(e1).price = 10000
   GetHealth(e1).percent = 40
   
 


 

  

 
  

   


