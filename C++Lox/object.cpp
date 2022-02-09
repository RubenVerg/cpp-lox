#include "object.h"

bool Obj::isString() {
	return type == ObjType::String;
}

std::string Obj::asStringUnsafe() {
	throw std::runtime_error("Called asStringUnsafe on a non-string Obj");
}

std::optional<std::string> Obj::asString() {
	if (isString()) return asStringUnsafe();
	return std::nullopt;
}

std::string Obj::stringify() {
	switch (type) {
		case ObjType::String:
			return asStringUnsafe();
		default:
			assert(false, "Cannot stringify unknown object type");
			return "";
	}
}

std::string ObjString::asStringUnsafe() {
	return str;
}
