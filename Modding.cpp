//---------------------------------------------------------------------------

#pragma hdrstop

#include "Modding.h"
#include "TrackUnit.h"
#include "TrainUnit.h"
#include "InterfaceUnit.h"
#include <Vcl.Dialogs.hpp>
#include <fstream>
#include <sstream>
#include <cctype>
#include <windows.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
std::unique_ptr<RuntimeModifier> Modifier{new RuntimeModifier};
std::unique_ptr<FeatureModManager> FeatureMods{new FeatureModManager};

namespace {

std::string trim_copy(const std::string& value) {
    size_t first = 0;
    while(first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) ++first;
    size_t last = value.size();
    while(last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) --last;
    return value.substr(first, last - first);
}

std::string lower_copy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string unquote_copy(const std::string& value) {
    std::string result = trim_copy(value);
    if(result.size() >= 2 && ((result.front() == '"' && result.back() == '"') ||
       (result.front() == '\'' && result.back() == '\''))) {
        result = result.substr(1, result.size() - 2);
    }
    return result;
}

bool parse_bool(const std::string& value) {
    const std::string normal = lower_copy(unquote_copy(value));
    return normal == "true" || normal == "yes" || normal == "1" || normal == "on";
}

TColor parse_colour(const std::string& value) {
    const std::string normal = lower_copy(unquote_copy(value));
    if(normal == "red") return clRed;
    if(normal == "green") return clGreen;
    if(normal == "yellow") return clYellow;
    if(normal == "white") return clWhite;
    if(normal == "black") return clBlack;
    if(normal == "blue") return clBlue;
    if(normal == "silver" || normal == "grey" || normal == "gray") return clSilver;
    try {
        return static_cast<TColor>(std::stoll(normal, nullptr, 0));
    } catch(...) {
        return clSilver;
    }
}

std::string encode_field(const std::string& value) {
    static const char digits[] = "0123456789ABCDEF";
    std::string result;
    for(unsigned char c : value) {
        if(std::isalnum(c) || c == '.' || c == '-' || c == '_' || c == ' ') result += static_cast<char>(c);
        else {
            result += '%';
            result += digits[(c >> 4) & 0x0f];
            result += digits[c & 0x0f];
        }
    }
    return result;
}

int hex_value(char c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

std::string decode_field(const std::string& value) {
    std::string result;
    for(size_t i = 0; i < value.size(); ++i) {
        if(value[i] == '%' && i + 2 < value.size()) {
            const int high = hex_value(value[i + 1]);
            const int low = hex_value(value[i + 2]);
            if(high >= 0 && low >= 0) {
                result += static_cast<char>((high << 4) | low);
                i += 2;
                continue;
            }
        }
        result += value[i];
    }
    return result;
}

std::vector<std::string> split_path(const std::string& path) {
    std::vector<std::string> result;
    size_t start = 0;
    for(;;) {
        const size_t dot = path.find('.', start);
        result.push_back(path.substr(start, dot == std::string::npos ? dot : dot - start));
        if(dot == std::string::npos) break;
        start = dot + 1;
    }
    return result;
}

bool to_int(const std::string& value, int& result) {
    try { size_t used = 0; result = std::stoi(value, &used, 0); return used == value.size(); }
    catch(...) { return false; }
}

bool to_double(const std::string& value, double& result) {
    try { size_t used = 0; result = std::stod(value, &used); return used == value.size(); }
    catch(...) { return false; }
}

std::string bool_string(bool value) { return value ? "true" : "false"; }

std::vector<std::string> split_csv(const std::string& value) {
    std::vector<std::string> result;
    std::stringstream input(value);
    std::string part;
    while(std::getline(input, part, ',')) result.push_back(trim_copy(part));
    return result;
}

// Lua is loaded dynamically so RailOS remains usable without a Lua runtime.
// Mods that declare a Lua entrypoint are activated when lua54.dll is available
// beside railway.exe or in Mods/Runtime.
struct lua_State;
typedef long long lua_Integer;
typedef intptr_t lua_KContext;
typedef int (__cdecl *lua_CFunction)(lua_State *L);
typedef int (__cdecl *lua_KFunction)(lua_State *L, int status, lua_KContext ctx);

lua_State* (__cdecl *p_luaL_newstate)() = nullptr;
void (__cdecl *p_luaL_openlibs)(lua_State*) = nullptr;
int (__cdecl *p_luaL_loadfilex)(lua_State*, const char*, const char*) = nullptr;
int (__cdecl *p_lua_pcallk)(lua_State*, int, int, int, lua_KContext, lua_KFunction) = nullptr;
void (__cdecl *p_lua_close)(lua_State*) = nullptr;
int (__cdecl *p_lua_getglobal)(lua_State*, const char*) = nullptr;
void (__cdecl *p_lua_setglobal)(lua_State*, const char*) = nullptr;
void (__cdecl *p_lua_createtable)(lua_State*, int, int) = nullptr;
void (__cdecl *p_lua_pushcclosure)(lua_State*, lua_CFunction, int) = nullptr;
void (__cdecl *p_lua_setfield)(lua_State*, int, const char*) = nullptr;
void (__cdecl *p_lua_pushinteger)(lua_State*, lua_Integer) = nullptr;
const char* (__cdecl *p_lua_pushstring)(lua_State*, const char*) = nullptr;
lua_Integer (__cdecl *p_lua_tointegerx)(lua_State*, int, int*) = nullptr;
const char* (__cdecl *p_lua_tolstring)(lua_State*, int, size_t*) = nullptr;
int (__cdecl *p_lua_type)(lua_State*, int) = nullptr;
void (__cdecl *p_lua_settop)(lua_State*, int) = nullptr;

FeatureModManager *active_feature_manager = nullptr;

const char* lua_string(lua_State *state, int index) {
    return p_lua_tolstring ? p_lua_tolstring(state, index, nullptr) : nullptr;
}

int __cdecl lua_set_track_overlay(lua_State *state) {
    if(!active_feature_manager) return 0;
    int valid = 0;
    const int track_position = static_cast<int>(p_lua_tointegerx(state, 1, &valid));
    const char *text = lua_string(state, 2);
    const int colour = static_cast<int>(p_lua_tointegerx(state, 3, nullptr));
    if(valid && text) active_feature_manager->lua_set_overlay(track_position, text, static_cast<TColor>(colour));
    return 0;
}

int __cdecl lua_remove_track_overlay(lua_State *state) {
    if(!active_feature_manager) return 0;
    int valid = 0;
    const int track_position = static_cast<int>(p_lua_tointegerx(state, 1, &valid));
    if(valid) active_feature_manager->lua_remove_overlay(track_position);
    return 0;
}

int __cdecl lua_request_redraw(lua_State*) {
    if(active_feature_manager) active_feature_manager->lua_redraw();
    return 0;
}

int __cdecl lua_get_value(lua_State *state) {
    if(!active_feature_manager) return 0;
    const char *path = lua_string(state, 1);
    if(!path) return 0;
    const std::string value = active_feature_manager->get_value(path);
    p_lua_pushstring(state, value.c_str());
    return 1;
}

int __cdecl lua_set_value(lua_State *state) {
    if(!active_feature_manager) return 0;
    const char *path = lua_string(state, 1);
    const char *value = lua_string(state, 2);
    p_lua_pushinteger(state, (path && value && active_feature_manager->set_value(path, value)) ? 1 : 0);
    return 1;
}

int __cdecl lua_command(lua_State *state) {
    if(!active_feature_manager) return 0;
    const char *name = lua_string(state, 1);
    const char *target = lua_string(state, 2);
    const char *value = lua_string(state, 3);
    p_lua_pushinteger(state, (name && active_feature_manager->command(name, target ? target : "", value ? value : "")) ? 1 : 0);
    return 1;
}

int __cdecl lua_on_event(lua_State *state) {
    if(!active_feature_manager) return 0;
    const char *event = lua_string(state, 1);
    const char *handler = lua_string(state, 2);
    if(event && handler) active_feature_manager->register_event_handler(event, handler);
    return 0;
}

template<typename T>
bool load_lua_symbol(HMODULE module, const char *name, T& target) {
    target = reinterpret_cast<T>(GetProcAddress(module, name));
    return target != nullptr;
}

} // namespace

void RuntimeModifier::create_directories_() const {

	std::vector<std::string> sub_directories_{
		"Graphics",
	};

	for(const std::string& dir : sub_directories_) {
		try {
			std::filesystem::create_directories(mods_directory_ + "/" + dir);
		} catch (const std::filesystem::filesystem_error& e) {
		   std::cerr << "RuntimeModifier Error: " << e.what() << "\n";
		}
	}
}


void RuntimeModifier::attach_callback(std::function<void()> callback) {
	// Attach to tracked callbacks, these will be executed
	// if the modifier is changed
	callbacks_.push_back(std::move(callback));
}

std::vector<std::string> RuntimeModifier::get_graphics_libraries() const {
	std::vector<std::string> graphics_dirs_;
	for(const auto& addr : std::filesystem::directory_iterator(mods_directory_ + "\\Graphics")) {
		if(!std::filesystem::is_directory(addr.status())) continue;
		graphics_dirs_.push_back(addr.path().filename().generic_string());
	}
	return graphics_dirs_;
}

void RuntimeModifier::set_graphics_library(const std::optional<std::string>& graphics_prefix) {
	graphics_library_ = graphics_prefix;
	trigger_callbacks_();
}

// Returns the current mod graphics directory, if applicable
std::optional<std::string> RuntimeModifier::get_current_graphics_directory() const {
	if (!graphics_library_.has_value()) {
		 return std::nullopt;
	}
	return mods_directory_ + "\\Graphics\\" + graphics_library_.value();
}

void RuntimeModifier::apply_color_scheme_(TBitmap* target) {
    // Graphics should be defined for a white background by default
	for(int y{0}; y < target->Height; ++y) {
		for(int x{0}; x < target->Width; ++y) {
			const TColor current_pixel_ = target->Canvas->Pixels[x][y];
            TColor new_color_ = current_pixel_;

			if(background_color_ != clWhite) {
				if(current_pixel_ == background_color_) {
					new_color_ = foreground_color_;
				} else if (current_pixel_ == clBlack) {
                    new_color_ = background_color_;
				}
			}

            target->Canvas->Pixels[x][y] = new_color_;
        }
    }
}

void RuntimeModifier::load_graphic(TBitmap* target, const std::string& graphic, const Transparency transparency) {
	const std::optional<std::string> current_graphics_dir{get_current_graphics_directory()};

    std::optional<std::string> local_file_ = std::nullopt;

	if(graphics_library_.has_value()) {
		std::string graphic_{graphic};
		std::transform(graphic_.begin(), graphic_.end(), graphic_.begin(),
		   [](unsigned char c){ return std::toupper(c);});
		local_file_ = std::optional<std::string>(
			current_graphics_dir.value() + "\\" + graphic_ + ".bmp"
		);
	}

	if(local_file_.has_value() && std::filesystem::exists(local_file_.value())) {
		target->LoadFromFile(local_file_.value().c_str());
	} else {
		target->LoadFromResourceName((int)HInstance, graphic.c_str());
	}

	if(transparency != Transparency::Undefined) {
		target->Transparent = transparency == Transparency::Transparent;
		target->TransparentColor = clWhite;
    }
}

std::optional<std::string> RuntimeModifier::get_library_icon_file(const std::string& library) const {
	const std::filesystem::path target_dir_{mods_directory_ + "/Graphics/" + library};
	if(!std::filesystem::exists(target_dir_) || !std::filesystem::is_directory(target_dir_)) {
		return std::nullopt;
	}
	const std::filesystem::path default_icon_ = target_dir_ / std::filesystem::path(library_icon_default_ + ".bmp");

	if(std::filesystem::exists(default_icon_)) return default_icon_.string();

	for (const auto& entry : std::filesystem::directory_iterator(target_dir_)) {
		if (!std::filesystem::is_regular_file(entry)) continue;

		std::string ext = entry.path().extension().string();
		for (auto& c : ext) c = tolower(c);

		if (ext == ".bmp") {
			return entry.path().string(); // first .bmp found
		}
	}
	return std::nullopt;
}

void RuntimeModifier::clear_callbacks() {
    callbacks_.clear();
}

// ---------------------------------------------------------------------------

FeatureModManager::FeatureModManager() {
    try {
        wchar_t executable_path[MAX_PATH];
        const DWORD length = GetModuleFileNameW(nullptr, executable_path, MAX_PATH);
        const std::filesystem::path executable_directory = length > 0
            ? std::filesystem::path(executable_path).parent_path()
            : std::filesystem::current_path();
        mods_directory_ = executable_directory / "Mods";
        features_directory_ = mods_directory_ / "Features";
        enabled_file_ = features_directory_ / "enabled.txt";
        log_file_ = mods_directory_ / "mod-loader.log";
        std::filesystem::create_directories(features_directory_);
        std::filesystem::create_directories(mods_directory_ / "Runtime");
    } catch(const std::filesystem::filesystem_error& e) {
        log_(std::string("Unable to create feature mod directories: ") + e.what());
    }
}

FeatureModManager::~FeatureModManager() {
    close_lua_();
}

void FeatureModManager::log_(const std::string& message) const {
    try {
        std::ofstream output(log_file_, std::ios::app);
        if(output) output << message << '\n';
    } catch(...) {
        // Mod logging must never prevent RailOS from starting.
    }
}

void FeatureModManager::load_enabled_ids_() {
    enabled_ids_.clear();
    std::ifstream input(enabled_file_);
    std::string line;
    while(std::getline(input, line)) {
        line = trim_copy(line);
        if(line.empty() || line[0] == '#' || line[0] == ';') continue;
        enabled_ids_.insert(line);
    }
}

bool FeatureModManager::load_manifest_(const std::filesystem::path& manifest, FeatureModDefinition& result) {
    std::ifstream input(manifest);
    if(!input) return false;

    result.directory = manifest.parent_path();
    std::string section;
    FeatureModAction *current_action = nullptr;
    FeatureModOverride *current_override = nullptr;
    std::string line;
    while(std::getline(input, line)) {
        line = trim_copy(line);
        if(line.empty() || line[0] == '#' || line[0] == ';') continue;
        if(line.front() == '[' && line.back() == ']') {
            section = trim_copy(line.substr(1, line.size() - 2));
            current_action = nullptr;
            current_override = nullptr;
            const std::string action_prefix = "action.";
            if(section.compare(0, action_prefix.size(), action_prefix) == 0) {
                result.actions.push_back(FeatureModAction());
                current_action = &result.actions.back();
                current_action->id = section.substr(action_prefix.size());
            }
            const std::string override_prefix = "override.";
            if(section.compare(0, override_prefix.size(), override_prefix) == 0) {
                result.overrides.push_back(FeatureModOverride());
                current_override = &result.overrides.back();
                current_override->id = section.substr(override_prefix.size());
            }
            continue;
        }

        const size_t equals = line.find('=');
        if(equals == std::string::npos) continue;
        const std::string key = lower_copy(trim_copy(line.substr(0, equals)));
        const std::string value = unquote_copy(line.substr(equals + 1));

        if(section == "mod") {
            if(key == "id") result.id = value;
            else if(key == "name") result.name = value;
            else if(key == "description") result.description = value;
            else if(key == "version") result.version = value;
            else if(key == "api") {
                try { result.api_version = std::stoi(value); } catch(...) { result.api_version = 0; }
            }
            else if(key == "enabled_by_default") result.enabled_by_default = parse_bool(value);
            else if(key == "lua") result.lua_entrypoint = result.directory / value;
            else if(key == "train_display") result.train_display = lower_copy(value);
            else if(key == "train_colour" || key == "train_color") result.train_colour = lower_copy(value);
            else if(key == "depends") {
                std::stringstream dependencies(value);
                std::string dependency;
                while(std::getline(dependencies, dependency, ',')) {
                    dependency = trim_copy(dependency);
                    if(!dependency.empty()) result.dependencies.push_back(dependency);
                }
            }
        }
        else if(current_action) {
            if(key == "event") current_action->event = value;
            else if(key == "caption") current_action->caption = value;
            else if(key == "edit_caption") current_action->edit_caption = value;
            else if(key == "remove_caption") current_action->remove_caption = value;
            else if(key == "command") current_action->command = value;
            else if(key == "lua_handler") current_action->lua_handler = value;
            else if(key == "max_length") {
                try { current_action->max_length = std::max(1, std::min(256, std::stoi(value))); } catch(...) {}
            }
            else if(key == "default_colour" || key == "default_color") current_action->default_colour = parse_colour(value);
        }
        else if(current_override) {
            if(key == "event") current_override->event = lower_copy(value);
            else if(key == "path") current_override->path = value;
            else if(key == "value") current_override->value = value;
        }
    }

    if(result.id.empty() || result.name.empty() || result.api_version != 1) {
        log_("Rejected manifest " + manifest.string() + ": id/name missing or unsupported API version");
        return false;
    }

    for(auto& action : result.actions) {
        action.mod_id = result.id;
        if(action.caption.empty()) action.caption = action.id;
        if(action.edit_caption.empty()) action.edit_caption = action.caption;
        if(action.remove_caption.empty()) action.remove_caption = "Remove " + action.caption;
    }
    for(auto& override_definition : result.overrides) override_definition.mod_id = result.id;
    return true;
}

void FeatureModManager::discover() {
    close_lua_();
    mods_.clear();
    actions_.clear();
    overrides_.clear();
    event_handlers_.clear();
    load_enabled_ids_();

    try {
        if(!std::filesystem::exists(features_directory_)) return;
        for(const auto& entry : std::filesystem::directory_iterator(features_directory_)) {
            if(!std::filesystem::is_directory(entry.status())) continue;
            const std::filesystem::path manifest = entry.path() / "mod.ini";
            if(!std::filesystem::exists(manifest)) continue;
            FeatureModDefinition mod;
            if(!load_manifest_(manifest, mod)) continue;
            mod.enabled = enabled_ids_.count(mod.id) > 0 || (enabled_ids_.empty() && mod.enabled_by_default);
            mods_.push_back(std::move(mod));
        }
        for(auto& mod : mods_) {
            if(!mod.enabled) continue;
            for(const auto& dependency : mod.dependencies) {
                bool dependency_enabled = false;
                for(const auto& candidate : mods_) {
                    if(candidate.id == dependency && candidate.enabled) {
                        dependency_enabled = true;
                        break;
                    }
                }
                if(!dependency_enabled) {
                    mod.enabled = false;
                    log_("Disabled feature mod " + mod.id + ": dependency " + dependency + " is not installed and enabled");
                    break;
                }
            }
        }
        for(const auto& mod : mods_) {
            if(!mod.enabled) continue;
            for(const auto& action : mod.actions) actions_.push_back(action);
            for(const auto& override_definition : mod.overrides) overrides_.push_back(override_definition);
            log_("Enabled feature mod " + mod.id + " (" + std::to_string(mod.actions.size()) + " actions)");
        }
    } catch(const std::filesystem::filesystem_error& e) {
        log_(std::string("Feature mod discovery failed: ") + e.what());
    }
    initialise_lua_();
    emit_event("startup");
    log_("Feature mod discovery complete: " + std::to_string(actions_.size()) + " actions available");
}

bool FeatureModManager::is_enabled(const std::string& mod_id) const {
    for(const auto& mod : mods_) if(mod.id == mod_id) return mod.enabled;
    return false;
}

bool FeatureModManager::train_interpose_labels_enabled() const {
    for(const auto& mod : mods_) {
        if(mod.enabled && mod.train_display == "interpose_label") return true;
    }
    return false;
}

bool FeatureModManager::train_lateness_colours_enabled() const {
    for(const auto& mod : mods_) {
        if(mod.enabled && mod.train_colour == "timetable_lateness") return true;
    }
    return false;
}

std::string FeatureModManager::get_value(const std::string& path) const {
    const std::vector<std::string> p = split_path(lower_copy(path));
    if(p.empty()) return "";

    if(p[0] == "simulation") {
        if(p.size() == 2 && p[1] == "clock" && TrainController) return std::to_string(double(TrainController->TTClockTime));
        if(p.size() == 2 && p[1] == "start_time" && TrainController) return std::to_string(double(TrainController->TimetableStartTime));
        if(p.size() == 2 && p[1] == "speed" && Interface) return std::to_string(Interface->TTClockSpeed);
        if(p.size() == 2 && p[1] == "mode" && Interface) {
            if(Interface->Level2OperMode == TInterface::Operating) return "operating";
            if(Interface->Level2OperMode == TInterface::Paused) return "paused";
            if(Interface->Level2OperMode == TInterface::PreStart) return "prestart";
            return "inactive";
        }
        if(p.size() == 2 && p[1] == "failure_mtbf_hours" && TrainController) return std::to_string(TrainController->MTBFHours);
    }

    if(p[0] == "trains" && TrainController) {
        if(p.size() == 2 && p[1] == "count") return std::to_string(TrainController->TrainVector.size());
        int index = -1;
        if(p.size() >= 3 && to_int(p[1], index) && index >= 0 && index < static_cast<int>(TrainController->TrainVector.size())) {
            const TTrain& train = TrainController->TrainVector.at(index);
            const std::string& field = p[2];
            if(field == "id") return std::to_string(train.TrainID);
            if(field == "headcode") return std::string(train.HeadCode.c_str());
            if(field == "description") return std::string(train.Description.c_str());
            if(field == "mode") return train.TrainMode == Timetable ? "timetable" : "signaller";
            if(field == "speed") return std::to_string(train.EntrySpeed);
            if(field == "max_speed") return std::to_string(train.MaxRunningSpeed);
            if(field == "mass") return std::to_string(train.Mass);
            if(field == "power") return std::to_string(train.PowerAtRail);
            if(field == "brake_rate") return std::to_string(train.MaxBrakeRate);
            if(field == "delay_minutes") return std::to_string(train.MinsDelayed);
            if(field == "background_colour" || field == "background_color") return std::to_string(static_cast<int>(train.BackgroundColour));
            if(field == "failed") return bool_string(train.TrainFailed);
            if(field == "crashed") return bool_string(train.Crashed);
            if(field == "derailed") return bool_string(train.Derailed);
            if(field == "stopped") return bool_string(const_cast<TTrain&>(train).Stopped());
            if(field == "lead_element") return std::to_string(train.LeadElement);
            if(field == "mid_element") return std::to_string(train.MidElement);
            if(field == "lag_element") return std::to_string(train.LagElement);
        }
    }

    if(p[0] == "timetable" && TrainController) {
        if(p.size() == 2 && p[1] == "count") return std::to_string(TrainController->TrainDataVector.size());
        int service = -1;
        if(p.size() >= 3 && to_int(p[1], service) && service >= 0 && service < static_cast<int>(TrainController->TrainDataVector.size())) {
            const TTrainDataEntry& entry = TrainController->TrainDataVector.at(service);
            const std::string& field = p[2];
            if(field == "headcode") return std::string(entry.HeadCode.c_str());
            if(field == "service_reference") return std::string(entry.ServiceReference.c_str());
            if(field == "description") return std::string(entry.FixedDescription.c_str());
            if(field == "max_speed") return std::to_string(entry.MaxRunningSpeed);
            if(field == "brake_rate") return std::to_string(entry.MaxBrakeRate);
            if(field == "power") return std::to_string(entry.PowerAtRail);
            if(field == "mass") return std::to_string(entry.Mass);
            if(field == "start_speed") return std::to_string(entry.StartSpeed);
            if(field == "signaller_speed") return std::to_string(entry.SignallerSpeed);
            if(field == "repeats") return std::to_string(entry.NumberOfTrains);
            if(field == "actions" && p.size() == 4 && p[3] == "count") return std::to_string(entry.ActionVector.size());
            if(field == "actions" && p.size() >= 5) {
                int action_index = -1;
                if(!to_int(p[3], action_index) || action_index < 0 || action_index >= static_cast<int>(entry.ActionVector.size())) return "";
                const TActionVectorEntry& action = entry.ActionVector.at(action_index);
                if(p[4] == "command") return std::string(action.Command.c_str());
                if(p[4] == "location") return std::string(action.LocationName.c_str());
                if(p[4] == "event_time") return std::to_string(double(action.EventTime));
                if(p[4] == "arrival_time") return std::to_string(double(action.ArrivalTime));
                if(p[4] == "departure_time") return std::to_string(double(action.DepartureTime));
                if(p[4] == "warning") return bool_string(action.Warning);
                if(p[4] == "min_dwell_seconds") return std::to_string(action.MinDwellTime);
            }
        }
    }

    if((p[0] == "track" || p[0] == "signals" || p[0] == "points") && Track) {
        if(p.size() == 2 && p[1] == "count") return std::to_string(Track->TrackVectorSize());
        int index = -1;
        if(p.size() >= 3 && to_int(p[1], index) && index >= 0 && index < Track->TrackVectorSize()) {
            const TTrackElement& element = Track->TrackElementAt(9000, index);
            const std::string& field = p[2];
            if(field == "id") return std::string(element.ElementID.c_str());
            if(field == "location") return std::string(element.LocationName.c_str());
            if(field == "active_name") return std::string(element.ActiveTrackElementName.c_str());
            if(field == "h") return std::to_string(element.HLoc);
            if(field == "v") return std::to_string(element.VLoc);
            if(field == "type") return std::to_string(static_cast<int>(element.TrackType));
            if(field == "speed_tag") return std::to_string(element.SpeedTag);
            if(field == "attribute" || field == "aspect" || field == "position") return std::to_string(element.Attribute);
            if(field == "failed") return bool_string(element.Failed);
            if(field == "calling_on") return bool_string(element.CallingOnSet);
            if(field == "length_01") return std::to_string(element.Length01);
            if(field == "length_23") return std::to_string(element.Length23);
            if(field == "speed_01") return std::to_string(element.SpeedLimit01);
            if(field == "speed_23") return std::to_string(element.SpeedLimit23);
            if(field == "train_id") return std::to_string(element.TrainIDOnElement);
        }
    }

    if(p[0] == "routes" && AllRoutes) {
        if(p.size() == 2 && p[1] == "count") return std::to_string(AllRoutes->AllRoutesSize());
        if(p.size() == 2 && p[1] == "locked_count") return std::to_string(AllRoutes->LockedRouteVector.size());
        int index = -1;
        if(p.size() >= 3 && to_int(p[1], index) && index >= 0 && index < static_cast<int>(AllRoutes->AllRoutesSize())) {
            const TOneRoute& route = AllRoutes->GetFixedRouteAt(9001, index);
            if(p[2] == "id") return std::to_string(route.RouteID);
            if(p[2] == "elements") return std::to_string(route.PrefDirSize());
        }
    }

    if(p[0] == "ui" && Interface && p.size() == 2 && p[1] == "status") return std::string(AnsiString(Interface->InfoPanel->Caption).c_str());
    return "";
}

bool FeatureModManager::set_value(const std::string& path, const std::string& value) {
    std::vector<std::string> p = split_path(lower_copy(path));
    if(p.size() >= 2 && p[1] == "*") {
        int count = 0;
        if(p[0] == "trains" && TrainController) count = static_cast<int>(TrainController->TrainVector.size());
        else if(p[0] == "timetable" && TrainController) count = static_cast<int>(TrainController->TrainDataVector.size());
        else if((p[0] == "track" || p[0] == "signals" || p[0] == "points") && Track) count = Track->TrackVectorSize();
        else return false;
        bool ok = true;
        for(int i = 0; i < count; ++i) {
            std::string concrete = path;
            const size_t wildcard = concrete.find('*');
            concrete.replace(wildcard, 1, std::to_string(i));
            ok = set_value(concrete, value) && ok;
        }
        return ok;
    }

    int integer = 0;
    double number = 0;
    const bool boolean = parse_bool(value);
    if(p.size() == 2 && p[0] == "simulation" && Interface && TrainController) {
        if(p[1] == "speed" && to_double(value, number) && number >= 0.0 && number <= 16.0) { Interface->TTClockSpeed = number; return true; }
        if(p[1] == "clock" && to_double(value, number)) { TrainController->TTClockTime = TDateTime(number); return true; }
        if(p[1] == "failure_mtbf_hours" && to_double(value, number) && number >= 1) { TrainController->MTBFHours = number; return true; }
    }
    if(p.size() >= 3 && p[0] == "trains" && TrainController) {
        int index = -1;
        if(!to_int(p[1], index) || index < 0 || index >= static_cast<int>(TrainController->TrainVector.size())) return false;
        TTrain& train = TrainController->TrainVector.at(index);
        if(p[2] == "headcode" && value.size() == 4) { train.HeadCode = value.c_str(); train.SetHeadCodeGraphics(9002, train.HeadCode); return true; }
        if(p[2] == "description") { train.Description = value.c_str(); return true; }
        if(p[2] == "max_speed" && to_double(value, number) && number >= 0 && number <= TTrain::MaximumSpeedLimit) { train.MaxRunningSpeed = number; return true; }
        if(p[2] == "mass" && to_int(value, integer) && integer > 0) { train.Mass = integer; train.AValue = sqrt(2 * train.PowerAtRail / train.Mass); return true; }
        if(p[2] == "power" && to_double(value, number) && number >= 0) { train.PowerAtRail = number; train.AValue = sqrt(2 * train.PowerAtRail / train.Mass); return true; }
        if(p[2] == "brake_rate" && to_double(value, number) && number > 0) { train.MaxBrakeRate = number; return true; }
        if(p[2] == "delay_minutes" && to_double(value, number)) { train.MinsDelayed = number; return true; }
        if((p[2] == "background_colour" || p[2] == "background_color") && to_int(value, integer)) { train.PlotTrainWithNewBackgroundColour(9003, static_cast<TColor>(integer), Display); return true; }
        if(p[2] == "failed") { train.TrainFailed = boolean; return true; }
        if(p[2] == "crashed") { train.Crashed = boolean; return true; }
        if(p[2] == "derailed") { train.Derailed = boolean; return true; }
    }
    if(p.size() >= 3 && p[0] == "timetable" && TrainController) {
        int service = -1;
        if(!to_int(p[1], service) || service < 0 || service >= static_cast<int>(TrainController->TrainDataVector.size())) return false;
        TTrainDataEntry& entry = TrainController->TrainDataVector.at(service);
        if(p[2] == "headcode") { entry.HeadCode = value.c_str(); return true; }
        if(p[2] == "service_reference") { entry.ServiceReference = value.c_str(); return true; }
        if(p[2] == "description") { entry.FixedDescription = value.c_str(); entry.ExplicitDescription = true; return true; }
        if(p[2] == "max_speed" && to_double(value, number)) { entry.MaxRunningSpeed = number; return true; }
        if(p[2] == "brake_rate" && to_double(value, number)) { entry.MaxBrakeRate = number; return true; }
        if(p[2] == "power" && to_double(value, number)) { entry.PowerAtRail = number; return true; }
        if(p[2] == "mass" && to_int(value, integer)) { entry.Mass = integer; return true; }
        if(p[2] == "start_speed" && to_int(value, integer)) { entry.StartSpeed = integer; return true; }
        if(p[2] == "signaller_speed" && to_int(value, integer)) { entry.SignallerSpeed = integer; return true; }
        if(p[2] == "actions" && p.size() >= 5) {
            int action_index = -1;
            if(!to_int(p[3], action_index) || action_index < 0 || action_index >= static_cast<int>(entry.ActionVector.size())) return false;
            TActionVectorEntry& action = entry.ActionVector.at(action_index);
            if(p[4] == "command") { action.Command = value.c_str(); return true; }
            if(p[4] == "location") { action.LocationName = value.c_str(); return true; }
            if(p[4] == "event_time" && to_double(value, number)) { action.EventTime = TDateTime(number); return true; }
            if(p[4] == "arrival_time" && to_double(value, number)) { action.ArrivalTime = TDateTime(number); return true; }
            if(p[4] == "departure_time" && to_double(value, number)) { action.DepartureTime = TDateTime(number); return true; }
            if(p[4] == "warning") { action.Warning = boolean; return true; }
            if(p[4] == "min_dwell_seconds" && to_double(value, number)) { action.MinDwellTime = number; return true; }
        }
    }
    if(p.size() >= 3 && (p[0] == "track" || p[0] == "signals" || p[0] == "points") && Track) {
        int index = -1;
        if(!to_int(p[1], index) || index < 0 || index >= Track->TrackVectorSize()) return false;
        TTrackElement& element = Track->TrackElementAt(9004, index);
        if(p[2] == "location") { element.LocationName = value.c_str(); return true; }
        if(p[2] == "active_name") { element.ActiveTrackElementName = value.c_str(); return true; }
        if((p[2] == "attribute" || p[2] == "aspect" || p[2] == "position") && to_int(value, integer)) { element.Attribute = integer; return true; }
        if(p[2] == "failed") { element.Failed = boolean; return true; }
        if(p[2] == "calling_on") { element.CallingOnSet = boolean; return true; }
        if(p[2] == "length_01" && to_int(value, integer)) { element.Length01 = integer; return true; }
        if(p[2] == "length_23" && to_int(value, integer)) { element.Length23 = integer; return true; }
        if(p[2] == "speed_01" && to_int(value, integer)) { element.SpeedLimit01 = integer; return true; }
        if(p[2] == "speed_23" && to_int(value, integer)) { element.SpeedLimit23 = integer; return true; }
    }
    if(p.size() == 2 && p[0] == "ui" && p[1] == "status" && Interface) { Interface->InfoPanel->Caption = value.c_str(); return true; }
    return false;
}

bool FeatureModManager::command(const std::string& command_name, const std::string& target, const std::string& value) {
    const std::string name = lower_copy(command_name);
    int index = -1;
    if(name == "ui.message") { ShowMessage(UnicodeString(value.c_str())); return true; }
    if(name == "ui.redraw") { lua_redraw(); return true; }
    if(name == "simulation.pause" && Interface) { Interface->Level2OperMode = TInterface::Paused; return true; }
    if(name == "simulation.resume" && Interface) { Interface->Level2OperMode = TInterface::Operating; return true; }
    if(name == "routes.cancel" && AllRoutes && to_int(target, index) && index >= 0 && index < static_cast<int>(AllRoutes->AllRoutesSize())) {
        AllRoutes->GetModifiableRouteAt(9005, index).ForceCancelRoute(9006); return true;
    }
    if(name == "track.add" && Track) {
        const std::vector<std::string> position = split_csv(target);
        const std::vector<std::string> parameters = split_csv(value);
        int h = 0, v = 0, tag = 0, aspect = 0;
        if(position.size() != 2 || parameters.empty() || !to_int(position[0], h) || !to_int(position[1], v) || !to_int(parameters[0], tag)) return false;
        if(parameters.size() > 1 && !to_int(parameters[1], aspect)) return false;
        bool plotted = false;
        Track->PlotAndAddTrackElement(9007, tag, aspect, h, v, plotted, true, true);
        return plotted;
    }
    if(name == "track.delete" && Track && Interface) {
        const std::vector<std::string> position = split_csv(target);
        int h = 0, v = 0;
        if(position.size() != 2 || !to_int(position[0], h) || !to_int(position[1], v)) return false;
        int erased = -1; bool success = false;
        Track->EraseTrackElement(9008, h, v, Interface->EveryPrefDir, erased, success, true);
        if(success && Interface->EveryPrefDir) Interface->EveryPrefDir->RealignAfterTrackErase(9009, erased);
        return success;
    }
    if(name == "events.emit") { emit_event(lower_copy(target), value); return true; }
    return false;
}

void FeatureModManager::register_event_handler(const std::string& event, const std::string& handler) {
    if(loading_lua_mod_ < 0 || loading_lua_mod_ >= static_cast<int>(mods_.size())) return;
    FeatureModEventHandler registration;
    registration.mod_id = mods_.at(loading_lua_mod_).id;
    registration.event = lower_copy(event);
    registration.handler = handler;
    event_handlers_.push_back(registration);
}

void FeatureModManager::emit_event(const std::string& event, const std::string& payload) {
    if(emitting_event_) return;
    emitting_event_ = true;
    const std::string normal_event = lower_copy(event);
    for(const auto& override_definition : overrides_) {
        if(lower_copy(override_definition.event) == normal_event && !set_value(override_definition.path, override_definition.value))
            log_("Override failed: " + override_definition.mod_id + "." + override_definition.id + " -> " + override_definition.path);
    }
    for(const auto& registration : event_handlers_) {
        if(registration.event != normal_event) continue;
        for(auto& mod : mods_) {
            if(mod.id != registration.mod_id || !mod.enabled || !mod.lua_state) continue;
            lua_State *state = static_cast<lua_State*>(mod.lua_state);
            if(p_lua_getglobal(state, registration.handler.c_str()) != 6) { p_lua_settop(state, -2); continue; }
            p_lua_pushstring(state, normal_event.c_str());
            p_lua_pushstring(state, payload.c_str());
            if(p_lua_pcallk(state, 2, 0, 0, 0, nullptr) != 0) {
                const char *error = lua_string(state, -1);
                log_("Lua event failed for " + mod.id + ": " + (error ? error : "unknown error"));
                p_lua_settop(state, -2);
            }
        }
    }
    emitting_event_ = false;
}

void FeatureModManager::save_session_state(const std::string& session_file, const TTrack *track) const {
    if(!track || session_file.empty()) return;
    try {
        std::ofstream output(session_file + ".mods", std::ios::trunc);
        if(!output) {
            log_("Unable to save feature mod session state for " + session_file);
            return;
        }
        output << "RAILOS_MOD_STATE 1\n";
        for(TTrack::TModTrackOverlayMap::const_iterator map_it = track->ModTrackOverlayMap.begin();
            map_it != track->ModTrackOverlayMap.end(); ++map_it) {
            for(TTrack::TModTrackOverlayVector::const_iterator overlay = map_it->second.begin();
                overlay != map_it->second.end(); ++overlay) {
                output << map_it->first << '\t'
                       << static_cast<int>(overlay->BackgroundColour) << '\t'
                       << encode_field(overlay->ModId) << '\t'
                       << encode_field(overlay->ActionId) << '\t'
                       << encode_field(std::string(overlay->Text.c_str())) << '\n';
            }
        }
    } catch(const std::exception& e) {
        log_(std::string("Feature mod session save failed: ") + e.what());
    }
}

void FeatureModManager::load_session_state(const std::string& session_file, TTrack *track) const {
    if(!track || session_file.empty()) return;
    track->ModTrackOverlayMap.clear();
    try {
        std::ifstream input(session_file + ".mods");
        std::string line;
        if(!std::getline(input, line)) return;
        if(trim_copy(line) != "RAILOS_MOD_STATE 1") {
            log_("Ignored unsupported feature mod session state for " + session_file);
            return;
        }
        while(std::getline(input, line)) {
            std::vector<std::string> fields;
            size_t start = 0;
            for(;;) {
                const size_t tab = line.find('\t', start);
                fields.push_back(line.substr(start, tab == std::string::npos ? tab : tab - start));
                if(tab == std::string::npos) break;
                start = tab + 1;
            }
            if(fields.size() != 5) continue;
            const std::string mod_id = decode_field(fields[2]);
            if(!is_enabled(mod_id)) continue;
            try {
                const int position = std::stoi(fields[0]);
                const TColor colour = static_cast<TColor>(std::stoi(fields[1]));
                if(position < 0 || position >= track->TrackVectorSize()) continue;
                track->SetModTrackOverlay(mod_id, decode_field(fields[3]), position,
                    AnsiString(decode_field(fields[4]).c_str()), colour);
            } catch(...) {
                log_("Ignored malformed feature mod state line");
            }
        }
    } catch(const std::exception& e) {
        log_(std::string("Feature mod session load failed: ") + e.what());
    }
}

void FeatureModManager::set_host_callbacks(
    std::function<void(const std::string&, const std::string&, int, const std::string&, TColor)> set_overlay,
    std::function<void(const std::string&, const std::string&, int)> remove_overlay,
    std::function<void()> redraw) {
    set_overlay_ = std::move(set_overlay);
    remove_overlay_ = std::move(remove_overlay);
    redraw_ = std::move(redraw);
}

void FeatureModManager::initialise_lua_() {
    bool lua_needed = false;
    for(const auto& mod : mods_) {
        if(mod.enabled && !mod.lua_entrypoint.empty()) { lua_needed = true; break; }
    }
    if(!lua_needed) return;

    lua_module_ = LoadLibraryW(L"lua54.dll");
    if(!lua_module_) {
        const std::filesystem::path runtime = mods_directory_ / "Runtime" / "lua54.dll";
        lua_module_ = LoadLibraryW(runtime.c_str());
    }
    if(!lua_module_) {
        log_("Lua mods are enabled, but lua54.dll was not found beside railway.exe or in Mods/Runtime");
        return;
    }

    bool ok = true;
    ok &= load_lua_symbol(lua_module_, "luaL_newstate", p_luaL_newstate);
    ok &= load_lua_symbol(lua_module_, "luaL_openlibs", p_luaL_openlibs);
    ok &= load_lua_symbol(lua_module_, "luaL_loadfilex", p_luaL_loadfilex);
    ok &= load_lua_symbol(lua_module_, "lua_pcallk", p_lua_pcallk);
    ok &= load_lua_symbol(lua_module_, "lua_close", p_lua_close);
    ok &= load_lua_symbol(lua_module_, "lua_getglobal", p_lua_getglobal);
    ok &= load_lua_symbol(lua_module_, "lua_setglobal", p_lua_setglobal);
    ok &= load_lua_symbol(lua_module_, "lua_createtable", p_lua_createtable);
    ok &= load_lua_symbol(lua_module_, "lua_pushcclosure", p_lua_pushcclosure);
    ok &= load_lua_symbol(lua_module_, "lua_setfield", p_lua_setfield);
    ok &= load_lua_symbol(lua_module_, "lua_pushinteger", p_lua_pushinteger);
    ok &= load_lua_symbol(lua_module_, "lua_pushstring", p_lua_pushstring);
    ok &= load_lua_symbol(lua_module_, "lua_tointegerx", p_lua_tointegerx);
    ok &= load_lua_symbol(lua_module_, "lua_tolstring", p_lua_tolstring);
    ok &= load_lua_symbol(lua_module_, "lua_type", p_lua_type);
    ok &= load_lua_symbol(lua_module_, "lua_settop", p_lua_settop);
    if(!ok) {
        log_("lua54.dll does not expose the required Lua 5.4 API");
        close_lua_();
        return;
    }

    active_feature_manager = this;
    for(size_t mod_index = 0; mod_index < mods_.size(); ++mod_index) {
        auto& mod = mods_[mod_index];
        if(!mod.enabled || mod.lua_entrypoint.empty()) continue;
        lua_State *state = p_luaL_newstate();
        if(!state) continue;
        mod.lua_state = state;
        p_luaL_openlibs(state);
        p_lua_createtable(state, 0, 7);
        p_lua_pushcclosure(state, lua_set_track_overlay, 0);
        p_lua_setfield(state, -2, "set_track_overlay");
        p_lua_pushcclosure(state, lua_remove_track_overlay, 0);
        p_lua_setfield(state, -2, "remove_track_overlay");
        p_lua_pushcclosure(state, lua_request_redraw, 0);
        p_lua_setfield(state, -2, "request_redraw");
        p_lua_pushcclosure(state, lua_get_value, 0);
        p_lua_setfield(state, -2, "get");
        p_lua_pushcclosure(state, lua_set_value, 0);
        p_lua_setfield(state, -2, "set");
        p_lua_pushcclosure(state, lua_command, 0);
        p_lua_setfield(state, -2, "command");
        p_lua_pushcclosure(state, lua_on_event, 0);
        p_lua_setfield(state, -2, "on");
        p_lua_setglobal(state, "railos");

        const std::string script_path = mod.lua_entrypoint.string();
        loading_lua_mod_ = static_cast<int>(mod_index);
        if(p_luaL_loadfilex(state, script_path.c_str(), nullptr) != 0 ||
           p_lua_pcallk(state, 0, 0, 0, 0, nullptr) != 0) {
            const char *error = lua_string(state, -1);
            log_("Lua load failed for " + mod.id + ": " + (error ? error : "unknown error"));
            p_lua_settop(state, -2);
            p_lua_close(state);
            mod.lua_state = nullptr;
        }
        loading_lua_mod_ = -1;
    }
}

void FeatureModManager::close_lua_() {
    if(p_lua_close) {
        for(auto& mod : mods_) {
            if(mod.lua_state) p_lua_close(static_cast<lua_State*>(mod.lua_state));
            mod.lua_state = nullptr;
        }
    }
    active_feature_manager = nullptr;
    if(lua_module_) FreeLibrary(lua_module_);
    lua_module_ = nullptr;
}

bool FeatureModManager::invoke_lua(size_t action_index, int track_vector_position) {
    if(action_index >= actions_.size()) return false;
    const FeatureModAction& action = actions_[action_index];
    if(action.lua_handler.empty()) return false;
    for(auto& mod : mods_) {
        if(mod.id != action.mod_id || !mod.lua_state) continue;
        lua_State *state = static_cast<lua_State*>(mod.lua_state);
        if(p_lua_getglobal(state, action.lua_handler.c_str()) != 6) {
            p_lua_settop(state, -2);
            log_("Lua handler not found: " + action.mod_id + "." + action.lua_handler);
            return false;
        }
        executing_action_ = static_cast<int>(action_index);
        p_lua_pushinteger(state, track_vector_position);
        const int result = p_lua_pcallk(state, 1, 0, 0, 0, nullptr);
        executing_action_ = -1;
        if(result != 0) {
            const char *error = lua_string(state, -1);
            log_("Lua handler failed for " + action.mod_id + ": " + (error ? error : "unknown error"));
            p_lua_settop(state, -2);
            return false;
        }
        return true;
    }
    return false;
}

void FeatureModManager::lua_set_overlay(int track_vector_position, const std::string& text, TColor colour) {
    if(executing_action_ < 0 || static_cast<size_t>(executing_action_) >= actions_.size() || !set_overlay_) return;
    const FeatureModAction& action = actions_[executing_action_];
    set_overlay_(action.mod_id, action.id, track_vector_position, text, colour);
}

void FeatureModManager::lua_remove_overlay(int track_vector_position) {
    if(executing_action_ < 0 || static_cast<size_t>(executing_action_) >= actions_.size() || !remove_overlay_) return;
    const FeatureModAction& action = actions_[executing_action_];
    remove_overlay_(action.mod_id, action.id, track_vector_position);
}

void FeatureModManager::lua_redraw() {
    if(redraw_) redraw_();
}
