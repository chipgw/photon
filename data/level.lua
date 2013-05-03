photon.gui.set_message("testing the message system...\n"
                        .. "you have " .. photon.player.get_item_count("tnt") .. " tnt.", 4)

photon.player.set_location(5, 2)

photon.level.set_victory_condition(function()
    if  photon.player.get_item_count("tnt") >= 12 then
        return 1;
    else
        return 0
    end
end)
