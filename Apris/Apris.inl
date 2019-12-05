//////////////////////////////////////////////////////////////////////////////
//
// Apris (https://github.com/Hapaxia/Apris)
// --
//
// Copyright(c) 2019 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#ifndef APRIS_APRIS_INL
#define APRIS_APRIS_INL

#include <utility>
#include <locale>

//#include <iostream>

Apris::Apris()
	: m_globalAlt{ -1 }
	, m_characters{ '$', '^', '|', '_' }
	, m_isProcessCapitalEnabled{ true }
	, m_currentBank{ 0u }
	, m_controlNumberBase{ 10u }
	, m_controlLimits{ 1u, 0u } // if second value zero, process all controls above the lower limit (no top limit) (values are the actual values used in the strings, not the vector indices)
	, m_locale()
	, m_banks(1u)
	, m_controlMap()
{
}

inline bool Apris::setCurrentBank(const std::size_t bankIndex)
{
	const bool success{ bankIndex < m_banks.size() };
	m_currentBank = bankIndex;
	return success;
}

inline void Apris::setBankAlt(const std::size_t bankIndex, const unsigned int alt)
{
	if (isBankIndexValid(bankIndex))
		m_banks[bankIndex].alt = alt;
}

std::size_t Apris::getNumberOfStringsInBank(const std::size_t bankIndex)
{
	if (!isBankIndexValid(bankIndex))
		return 0u;
	return m_banks[bankIndex].strings.size();
}

inline std::string Apris::getString(const std::size_t stringIndex)
{
	return getString(m_currentBank, stringIndex);
}

std::string Apris::getString(const std::size_t bankIndex, const std::size_t stringIndex)
{
	if (!isBankIndexValid(bankIndex))
		return "";
	if (stringIndex < m_banks[bankIndex].strings.size())
		return m_banks[bankIndex].strings[stringIndex];
	return "";
}

inline void Apris::setString(const std::size_t stringIndex, const std::string& string)
{
	setString(m_currentBank, stringIndex, string);
}

void Apris::setString(const std::size_t bankIndex, const std::size_t stringIndex, const std::string& string)
{
	if (!isBankIndexValid(bankIndex))
		return;
	if (stringIndex < m_banks[bankIndex].strings.size())
		m_banks[bankIndex].strings[stringIndex] = string;
}

std::string Apris::processString(const std::size_t bankIndex, const std::string& string, const int controlOffset, const bool processAlt) const
{
	std::string s;
	if (processAlt)
	{
		std::string s{ priv_process(string, true, bankIndex, controlOffset) }; // first process alt (alternative strings)
		return priv_process(s, false, bankIndex, controlOffset); // then process control (bank strings)
	}
	else
		return priv_process(string, false, bankIndex, controlOffset); // only process control (bank strings)
}

inline std::string Apris::processString(const std::string& string, const int controlOffset, const bool processAlt) const
{
	return processString(m_currentBank, string, controlOffset, processAlt);
}

inline void Apris::addStringToBank(const std::size_t bankIndex, const std::string& string)
{
	m_banks[bankIndex].strings.emplace_back(string);
}

void Apris::addStringsToBank(const std::size_t bankIndex, const std::vector<std::string>& strings)
{
	for (auto& string : strings)
		addStringToBank(bankIndex, string);
}

inline void Apris::addStringToCurrentBank(const std::string& string)
{
	m_banks[m_currentBank].strings.emplace_back(string);
}

inline void Apris::addStringsToCurrentBank(const std::vector<std::string>& strings)
{
	for (auto& string : strings)
		addStringToCurrentBank(string);
}

void Apris::resetBank(const std::size_t bankIndex)
{
	if (isBankIndexValid(bankIndex))
	{
		m_banks[bankIndex].alt = false;
		m_banks[bankIndex].strings.clear();
	}
}

void Apris::resetAllBanks()
{
	for (auto& bank : m_banks)
	{
		bank.alt = false;
		bank.strings.clear();
	}
}

inline bool Apris::getBankAlt(const std::size_t bankIndex) const
{
	return (isBankIndexValid(bankIndex) && m_banks[bankIndex].alt);
}

void Apris::removeControlMap(const std::size_t controlNumber)
{
	auto it{ m_controlMap.begin() };
	while (it != m_controlMap.end())
	{
		it = m_controlMap.find(controlNumber);
		if (it != m_controlMap.end())
			it = m_controlMap.erase(it);
	}
}

inline bool Apris::getControlMapExists(const std::size_t controlNumber)
{
	return m_controlMap.find(controlNumber) != m_controlMap.end();
}

std::string Apris::getControlMap(const std::size_t controlNumber)
{
	auto it{ m_controlMap.find(controlNumber) };
	if (it == m_controlMap.end())
		return "";
	return it->second;
}











// PRIVATE

std::string Apris::priv_getStringFromInteger(const std::size_t bankIndex, const int integer, const int controlOffset) const
{
	// returns the string from the bank
	// the string index is integer minus one (e.g. 1 is the first string, 3 is the third) and then offset by controlOffset
	// any negative integers are converted to their positive equivalents before choosing which string to use
	// if the integer is negative, the first character of the string is capitalised (using the stored "locale")
	const int n{ priv_abs(integer) + controlOffset - 1 };
	if (n < 0 || n >= static_cast<int>(m_banks[bankIndex].strings.size()))
		return "";
	std::string t{ m_banks[bankIndex].strings[n] };
	if (integer < 0)
		t[0] = std::toupper(t[0], m_locale);
	return t;
}

std::string Apris::priv_process(const std::string& string, const bool isAlt, const std::size_t bankIndex, const int controlOffset) const
{
	const char limiterCharacter{ isAlt ? m_characters.alt : m_characters.control};
	std::string s;

	// find limiters
	std::vector<std::size_t> limiterPositions;
	std::size_t f{ string.find(limiterCharacter) };
	while (f != std::string::npos)
	{
		limiterPositions.emplace_back(f);
		f = string.find(limiterCharacter, f + 1u);
	}

	// pair up limiters (as position and length)
	std::vector<std::pair<unsigned int, unsigned int>> limiterPairs;
	for (std::size_t i{ 0u }; i < limiterPositions.size() / 2u; ++i)
	{
		const std::size_t first{ limiterPositions[i * 2u] };
		const std::size_t second{ limiterPositions[i * 2u + 1u] };
		const std::size_t length{ second - first + 1u };
		if (length > 2u)
			limiterPairs.emplace_back(first, length);
	}

	// if there are no pairs of limiters, change nothing
	if (limiterPairs.size() == 0u)
		return string;

	// parse sections between limiters and store them in a vector
	// if control limiter (alt = false), read integer value (or get integer value from control map string) between limiter
	// if alt limiter (alt = true), store all strings separated by the separater character in a vector
	std::vector<std::vector<std::string>> stringOptions;
	std::vector<int> stringNumbers;
	if (!isAlt)
		stringNumbers.resize(limiterPairs.size());
	else
		stringOptions.resize(limiterPairs.size());
	for (std::size_t i{ 0u }; i < limiterPairs.size(); ++i)
	{
		std::string section{ string.substr(limiterPairs[i].first + 1u, limiterPairs[i].second - 2u) };
		if (!isAlt)
		{
			// if conversion from string to integer fails, store number as zero unless the section is a control map string in which case get the integer from that.
			// (if a number is zero, that section will be simply removed from the final string.)
			try
			{
				stringNumbers[i] = std::stoi(section, 0, m_controlNumberBase);
			}
			catch (...)
			{
				stringNumbers[i] = 0; // store as 0 to start with. it can be changed if the section is a control map string

				// if section is a control map string, retrieve the integer value from that
				std::size_t index{ 0u };
				for (auto& stringMap : m_controlMap)
				{
					bool isCapital{ section[0] == '_' };
					std::size_t startPosition{ isCapital ? 1u : 0u };
					if (!m_isProcessCapitalEnabled)
						isCapital = false;
					if (isCapital)
						section[startPosition] = std::tolower(section[startPosition], m_locale);
					++index;
					if (section.substr(startPosition) == stringMap.second)
					{
						stringNumbers[i] = isCapital ? -static_cast<int>(index) : static_cast<int>(index);
						break;
					}
				}
			}
			if (!m_isProcessCapitalEnabled && stringNumbers[i] < 0)
				stringNumbers[i] = -stringNumbers[i];
		}
		else
		{
			// splits on each separator character found and stores all split as a string within a vector
			std::size_t start{ 0u };
			std::size_t fs{ section.find(m_characters.separation) };
			std::size_t length{ fs - start };
			if (fs == std::string::npos)
				length = section.length();
			while (fs < section.length())
			{
				stringOptions[i].emplace_back(section.substr(start, length));
				start = fs + 1u;
				fs = section.find(m_characters.separation, start);
				length = fs - start;
			}
			stringOptions[i].emplace_back(section.substr(start, length));
		}
	}
	
	// re-build string
	std::size_t p{ 0u };
	std::size_t alt{ m_globalAlt < 0 ? (m_globalAlt == -2 ? (m_banks[bankIndex].alt + (1u - m_banks[bankIndex].alt % 2u) - (1u - (m_banks[bankIndex].alt + 1u) % 2)) : m_banks[bankIndex].alt) : static_cast<std::size_t>(m_globalAlt) };
	for (std::size_t i{ 0u }; i < limiterPairs.size(); ++i)
	{
		// copy unaltered section up to limiter
		s += string.substr(p, limiterPairs[i].first - p);

		// move position pointer to start of next unaltered section
		p = limiterPairs[i].first + limiterPairs[i].second;

		// add modified section
		if (!isAlt)
		{
			// add string from bank
			// if (valid) value is not within the value limits, the entire string section - including limiter characters - is left in the final string
			// note that if stringNumbers[i] is zero, it's because it failed to convert the string section to an integer so it's just completely removed from the final string
			std::size_t n{ static_cast<std::size_t>(priv_abs(stringNumbers[i])) };
			if (n > 0u)
			{
				if ((n >= m_controlLimits.first) && ((m_controlLimits.second == 0u) || (n <= m_controlLimits.second)))
					s += priv_getStringFromInteger(bankIndex, stringNumbers[i], controlOffset);
				else
					s += string.substr(limiterPairs[i].first, limiterPairs[i].second);
			}
		}
		else
		{
			if (alt < stringOptions[i].size())
				s += stringOptions[i][alt];
		}
	}
	// add rest of unaltered string after all pairs of limiters
	if (p < string.length())
		s += string.substr(p);

	return s;
}

#endif // APRIS_APRIS_INL
