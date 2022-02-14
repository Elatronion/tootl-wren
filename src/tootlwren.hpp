#ifndef TOOTLWREN_H
#define TOOTLWREN_H

#include <Godot.hpp>
#include <Node.hpp>

namespace godot {

class TootlWren : public Node {
	GODOT_CLASS(TootlWren, Node)

public:
	static void _register_methods();

	TootlWren();
	~TootlWren();
	
	void _init();

	void parse_wren_snippet(String str);
};

}

#endif
