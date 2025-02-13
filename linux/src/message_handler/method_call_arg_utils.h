#pragma once

#include <flutter_linux/flutter_linux.h>

#include <vector>
#include <string>

namespace FLWM {
    /**
     * This is a utility class used to get the arguments from the method call from
     * Flutter or dart code.
     *
     * Here we are assuming that the arguments are passed as a map from the dart code.
     * So we are using the [fl_value_lookup_string] method to get the arguments from the
     * method call.
     */
    class MethodCallArgUtils {
    public:
        /**
         * @brief Get the integer argument from the method call.
         *
         * @param methodCall  The method call from which the argument needs to be extracted.
         * @param argumentName  The name of the argument that needs to be extracted.
         *
         * return int  The integer argument extracted from the method call.
         */
        static int getInt(FlMethodCall* methodCall, const char* argumentName);

        /**
         * @brief Get the string argument from the method call.
         *
         * @param methodCall  The method call from which the argument needs to be extracted.
         * @param argumentName  The name of the argument that needs to be extracted.
         *
         * return const char*  The string argument extracted from the method call.
         */
        static const char* getString(FlMethodCall* methodCall, const char* argumentName);

        /**
         * @brief Get the double argument from the method call.
         *
         * @param methodCall  The method call from which the argument needs to be extracted.
         * @param argumentName  The name of the argument that needs to be extracted.
         *
         * return double  The double argument extracted from the method call.
         */
        static double getDouble(FlMethodCall* methodCall, const char* argumentName);

        /**
         * @brief Get the boolean argument from the method call.
         *
         * @param methodCall  The method call from which the argument needs to be extracted.
         * @param argumentName  The name of the argument that needs to be extracted.
         *
         * return bool  The boolean argument extracted from the method call.
         */
        static bool getBool(FlMethodCall* methodCall, const char* argumentName);

        /**
         * @brief Get the list of strings from the method call.
         *
         * @param methodCall  The method call from which the argument needs to be extracted.
         * @param argumentName  The name of the argument that needs to be extracted.
         *
         * return std::vector<std::string>  The list of strings extracted from the method call.
         */
        static std::vector<std::string> getStringList(FlMethodCall* methodCall, const char* argumentName);

    private:
        /**
         * @brief Check if the given argument is present in the method call.
         *
         * @param methodCall  The method call from which the argument needs to be checked.
         * @param argumentName  The name of the argument that needs to be checked.
         * @param argumentType  The type of the argument that needs to be checked.
         *
         * return bool  True if the argument is present in the method call, false otherwise.
         */
        static bool hasArgument(FlMethodCall* methodCall, const char* argumentName, FlValueType argumentType);
    };
}