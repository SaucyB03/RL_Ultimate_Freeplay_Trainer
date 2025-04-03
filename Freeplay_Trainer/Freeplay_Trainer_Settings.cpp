#include "pch.h"
#include "Freeplay_Trainer.h"

#include <string>

void Freeplay_Trainer::RenderSettings() {
    static int item_current_1 = 0;
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    if (ImGui::BeginTabBar("Types of Customization", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Individual Shot Editor"))
        {
            ImGui::Text("Choose Any Shot Preset To Customize To YOUR Exact Liking!");

            ImGui::Combo("Select Shot Present To Customize", &item_current_1, "Car\0Goal\0Center\0\0");

            ImGui::Separator();
            ImGui::Text("Name of Preset:");

            static char str1[128] = "";
            ImGui::InputTextWithHint("##textinput", "Shot1", str1, IM_ARRAYSIZE(str1));

            ImGui::Separator();
            ImGui::Text("Initial Position:");

            static float vec3a[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::DragFloat3("Initial Position", vec3a, 1);

            static int item_current_2 = 0;
            ImGui::Combo("Position Relative to What?", &item_current_2, "Car\0Goal\0Center\0\0");

            ImGui::Separator();
            ImGui::Text("Initial Velocity:");

            static float sp = 1.0f;
            ImGui::DragFloat("Speed", &sp, 0.5f, 0.0f, 250.0f);

            static float veca[3] = { 1.0f, 1.0f, 1.0f };
            ImGui::DragFloat3("Direction", veca, 1);

            static bool ind = false;
            ImGui::Checkbox("Show Indicator", &ind);

            ImGui::Separator();
            ImGui::Text("Variance:");

            static bool dir_ind = false;
            ImGui::Checkbox("Directional Variance", &dir_ind);

            if (dir_ind) {
                static float sp = 1.0f;
                ImGui::DragFloat("Variance", &sp, 0.5f, 0.0f, 100.0f);
            }

            static bool pos_ind = false;
            ImGui::Checkbox("Positional Variance", &pos_ind);

            if (pos_ind) {
                static float sp = 1.0f;
                ImGui::DragFloat("Variance", &sp, 0.5f, 0.0f, 100.0f);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Group Preset Customizer"))
        {

            ImGui::Text("Customize Up To 4 Unique D-Pad/1234 Presets!");
            ImGui::Separator();

            static char preset1_name[128] = "Passing";
            static char tmp[128] = "";
            static char pres[9] = "Preset: ";
            static char out_text[140] = "";
            strcpy(out_text, pres);
            strcat(out_text, preset1_name);
            ImGui::Text(out_text);
            //ImGui::Text(tmp);

            if (ImGui::InputTextWithHint("##textinput", preset1_name, tmp, IM_ARRAYSIZE(tmp), ImGuiInputTextFlags_EnterReturnsTrue)) {
                strcpy(preset1_name, tmp);
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);
            static int arr[4];
            ImGui::Combo("Left/1", &arr[0], "Car\0Goal\0Center\0\0"); ImGui::SameLine();
            ImGui::Combo("Right/2", &arr[1], "Car\0Goal\0Center\0\0"); ImGui::SameLine();
            ImGui::Combo("Up/3", &arr[2], "Car\0Goal\0Center\0\0"); ImGui::SameLine();
            ImGui::Combo("Left/4", &arr[3], "Car\0Goal\0Center\0\0");

            ImGui::PopItemWidth();


            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}



