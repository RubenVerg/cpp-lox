#include "value.h"
#include "object.h"

bool Value::isBool() {
	return type == ValueType::Bool;
}

bool Value::asBoolUnsafe() {
	return std::get<bool>(contents);
}

std::optional<bool> Value::asBool() {
	if (isBool()) return std::make_optional(asBoolUnsafe());
	return std::nullopt;
}

bool Value::isNumber() {
	return type == ValueType::Number;
}

double Value::asNumberUnsafe() {
	return std::get<double>(contents);
}

std::optional<double> Value::asNumber() {
	if (isNumber()) return std::make_optional(asNumberUnsafe());
	return std::nullopt;
}

bool Value::isObj() {
	return type == ValueType::Obj;
}

std::shared_ptr<Obj> Value::asObjUnsafe() {
	return std::get<std::shared_ptr<Obj>>(contents);
}

std::optional<std::shared_ptr<Obj>> Value::asObj() {
	if (isObj()) return std::make_optional(asObjUnsafe());
	return std::nullopt;
}

bool Value::isNil() {
	return type == ValueType::Nil;
}

bool Value::castToBool() {
	switch (type) {
		case ValueType::Nil:
			return false;
		case ValueType::Number:
			return true;
		case ValueType::Bool:
			return asBoolUnsafe();
		case ValueType::Obj:
			return true;
		default:
			unreachable();
			return false;
	}
}

std::string Value::stringify() {
	switch (type) {
		case ValueType::Bool:
			return asBoolUnsafe() ? "true" : "false";
		case ValueType::Nil:
			return "nil";
		case ValueType::Number:
			return read_cast<std::string>(asNumberUnsafe());
		case ValueType::Obj:
			return asObjUnsafe()->stringify();
		default:
			unreachable();
			return "";
	}
}

void Value::print() {
	std::cout << stringify();
}

bool operator==(Value a, Value b) {
	if (a.type != b.type) return false;
	switch (a.type) {
		case ValueType::Bool:
			return a.asBoolUnsafe() == b.asBoolUnsafe();
		case ValueType::Number:
			return a.asNumberUnsafe() == b.asNumberUnsafe();
		case ValueType::Nil:
			return true;
		case ValueType::Obj:
			return *a.asObjUnsafe() == *b.asObjUnsafe();
		default:
			unreachable();
			return false;
	}
}

bool operator==(Obj& a, Obj& b) {
	if (a.type != b.type) return false;
	switch (a.type) {
		case ObjType::String:
			return a.asStringUnsafe() == b.asStringUnsafe();
		default:
			unreachable();
			return false;
	}
}