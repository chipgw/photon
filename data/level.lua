-- sets the popup message with a timeout of 4 seconds.
photon.gui.set_message("first I want you to gather 12 tnt...", 4)

-- sets the photon's location.
photon.player.set_location(5, 2)

-- victory condition function returns > 0 on victory, < 0 on defeat and 0 otherwise.
photon.level.set_victory_condition(function()
    if  photon.player.get_item_count("tnt") >= 12 then
        -- enables free movement.
        photon.player.snap_to_beam(false)
        photon.gui.set_message("good work! now destroy all the targets...", 4)

        -- sets a new victory condition. (objective 2)
        photon.level.set_victory_condition(function()
            if  photon.level.get_item_count("target") < 1 then
                photon.gui.set_message("congratulations you have won!\nnow leave the level using a receiver.", 4)
                return 1;
            end
            return 0
        end)
    end
    return 0
end)
