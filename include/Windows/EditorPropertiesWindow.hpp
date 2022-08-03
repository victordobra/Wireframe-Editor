#pragma once

#include "Core.hpp"

namespace wfe::editor {
    void LoadEditorProperties();
    void SaveEditorProperties();

    string GetDefaultTemplatePath();
    void SetDefaultTemplatePath(const string& newPath);
    string GetDefaultProjectPath();
    void SetDefaultProjectPath(const string& newPath);
    vector<string> GetTemplatePaths();
}