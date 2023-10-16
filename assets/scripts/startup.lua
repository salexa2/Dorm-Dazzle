-- Create game entities using the exposed C++ functions and enums



   print("This is a start up Lua function.")
   LoadTexture("fox", "assets/fox.png")
   
   e1 = CreateEntity()
   
   GetSprite(e1).image_name = "fox"
   GetSprite(e1).position.x= 0
   GetSprite(e1).position.y = 0
   GetSprite(e1).position.z = 0
   GetSprite(e1).scale.x = 50
   GetSprite(e1).scale.y = 50
   GetScript(e1).name = "fox"
 

  

 
  

   


