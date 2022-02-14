#include "tootlwren.hpp"
#include "wren.h"

#include <stdio.h>
#include <Resource.hpp>
#include <ResourceLoader.hpp>

using namespace godot;

TootlWren* global_this;
Node* global_game_manager;

void TootlWren::_register_methods() {
	register_method("parse_wren_snippet", &TootlWren::parse_wren_snippet);
	
	register_signal<TootlWren>((char *)"load_scene", "node", GODOT_VARIANT_TYPE_OBJECT, "scene_to_load", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_dialogue", "node", GODOT_VARIANT_TYPE_OBJECT, "dialogue_to_load", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_sfx", "node", GODOT_VARIANT_TYPE_OBJECT, "sfx_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_bgm", "node", GODOT_VARIANT_TYPE_OBJECT, "bgm_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_voice", "node", GODOT_VARIANT_TYPE_OBJECT, "voice_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_imv", "node", GODOT_VARIANT_TYPE_OBJECT, "imv_to_load", GODOT_VARIANT_TYPE_STRING);
}

TootlWren::TootlWren() {}
TootlWren::~TootlWren() {}

void TootlWren::_init() {}

// WREN WREN WREN WREN WREN //

void wrenSceneLoad(WrenVM* vm) {
	const char* str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("load_scene", String(str));
}

void wrenDialogueLoad(WrenVM* vm) {
	const char* str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("load_dialogue", String(str));
}

void wrenPlaySFX(WrenVM* vm) {
	const char* str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("play_sfx", String(str));
}

void wrenPlayBGM(WrenVM* vm) {
	const char* str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("play_bgm", String(str));
}

void wrenPlayVoice(WrenVM* vm) {
	const char* str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("play_voice", String(str));
}

void wrenInventoryAdd(WrenVM* vm) {
	const char* item_name = wrenGetSlotString(vm, 1);
	global_game_manager->call("AddItem", item_name);
}

void wrenInventoryRemove(WrenVM* vm) {
	const char* item_name = wrenGetSlotString(vm, 1);
	global_game_manager->call("RemoveItem", item_name);
}

void wrenInventoryHas(WrenVM* vm) {
	const char* item_name = wrenGetSlotString(vm, 1);
	bool has_item = global_game_manager->call("HasItem", item_name);
	wrenSetSlotBool(vm, 0, has_item);
}

void wrenLoadAnimation(WrenVM* vm) {
	const char* path_str = wrenGetSlotString(vm, 1);
	global_this->emit_signal("load_imv", String(path_str));
}

void wrenDefaultFunc(WrenVM* vm) {}

WrenForeignMethodFn bindForeignMethod(
	WrenVM* vm,
	const char* module,
	const char* className,
	bool isStatic,
	const char* signature)
{
	if (strcmp(module, "televoid-core") == 0)
	{
		if (strcmp(className, "Scene") == 0)
		{
			if (isStatic && strcmp(signature, "load(_)") == 0)
			{
				return wrenSceneLoad; // C function for Scene.load(_).
			}
			// Other foreign methods...
		}
		else if (strcmp(className, "Inventory") == 0) {
			if (isStatic && strcmp(signature, "add(_)") == 0) {
				return wrenInventoryAdd; // C function for Inventory.add(_).
			}
			else if (isStatic && strcmp(signature, "remove(_)") == 0) {
				return wrenInventoryRemove; // C function for Inventory.remove(_).
			}
			else if (isStatic && strcmp(signature, "has(_)") == 0) {
				return wrenInventoryHas;
			}
		}
		else if (strcmp(className, "Dialogue") == 0) {
			if (isStatic && strcmp(signature, "load(_)") == 0)
			{
				return wrenDialogueLoad; // C function for Dialogue.load(_).
			}
		}
		/*else if (strcmp(className, "Minigame") == 0) {
			if (isStatic && strcmp(signature, "load(_,_)") == 0)
			{
				return wrenMinigameLoad;
			}
		}*/
		else if (strcmp(className, "Audio") == 0) {
			if (isStatic && strcmp(signature, "playSFX(_)") == 0) {
				return wrenPlaySFX;
			}
			else if (isStatic && strcmp(signature, "playBGMOnce(_)") == 0) {
				return wrenPlayBGM;//Once;
			}
			else if (isStatic && strcmp(signature, "playBGM(_)") == 0) {
				return wrenPlayBGM;
			}
			else if (isStatic && strcmp(signature, "playVoice(_)") == 0) {
				return wrenPlayVoice;
			}
		}
		else if (strcmp(className, "Animation") == 0) {
			if (isStatic && strcmp(signature, "load(_)") == 0) {
				return wrenLoadAnimation;
			}
		}
		/*else if (strcmp(className, "Character") == 0) {
			if (isStatic && strcmp(signature, "walk(_,_,_)") == 0) {
				return wrenCharacterWalk;
			}
		}
		else if (strcmp(className, "GameSaver") == 0) {
			if (isStatic && strcmp(signature, "save()") == 0) {
				return wrenGameSave;
			}
			else if (isStatic && strcmp(signature, "load()") == 0) {
				return wrenGameLoad;
			}
		}*/
		// Other classes in televoid-core...
	}
	/*else if (strcmp(module, "minigame-core") == 0) {
		if (strcmp(className, "Window") == 0) {
			if (isStatic && strcmp(signature, "render(_,_,_,_,_,_)") == 0) {
				return wrenRender;
			}
			else if (isStatic && strcmp(signature, "renderText(_,_,_,_)") == 0) {
				return wrenRenderText;
			}
			else if (isStatic && strcmp(signature, "getWidth()") == 0) {
				return wrenWindowWidth;
			}
			else if (isStatic && strcmp(signature, "getHeight()") == 0) {
				return wrenWindowHeight;
			}
		}
		else if (strcmp(className, "Input") == 0) {
			if (isStatic && strcmp(signature, "getKey(_)") == 0) {
				return wrenInputGetKey;
			}
			else if (isStatic && strcmp(signature, "getKeyDown(_)") == 0) {
				return wrenInputGetKeyDown;
			}
			else if (isStatic && strcmp(signature, "getKeyUp(_)") == 0) {
				return wrenInputGetKeyUp;
			}
			else if (isStatic && strcmp(signature, "getMouse(_)") == 0) {
				return wrenInputGetMouse;
			}
			else if (isStatic && strcmp(signature, "getMouseDown(_)") == 0) {
				return wrenInputGetMouseDown;
			}
			else if (isStatic && strcmp(signature, "getMouseUp(_)") == 0) {
				return wrenInputGetMouseUp;
			}
			else if (isStatic && strcmp(signature, "getMouseX()") == 0) {
				return wrenInputGetMouseX;
			}
			else if (isStatic && strcmp(signature, "getMouseY()") == 0) {
				return wrenInputGetMouseY;
			}
		}
		else if (strcmp(className, "ResourceManager") == 0) {
			if (isStatic && strcmp(signature, "loadTexture(_,_)") == 0) {
				return wrenLoadTexture;
			}
			else if (isStatic && strcmp(signature, "loadAudio(_,_)") == 0) {
				return wrenLoadAudio;
			}
		}
		else if (strcmp(className, "Math") == 0) {
			if (isStatic && strcmp(signature, "lerp(_,_,_)") == 0) {
				return wrenLerp;
			}
			else if (isStatic && strcmp(signature, "easeinout(_,_,_)") == 0) {
				return wrenEaseInOut;
			}
			else if (isStatic && strcmp(signature, "AABB(_,_,_,_,_,_,_,_)") == 0) {
				return wrenAABB;
			}
			else if (isStatic && strcmp(signature, "rand(_,_)") == 0) {
				return wrenRand;
			}
		}
		else if (strcmp(className, "Options") == 0) {
			if (isStatic && strcmp(signature, "set(_,_,_,_)") == 0) {
				return wrenOptionsSet;
			}
			else if (isStatic && strcmp(signature, "get(_)") == 0) {
				return wrenOptionsGet;
			}
			else if (isStatic && strcmp(signature, "load()") == 0) {
				return wrenOptionsLoad;
			}
		}
	}*/
	// Other modules...
	return wrenDefaultFunc;
}

WrenLoadModuleResult loadModule(WrenVM* vm, const char* name) {
	WrenLoadModuleResult result = { 0 };
	char path[255];
	sprintf(
		path,
		"res://res/scripts/%s.wren",
		name
	);

	Ref<Resource> wren_script = (Ref<Resource>) ResourceLoader::get_singleton()->load(path);

	char* source_code = (char*)malloc(10000);
	sprintf(source_code, "%s\0", wren_script->get_name().utf8().get_data()); // Get Source Code From Resource Name
	result.source = source_code;

	return result;
}

void writeFn(WrenVM* vm, const char* text) {
	Godot::print(text);
}

void errorFn(WrenVM* vm, WrenErrorType errorType,
	const char* module, const int line,
	const char* msg)
{
	char c_message[80];
	switch (errorType)
	{
	case WREN_ERROR_COMPILE:
	{
		sprintf(c_message, "[%s line %d] [Error] %s", module, line, msg);
	} break;
	case WREN_ERROR_STACK_TRACE:
	{
		sprintf(c_message, "[%s line %d] in %s", module, line, msg);
	} break;
	case WREN_ERROR_RUNTIME:
	{
		sprintf(c_message, "[Runtime Error] %s", msg);
	} break;
	}
	
	Godot::print(c_message);
}

void TootlWren::parse_wren_snippet(String snippet) {

	Node* game_manager = get_node_or_null(NodePath("/root/GameManager"));
	// Good practice to check if node is NULL
	// TODO, better error handling
	if (game_manager == NULL) {
		Godot::print("GM Is NULL!");
	}
	global_game_manager = game_manager;
	global_this = this;
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.writeFn = &writeFn;
	config.errorFn = &errorFn;
	config.bindForeignMethodFn = &bindForeignMethod;
	config.loadModuleFn = loadModule;
	WrenVM* vm = wrenNewVM(&config);
	char source[snippet.length()];
	sprintf(
		source,
		"%s",
		snippet.utf8().get_data()
	);
	WrenInterpretResult result = wrenInterpret(vm, "main", source);
	wrenFreeVM(vm);
}
