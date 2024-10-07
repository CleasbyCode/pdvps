#pragma once

#include <fstream>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "imageVec.cpp"
#include "information.cpp"
#include "crc32.cpp"
#include "valueUpdater.cpp"
#include "pdvps.cpp"

void
	startPdv(const std::string&),
	valueUpdater(std::vector<uint8_t>&, uint32_t, const uint32_t, uint8_t),
	displayInfo();
