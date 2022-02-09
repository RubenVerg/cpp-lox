#pragma once

#include "common.h"

struct Obj;

enum class ValueType : size_t {
	Bool,
	Number,
	Nil,
	Obj,
};

struct Value {
	private:
	std::variant<bool, double, std::shared_ptr<Obj>> contents;

	public:
	ValueType type;

	Value() : type{ ValueType::Nil } {}
	Value(bool boolean) : type{ ValueType::Bool }, contents{ boolean } {}
	Value(double number) : type{ ValueType::Number }, contents{ number }{}
	Value(std::shared_ptr<Obj> ptr) : type{ ValueType::Obj }, contents{ std::move(ptr) }{}

	bool isBool() {
		return type == ValueType::Bool;
	}

	bool asBoolUnsafe() {
		return std::get<bool>(contents);
	}

	std::optional<bool> asBool() {
		if (isBool()) return std::make_optional(asBoolUnsafe());
		return std::nullopt;
	}

	bool isNumber() {
		return type == ValueType::Number;
	}

	double asNumberUnsafe() {
		return std::get<double>(contents);
	}

	std::optional<double> asNumber() {
		if (isNumber()) return std::make_optional(asNumberUnsafe());
		return std::nullopt;
	}

	bool isObj() {
		return type == ValueType::Obj;
	}

	std::shared_ptr<Obj> asObjUnsafe() {
		return std::get<std::shared_ptr<Obj>>(contents);
	}

	std::optional<std::shared_ptr<Obj>> asObj() {
		if (isObj()) return std::make_optional(asObjUnsafe());
		return std::nullopt;
	}

	bool isNil() {
		return type == ValueType::Nil;
	}

	bool castToBool();

	std::string stringify();
	void print() {
		std::cout << stringify();
	}
};

bool operator==(Value a, Value b);