#ifndef TOOTLWREN_H
#define TOOTLWREN_H

#include <Godot.hpp>
#include <Node.hpp>
#include "wren.h"

namespace godot {

class TootlWren : public Node {
	GODOT_CLASS(TootlWren, Node)

private:
	WrenVM* wren_vm = NULL;

public:
	static void _register_methods();

	TootlWren();
	~TootlWren();
	
	void _init();

	void parse_wren_snippet(String str);
	void init_wren_minigame(String script_source);
	void func_minigame_update(float delta);
};

}

#endif
