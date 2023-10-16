
spaceKeyPressed = false
if KeyisPressed(KEYBOARD.SPACE) then
    if(spaceKeyPressed == false) then
        GetSprite(e1).position.x =   GetSprite(e1).position.x + .1
        spaceKeyPressed = true
    end
end

if KeyisReleased(KEYBOARD.SPACE) then
    spaceKeyPressed = false
end

