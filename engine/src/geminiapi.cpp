#include "../include/geminiapi.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

GeminiClientWrapper GeminiClientWrapper::CreateFromModule(
        const std::string& module_dir,
        const std::string& module_name,
        const std::string& class_name,
        const std::string& model,
        const std::string& api_key_env_var)
{
    py::gil_scoped_acquire gil;

    py::module_ sys = py::module_::import("sys");
    // Prepend your api directory so requester.py can be found
    sys.attr("path").attr("insert")(0, module_dir);

    py::module_ mod = py::module_::import(module_name.c_str());
    py::object cls = mod.attr(class_name.c_str());

    py::object obj;
    if (!model.empty() && !api_key_env_var.empty()) {
        obj = cls(model, api_key_env_var);
    } else if (!model.empty()) {
        obj = cls(model);
    } else {
        obj = cls();
    }

    return GeminiClientWrapper(obj);
}

std::string GeminiClientWrapper::generate_text(const std::string& input)
{
    std::lock_guard<std::mutex> lock(mu);
    py::gil_scoped_acquire gil;

    try {
        py::object out = instance.attr("infer_text")(input);
        return out.cast<std::string>();
    } catch (const py::error_already_set& e) {
        throw std::runtime_error(std::string("Python error in generate_text: ") + e.what());
    }
}

std::string GeminiClientWrapper::generate_json_string(const std::string& input)
{
    std::lock_guard<std::mutex> lock(mu);
    py::gil_scoped_acquire gil;

    try {
        py::object out = instance.attr("infer_json")(input); // expected dict-like
        py::module_ json = py::module_::import("json");
        py::object s = json.attr("dumps")(out);
        return s.cast<std::string>();
    } catch (const py::error_already_set& e) {
        throw std::runtime_error(std::string("Python error in generate_json_string: ") + e.what());
    }
}

void GeminiClientWrapper::reset(py::object new_instance)
{
    std::lock_guard<std::mutex> lock(mu);
    py::gil_scoped_acquire gil;
    instance = std::move(new_instance);
}