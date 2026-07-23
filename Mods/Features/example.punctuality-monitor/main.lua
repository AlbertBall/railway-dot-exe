local last_simulation_minute = nil

local function update_summary()
    local train_count = tonumber(railos.get("trains.count")) or 0
    local timetable_count = 0
    local late_count = 0
    local worst_delay = 0
    local worst_headcode = ""

    for index = 0, train_count - 1 do
        local path = "trains." .. index .. "."

        if railos.get(path .. "mode") == "timetable" then
            timetable_count = timetable_count + 1

            local delay = tonumber(railos.get(path .. "delay_minutes")) or 0
            if delay > 0 then
                late_count = late_count + 1
            end

            if delay > worst_delay then
                worst_delay = delay
                worst_headcode = railos.get(path .. "headcode")
            end
        end
    end

    local summary = string.format(
        "Punctuality: %d timetable trains | %d late",
        timetable_count,
        late_count
    )

    if worst_headcode ~= "" then
        summary = summary .. string.format(
            " | worst: %s (+%.1f min)",
            worst_headcode,
            worst_delay
        )
    end

    railos.set("ui.status", summary)
end

function on_tick(event, clock_value)
    local clock = tonumber(clock_value)
    if clock == nil then
        return
    end

    local simulation_minute = math.floor(clock * 1440)
    if simulation_minute == last_simulation_minute then
        return
    end

    last_simulation_minute = simulation_minute
    update_summary()
end

railos.on("tick", "on_tick")
