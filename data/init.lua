print("hello from lua! :)")

if photon.build.is_debug() then
  photon.level.load("level.xml")
else
  photon.window.toggle_fullscreen()
end

photon.gui.set_message("testing the message system...", 4)
