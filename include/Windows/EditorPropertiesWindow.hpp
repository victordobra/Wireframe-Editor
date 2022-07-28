#pragma once

#include "Core.hpp"

namespace wfe::editor {
    string GetDefaultTemplatePath();
    void SetDefaultTemplatePath(const string& newPath);
    string GetDefaultProjectPath();
    void SetDefaultProjectPath(const string& newPath);
}