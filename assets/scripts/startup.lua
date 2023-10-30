-- Create game entities using the exposed C++ functions and enums



   print("This is a start up Lua function.")
   LoadTexture("room", "assets/room.jpg")
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
   

   --Load Sounds
   LoadSound("twinkle", "assets/sounds/twinkle.wav")
   PlaySound("twinkle") -- play sound

   e1 = CreateEntity() --EID = 0
   e0 = CreateEntity() --EID =1

   bed = CreateEntity() --EID = 2
   lamp = CreateEntity() -- EID = 3
   desk = CreateEntity()--EID = 4
   dresser = CreateEntity() --EID = 5
   fridge = CreateEntity()-- EID= 6
   window = CreateEntity() -- EID= 7
   Wall = CreateEntity() 
   Rug = CreateEntity() 

   
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

   GetSprite(lamp).image_name = "boringlamp"
   GetSprite(lamp).position.x= -55
   GetSprite(lamp).position.y =15
   GetSprite(lamp).position.z = 0
   GetSprite(lamp).scale.x = 20
   GetSprite(lamp).scale.y = 20
   GetScript(lamp).name = "lamp"
   
   GetSprite(e1).image_name = "fox"
   GetSprite(e1).position.x= -90
   GetSprite(e1).position.y = -80
   GetSprite(e1).position.z = 0
   GetSprite(e1).scale.x = 25
   GetSprite(e1).scale.y = 25
   GetScript(e1).name = "fox"
   GetMoney(e1).price = 10000
   print(e1)
   print(GetMoney(e1).price)
 


 

  

 
  

   


