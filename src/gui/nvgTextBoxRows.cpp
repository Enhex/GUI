//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "nvgTextBoxRows.h"

#include <fontstash.h>

#define NVG_MAX_FONTIMAGES       4
#define NVG_MAX_STATES 32

struct NVGstate {
	NVGcompositeOperationState compositeOperation;
	int shapeAntiAlias;
	NVGpaint fill;
	NVGpaint stroke;
	float strokeWidth;
	float miterLimit;
	int lineJoin;
	int lineCap;
	float alpha;
	float xform[6];
	NVGscissor scissor;
	float fontSize;
	float letterSpacing;
	float lineHeight;
	float fontBlur;
	int textAlign;
	int fontId;
};
typedef struct NVGstate NVGstate;

struct NVGpoint {
	float x,y;
	float dx, dy;
	float len;
	float dmx, dmy;
	unsigned char flags;
};
typedef struct NVGpoint NVGpoint;

struct NVGpathCache {
	NVGpoint* points;
	int npoints;
	int cpoints;
	NVGpath* paths;
	int npaths;
	int cpaths;
	NVGvertex* verts;
	int nverts;
	int cverts;
	float bounds[4];
};
typedef struct NVGpathCache NVGpathCache;

struct NVGcontext {
	NVGparams params;
	float* commands;
	int ccommands;
	int ncommands;
	float commandx, commandy;
	NVGstate states[NVG_MAX_STATES];
	int nstates;
	NVGpathCache* cache;
	float tessTol;
	float distTol;
	float fringeWidth;
	float devicePxRatio;
	struct FONScontext* fs;
	int fontImages[NVG_MAX_FONTIMAGES];
	int fontImageIdx;
	int drawCallCount;
	int fillTriCount;
	int strokeTriCount;
	int textTriCount;
};

static NVGstate* nvg__getState(NVGcontext* ctx)
{
	return &ctx->states[ctx->nstates-1];
}

void nvgTextBox(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end, std::vector<NVGtextRow>& rows_out)
{
	NVGstate* state = nvg__getState(ctx);
	NVGtextRow rows[2];
	int nrows = 0, i;
	int oldAlign = state->textAlign;
	int haling = state->textAlign & (NVG_ALIGN_LEFT | NVG_ALIGN_CENTER | NVG_ALIGN_RIGHT);
	int valign = state->textAlign & (NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE | NVG_ALIGN_BOTTOM | NVG_ALIGN_BASELINE);
	float lineh = 0;

	if (state->fontId == FONS_INVALID) return;

	nvgTextMetrics(ctx, NULL, NULL, &lineh);

	state->textAlign = NVG_ALIGN_LEFT | valign;

	while ((nrows = nvgTextBreakLines(ctx, string, end, breakRowWidth, rows, 2))) {
		for (i = 0; i < nrows; i++) {
			NVGtextRow* row = &rows[i];
			rows_out.emplace_back(*row);
			if (haling & NVG_ALIGN_LEFT)
				nvgText(ctx, x, y, row->start, row->end);
			else if (haling & NVG_ALIGN_CENTER)
				nvgText(ctx, x + breakRowWidth*0.5f - row->width*0.5f, y, row->start, row->end);
			else if (haling & NVG_ALIGN_RIGHT)
				nvgText(ctx, x + breakRowWidth - row->width, y, row->start, row->end);
			y += lineh * state->lineHeight;
		}
		string = rows[nrows-1].next;
	}

	state->textAlign = oldAlign;
}

std::vector<NVGtextRow> nvgTextBoxGetRows(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end)
{
	auto* state = nvg__getState(ctx);
	NVGtextRow rows[2];
	int nrows = 0, i;
	int oldAlign = state->textAlign;
	int valign = state->textAlign & (NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE | NVG_ALIGN_BOTTOM | NVG_ALIGN_BASELINE);

	if (state->fontId == FONS_INVALID)
		return {};

	state->textAlign = NVG_ALIGN_LEFT | valign;

	std::vector<NVGtextRow> rows_out;
	while ((nrows = nvgTextBreakLines(ctx, string, end, breakRowWidth, rows, 2))) {
		for (i = 0; i < nrows; i++) {
			rows_out.emplace_back(rows[i]);
		}
		string = rows[nrows-1].next;
	}

	state->textAlign = oldAlign;
	return rows_out;
}