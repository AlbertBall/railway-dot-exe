//---------------------------------------------------------------------------

#ifndef ModdingH
#define ModdingH

#include <Graphics.hpp>

#include <optional>
#include <memory>
#include <vector>
#include <filesystem>
#include <map>
#include <set>
#include <functional>
#include <windows.h>

#include "Utilities.h"

//---------------------------------------------------------------------------
// Introduce concept of "Modding", users can replace content in Mods directory

extern std::string RESOURCE_PREFIX;
extern const std::string MODS_DIRECTORY;

enum class Transparency {
	Transparent,
	NoTransparency,
    Undefined
};


class RuntimeModifier {
	private:
		std::optional<std::string> graphics_library_{std::nullopt};

		const std::string mods_directory_{"Mods"};
		const std::string library_icon_default_{"Library"};
		TColor foreground_color_{clBlack};
        TColor background_color_{clWhite};

		std::vector<std::function<void()>> callbacks_;

		void trigger_callbacks_() {
            set_color_scheme_();
			for(auto& callback : callbacks_) {
                callback();
            }
		}
        void apply_color_scheme_(TBitmap*);
		void create_directories_() const;
		void set_color_scheme_() {
			background_color_ = Utilities->clTransparent;
            foreground_color_ = (background_color_ == clWhite) ? clBlack : clWhite;
		}
	public:
		RuntimeModifier() {
			create_directories_();
		}

		std::optional<std::string> get_current_graphics_library() const {
            return graphics_library_;
        }

		void attach_callback(std::function<void()> callback);

		std::vector<std::string> get_graphics_libraries() const;

		void set_graphics_library(const std::optional<std::string>& graphics_prefix);

		std::optional<std::string> get_library_icon_file(const std::string& library) const;

		// Returns the current mod graphics directory, if applicable
		std::optional<std::string> get_current_graphics_directory() const;

		void load_graphic(TBitmap* target, const std::string& graphic, const Transparency transparency = Transparency::Undefined);

        void clear_callbacks();
};

extern std::unique_ptr<RuntimeModifier> Modifier;

// ---------------------------------------------------------------------------
// Feature mods
//
// Feature mods live below Mods/Features/<mod id>. Every mod has a mod.ini
// manifest. Simple mods declare their actions in that manifest; advanced mods
// may additionally provide a Lua entry point. Both use the same host actions.

struct FeatureModAction {
    std::string mod_id;
    std::string id;
    std::string event;
    std::string caption;
    std::string edit_caption;
    std::string remove_caption;
    std::string command;
    std::string lua_handler;
    int max_length{32};
    TColor default_colour{clSilver};
};

struct FeatureModOverride {
    std::string mod_id;
    std::string id;
    std::string event{"startup"};
    std::string path;
    std::string value;
};

struct FeatureModEventHandler {
    std::string mod_id;
    std::string event;
    std::string handler;
};

struct FeatureModDefinition {
    std::string id;
    std::string name;
    std::string description;
    std::string version;
    int api_version{1};
    bool enabled_by_default{false};
    bool enabled{false};
    std::filesystem::path directory;
    std::filesystem::path lua_entrypoint;
    std::vector<std::string> dependencies;
    std::string train_display;
    std::string train_colour;
    std::vector<FeatureModAction> actions;
    std::vector<FeatureModOverride> overrides;
    void *lua_state{nullptr};
};

class TTrack;

class FeatureModManager {
private:
    std::filesystem::path mods_directory_;
    std::filesystem::path features_directory_;
    std::filesystem::path enabled_file_;
    std::filesystem::path log_file_;
    std::vector<FeatureModDefinition> mods_;
    std::vector<FeatureModAction> actions_;
    std::vector<FeatureModOverride> overrides_;
    std::vector<FeatureModEventHandler> event_handlers_;
    std::set<std::string> enabled_ids_;
    HMODULE lua_module_{nullptr};
    int executing_action_{-1};
    int loading_lua_mod_{-1};
    bool emitting_event_{false};

    std::function<void(const std::string&, const std::string&, int, const std::string&, TColor)> set_overlay_;
    std::function<void(const std::string&, const std::string&, int)> remove_overlay_;
    std::function<void()> redraw_;

    void load_enabled_ids_();
    bool load_manifest_(const std::filesystem::path& manifest, FeatureModDefinition& result);
    void initialise_lua_();
    void close_lua_();
    void log_(const std::string& message) const;

public:
    FeatureModManager();
    ~FeatureModManager();

    void discover();
    const std::vector<FeatureModAction>& actions() const { return actions_; }
    const std::vector<FeatureModDefinition>& mods() const { return mods_; }
    bool is_enabled(const std::string& mod_id) const;
    bool train_interpose_labels_enabled() const;
    bool train_lateness_colours_enabled() const;
    bool invoke_lua(size_t action_index, int track_vector_position);
    std::string get_value(const std::string& path) const;
    bool set_value(const std::string& path, const std::string& value);
    bool command(const std::string& command_name, const std::string& target, const std::string& value);
    void register_event_handler(const std::string& event, const std::string& handler);
    void emit_event(const std::string& event, const std::string& payload = "");
    void save_session_state(const std::string& session_file, const TTrack *track) const;
    void load_session_state(const std::string& session_file, TTrack *track) const;
    void lua_set_overlay(int track_vector_position, const std::string& text, TColor colour);
    void lua_remove_overlay(int track_vector_position);
    void lua_redraw();

    void set_host_callbacks(
        std::function<void(const std::string&, const std::string&, int, const std::string&, TColor)> set_overlay,
        std::function<void(const std::string&, const std::string&, int)> remove_overlay,
        std::function<void()> redraw);
};

extern std::unique_ptr<FeatureModManager> FeatureMods;
#endif
