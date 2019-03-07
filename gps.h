#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "StringHandler.h"

namespace NTRIPClient
{
	class GPS {

	public:
		int LastFixQuality = -1;
		int LastStatsTracked = 0;
		double LastVDOP = 0;
		double LastPDOP = 0;
		double LastAltitude = -1;
		double LastHeading = 0;
		double LastSpeed = 0;
		double LastSpeedSmoothed = 0;
		Decimal LastHDOP = 0;

	private:
		std::vector<float> RecentSpeeds = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	public:
		std::string LastStationID = "0";
		std::string LastCorrAge = "-";
		std::string RecordQueue = "";

	public:
		void ProcessNMEAdata(const std::string &x);

	private:
		void ProcessGPGGA(const std::string &code);
		void ProcessGPGSA(const std::string &code);
		void ProcessGPRMC(const std::string &code);

		void RecordLine(const std::string &Line);

	public:
		void WriteRecordingQueueToFile();

		std::string GenerateGPGGAcode();

		std::string CalculateChecksum(const std::string &sentence);
	};
}