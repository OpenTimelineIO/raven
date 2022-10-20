// Inspector

#include "app.h"
#include "widgets.h"
#include "inspector.h"

void DrawInspector()
{
  char buf[10000];
  if (appState.selected_text == "") {
    snprintf(buf, sizeof(buf), "Nothing selected.");
  }else{
    snprintf(buf, sizeof(buf), "%s", appState.selected_text.c_str());
  }
  ImGui::InputTextMultiline("Inspector", buf, sizeof(buf), ImVec2(-FLT_MIN, -FLT_MIN), 0);
}

