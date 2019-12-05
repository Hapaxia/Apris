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

#ifndef APRIS_APRIS_HPP
#define APRIS_APRIS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <locale>

class Apris
{
public:
	Apris();
	std::string processString(std::size_t bankIndex, const std::string& string, int controlOffset = 0, bool processAlt = true) const;
	std::string processString(const std::string& string, int controlOffset = 0, bool processAlt = true) const;

	void setNumberOfBanks(std::size_t size) { m_banks.resize(size); }
	void addStringToBank(std::size_t bankIndex, const std::string& string);
	void addStringsToBank(std::size_t bankIndex, const std::vector<std::string>& strings);
	void addStringToCurrentBank(const std::string& string);
	void addStringsToCurrentBank(const std::vector<std::string>& strings);
	void resetBank(std::size_t bankIndex);
	void resetAllBanks();
	bool setCurrentBank(std::size_t bankIndex);
	void setString(std::size_t stringIndex, const std::string& string);
	void setString(std::size_t bankIndex, std::size_t stringIndex, const std::string& string);
	void setControlCharacter(char character) { m_characters.control = character; }
	void setAltCharacter(char character) { m_characters.alt = character; }
	void setSeparationCharacter(char character) { m_characters.separation = character; }
	void setCapitalCharacter(char character) { m_characters.capital = character; }
	void setCurrentBankAlt(unsigned int alt) { setBankAlt(m_currentBank, alt); }
	void setGlobalAlt(int globalAlt) { m_globalAlt = globalAlt; }
	void setBankAlt(std::size_t bankIndex, unsigned int alt);
	void setControlNumberBase(unsigned int controlNumberBase) { m_controlNumberBase = static_cast<int>(controlNumberBase); }
	void setControlLimits(std::size_t lowerLimit, std::size_t upperLimit) { m_controlLimits = { lowerLimit, upperLimit }; }
	void setLocale(const std::locale& locale) { m_locale = locale; }
	void setIsProcessCapitalEnabled(bool isProcessCapitalEnabled) { m_isProcessCapitalEnabled = isProcessCapitalEnabled; }
	void setControlMap(std::size_t controlNumber, const std::string& mappedString) { m_controlMap[controlNumber] = mappedString; };
	void clearControlMap() { m_controlMap.clear(); }
	void removeControlMap(std::size_t controlNumber);
	

	std::size_t getNumberOfBanks() const { return m_banks.size(); };
	bool isBankIndexValid(std::size_t bankIndex) const { return bankIndex < getNumberOfBanks(); }
	std::size_t getNumberOfStringsInBank(std::size_t bankIndex);
	std::size_t getCurrentBank() const { return m_currentBank; }
	std::string getString(std::size_t stringIndex);
	std::string getString(std::size_t bankIndex, std::size_t stringIndex);
	char getControlCharacter() const { return m_characters.control; }
	char getAltCharacter() const { return m_characters.alt; }
	char getSeparationCharacter() { return m_characters.separation; }
	char getCapitalCharacter() { return m_characters.capital; }
	bool getCurrentBankAlt() const { return getBankAlt(m_currentBank); }
	int getGlobalAlt() const { return m_globalAlt; }
	bool getBankAlt(std::size_t bankIndex) const;
	unsigned int getControlNumberBase() const { return static_cast<unsigned int>(m_controlNumberBase); }
	std::size_t getControlLowerLimit() const { return m_controlLimits.first; }
	std::size_t getControlUpperLimit() const { return m_controlLimits.second; }
	std::locale getLocale() const { return m_locale; }
	bool getIsProcessCapitalEnabled() const { return m_isProcessCapitalEnabled; }
	bool getControlMapExists(std::size_t controlNumber);
	std::string getControlMap(std::size_t controlNumber);// { return m_controlMap[controlNumber]; }

private:
	// for global alt:
	//  -1 is "normal", which uses the bank's alt value
	//  -2 is "flip", which swaps each pair of alternatives (odds and evens are swapped e.g. first with second and third with fourth)
	//   0+ is number of alternative to use regardless of the bank (overrides bank value) - zero-based index
	int m_globalAlt;

	struct Characters
	{
		char control;
		char alt;
		char separation;
		char capital;
	} m_characters;
	bool m_isProcessCapitalEnabled;
	std::size_t m_currentBank;
	int m_controlNumberBase;
	std::pair<std::size_t, std::size_t> m_controlLimits;
	std::locale m_locale;
	struct Bank
	{
		unsigned int alt{ 0u }; // zero-based index
		std::vector<std::string> strings;
	};
	std::vector<Bank> m_banks;
	std::unordered_map<std::size_t, std::string> m_controlMap;

	std::string priv_getStringFromInteger(const std::size_t bankIndex, const int integer, const int controlOffset) const;
	std::string priv_process(const std::string& string, const bool alt, const std::size_t bankIndex, const int controlOffset) const;
	template <class T>
	T priv_abs(const T& value) const { return (value < 0) ? -value : value; }
};

#include "Apris.inl"
#endif // APRIS_APRIS_HPP
