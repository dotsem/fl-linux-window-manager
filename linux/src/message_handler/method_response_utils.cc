#include <message_handler/method_response_utils.h>

FlMethodResponse *FLWM::MethodResponseUtils::methodNotImplementedError()
{
    return FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
}

FlMethodResponse *FLWM::MethodResponseUtils::successResponse()
{
    g_autoptr(FlValue) result = fl_value_new_null();

    return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *FLWM::MethodResponseUtils::successResponse(int value)
{
    g_autoptr(FlValue) result = fl_value_new_int(value);

    return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *FLWM::MethodResponseUtils::successResponse(const char *value)
{
    g_autoptr(FlValue) result = fl_value_new_string(value);

    return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *FLWM::MethodResponseUtils::successResponse(double value)
{
    g_autoptr(FlValue) result = fl_value_new_float(value);

    return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *FLWM::MethodResponseUtils::successResponse(bool value)
{
    g_autoptr(FlValue) result = fl_value_new_bool(value);

    return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}
FlMethodResponse *FLWM::MethodResponseUtils::successResponse(FlValue *value)
{
    // fl_value_ref(value) if 'value' might be unref'd elsewhere before response is fully processed.
    // If 'value' is freshly created and only used here, direct use is fine.
    return FL_METHOD_RESPONSE(fl_method_success_response_new(value));
}