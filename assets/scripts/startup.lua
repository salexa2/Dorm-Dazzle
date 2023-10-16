-- Create game entities using the exposed C++ functions and enums



   print("This is a start up Lua function.")
   LoadTexture("room", "assets/room.png")
   LoadTexture("fox", "assets/fox.png")
   
   e1 = CreateEntity()
   e0 = CreateEntity()

   
   GetSprite(e0).image_name = "room"
   GetSprite(e0).position.x= 0
   GetSprite(e0).position.y = 0
   GetSprite(e0).position.z = 0
   GetSprite(e0).scale.x = 100
   GetSprite(e0).scale.y = 100
   GetScript(e0).name = "room"
   
   GetSprite(e1).image_name = "fox"
   GetSprite(e1).position.x= 0
   GetSprite(e1).position.y = 0
   GetSprite(e1).position.z = 0
   GetSprite(e1).scale.x = 50
   GetSprite(e1).scale.y = 50
   GetScript(e1).name = "fox"

 

  

 
  

   


