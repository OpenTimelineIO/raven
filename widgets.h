// widgets.h

bool Splitter(const char *str_id, bool split_vertically, float thickness,
              float *size1, float *size2, float min_size1, float min_size2,
              float splitter_long_axis_size = -1.0f, float hover_extend = 0.0f);
bool KnobScalar(const char *label, ImGuiDataType data_type, void *p_data,
                float v_speed, const void *p_min, const void *p_max,
                const char *format, ImGuiSliderFlags flags);
bool KnobFloat(const char *label, float *v, float v_speed = 1.0f,
               float v_min = 0.0f, float v_max = 1.0f,
               const char *format = "%.2f", ImGuiSliderFlags flags = 0);
bool KnobInt(const char *label, int *v, float v_speed = 1.0f, int v_min = 0.0f,
             int v_max = 1.0f, const char *format = "%d",
             ImGuiSliderFlags flags = 0);
