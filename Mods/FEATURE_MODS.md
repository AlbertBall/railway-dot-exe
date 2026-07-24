# RailOS feature modding guide

Feature mods can inspect and change the live simulation without rebuilding RailOS. The API covers trains, timetables, signals, points, routes, failures, user-interface feedback, track editing, events, and simulation timing.

There are two ways to use the same host API:

- Declarative overrides in `mod.ini` suit fixed configuration changes and require no programming.
- Lua scripts suit decisions, calculations, event handling, and behaviour which changes while the simulation runs.

Several feature mods may be enabled together. Mods should make the smallest change they need and should declare dependencies when they build on another mod.

## Mod folder

Install each mod in its own folder beside `railway.exe`:

```text
railway.exe
Mods/
  Features/
    enabled.txt
    author.example-mod/
      mod.ini
      main.lua
  Runtime/
    lua54.dll
```

`enabled.txt` contains one enabled mod id per line. A feature mod is discovered when RailOS starts, so installing, updating, enabling, or disabling one requires a restart.

## Minimal manifest

```ini
[mod]
id = author.example-mod
name = Example Mod
description = A short explanation shown in the package manager.
version = 1.0.0
api = 1
enabled_by_default = false
```

Use a stable, globally distinctive `id`; reverse-domain or `author.mod-name` naming is recommended. API version `1` is currently required.

### Manifest fields

| Field | Required | Meaning |
| --- | --- | --- |
| `id` | Yes | Stable mod identity used by dependencies and saved state. |
| `name` | Yes | Human-readable name. |
| `description` | Recommended | Package-manager description. |
| `version` | Recommended | Mod version. |
| `api` | Yes | Feature API version; currently `1`. |
| `enabled_by_default` | No | Enables the mod when no explicit enabled list exists. |
| `depends` | No | Comma-separated ids which must be installed and enabled. |
| `lua` | No | Lua entry file relative to the mod folder. |
| `train_display` | No | Built-in train rendering capability; `interpose_label` is supported. |
| `train_colour` | No | Built-in train colour capability; `timetable_lateness` is supported. `train_color` is an alias. |

Example dependency:

```ini
[mod]
id = author.extension
name = Example Extension
description = Extends another feature mod.
version = 1.0.0
api = 1
depends = author.base-mod
```

RailOS disables a mod at load time when any declared dependency is missing or disabled.

## Declarative overrides

An override writes a value to an API path whenever its event occurs:

```ini
[override.faster-clock]
event = startup
path = simulation.speed
value = 2
```

Each override section needs a unique name after `override.` and supports:

| Field | Meaning |
| --- | --- |
| `event` | Event which triggers the write. Defaults to `startup`. |
| `path` | Writable API path from the reference below. |
| `value` | Value passed to that path. |

Wildcards apply one value to every current member of a collection:

```ini
[override.network-speed-limit]
event = startup
path = track.*.speed_01
value = 80
```

Wildcards are accepted at the index position of `trains`, `timetable`, `track`, `signals`, and `points`. They affect the members present when the event is delivered; they do not automatically affect objects created later. Use an appropriate later event or Lua for continuous behaviour.

An unsuccessful override is skipped and recorded in `Mods/mod-loader.log`.

## Lua API

Declare a script in the manifest:

```ini
[mod]
id = author.dynamic-mod
name = Dynamic Mod
description = Demonstrates event-driven behaviour.
version = 1.0.0
api = 1
lua = main.lua
```

RailOS provides this table to the script:

```lua
value = railos.get(path)
ok = railos.set(path, value)
ok = railos.command(name, target, value)
railos.on(event_name, handler_name)

railos.set_track_overlay(track_index, text, colour)
railos.remove_track_overlay(track_index)
railos.request_redraw()
```

`get` always returns a string. An unknown path returns an empty string. `set` and `command` return `1` for success and `0` for failure. Lua numbers can be passed as values; use `tonumber` when doing arithmetic on a value returned by `get`.

Example:

```lua
function on_train_update(event, train_id)
    local count = tonumber(railos.get("trains.count")) or 0
    for index = 0, count - 1 do
        if railos.get("trains." .. index .. ".id") == train_id then
            local delay = tonumber(railos.get("trains." .. index .. ".delay_minutes")) or 0
            if delay >= 10 then
                railos.set("trains." .. index .. ".background_colour", 255)
            end
            break
        end
    end
end

railos.on("train_updated", "on_train_update")
```

Lua 5.4 is loaded only when an enabled mod declares a Lua file. Put `lua54.dll` beside `railway.exe` or in `Mods/Runtime`.

Lua mods are trusted code. RailOS opens the standard Lua libraries and does not sandbox scripts. Only install scripted mods from authors you trust.

## Events

Register a named Lua function with `railos.on`. RailOS calls it as `handler(event_name, payload)`.

| Event | Payload | When it occurs |
| --- | --- | --- |
| `startup` | Empty | Enabled mods have been discovered and Lua scripts loaded. |
| `tick` | Simulation clock as a `TDateTime` number | The master simulation timer runs. This is a high-frequency event. |
| `train_updated` | RailOS train ID | A live train finishes an update cycle. |
| `track_context` | Track vector index | A track element's context menu is prepared. |
| `session_loaded` | Session filename | A session and its mod state have loaded. |
| `session_saved` | Session filename | A session and its mod state have saved. |

Mods can publish their own events:

```lua
railos.command("events.emit", "author.my-event", "payload text")
```

```lua
function receive(event, payload)
    -- React to another mod.
end

railos.on("author.my-event", "receive")
```

Prefix custom event names with the mod id to avoid collisions. Event delivery is synchronous. Nested event emission during a handler is ignored, preventing recursive event loops. Keep `tick` and `train_updated` handlers short.

## Data path rules

Paths are case-insensitive. Collection indexes are zero-based vector positions and may change as objects are inserted or removed. `trains.<index>.id` is the separate RailOS train ID; use it to find the current vector index again rather than retaining an old index.

The `signals` and `points` namespaces use track vector indexes so a value obtained from `track_context` can be used directly. They are semantic aliases over the same track-element address space; check `type` before assuming an element is a signal or point.

The access column below uses `R` for readable and `W` for writable.

## Simulation behaviour

| Path | Access | Value |
| --- | --- | --- |
| `simulation.clock` | R/W | Current simulation time as a Delphi/VCL `TDateTime` number. |
| `simulation.start_time` | R | Timetable start time as `TDateTime`. |
| `simulation.speed` | R/W | Clock multiplier from `0` to `16`. |
| `simulation.mode` | R | `operating`, `paused`, `prestart`, or `inactive`. |
| `simulation.failure_mtbf_hours` | R/W | Train failure mean time between failures; writable values must be at least `1`. |

A `TDateTime` value stores whole days before the decimal point and the time of day as a fraction. Add one minute with `1 / 1440` and one second with `1 / 86400`.

## Live trains

Use `trains.count` and indexes from `0` to `count - 1`.

| Path | Access | Value or constraint |
| --- | --- | --- |
| `trains.count` | R | Number of live trains. |
| `trains.<index>.id` | R | Stable RailOS train ID. |
| `trains.<index>.headcode` | R/W | Four-character headcode. Writes must be exactly four bytes. |
| `trains.<index>.description` | R/W | Description. |
| `trains.<index>.mode` | R | `timetable` or `signaller`. |
| `trains.<index>.speed` | R | Current/entry speed used by the simulation. |
| `trains.<index>.max_speed` | R/W | Maximum running speed, within RailOS's global maximum. |
| `trains.<index>.mass` | R/W | Positive mass; acceleration is recalculated. |
| `trains.<index>.power` | R/W | Non-negative power at rail; acceleration is recalculated. |
| `trains.<index>.brake_rate` | R/W | Positive maximum braking rate. |
| `trains.<index>.delay_minutes` | R/W | Signed timetable delay in minutes. |
| `trains.<index>.background_colour` | R/W | Numeric Windows/VCL `TColor`; `background_color` is an alias. |
| `trains.<index>.failed` | R/W | Boolean. |
| `trains.<index>.crashed` | R/W | Boolean. |
| `trains.<index>.derailed` | R/W | Boolean. |
| `trains.<index>.stopped` | R | Whether the train is stopped. |
| `trains.<index>.lead_element` | R | Lead track vector index. |
| `trains.<index>.mid_element` | R | Middle track vector index. |
| `trains.<index>.lag_element` | R | Lag track vector index. |

## Timetables

Use `timetable.count` for service entries. Timetable edits affect the loaded in-memory timetable; authors are responsible for preserving valid commands, times, and rolling-stock values.

| Path | Access | Value |
| --- | --- | --- |
| `timetable.count` | R | Number of service entries. |
| `timetable.<service>.headcode` | R/W | Service headcode. |
| `timetable.<service>.service_reference` | R/W | Service reference. |
| `timetable.<service>.description` | R/W | Fixed description; a write marks it explicit. |
| `timetable.<service>.max_speed` | R/W | Maximum running speed. |
| `timetable.<service>.brake_rate` | R/W | Maximum braking rate. |
| `timetable.<service>.power` | R/W | Power at rail. |
| `timetable.<service>.mass` | R/W | Train mass. |
| `timetable.<service>.start_speed` | R/W | Entry/start speed. |
| `timetable.<service>.signaller_speed` | R/W | Default speed for signaller-controlled operation. |
| `timetable.<service>.repeats` | R | Number of repeated trains. |
| `timetable.<service>.actions.count` | R | Number of timetable actions. |
| `timetable.<service>.actions.<action>.command` | R/W | RailOS timetable command. |
| `timetable.<service>.actions.<action>.location` | R/W | Location name. |
| `timetable.<service>.actions.<action>.event_time` | R/W | Event `TDateTime`. |
| `timetable.<service>.actions.<action>.arrival_time` | R/W | Arrival `TDateTime`. |
| `timetable.<service>.actions.<action>.departure_time` | R/W | Departure `TDateTime`. |
| `timetable.<service>.actions.<action>.warning` | R/W | Boolean warning flag. |
| `timetable.<service>.actions.<action>.min_dwell_seconds` | R/W | Minimum dwell in seconds. |

## Track, signals, points, and failures

`track`, `signals`, and `points` accept the same paths and global track vector indexes.

| Path | Access | Value |
| --- | --- | --- |
| `<namespace>.count` | R | Size of the track vector/address space. |
| `<namespace>.<index>.id` | R | Element identifier. |
| `<namespace>.<index>.location` | R/W | Location name. |
| `<namespace>.<index>.active_name` | R/W | Active element name. |
| `<namespace>.<index>.h` | R | Horizontal grid coordinate. |
| `<namespace>.<index>.v` | R | Vertical grid coordinate. |
| `<namespace>.<index>.type` | R | Numeric RailOS track type/tag. |
| `<namespace>.<index>.speed_tag` | R | Speed-tag metadata. |
| `<namespace>.<index>.attribute` | R/W | Raw element attribute. |
| `<namespace>.<index>.aspect` | R/W | Alias of `attribute`, intended for signals. |
| `<namespace>.<index>.position` | R/W | Alias of `attribute`, intended for points. |
| `<namespace>.<index>.failed` | R/W | Element failure flag. |
| `<namespace>.<index>.calling_on` | R/W | Signal calling-on flag. |
| `<namespace>.<index>.length_01` | R/W | Length through links 0-1. |
| `<namespace>.<index>.length_23` | R/W | Length through links 2-3. |
| `<namespace>.<index>.speed_01` | R/W | Speed limit through links 0-1. |
| `<namespace>.<index>.speed_23` | R/W | Speed limit through links 2-3. |
| `<namespace>.<index>.train_id` | R | Train occupying the element, or RailOS's empty sentinel. |

`attribute`, `aspect`, and `position` expose RailOS's raw numeric state. Only write values valid for that element type. Changing topology-related values while trains or routes use an element can invalidate a simulation.

### Persistent track overlays

A mod may add one or more context-menu actions to track elements:

```ini
[action.note]
event = track_context
caption = Add Note...
edit_caption = Edit Note...
remove_caption = Remove Note
command = text_overlay
max_length = 32
default_colour = silver
```

| Action field | Meaning |
| --- | --- |
| `event` | Use `track_context`. |
| `caption` | Add/invoke menu caption. |
| `edit_caption` | Caption when this action already owns an overlay there. |
| `remove_caption` | Removal caption. |
| `command` | `text_overlay` invokes the built-in editor. |
| `lua_handler` | Lua function called with the selected track index instead. |
| `max_length` | Built-in editor limit, clamped to `1`-`256`. |
| `default_colour` | Named or numeric initial colour; `default_color` is an alias. |

For scripted actions:

```ini
[action.inspect]
event = track_context
caption = Inspect Element
lua_handler = inspect_element
```

```lua
function inspect_element(track_index)
    local name = railos.get("track." .. track_index .. ".location")
    railos.command("ui.message", "", name)
end
```

Overlays are owned by mod id and action id. Several mods can place overlays on one element without replacing one another. Overlay state is stored in `<session filename>.mods`; the original RailOS session format is unchanged.

## Routes

| Path | Access | Value |
| --- | --- | --- |
| `routes.count` | R | Number of fixed routes. |
| `routes.locked_count` | R | Number of locked-route records. |
| `routes.<index>.id` | R | RailOS route ID. |
| `routes.<index>.elements` | R | Number of preferred-direction elements in the route. |

Route cancellation is provided as a command because it must use RailOS route cleanup rather than changing route fields directly.

## User interface

| Path | Access | Value |
| --- | --- | --- |
| `ui.status` | R/W | Text displayed in the RailOS information panel. |

Use commands for modal messages and redraws. Avoid showing modal messages from `tick` or `train_updated`, as this interrupts operation.

## Commands

Commands have three arguments: `name`, `target`, and `value`. Unused arguments may be empty strings.

| Name | Target | Value | Effect |
| --- | --- | --- | --- |
| `ui.message` | Unused | Message text | Shows a RailOS message box. |
| `ui.redraw` | Unused | Unused | Requests a full display redraw. |
| `simulation.pause` | Unused | Unused | Enters paused operating mode. |
| `simulation.resume` | Unused | Unused | Enters operating mode. |
| `routes.cancel` | Route vector index | Unused | Force-cancels the route using RailOS cleanup. |
| `track.add` | `h,v` | `track_tag,attribute` | Adds and plots a track element. `attribute` is optional and defaults to `0`. |
| `track.delete` | `h,v` | Unused | Deletes the element and realigns preferred directions. |
| `events.emit` | Event name | Payload | Publishes a synchronous custom event. |

Examples:

```lua
railos.command("simulation.pause", "", "")
railos.command("routes.cancel", "3", "")
railos.command("track.add", "40,18", "1,0")
railos.command("track.delete", "40,18", "")
railos.command("ui.redraw", "", "")
```

Track editing uses RailOS's numeric track tags. Additions must be valid for the chosen grid position. Prefer making topology changes while the railway is inactive, then redraw.

## Values and compatibility

Booleans written as `true`, `yes`, `1`, or `on` are true, case-insensitively; other values are false.

Colours are numeric Windows/VCL `TColor` values. Declarative overlay colours also accept `red`, `green`, `yellow`, `white`, `black`, `blue`, `silver`, `grey`, and `gray`. Numeric colour values are BGR-style VCL values, not CSS RGB values.

Raw RailOS enumerations such as track `type`, signal `aspect`, point `position`, and timetable action `command` are intentionally not translated. A mod which writes them should document which RailOS data conventions it expects.

Treat every write as a live edit to simulation state. The API validates indexes and basic numeric ranges, but it cannot prove that a combination of values is operationally safe. In particular:

- do not delete occupied, routed, or referenced track;
- do not assign signal aspects or point positions invalid for their element type;
- do not corrupt timetable action order or command syntax;
- recalculate decisions from current state rather than retaining vector indexes;
- use dependencies instead of assuming another mod is installed;
- namespace custom events and overlay action ids.

Loader decisions, rejected dependencies, failed declarative overrides, missing Lua runtime messages, and Lua errors are written to `Mods/mod-loader.log`.

## Graphics replacement

Graphics libraries under `Mods/Graphics` remain separate from feature mods. They replace named bitmap resources and do not receive events or alter simulation state. A package may contain both graphics and feature content, but each system is loaded independently.
