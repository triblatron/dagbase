#include "config/config.h"

#include "core/Editable.h"
#include "util/EnumValue.h"
#include "core/TypeRegistry.h"

#include "imgui.h"

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

    void Editable::editType(const char *label, EnumValue *value)
    {
        if (value)
        {
            if (ImGui::BeginCombo(label, value->selectedString().c_str(), 0))
            {
                auto type = value->type();

                if (type)
                {
                    auto enumValue = type->minValue;
                    int n=0;
                    while (enumValue != type->unknownValue)
                    {
                        const bool isSelected = (value->selectedIndex() == n);
                        if (ImGui::Selectable(type->toString(enumValue).c_str(), isSelected))
                            value->selectedIndex() = n;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                        enumValue = type->nextValue(enumValue);
                        ++n;
                    }
                }
                ImGui::EndCombo();
            }

        }
    }
}
