-- sets the popup message with a timeout of 4 seconds.
photon.gui.set_message("Welcome to Photon " .. photon.build.version() .. "!", 4)

photon.after(function()
    photon.gui.set_message("first I want you to gather 20 tnt...", 4)
end, 3)

-- sets the photon's location.
photon.player.set_location(2, 2)

halfway_message_shown = false

-- victory condition function returns > 0 on victory, < 0 on defeat and 0 otherwise.
photon.level.set_victory_condition(function()
    local tnt_count = photon.player.get_item_count("tnt")
    if tnt_count >= 20 then
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
    elseif not halfway_message_shown and tnt_count >= 10 then
        photon.gui.set_message("halfway there...", 4)
        halfway_message_shown = true
    end
    return 0
end)
