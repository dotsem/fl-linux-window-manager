#include <iostream>

#include <message_handler/method_call_arg_utils.h>

int FLWM::MethodCallArgUtils::getInt(FlMethodCall* method_call, const char* argument_name) {
    if (!hasArgument(method_call, argument_name, FL_VALUE_TYPE_INT)) {
        return 0;
    }

    FlValue* argsMap = fl_method_call_get_args(method_call);
    FlValue* argument = fl_value_lookup_string(argsMap, argument_name);

    return fl_value_get_int(argument);
}

const char* FLWM::MethodCallArgUtils::getString(FlMethodCall* method_call, const char* argument_name) {
    if (!hasArgument(method_call, argument_name, FL_VALUE_TYPE_STRING)) {
        return nullptr;
    }

    FlValue* argsMap = fl_method_call_get_args(method_call);
    FlValue* argument = fl_value_lookup_string(argsMap, argument_name);

    return fl_value_get_string(argument);
}

double FLWM::MethodCallArgUtils::getDouble(FlMethodCall* method_call, const char* argument_name) {
    if (!hasArgument(method_call, argument_name, FL_VALUE_TYPE_FLOAT)) {
        return 0.0;
    }

    FlValue* argsMap = fl_method_call_get_args(method_call);
    FlValue* argument = fl_value_lookup_string(argsMap, argument_name);

    return fl_value_get_float(argument);
}

bool FLWM::MethodCallArgUtils::getBool(FlMethodCall* method_call, const char* argument_name) {
    if (!hasArgument(method_call, argument_name, FL_VALUE_TYPE_BOOL)) {
        return false;
    }

    FlValue* argsMap = fl_method_call_get_args(method_call);
    FlValue* argument = fl_value_lookup_string(argsMap, argument_name);

    return fl_value_get_bool(argument);
}

bool FLWM::MethodCallArgUtils::hasArgument(FlMethodCall* method_call,
    const char* argument_name, FlValueType argument_type) {
    /// Get the arguments map from the method call.
    FlValue* argsMap = fl_method_call_get_args(method_call);

    /// Check if the arguemnts map is empty.
    if (argsMap == nullptr || fl_value_get_type(argsMap) != FL_VALUE_TYPE_MAP) {
        std::cout << "Arguments map is empty. Cannot get " << argument_name << " arguement." << std::endl;

        return false;
    }

    /// Check if the argument is present in the arguments map.
    FlValue* value = fl_value_lookup_string(argsMap, argument_name);

    if (value == nullptr) {
        std::cout << "Argument with name '" << argument_name
            << "' is not found in the method args map." << std::endl;

        return false;
    }

    /// Check if the argument is of the correct type.
    if (fl_value_get_type(value) != argument_type) {
        std::cout << "Argument with name '" << argument_name
            << "' is not of the correct type." << std::endl;

        return false;
    }

    return true;
}