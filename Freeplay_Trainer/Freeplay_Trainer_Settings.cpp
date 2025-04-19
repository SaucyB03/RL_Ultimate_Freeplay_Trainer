#include "pch.h"
#include "Freeplay_Trainer.h"

void Freeplay_Trainer::RenderSettings() {
    CVarWrapper enabled = cvarManager->getCvar("enable_UTF_plugin");
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
    if (ErrorCheck()) { return; }

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
            vector<int> p1_binds = groupIndices.at(0);
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
                ImGui::Text( "Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 1")) {
                if (p1_buf[0] != '\0') {
                    groupNames.at(0) = p1_buf;
                    std::memset(p1_buf, '\0', sizeof(p1_buf));
                }
                groupIndices.at(0) = { shot1_binds[0], shot1_binds[1], shot1_binds[2], shot1_binds[3] };

                savePresets();

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
                ImGui::Text( "Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 2")) {
                if (p2_buf[0] != '\0') {
                    groupNames.at(1) = p2_buf;
                    std::memset(p2_buf, '\0', sizeof(p2_buf));
                }
                groupIndices.at(1) = { shot2_binds[0], shot2_binds[1], shot2_binds[2], shot2_binds[3] };

                savePresets();
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
                ImGui::Text( "Make sure to save your changes! Then you can select this preset!");
            }

            if (ImGui::Button("Save Preset 3")) {
                if (p3_buf[0] != '\0') {
                    groupNames.at(2) = p3_buf;
                    std::memset(p3_buf, '\0', sizeof(p3_buf));
                }
                groupIndices.at(2) = { shot3_binds[0], shot3_binds[1], shot3_binds[2], shot3_binds[3] };

                savePresets();
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
                bool select_p4 = (chosen.getIntValue() != 0 && chosen.getIntValue() != 1 && chosen.getIntValue() != 2 && chosen.getIntValue() == 3);
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

                savePresets();
                LOG("Preset Successfully Saved!");

                edited[3] = false;

            }
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Individual Shot Editor"))
        {
            
            ImGui::Text("Choose Any Shot Preset To Customize To YOUR Exact Liking!");

            static bool edit = false;
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
                cur_shot = current_shot;
            }

            ImGui::Separator();
            ImGui::Text("Name of Preset:");

            char new_name[128];
            if (ImGui::InputTextWithHint("##textinput", names.at(cur_shot).c_str(), new_name, IM_ARRAYSIZE(new_name), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (new_name[0] != '\0') {
                    names.at(cur_shot) = new_name;
                    edit = true;
                }
            }

            ImGui::Separator();
            ImGui::Text("Initial Position:");

            vector<float> curPos = initPosAll.at(cur_shot);
            float vec3Pos[3] = { curPos.at(0), curPos.at(1), curPos.at(2) };
            if (ImGui::DragFloat3("Initial Position", vec3Pos, 1)) {
                initPosAll.at(cur_shot) = { vec3Pos[0],vec3Pos[1],vec3Pos[2] };
                edit = true;
            }

            int current_relative = rel_to.at(cur_shot);
            if (ImGui::Combo("Position Relative to What?", &current_relative, "Center\0Goal\0Car\0\0")) {
                rel_to.at(cur_shot) = current_relative;
                edit = true;
            }

            if (current_relative == 2) {
                static bool ballLock = false;
                if (ImGui::Checkbox("Lock All Ball Axis To Car", &ballLock)) {
                    ballLocked = ballLock;
                }
                static bool arrowLock = false;
                if (ImGui::Checkbox("Lock Arrow Axis To Car", &arrowLock)) {
                    arrowLocked = arrowLock;
                }
            }


            ImGui::Separator();
            ImGui::Text("Initial Velocity:");

            float speed = speeds.at(cur_shot);
            if (ImGui::DragFloat("Speed", &speed, 0.5f, 0.0f, 215.0f)) {
                speeds.at(cur_shot) = speed;
                edit = true;
            }

            vector<float> curDir = initDir.at(cur_shot);
            float vecDir[2] = { curDir.at(0), curDir.at(1)};
            bool dir_ind = usingDirVar.at(cur_shot);
            float var = variance.at(cur_shot);

            if (speed != 0.0) {
                if (ImGui::DragFloat2("Direction (Degrees)", vecDir, 0.5f,-360.0f, 360.0f)) {
                    initDir.at(cur_shot) = { vecDir[0], vecDir[1]};
                    edit = true;
                }
                ImGui::Separator();

                ImGui::Text("Variance:");

                ImGui::BeginGroup();
                if (ImGui::Checkbox("Directional Variance", &dir_ind)) {
                    usingDirVar.at(cur_shot) = dir_ind;
                    edit = true;
                }

                if (dir_ind) {
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
                    if (ImGui::DragFloat("Directional Variance (Degrees)", &var, 0.5f, 1.0f, 25.0f)) {
                        variance.at(cur_shot) = var;
                        edit = true;
                    }
                }
                ImGui::EndGroup();

                ImGui::SameLine();

            }

            ImGui::BeginGroup();
            
            bool pos_ind = usingPosVar.at(cur_shot);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
            if (ImGui::Checkbox("Positional Variance", &pos_ind)) {
                usingPosVar.at(cur_shot) = pos_ind;
                edit = true;
            }

            int cur_shape = posVarShape.at(cur_shot);
            if (pos_ind) {
                if (ImGui::Combo("Bounding Shape For Random Spawn", &cur_shape, "Cuboid\0Sphere\0\0")) {
                    posVarShape.at(cur_shot) = cur_shape;
                    edit = true;
                }
                if (cur_shape == 0) {
                    vector<float> curCube = cuboid.at(cur_shot);
                    float vecCube[3] = { curCube.at(0), curCube.at(1), curCube.at(2) };
                    if (ImGui::DragFloat3("Cuboid Size", vecCube, 1)) {
                        cuboid.at(cur_shot) = { vecCube[0],vecCube[1],vecCube[2] };
                        edit = true;
                    }
                }
                else {
                    float radius = sphere.at(cur_shot);
                    if (ImGui::DragFloat("Sphere Radius", &radius, 0.5f, BALL_RADIUS, 400.0f)) {
                        sphere.at(cur_shot) = radius;
                        edit = true;
                    }
                }
            }
            ImGui::EndGroup();

            ImGui::Separator();

            static bool ball_ind = false;
            static bool line_ind = false;
            if (ImGui::Checkbox("Show Ball Indicator", &ball_ind)) {
                ball_indicator = ball_ind;
            }
            if (ball_ind) {
                ImGui::SameLine();
                if (ImGui::Checkbox("Show Line Indicator", &line_ind)) {
                    line_indicator = line_ind;
                }
            }

            if (edit) {
                ImGui::Text("You have unsaved changes!");
            }


            if (ImGui::Button("Save All Changes")) {
                edit = false;
                savePresets();
                LOG("Preset Successfully Saved!");

            }
            
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }   
    
}

bool Freeplay_Trainer::ErrorCheck() {
    string error_msg = "";
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
    if (cuboid.empty()) {
        error_msg += "cuboid, ";
    }
    if (sphere.empty()) {
        error_msg += "sphere, ";
    }
    if (groupIndices.empty()) {
        error_msg += "group presets, ";
    }
    if (groupNames.empty()) {
        error_msg += "group names, ";
    }

    if (error_msg != "") {
        string error = "ERROR: " + error_msg + "params never initialized. Check save file.";
        ImGui::Text(error.c_str());
        return true;
    }
    return false;

}
