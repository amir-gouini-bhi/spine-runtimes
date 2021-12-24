/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifdef SPINE_UE4
#include "SpinePluginPrivatePCH.h"
#endif

#include <spine/RootMotionTimeline.h>

#include <spine/Event.h>
#include <spine/Skeleton.h>

#include <spine/Bone.h>

using namespace spine;

RTTI_IMPL(RootMotionTimeline, CurveTimeline2)

const String RootMotionTimeline::RootMotionBoneName = "move";

RootMotionTimeline::RootMotionTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline2(frameCount,
																											bezierCount),
																							 _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_X << 32) | boneIndex,
						((PropertyId) Property_Y << 32) | boneIndex};
	setPropertyIds(ids, 2);
}

RootMotionTimeline::~RootMotionTimeline() {
}

void RootMotionTimeline::computePositionAtTime(float time, float& out_x, float& out_y)
{
	out_x = 0;
	out_y = 0;
	
	int i = Animation::search(_frames, time, CurveTimeline2::ENTRIES);
	int curveType = (int) _curves[i / CurveTimeline2::ENTRIES];
	switch (curveType) {
		case CurveTimeline::LINEAR: {
			float before = _frames[i];
			out_x = _frames[i + CurveTimeline2::VALUE1];
			out_y = _frames[i + CurveTimeline2::VALUE2];
			float t = (time - before) / (_frames[i + CurveTimeline2::ENTRIES] - before);
			out_x += (_frames[i + CurveTimeline2::ENTRIES + CurveTimeline2::VALUE1] - out_x) * t;
			out_y += (_frames[i + CurveTimeline2::ENTRIES + CurveTimeline2::VALUE2] - out_y) * t;
			break;
		}
		case CurveTimeline::STEPPED: {
			out_x = _frames[i + CurveTimeline2::VALUE1];
			out_y = _frames[i + CurveTimeline2::VALUE2];
			break;
		}
		default: {
			out_x = getBezierValue(time, i, CurveTimeline2::VALUE1, curveType - CurveTimeline::BEZIER);
			out_y = getBezierValue(time, i, CurveTimeline2::VALUE2,
							   curveType + CurveTimeline::BEZIER_SIZE - CurveTimeline::BEZIER);
		}
	}
}

void RootMotionTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
							  MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);
	
	Bone *bone = skeleton._bones[_boneIndex];
	if (!bone->_active) return;

	if (time < _frames[0]) {
		return;
	}

	float oldX, oldY;
	float newX, newY;
	computePositionAtTime(MathUtil::max(lastTime, 0.f), oldX, oldY);
	computePositionAtTime(time, newX, newY);

	float x, y;
	
	// if animation looped back to beginning, offset correctly
	if (lastTime > time) {
		float endTime = _frames[_frames.size() - CurveTimeline2::ENTRIES];
		float endX, endY;
		computePositionAtTime(endTime, endX, endY);
		float startX, startY;
		computePositionAtTime(0.f, startX, startY);
		
		x = (endX - oldX) + (newX - startX);
		y = (endY - oldY) + (newY - startY);
	} else {
		x = newX - oldX;
		y = newY - oldY;
	}

	switch (blend) {
		case MixBlend_Setup:
		case MixBlend_First:
		case MixBlend_Replace:
			skeleton._rootMotionDeltaX = x * alpha;
			skeleton._rootMotionDeltaY = y * alpha;
			break;
		case MixBlend_Add:
			skeleton._rootMotionDeltaX += x * alpha;
			skeleton._rootMotionDeltaY += y * alpha;
	}
}

RTTI_IMPL(RootMotionXTimeline, CurveTimeline1)

RootMotionXTimeline::RootMotionXTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline1(
																									   frameCount, bezierCount),
																							   _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_X << 32) | boneIndex};
	setPropertyIds(ids, 1);
}

RootMotionXTimeline::~RootMotionXTimeline() {
}

void RootMotionXTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
							   MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);

	Bone *bone = skeleton._bones[_boneIndex];
	if (!bone->_active) return;

	if (time < _frames[0]) {
		return;
	}

	float oldX = getCurveValue(lastTime);
	float newX = getCurveValue(time);
	float x = newX - oldX;
	
	switch (blend) {
		case MixBlend_Setup:
		case MixBlend_First:
		case MixBlend_Replace:
			skeleton._rootMotionDeltaX = x * alpha;
			break;
		case MixBlend_Add:
			skeleton._rootMotionDeltaX += x * alpha;
	}
}

RTTI_IMPL(RootMotionYTimeline, CurveTimeline1)

RootMotionYTimeline::RootMotionYTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline1(
																									   frameCount, bezierCount),
																							   _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_Y << 32) | boneIndex};
	setPropertyIds(ids, 1);
}

RootMotionYTimeline::~RootMotionYTimeline() {
}

void RootMotionYTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
							   MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);

	Bone *bone = skeleton._bones[_boneIndex];
	if (!bone->_active) return;

	if (time < _frames[0]) {
		return;
	}

	float oldY = getCurveValue(lastTime);
	float newY = getCurveValue(time);
	float y = newY - oldY;
	
	switch (blend) {
		case MixBlend_Setup:
		case MixBlend_First:
		case MixBlend_Replace:
			skeleton._rootMotionDeltaY = y * alpha;
			break;
		case MixBlend_Add:
			skeleton._rootMotionDeltaY += y * alpha;
	}
}
