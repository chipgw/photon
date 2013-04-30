print("hello from lua! :)")

if photon.build.is_debug() then
  photon.level.load("level.xml")
else
  photon.window.toggle_fullscreen()
end

photon.gui.set_message("testing the message system...\n"
                        .. "you have " .. photon.player.get_item_count("tnt") .. " tnt.", 4)

photon.player.set_location(5, 2)
