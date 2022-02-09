#pragma once

#include "common.h"
#include "value.h"

enum class ObjType {
	String,
};

struct Obj {
	ObjType type;

	bool isString();

	virtual std::string asStringUnsafe();
	std::optional<std::string> asString();

	std::string stringify();

	Obj(ObjType t) : type{ t } {}
};

bool operator==(Obj& a, Obj& b);

struct ObjString : Obj {
	std::string str;

	std::string asStringUnsafe() override;

	ObjString(std::string string) : Obj{ ObjType::String }, str{ string } {}
};