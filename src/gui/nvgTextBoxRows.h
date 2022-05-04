#pragma once

// version that returns the rows infomation
void nvgTextBox(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end, std::vector<NVGtextRow>& rows_out);
// get rows without drawing
std::vector<NVGtextRow> nvgTextBoxGetRows(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end);