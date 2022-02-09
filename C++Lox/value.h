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

	bool isBool();
	bool asBoolUnsafe();
	std::optional<bool> asBool();

	bool isNumber();
	double asNumberUnsafe();
	std::optional<double> asNumber();

	bool isObj();
	std::shared_ptr<Obj> asObjUnsafe();
	std::optional<std::shared_ptr<Obj>> asObj();

	bool isNil();

	bool castToBool();

	std::string stringify();
	void print();
};

bool operator==(Value a, Value b);