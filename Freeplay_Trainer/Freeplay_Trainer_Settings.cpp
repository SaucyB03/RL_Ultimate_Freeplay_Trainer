#include "pch.h"
#include "Freeplay_Trainer.h"

void Freeplay_Trainer::RenderSettings() {
    CVarWrapper enabled = cvarManager->getCvar("enable_plugin");
    bool enable_val = enabled.getBoolValue();
    if (ImGui::Checkbox("Enable Plugin", &enable_val)) {
        enabled.setValue(enable_val);
    }

    if (!enable_val) {
        return;
    }


    //Error Checking:
    CVarWrapper chosen = cvarManager->getCvar("preset_choice");
    if (!chosen) { return; }
    if (errorCheck()) { return; };

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    if (ImGui::BeginTabBar("Types of Customization", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Group Preset Customizer"))
        {
            //////////////////// Preset 1 ////////////////
            static string pres = "Preset: ";
            static bool edited[4] = { false,false,false,false };
            static string key_names[4] = { "Left/1", "Right/2", "Up/3", "Down/4" };


            ImGui::Text("Customize Up To 4 Unique D-Pad/1234 Presets!");
            ImGui::Separator();


            //Preset 1:
            const string preset1_name = groupNames.at(0);
            static char p1_buf[128];
            const string p1_out_name = pres + preset1_name;
            ImGui::Text(p1_out_name.c_str());
            ImGui::InputTextWithHint("##groupNP1", preset1_name.c_str(), p1_buf, IM_ARRAYSIZE(p1_buf));

            if (p1_buf[0] != '\0') {
                edited[0] = true;
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);

            //Preset #1 Options:
            static vector<int> p1_binds = groupIndices.at(0);
            static int shot1_binds[NUM_KEYB] = { p1_binds[0], p1_binds[1], p1_binds[2], p1_binds[3] };

            //Iterate through the 4 combo options
            static string key_names_p1;
            for (int keyB = 0; keyB < NUM_KEYB; keyB++) {
                key_names_p1 = key_names[keyB] + "##preset1";
                if (ImGui::BeginCombo(key_names_p1.c_str(), names.at(shot1_binds[keyB] - 1).c_str()))
                {
                    for (int n = 0; n < names.size(); n++)
                    {
                        //Update selected values
                        const bool is_selected = (shot1_binds[keyB] == n + 1);
                        if (ImGui::Selectable(names.at(n).c_str(), is_selected)) {
                            shot1_binds[keyB] = n + 1;
                            edited[0] = true;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if (keyB < NUM_KEYB - 1) {
                    ImGui::SameLine();
                }
            }


            ImGui::PopItemWidth();

            if (!edited[0]) {
                bool select_p1 = (chosen.getIntValue() == 0 && chosen.getIntValue() != 1 && chosen.getIntValue() != 2 && chosen.getIntValue() != 3);
                if (ImGui::Checkbox("Select Preset 1", &select_p1)) {
                    chosen.setValue(0);
                }
            }
            else {
                ImGui::Text("Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 1")) {
                if (p1_buf[0] != '\0') {
                    groupNames.at(0) = p1_buf;
                    std::memset(p1_buf, '\0', sizeof(p1_buf));
                }
                groupIndices.at(0) = { shot1_binds[0], shot1_binds[1], shot1_binds[2], shot1_binds[3] };

                savePresets(Groups_Only);
                LOG("Preset Successfully Saved!");

                edited[0] = false;

            }


            ImGui::Separator();

            //////////////////// Preset 2 ////////////////

            const string preset2_name = groupNames.at(1);
            static char p2_buf[128];
            const string p2_out_name = pres + preset2_name;
            ImGui::Text(p2_out_name.c_str());
            ImGui::InputTextWithHint("##groupNP2", preset2_name.c_str(), p2_buf, IM_ARRAYSIZE(p2_buf));

            if (p2_buf[0] != '\0') {
                edited[1] = true;
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);

            //Preset #2 Options:
            static vector<int> p2_binds = groupIndices.at(1);
            static int shot2_binds[NUM_KEYB] = { p2_binds[0], p2_binds[1], p2_binds[2], p2_binds[3] };

            //Iterate through the 4 combo options
            static string key_names_p2;
            for (int keyB = 0; keyB < NUM_KEYB; keyB++) {
                key_names_p2 = key_names[keyB] + "##preset2";
                if (ImGui::BeginCombo(key_names_p2.c_str(), names.at(shot2_binds[keyB] - 1).c_str()))
                {
                    for (int n = 0; n < names.size(); n++)
                    {
                        //Update selected values
                        const bool is_selected = (shot2_binds[keyB] == n + 1);
                        if (ImGui::Selectable(names.at(n).c_str(), is_selected)) {
                            shot2_binds[keyB] = n + 1;
                            edited[1] = true;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if (keyB < NUM_KEYB - 1) {
                    ImGui::SameLine();
                }
            }


            ImGui::PopItemWidth();

            if (!edited[1]) {
                bool select_p2 = (chosen.getIntValue() != 0 && chosen.getIntValue() == 1 && chosen.getIntValue() != 2 && chosen.getIntValue() != 3);
                if (ImGui::Checkbox("Select Preset 2", &select_p2)) {
                    chosen.setValue(1);
                }
            }
            else {
                ImGui::Text("Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 2")) {
                if (p2_buf[0] != '\0') {
                    groupNames.at(1) = p2_buf;
                    std::memset(p2_buf, '\0', sizeof(p2_buf));
                }
                groupIndices.at(1) = { shot2_binds[0], shot2_binds[1], shot2_binds[2], shot2_binds[3] };

                savePresets(Groups_Only);
                LOG("Preset Successfully Saved!");

                edited[1] = false;

            }


            ImGui::Separator();

            //////////////////// Preset 3 ////////////////
            const string preset3_name = groupNames.at(2);
            static char p3_buf[128];
            const string p3_out_name = pres + preset3_name;
            ImGui::Text(p3_out_name.c_str());
            ImGui::InputTextWithHint("##groupNP3", preset3_name.c_str(), p3_buf, IM_ARRAYSIZE(p3_buf));

            if (p3_buf[0] != '\0') {
                edited[2] = true;
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);

            //Preset #3 Options:
            static vector<int> p3_binds = groupIndices.at(2);
            static int shot3_binds[NUM_KEYB] = { p3_binds[0], p3_binds[1], p3_binds[2], p3_binds[3] };

            //Iterate through the 4 combo options
            static string key_names_p3;
            for (int keyB = 0; keyB < NUM_KEYB; keyB++) {
                key_names_p3 = key_names[keyB] + "##preset3";
                if (ImGui::BeginCombo(key_names_p3.c_str(), names.at(shot3_binds[keyB] - 1).c_str()))
                {
                    for (int n = 0; n < names.size(); n++)
                    {
                        //Update selected values
                        const bool is_selected = (shot3_binds[keyB] == n + 1);
                        if (ImGui::Selectable(names.at(n).c_str(), is_selected)) {
                            shot3_binds[keyB] = n + 1;
                            edited[2] = true;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if (keyB < NUM_KEYB - 1) {
                    ImGui::SameLine();
                }
            }


            ImGui::PopItemWidth();

            if (!edited[2]) {
                bool select_p3 = (chosen.getIntValue() != 0 && chosen.getIntValue() != 1 && chosen.getIntValue() == 2 && chosen.getIntValue() != 3);
                if (ImGui::Checkbox("Select Preset 3", &select_p3)) {
                    chosen.setValue(2);
                }
            }
            else {
                ImGui::Text("Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 3")) {
                if (p3_buf[0] != '\0') {
                    groupNames.at(2) = p3_buf;
                    std::memset(p3_buf, '\0', sizeof(p3_buf));
                }
                groupIndices.at(2) = { shot3_binds[0], shot3_binds[1], shot3_binds[2], shot3_binds[3] };

                savePresets(Groups_Only);
                LOG("Preset Successfully Saved!");

                edited[2] = false;

            }

            ImGui::Separator();

            //////////////////// Preset 4 ////////////////

            const string preset4_name = groupNames.at(3);
            static char p4_buf[128];
            const string p4_out_name = pres + preset4_name;
            ImGui::Text(p4_out_name.c_str());
            ImGui::InputTextWithHint("##groupNP4", preset4_name.c_str(), p4_buf, IM_ARRAYSIZE(p4_buf));

            if (p4_buf[0] != '\0') {
                edited[3] = true;
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);

            //Preset #4 Options:
            static vector<int> p4_binds = groupIndices.at(3);
            static int shot4_binds[NUM_KEYB] = { p4_binds[0], p4_binds[1], p4_binds[2], p4_binds[3] };

            //Iterate through the 4 combo options
            static string key_names_p4;
            for (int keyB = 0; keyB < NUM_KEYB; keyB++) {
                key_names_p4 = key_names[keyB] + "##preset4";
                if (ImGui::BeginCombo(key_names_p4.c_str(), names.at(shot4_binds[keyB] - 1).c_str()))
                {
                    for (int n = 0; n < names.size(); n++)
                    {
                        //Update selected values
                        const bool is_selected = (shot4_binds[keyB] == n + 1);
                        if (ImGui::Selectable(names.at(n).c_str(), is_selected)) {
                            shot4_binds[keyB] = n + 1;
                            edited[3] = true;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if (keyB < NUM_KEYB - 1) {
                    ImGui::SameLine();
                }
            }


            ImGui::PopItemWidth();

            if (!edited[3]) {
                bool select_p4 = (chosen.getIntValue() !=  0 && chosen.getIntValue() != 1 && chosen.getIntValue() !=  2 && chosen.getIntValue() == 3);
                if (ImGui::Checkbox("Select Preset 4", &select_p4)) {
                    chosen.setValue(3);
                }
            }
            else {
                ImGui::Text("Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 4")) {
                if (p4_buf[0] != '\0') {
                    groupNames.at(3) = p4_buf;
                    std::memset(p4_buf, '\0', sizeof(p4_buf));
                }
                groupIndices.at(3) = { shot4_binds[0], shot4_binds[1], shot4_binds[2], shot4_binds[3] };

                savePresets(Groups_Only);
                LOG("Preset Successfully Saved!");

                edited[3] = false;

            }


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Individual Shot Editor"))
        {

            ImGui::Text("Choose Any Shot Preset To Customize To YOUR Exact Liking!");

            static int current_shot = 0;
            if (ImGui::BeginCombo("Select Shot Present To Customize", names.at(current_shot).c_str()))
            {
                for (int n = 0; n < names.size(); n++)
                {
                    const bool is_selected = (current_shot == n);
                    if (ImGui::Selectable(names.at(n).c_str(), is_selected))
                        current_shot = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::Text("Name of Preset:");

            static char new_name[128];
            ImGui::InputTextWithHint("##textinput", names.at(current_shot).c_str(), new_name, IM_ARRAYSIZE(new_name));

            ImGui::Separator();
            ImGui::Text("Initial Position:");

            static vector<float> curPos = initPosAll.at(current_shot);
            static float vec3Pos[3] = { curPos.at(0), curPos.at(1), curPos.at(2) };
            ImGui::DragFloat3("Initial Position", vec3Pos, 1);

            static int current_relative = rel_to.at(current_shot);
            ImGui::Combo("Position Relative to What?", &current_relative, "Center\0Goal\0Car\0\0");

            ImGui::Separator();
            ImGui::Text("Initial Velocity:");

            static float speed = speeds.at(current_shot);
            ImGui::DragFloat("Speed", &speed, 0.5f, 0.0f, 250.0f);

            static vector<float> curDir = initDir.at(current_shot);
            static float vec3Dir[3] = { curDir.at(0), curDir.at(1), curDir.at(2) };
            ImGui::DragFloat3("Direction", vec3Dir, 1);

            ///*static bool ind = false;
            //ImGui::Checkbox("Show Indicator", &ind);*/

            ImGui::Separator();
            ImGui::Text("Variance:");

            static bool dir_ind = usingDirVar.at(current_shot);
            ImGui::Checkbox("Directional Variance", &dir_ind);

            static float var = variance.at(current_shot);
            if (dir_ind) {
                ImGui::DragFloat("Directional Variance", &var, 0.5f, 0.0f, 100.0f);
            }

            static bool pos_ind = usingPosVar.at(current_shot);
            ImGui::Checkbox("Positional Variance", &pos_ind);

            static int cur_shape = posVarShape.at(current_shot);
            if (pos_ind) {
                ImGui::Combo("Position Relative to What?", &cur_shape, "Cuboid\0Sphere\0\0");
            }

            if (ImGui::Button("Save")) {
                names.at(current_shot) = new_name;
                initPosAll.at(current_shot) = { vec3Pos[0],vec3Pos[1],vec3Pos[2] };
                rel_to.at(current_shot) = current_relative;
                speeds.at(current_shot) = speed;
                usingDirVar.at(current_shot) = dir_ind;
                variance.at(current_shot) = var;
                usingPosVar.at(current_shot) = pos_ind;
                posVarShape.at(current_shot) = cur_shape;

                savePresets(Shots_Only);
                LOG("Preset Successfully Saved!");

            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }        
}

bool Freeplay_Trainer::errorCheck() {
    static string error_msg = "";
    if (names.empty()) {
        error_msg += "preset names, ";
    }
    if (initPosAll.empty()) {
        error_msg += "positions, ";
    }
    if (rel_to.empty()) {
        error_msg += "relative positions, ";
    }
    if (speeds.empty()) {
        error_msg += "speeds, ";
    }
    if (initDir.empty()) {
        error_msg += "initial direction, ";
    }
    if (usingDirVar.empty()) {
        error_msg += "usign directional variance, ";
    }
    if (variance.empty()) {
        error_msg += "variance, ";
    }
    if (usingPosVar.empty()) {
        error_msg += "using positional variance, ";
    }
    if (posVarShape.empty()) {
        error_msg += "positional variance shape, ";
    }
    if (groupIndices.empty()) {
        error_msg += "group presets, ";
    }
    if (groupNames.empty()) {
        error_msg += "group names, ";
    }

    if (error_msg != "") {
        static string error = "ERROR: " + error_msg + "params never initialized. Check save file.";
        ImGui::Text(error.c_str());
        return true;
    }
    return false;

}