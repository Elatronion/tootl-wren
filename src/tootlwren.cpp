#include "tootlwren.hpp"

#include <stdio.h>
#include <stdint.h>
#include <Resource.hpp>
#include <ResourceLoader.hpp>
#include <Input.hpp>
#include <Viewport.hpp>

using namespace godot;

TootlWren* global_this;
Node* global_game_manager;
float MINIGAME_WIDTH = 800;
float MINIGAME_HEIGHT = 600;

void TootlWren::_register_methods() {
	register_method("parse_wren_snippet", &TootlWren::parse_wren_snippet);
	register_method("init_wren_minigame", &TootlWren::init_wren_minigame);
	register_method("func_minigame_update", &TootlWren::func_minigame_update);
	
	
	register_signal<TootlWren>((char *)"load_scene", "node", GODOT_VARIANT_TYPE_OBJECT, "scene_to_load", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_dialogue", "node", GODOT_VARIANT_TYPE_OBJECT, "dialogue_to_load", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_sfx", "node", GODOT_VARIANT_TYPE_OBJECT, "sfx_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_bgm", "node", GODOT_VARIANT_TYPE_OBJECT, "bgm_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"play_voice", "node", GODOT_VARIANT_TYPE_OBJECT, "voice_to_play", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_imv", "node", GODOT_VARIANT_TYPE_OBJECT, "imv_to_load", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_minigame", "node", GODOT_VARIANT_TYPE_OBJECT, "minigame_to_load", GODOT_VARIANT_TYPE_STRING, "can_exit", GODOT_VARIANT_TYPE_BOOL);
	register_signal<TootlWren>((char *)"render", "node", GODOT_VARIANT_TYPE_OBJECT, "sprite", GODOT_VARIANT_TYPE_STRING, "x", GODOT_VARIANT_TYPE_REAL, "y", GODOT_VARIANT_TYPE_REAL, "w", GODOT_VARIANT_TYPE_REAL, "h", GODOT_VARIANT_TYPE_REAL, "r", GODOT_VARIANT_TYPE_REAL);
	register_signal<TootlWren>((char *)"render_text", "node", GODOT_VARIANT_TYPE_OBJECT, "string", GODOT_VARIANT_TYPE_STRING, "x", GODOT_VARIANT_TYPE_REAL, "y", GODOT_VARIANT_TYPE_REAL, "s", GODOT_VARIANT_TYPE_REAL);
	register_signal<TootlWren>((char *)"load_texture", "node", GODOT_VARIANT_TYPE_OBJECT, "sprite_path", GODOT_VARIANT_TYPE_STRING, "sprite_name", GODOT_VARIANT_TYPE_STRING);
	register_signal<TootlWren>((char *)"load_audio", "node", GODOT_VARIANT_TYPE_OBJECT, "audio_path", GODOT_VARIANT_TYPE_STRING, "audio_name", GODOT_VARIANT_TYPE_STRING);
}

TootlWren::TootlWren() {}
TootlWren::~TootlWren()
{
	Godot::print("Goodbye!");
	if(wren_vm) wrenFreeVM(wren_vm);
}

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

void wrenMinigameLoad(WrenVM* vm) {
	const char* path_str = wrenGetSlotString(vm, 1);
	bool can_exit = wrenGetSlotBool(vm, 2);
	global_this->emit_signal("load_minigame", String(path_str), can_exit);
}

// Start Random //
int max(int a, int b) { return (a > b) ? a : b; }

int nLehmer = 0;
uint32_t Lehmer32()
{
	nLehmer += 0xe120fc15;
	uint64_t tmp;
	tmp = (uint64_t)nLehmer * 0x4a39b70d;
	uint32_t m1 = (tmp >> 32) ^ tmp;
	tmp = (uint64_t)m1 * 0x12fad5c9;
	uint32_t m2 = (tmp >> 32) ^ tmp;
	return m2;
}

int randomInt(int min, int max)
{
	return (Lehmer32() % (max - min)) + min;
}

double randomDouble(double min, double max)
{
	return ((double)Lehmer32() / (double)(0x7FFFFFFF)) * (max - min) + max;
}

void randomSeed(int seed) {
	nLehmer = seed;
}
// End Random //

// Start Interpolation
float mathFunc_lerp(float a, float b, float f) {
	return a + f * (b - a);
}

float mathFunc_easeinout(float a, float b, float f) {
	float diff = b - a;
	return (pow(f, 2) / (pow(f, 2) + pow(1 - f, 2))) * diff + a;
}
// End Interpolation

void wrenLerp(WrenVM* vm) {
	float a = wrenGetSlotDouble(vm, 1);
	float b = wrenGetSlotDouble(vm, 2);
	float f = wrenGetSlotDouble(vm, 3);
	wrenSetSlotDouble(vm, 0, mathFunc_lerp(a, b, f));
}

void wrenEaseInOut(WrenVM* vm) {
	float a = wrenGetSlotDouble(vm, 1);
	float b = wrenGetSlotDouble(vm, 2);
	float f = wrenGetSlotDouble(vm, 3);
	wrenSetSlotDouble(vm, 0, mathFunc_easeinout(a, b, f));
}

void wrenAABB(WrenVM* vm) {
	float Ax = wrenGetSlotDouble(vm, 1);
	float Ay = wrenGetSlotDouble(vm, 2);
	float Aw = wrenGetSlotDouble(vm, 3);
	float Ah = wrenGetSlotDouble(vm, 4);
	Ax = Ax - Aw/2;
	Ay = Ay - Ah/2;
	float AX = Ax + Aw;
	float AY = Ay + Ah;

	float Bx = wrenGetSlotDouble(vm, 5);
	float By = wrenGetSlotDouble(vm, 6);
	float Bw = wrenGetSlotDouble(vm, 7);
	float Bh = wrenGetSlotDouble(vm, 8);
	Bx = Bx - Bw/2;
	By = By - Bh/2;
	float BX = Bx + Bw;
	float BY = By + Bh;

	bool AABB = !(AX < Bx || BX < Ax || AY < By || BY < Ay);

	wrenSetSlotBool(vm, 0, AABB);
}

void wrenRand(WrenVM* vm) {
	int from = (int)(wrenGetSlotDouble(vm, 1));
	int to = (int)(wrenGetSlotDouble(vm, 2));

	int rand_value = randomInt(from, to);
	wrenSetSlotDouble(vm, 0, rand_value);
}

void wrenInputGetKey(WrenVM* vm) {
	int key_code = wrenGetSlotDouble(vm, 1);
	bool is_key_down = global_game_manager->call("GetKey", key_code);
	wrenSetSlotBool(vm, 0, is_key_down);
}

void wrenInputGetKeyDown(WrenVM* vm) {
	int key_code = wrenGetSlotDouble(vm, 1);
	bool is_key_just_down = global_game_manager->call("GetKeyDown", key_code);
	wrenSetSlotBool(vm, 0, is_key_just_down);
}

void wrenInputGetKeyUp(WrenVM* vm) {
	int key_code = wrenGetSlotDouble(vm, 1);
	bool is_key_just_up = global_game_manager->call("GetKeyUp", key_code);
	wrenSetSlotBool(vm, 0, is_key_just_up);
}

void wrenInputGetMouse(WrenVM* vm) {
	int mouse_button = wrenGetSlotDouble(vm, 1);
	bool is_mouse_down = global_game_manager->call("GetMouse", mouse_button);
	wrenSetSlotBool(vm, 0, is_mouse_down);
}

void wrenInputGetMouseDown(WrenVM* vm) {
	int mouse_button = wrenGetSlotDouble(vm, 1);
	bool is_mouse_just_down = global_game_manager->call("GetMouseDown", mouse_button);
	wrenSetSlotBool(vm, 0, is_mouse_just_down);
}

void wrenInputGetMouseUp(WrenVM* vm) {
	int mouse_button = wrenGetSlotDouble(vm, 1);
	bool is_mouse_just_up = global_game_manager->call("GetMouseUp", mouse_button);
	wrenSetSlotBool(vm, 0, is_mouse_just_up);
}

void wrenRender(WrenVM* vm) {
	const char* sprite_name = wrenGetSlotString(vm, 1);
	float x = wrenGetSlotDouble(vm, 2);
	float y = wrenGetSlotDouble(vm, 3);
	float w = wrenGetSlotDouble(vm, 4);
	float h = wrenGetSlotDouble(vm, 5);
	float r = wrenGetSlotDouble(vm, 6);
	global_this->emit_signal("render", String(sprite_name), x, y, w, h, r);
}

void wrenRenderText(WrenVM* vm) {
	const char* string = wrenGetSlotString(vm, 1);
	float x = wrenGetSlotDouble(vm, 2);
	float y = wrenGetSlotDouble(vm, 3);
	float s = wrenGetSlotDouble(vm, 4);
	global_this->emit_signal("render_text", String(string), x, y, s);
}

void wrenLoadTexture(WrenVM* vm) {
	const char* path_str = wrenGetSlotString(vm, 1);
	const char* name_str = wrenGetSlotString(vm, 2);
	global_this->emit_signal("load_texture", String(path_str), String(name_str));
}

void wrenLoadAudio(WrenVM* vm) {
	const char* path_str = wrenGetSlotString(vm, 1);
	const char* name_str = wrenGetSlotString(vm, 2);
	global_this->emit_signal("load_audio", String(path_str), String(name_str));
}

void wrenWindowWidth(WrenVM* vm) {
	float width = MINIGAME_WIDTH;
	wrenSetSlotDouble(vm, 0, width);
}

void wrenWindowHeight(WrenVM* vm) {
	float height = MINIGAME_HEIGHT;
	wrenSetSlotDouble(vm, 0, height);
}

void wrenInputGetMouseX(WrenVM* vm) {
	float mouse_x = global_game_manager->call("GetMouseX");
	wrenSetSlotDouble(vm, 0, mouse_x);
}

void wrenInputGetMouseY(WrenVM* vm) {
	float mouse_y = global_game_manager->call("GetMouseY");
	wrenSetSlotDouble(vm, 0, mouse_y);
}

void wrenGameLoad(WrenVM* vm) {
	global_game_manager->call("LoadGame");
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
		else if (strcmp(className, "Minigame") == 0) {
			if (isStatic && strcmp(signature, "load(_,_)") == 0)
			{
				return wrenMinigameLoad;
			}
		}
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
		}*/
		else if (strcmp(className, "GameSaver") == 0) {
			/*if (isStatic && strcmp(signature, "save()") == 0) {
				return wrenGameSave;
			}
			else */if (isStatic && strcmp(signature, "load()") == 0) {
				return wrenGameLoad;
			}
		}
		// Other classes in televoid-core...
	}
	else if (strcmp(module, "minigame-core") == 0) {
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
		/*else if (strcmp(className, "Options") == 0) {
			if (isStatic && strcmp(signature, "set(_,_,_,_)") == 0) {
				return wrenOptionsSet;
			}
			else if (isStatic && strcmp(signature, "get(_)") == 0) {
				return wrenOptionsGet;
			}
			else if (isStatic && strcmp(signature, "load()") == 0) {
				return wrenOptionsLoad;
			}
		}*/
	}
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

void TootlWren::init_wren_minigame(String script_source) {
	Node* game_manager = get_node_or_null(NodePath("/root/GameManager"));
	if (game_manager == NULL) {
		Godot::print("Minigame GM Is NULL!");
	}
	global_game_manager = game_manager;
	global_this = this;
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.writeFn = &writeFn;
	config.errorFn = &errorFn;
	config.bindForeignMethodFn = &bindForeignMethod;
	config.loadModuleFn = loadModule;
	wren_vm = wrenNewVM(&config);
	char source[script_source.length()];
	sprintf(
		source,
		"%s",
		script_source.utf8().get_data()
	);
	WrenInterpretResult result = wrenInterpret(wren_vm, "main", source);
}

void TootlWren::func_minigame_update(float delta) {
	global_this = this;
	MINIGAME_WIDTH  = get_viewport()->get_size().x;
	MINIGAME_HEIGHT = get_viewport()->get_size().y;

	// Load the class into slot 0.
	wrenEnsureSlots(wren_vm, 2);
	wrenGetVariable(wren_vm, "main", "game", 0);
	WrenHandle* game_variable = wrenGetSlotHandle(wren_vm, 0);
	wrenSetSlotHandle(wren_vm, 0, game_variable);
	wrenSetSlotDouble(wren_vm, 1, delta);

	WrenHandle* game_update_method = wrenMakeCallHandle(wren_vm, "update(_)");

	wrenCall(wren_vm, game_update_method);

	wrenSetSlotHandle(wren_vm, 0, game_variable);
	WrenHandle* game_rendering_method = wrenMakeCallHandle(wren_vm, "render()");
	wrenCall(wren_vm, game_rendering_method);
}
