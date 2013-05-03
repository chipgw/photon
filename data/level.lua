photon.gui.set_message("first I want you to gather 12 tnt...", 4)

photon.player.set_location(5, 2)

photon.level.set_victory_condition(function()
    if  photon.player.get_item_count("tnt") >= 12 then
        photon.player.snap_to_beam(false)
        photon.gui.set_message("good work! now destroy all the targets...", 4)
        --
        photon.level.set_victory_condition(function()
            if  photon.level.get_item_count("target") < 1 then
                photon.gui.set_message("congradulations you have won!\nnow leave the level using a reciever.", 4)
                return 1;
            end
            return 0
        end)
    end
    return 0
end)
