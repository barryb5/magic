#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

namespace py = pybind11;
class GeminiClientWrapper
{
private:
    py::object instance;
    std::mutex mu;
public:
    GeminiClientWrapper(py::object gemini_instance) : instance(gemini_instance) {
        std::cout << "Gemini instance initialized." << std::endl;
    }

    static GeminiClientWrapper CreateFromModule(
        const std::string& module_dir,
        const std::string& module_name,
        const std::string& class_name,
        const std::string& model = "",
        const std::string& api_key_env_var = "");
    std::string generate_text(const std::string& input);
    std::string generate_json_string(const std::string& input);
    void reset(py::object new_instance);
};