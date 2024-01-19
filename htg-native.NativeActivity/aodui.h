#pragma once
#ifndef __rosemary_project_util_aodui_h_
#define __rosemary_project_util_aodui_h_
#include "utils.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_uielement.h"
#include "imgui/imgui_internal.h"
#include <typeinfo>
#include <any>
#include <nlohmann/json/json.hpp>

namespace AOD {
	struct TimeClockDigital : rsm::GenericHook{
		ImRGB color;
		
	};
};


#endif