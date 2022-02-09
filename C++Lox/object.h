#pragma once

#include "common.h"
#include "value.h"

enum class ObjType {
	String,
};

struct ObjString;

struct Obj {
	ObjType type;

	bool isString() {
		return type == ObjType::String;
	}

	virtual std::string asStringUnsafe() {
		throw std::runtime_error("Called asStringUnsafe on a non-string Obj");
	}
	std::optional<std::string> asString() {
		if (isString()) return asStringUnsafe();
		return std::nullopt;
	}

	std::string stringify();

	Obj(ObjType t) : type{ t } {}
};

bool operator==(Obj& a, Obj& b);

struct ObjString : Obj {
	std::string str;

	std::string asStringUnsafe() override {
		return str;
	}

	ObjString(std::string string) : Obj{ ObjType::String }, str{ string } {}
};

inline std::string Obj::stringify() {
	switch (type) {
		case ObjType::String:
			return asStringUnsafe();
		default:
			assert(false, "Cannot stringify unknown object type");
			return "";
	}
}