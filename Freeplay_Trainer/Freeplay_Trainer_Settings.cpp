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
    
    ImGui::Text("");
    if (ImGui::BeginTabBar("Types of Customization", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Group Preset Customizer"))
        {
            ImGui::Text("Customize Up To 4 Unique D-Pad/1234 Presets!");
            ImGui::Separator();

            static bool edited[4] = { false,false,false,false };
            string gNames[4] = {groupNames.at(0), groupNames.at(1), groupNames.at(2), groupNames.at(3) };
            string lNames[4] = { groupNames.at(0), groupNames.at(1), groupNames.at(2), groupNames.at(3) };

            static bool presetSelect[4] = { true, false, false, false };
            static int active = 0;

            //Marks the changes that were made and if its selected
            for (int i = 0; i < 4; i++) {
                if (edited[i]) {
                    lNames[i] = "*" + groupNames.at(i) + "*";
                }
                if (i == chosen.getIntValue()) {
                    lNames[i] += " - Selected";
                }
            }

            const char* const lNamesC[4] = { lNames[0].c_str(), lNames[1].c_str(), lNames[2].c_str(), lNames[3].c_str() };

            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.15f);
            if (ImGui::ListBox("##PresetList", &active, lNamesC, 4)) {
                presetSelect[0] = false;
                presetSelect[1] = false;
                presetSelect[2] = false;
                presetSelect[3] = false;
                presetSelect[active] = true;
            }

            ImGui::SameLine();

            static string key_names[4] = { "Left/1", "Right/2", "Up/3", "Down/4" };

            
            static char p_buf[128];


            ImGui::BeginChild("preseteditor", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.85f), true);
            string outStr = "Editing Preset: " + gNames[active];
            ImGui::Text(outStr.c_str());
            ImGui::Separator();
            ImGui::Text("Rename:");
            ImGui::InputTextWithHint("##groupNP1", gNames[active].c_str(), p_buf, IM_ARRAYSIZE(p_buf));

            if (p_buf[0] != '\0') {
                edited[active] = true;
            }

            ImGui::Text("Choose D-Pad Binds:");

            ImGui::PushItemWidth(100);

            //Preset Options:
            vector<int> p_binds = groupIndices.at(active);
            int shot_binds[NUM_KEYB] = { p_binds[0], p_binds[1], p_binds[2], p_binds[3] };

            //Iterate through the 4 combo options
            for (int keyB = 0; keyB < NUM_KEYB; keyB++) {
                //if (ImGui::BeginCombo(key_names[keyB].c_str(), names.at(shot_binds[keyB] - 1).c_str()))
                if (ImGui::BeginCombo(key_names[keyB].c_str(), names.at(shot_binds[keyB]).c_str()))
                {
                    for (int n = 0; n < names.size(); n++)
                    {
                        //Update selected values
                        const bool is_selected = (shot_binds[keyB] == n);
                        if (ImGui::Selectable(names.at(n).c_str(), is_selected)) {
                            groupIndices.at(active).at(keyB) = n;
                            edited[active] = true;
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

            static string save = "Save Changes To Preset " + gNames[active];
            if (ImGui::Button(save.c_str())) {
                if (p_buf[0] != '\0') {
                    groupNames.at(active) = p_buf;
                    std::memset(p_buf, '\0', sizeof(p_buf));
                }
                groupIndices.at(active) = { shot_binds[0], shot_binds[1], shot_binds[2], shot_binds[3] };

                savePresets();

                LOG("Preset Successfully Saved!");

                edited[active] = false;

            }

            if (chosen.getIntValue() != active && !edited[active]) {
                if (ImGui::Button("Use this Preset?")) {
                    chosen.setValue(active);
                }
            }
            else{
                if (chosen.getIntValue() == active) {
                    ImGui::Text("This Preset is Currently Selected");
                }
                if (edited[active]) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "*Make sure to save your changes! Then you can select this preset!*");
                }
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Individual Shot Editor"))
        {
            
            ImGui::Text("Choose Any Shot Preset To Customize To YOUR Exact Liking!");
            ImGui::Separator();

            static bool edit = false;
            static int current_shot = 0;
            ImGui::BeginGroup();
            ImGui::Text("Select Shot To Customize");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.15f);
            if (ImGui::BeginCombo("##shotCombo", names.at(current_shot).c_str()))
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


            ImGui::Text("Rename Preset:");

            char new_name[128];
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.15f);
            if (ImGui::InputTextWithHint("##textinput", names.at(cur_shot).c_str(), new_name, IM_ARRAYSIZE(new_name), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (new_name[0] != '\0') {
                    names.at(cur_shot) = new_name;
                    edit = true;
                }
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginChild("shoteditor", ImVec2(0, 0), true);
            if (ImGui::TreeNodeEx("Initial Position", ImGuiTreeNodeFlags_Framed)) {

                if (rel_to.at(cur_shot) != 3) {
                    vector<float> curPos = initPosAll.at(cur_shot);
                    float vec3Pos[3] = { curPos.at(0), curPos.at(1), curPos.at(2) };
                    if (ImGui::DragFloat3("Initial Position", vec3Pos, 1)) {
                        initPosAll.at(cur_shot) = { vec3Pos[0],vec3Pos[1],vec3Pos[2] };
                        edit = true;
                    }
                }



                int current_relative = rel_to.at(cur_shot);
                if (ImGui::Combo("Position Relative to What?", &current_relative, "Center\0Goal\0Car\0Ball\0\0")) {
                    rel_to.at(cur_shot) = current_relative;
                    edit = true;
                }

                /*if (current_relative == 2) {
                    bool ballLock = ballLocked;
                    if (ImGui::Checkbox("Lock All Ball Axis To Car", &ballLock)) {
                        ballLocked = ballLock;
                    }
                    ImGui::SameLine();
                    bool arrowLock = arrowLocked;
                    if (ImGui::Checkbox("Lock Arrow Axis To Car", &arrowLock)) {
                        arrowLocked = arrowLock;
                    }
                }*/
                ImGui::TreePop();
            }

            float speed = speeds.at(cur_shot);

            if (ImGui::TreeNodeEx("Direction", ImGuiTreeNodeFlags_Framed)) {

                static int modeActive = (dirMode.at(cur_shot)) ? 0 : 1;

                const char* modeC[2] = { "Shoot At Mode", "Manual Mode" };
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.15f);
                if (ImGui::ListBox("##dirMode", &modeActive, modeC, 2)) {
                    dirMode.at(cur_shot) = (modeActive == 0);
                    edit = true;
                }

                ImGui::SameLine();
                ImGui::BeginGroup();

                vector<float> curDir = initDir.at(cur_shot);
                float vecDir[2] = { curDir.at(0), curDir.at(1) };

                if (modeActive == 0) {
                    ImGui::Text("Choose What to Shoot the Ball At:");
                    int shootOption = shootAt.at(cur_shot);
                    const string shootOptions[4] = { "Car Body", "Leading the Car", "Goal", "Backboard" };
                    if (ImGui::BeginCombo("##shotOption", shootOptions[shootOption].c_str()))
                    {
                        for (int n = 0; n < 4; n++)
                        {
                            const bool is_selected = (shootOption == n);
                            if (ImGui::Selectable(shootOptions[n].c_str(), is_selected))
                                shootOption = n;
                            edit = true;

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                        shootAt.at(cur_shot) = shootOption;
                    }

                    float time = timeTo.at(cur_shot);
                    float lTime = leadOff.at(cur_shot);

                    ImGui::BeginGroup();
                    ImGui::Text("Time To Reach Destination:");

                    if (shootOption == 1) {
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                    }

                    if (ImGui::DragFloat("(seconds)##timeTo", &time, 0.5f, 0.1f, 5.0f)) {
                        timeTo.at(cur_shot) = time;
                        edit = true;
                    }
                    ImGui::EndGroup();

                    if (shootOption == 1) {
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        ImGui::Text("Leading Distance");
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::DragFloat("(cm)##predict", &lTime, 0.5f, 0.0f, 2000.0f)) {
                            leadOff.at(cur_shot) = lTime;
                            edit = true;
                        }
                        ImGui::EndGroup();
                    }
                }
                else {

                    ImGui::Text("Speed");
                    if (rel_to.at(cur_shot) == 2 || speed == 0.0) {
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                    }

                    if (ImGui::DragFloat("KPH", &speed, 0.5f, 0.0f, 215.0f)) {
                        speeds.at(cur_shot) = speed;
                        cur_speed = speed;
                        edit = true;
                    }
                    if (rel_to.at(cur_shot) == 2 && speed != 0.0) {

                        bool aVel = addVel.at(cur_shot);
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::Checkbox("Add Velocity to Cars", &aVel)) {
                            addVel.at(cur_shot) = aVel;
                            edit = true;
                        }
                    }
                    else if (speed == 0.0) {
                        bool canFreeze = willFreeze.at(cur_shot);
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::Checkbox("Does Ball Remain Frozen?", &canFreeze)) {
                            willFreeze.at(cur_shot) = canFreeze;
                            edit = true;
                        }
                    }

                    if (speed != 0.0) {
                        ImGui::Text("Direction");
                        if (ImGui::DragFloat2("(Degrees)", vecDir, 0.5f, -360.0f, 360.0f)) {
                            initDir.at(cur_shot) = { vecDir[0], vecDir[1] };
                            edit = true;
                        }
                    }
                }
                ImGui::EndGroup();
                ImGui::TreePop();

            }

            if (ImGui::TreeNodeEx("Mirroring", ImGuiTreeNodeFlags_Framed)) {
                // If either position or direction related to the goal
                ImGui::BeginGroup();
                ImGui::Text("When to Mirror Along Half:");

                int miry = mirror.at(cur_shot).at(1);
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                if (ImGui::Combo("##MirGoalCombo", &miry, "Never\0Same Half as Goal\0Goal Car is Facing\0Random Half\0\0")) {
                    mirror.at(cur_shot).at(1) = miry;
                    edit = true;
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                ImGui::Text("When to Mirror Along Center:");

                int mirx = mirror.at(cur_shot).at(0);
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                if (ImGui::Combo("##MirWallCombo", &mirx, "Never\0Same Side as Wall\0Wall Car is Facing\0Random Side\0\0")) {
                    mirror.at(cur_shot).at(0) = mirx;
                    edit = true;
                }
                ImGui::EndGroup();

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Variance", ImGuiTreeNodeFlags_Framed)) {

                ImGui::BeginGroup();

                bool pos_ind = usingPosVar.at(cur_shot);
                if (ImGui::Checkbox("Positional Variance", &pos_ind)) {
                    usingPosVar.at(cur_shot) = pos_ind;
                    edit = true;
                }


                int cur_shape = posVarShape.at(cur_shot);
                if (pos_ind) {
                    ImGui::SameLine();
                    static bool pos_var_ind = posVarInd;
                    if (ImGui::Checkbox("Show Indicator##posvarind", &pos_var_ind)) {
                        posVarInd = pos_var_ind;
                    }

                    ImGui::Text("Bounding Shape For Random Spawn");
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                    if (ImGui::Combo("##ShapeCombo", &cur_shape, "Cuboid\0Sphere\0\0")) {
                        posVarShape.at(cur_shot) = cur_shape;
                        edit = true;
                    }
                    if (cur_shape == 0) {
                        vector<float> curCube = cuboid.at(cur_shot);
                        float vecCube[3] = { curCube.at(0), curCube.at(1), curCube.at(2) };

                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::DragFloat3("Cuboid Size", vecCube, 1)) {
                            cuboid.at(cur_shot) = { vecCube[0],vecCube[1],vecCube[2] };
                            edit = true;
                        }
                    }
                    else {
                        float radius = sphere.at(cur_shot);
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::DragFloat("Sphere Radius", &radius, 0.5f, BALL_RADIUS, 400.0f)) {
                            sphere.at(cur_shot) = radius;
                            edit = true;
                        }
                    }
                }
                ImGui::EndGroup();


                bool dir_ind = usingDirVar.at(cur_shot);
                float var = variance.at(cur_shot);

                if (speed != 0.0) {
                    ImGui::SameLine();
                    ImGui::BeginGroup();

                    if (ImGui::Checkbox("Directional Variance", &dir_ind)) {
                        usingDirVar.at(cur_shot) = dir_ind;
                        edit = true;
                    }

                    if (dir_ind) {
                        ImGui::SameLine();

                        bool dir_var_ind = dirVarInd;
                        if (ImGui::Checkbox("Show Indicator##dirvarind", &dir_var_ind)) {
                            dirVarInd = dir_var_ind;
                        }

                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
                        if (ImGui::DragFloat("(Degrees)##DirVar ", &var, 0.5f, 1.0f, 25.0f)) {
                            variance.at(cur_shot) = var;
                            edit = true;
                        }
                    }
                    ImGui::EndGroup();
                }
                ImGui::TreePop();
            }

            ImGui::Separator();
            static bool ball_ind = ball_indicator;
            static bool line_ind = line_indicator;
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
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "You have unsaved changes!");
            }


            if (ImGui::Button("Save All Changes")) {
                edit = false;
                savePresets();
                LOG("Preset Successfully Saved!");

            }
            ImGui::SameLine();

            if (ImGui::Button("Revert Shot to Default")) {
                ImGui::OpenPopup("Revert to Default?");
            }

            if (ImGui::BeginPopupModal("Revert to Default?", NULL)) {
                string txt = "Are you sure your want to revert " + names.at(cur_shot) + "?\nThis cannot be undone!";
                ImGui::Text(txt.c_str());

                if (ImGui::Button("Yes", ImVec2(120, 0))) { 
                    resetPreset(cur_shot);
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            ImGui::EndChild();

            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("General Settings")) {
            ImGui::Text("General Indicator Settings");
            if (ImGui::Checkbox("Toggle All Indicators", &togAll)) {
                posVarInd = togAll;
                dirVarInd = togAll;
                ball_indicator = togAll;
                line_indicator = togAll;
            }
            ImGui::Separator();
            ImGui::Text("Change Indicator Colors");
            ImVec4 bCol = VecLinearColorToVec4(colors, 0);
            ImVec4 aCol = VecLinearColorToVec4(colors, 1);
            ImVec4 pCol = VecLinearColorToVec4(colors, 2);
            ImVec4 dCol = VecLinearColorToVec4(colors, 3);

            if (ImGui::ColorEdit4("Ball Indicator Color", (float*)&bCol, ImGuiColorEditFlags_NoInputs)) {
                AssignLinearColorFromVec4(bCol, 0);
            }
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Arrow Indicator Color", (float*)&aCol, ImGuiColorEditFlags_NoInputs)) {
                AssignLinearColorFromVec4(aCol, 1);
            }
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Position Indicator Color", (float*)&pCol, ImGuiColorEditFlags_NoInputs)) {
                AssignLinearColorFromVec4(pCol, 2);
            }
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Directon Indicator Color", (float*)&dCol, ImGuiColorEditFlags_NoInputs)) {
                AssignLinearColorFromVec4(dCol, 3);
            }


            if (ImGui::Button("Revert All to Default")) {
                ImGui::OpenPopup("Revert All to Default?");
            }

            if (ImGui::BeginPopupModal("Revert All to Default?", NULL)) {
                string txt = "Are you sure your want to revert all settings to default?\nThis cannot be undone!";
                ImGui::Text(txt.c_str());

                if (ImGui::Button("Yes", ImVec2(120, 0))) { 
                    resetPreset(-1);
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
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
    if (dirMode.empty()) {
        error_msg += "dirMode, ";
    }
    if (shootAt.empty()) {
        error_msg += "shootAt, ";
    }
    if (timeTo.empty()) {
        error_msg += "timeTo, ";
    }
    if (initDir.empty()) {
        error_msg += "initial direction, ";
    }
    if (usingDirVar.empty()) {
        error_msg += "using directional variance, ";
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
