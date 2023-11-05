-- Create game entities using the exposed C++ functions and enums



   print("This is a start up Lua function.")
   LoadTexture("room", "assets/dormroom.png")
   LoadTexture("fox", "assets/fox.png")

   --BED TEXTURES--
   LoadTexture("bed", "assets/bed.png")
   LoadTexture("patriotbed", "assets/patriotbed.png")
   LoadTexture("patriotbed2", "assets/patriotbed2.png") --size 206% on the website or save the json file and modify the sprite but DONT move it
   LoadTexture("richsnitchbed", "assets/richsnitchbed.png")
   LoadTexture("gamerbed", "assets/gamerbed.png")
   LoadTexture("gamerbed2", "assets/gamerbed2.png")
   LoadTexture("pridebed", "assets/pridebed.png")
   LoadTexture("gothbed", "assets/gothbed.png")
   LoadTexture("gothbed2", "assets/gothbed2.png")
   LoadTexture("halloweenbed", "assets/halloweenbed.png")
   --LAMP TEXTURES--
   LoadTexture("boringlamp", "assets/boringlamp.png")
   LoadTexture("gamerlamp", "assets/gamerlamp.png")
   LoadTexture("patriotlamp", "assets/patriotlamp.png")

   --------DESK TEXTURES-------
   LoadTexture("boringdesk","assets/boringdesk.png")
   LoadTexture("pcdesk","assets/deskpc.png")
   LoadTexture("deskconsole","assets/deskconsole.png")
   LoadTexture("desktv","assets/desktv.png")


   
  ---------DRESSER TEXTURES-------------
  LoadTexture("boringdresser","assets/boringdresser.png")
  LoadTexture("animedresser","assets/animedresser.png")
  LoadTexture("gamingdresser","assets/gamingdresser.png")
  LoadTexture("patriotdresser","assets/patriotdresser.png")
   ---------FRIDGE TEXTURES---------------
   
   LoadTexture("ramen", "assets/fridgeramen.png")
   LoadTexture("food", "assets/fridgefood.png")
   LoadTexture("microwave", "assets/microwave.png")
   LoadTexture("towel", "assets/boringtowel.png")
   --------FLOOR TEXTURES---------------
   LoadTexture("boringfloor", "assets/boringfloor.png")
   LoadTexture("gamefloor", "assets/gamefloor.png")
   LoadTexture("patriotfloor", "assets/patriotfloor.png")

   --Load Sounds-------
   LoadSound("twinkle", "assets/sounds/twinkle.wav")
   PlaySound("twinkle") -- play sound

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
   GetSprite(e0).position.y = 20
   GetSprite(e0).position.z = 0
   GetSprite(e0).scale.x = 140
   GetSprite(e0).scale.y = 115
   GetScript(e0).name = "room"
   --print("e0:",e0)

  
   GetSprite(bed).image_name = "bed"
   GetSprite(bed).position.x= 50
   GetSprite(bed).position.y = -36
   GetSprite(bed).position.z = 0
   GetSprite(bed).scale.x = 80
   GetSprite(bed).scale.y = 80
   GetScript(bed).name = "bed"--print("bed:",bed)

   GetSprite(desk).image_name = "boringdesk"
   GetSprite(desk).position.x= -30
   GetSprite(desk).position.y =15
   GetSprite(desk).position.z = 0
   GetSprite(desk).scale.x = 30
   GetSprite(desk).scale.y = 30
   GetScript(desk).name = "desk"

   GetSprite(lamp).image_name = "boringlamp"
   GetSprite(lamp).position.x= -55
   GetSprite(lamp).position.y =15
   GetSprite(lamp).position.z = 0
   GetSprite(lamp).scale.x = 20
   GetSprite(lamp).scale.y = 20
   GetScript(lamp).name = "lamp"


   GetSprite(dresser).image_name = "boringdresser"
   GetSprite(dresser).position.x= -20
   GetSprite(dresser).position.y = 40
   GetSprite(dresser).position.z = 0
   GetSprite(dresser).scale.x = 25
   GetSprite(dresser).scale.y = 25
   GetScript(dresser).name = "dresser"


   GetSprite(Rug).image_name = "boringfloor"
   GetSprite(Rug).position.x= 15
   GetSprite(Rug).position.y = -50
   GetSprite(Rug).position.z = 0
   GetSprite(Rug).scale.x = 90
   GetSprite(Rug).scale.y = 90
   GetScript(Rug).name = "floor"

   
   GetSprite(fridge).image_name = "towel"
   GetSprite(fridge).position.x= -3
   GetSprite(fridge).position.y = 55
   GetSprite(fridge).position.z = 0
   GetSprite(fridge).scale.x = 15
   GetSprite(fridge).scale.y = 15
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
   
 


 

  

 
  

   


