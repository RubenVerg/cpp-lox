#include "value.h"
#include "object.h"

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