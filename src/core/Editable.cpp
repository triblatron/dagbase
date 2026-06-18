#include "config/config.h"

#include "core/Editable.h"
#include "imgui/imgui.h"

namespace dagbase
{
    void dagbase::Editable::editType(const char* label, std::int64_t* value)
    {
        ImGui::InputScalar(label, ImGuiDataType_S64, value);
    }

    void dagbase::Editable::editType(const char* label, bool* value)
    {
        ImGui::Checkbox(label, value);
    }

    void dagbase::Editable::editType(const char* label, double* value)
    {
        ImGui::InputScalar(label, ImGuiDataType_Double, value);
    }

    void dagbase::Editable::editType(const char* label, std::string* value)
    {
        // TODO:Use string support
    }
}
