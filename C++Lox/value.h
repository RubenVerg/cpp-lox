#pragma once

#include "common.h"

enum class ValueType : size_t {
	Bool,
	Number,
	Nil,
};

struct Value {
	private:
	union {
		bool _bool;
		double _number;
	} contents{ 0 };

	public:
	ValueType type;

	Value() : type{ ValueType::Nil } {}
	Value(bool boolean) : type{ ValueType::Bool } { contents._bool = boolean; }
	Value(double number) : type{ ValueType::Number } { contents._number = number; }

		bool isBool() {
		return type == ValueType::Bool;
	}

	bool asBoolUnsafe() {
		return contents._bool;
	}

	std::optional<bool> asBool() {
		if (isBool()) return std::make_optional(asBoolUnsafe());
		return std::nullopt;
	}

	bool isNumber() {
		return type == ValueType::Number;
	}

	double asNumberUnsafe() {
		return contents._number;
	}

	std::optional<double> asNumber() {
		if (isNumber()) return std::make_optional(asNumberUnsafe());
		return std::nullopt;
	}

	bool isNil() {
		return type == ValueType::Nil;
	}
};

using ValueArray = std::vector<Value>;

void printValue(Value value);