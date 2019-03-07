#include "gps.h"

namespace NTRIPClient
{
	void GPS::ProcessNMEAdata(std::string &x) {

		int charlocation = (int)x.rfind("$"); // Find location of last $

		if (charlocation == -1 || charlocation + 1 > x.length() - 5) {
			return;
		}

		x = x.substr(charlocation);

		charlocation = (int)x.find("*");

		if (charlocation == -1) {
			return;
		}

		if (x.length() < charlocation + 3) {
			return;
		}
		else if (x.length() > charlocation + 3) {
			return;
		}

		if (x.length() < 8) {
			return;
		}
	
		std::vector<std::string> aryNMEALine = StringHandler::split(x, '*');

		if (CalculateChecksum(StringHandler::replace(aryNMEALine[0], "$", "")) == aryNMEALine[1]) {

			/*if (aryNMEALine[0].substr(0, 6) == "$GPRMC" || aryNMEALine[0].substr(0, 6) == "$GNRMC") {
				
				ProcessGPRMC(aryNMEALine[0]);	
				
			}*/
			
			if (aryNMEALine[0].substr(0, 6) == "$GPGGA" || aryNMEALine[0].substr(0, 6) == "$GNGGA") {
				

					return x;							
				//MainForm::MostRecentGGA = x;

				/*ProcessGPGGA(aryNMEALine[0]);

				if (MainForm::DefaultInstance()->WriteNMEAToFile) {
				
					Recordline(x);				
				}*/
			
			} 
			/*if (aryNMEALine[0].substr(0, 6) == "$GPGSA" || aryNMEALine[0].substr(0, 6) == "$GNGSA") {
				
				ProcessGPGSA(aryNMEALine[0]);				
			}*/
		}		
	}

	/*void GPS::ProcessGPGGA(std::string &code) {

		double InLatitude = 0;
		double InLongitude = 0;

		std::vector<std::string> aryNMEAString = StringHandler::split(code, ',');

		if (aryNMEALine -> GetUpperBound(0) > 13) {
			if (aryNMEAString[2] != "" & aryNMEAString[3] != "" & aryNMEAString[4] != "" & aryNMEAString[5] != "") {

				if (StringHandler::IsNumeric(aryNMEAString[2]) && StringHandler::IsNumeric(aryNMEAString[4])) {

					double snglat = (std::stod(aryNMEAString[2] / 100);
					double snglatmins = snglat % 1;

					snglat = snglat - snglatmins;
					snglatmins = snglatmins * 100 / 60;

					InLatitude = snglat + snglatmins;

					if (aryNMEAString[3] == "S") {
						InLongitude = 0 - InLongitude;
					}

					double snglon = (std::stod(aryNMEAString[4]) / 100);
					double snglonmins = snglon % 1;

					snglon = snglon - snglonmins;
					snglonmins = snglonmin * 100 / 60;
					InLongitude = snglon + snglonmins;

					if (aryNMEAString[5] == "W") {
						InLongitude = 0 - InLongitude;
					}
				}
				else {
					MainForm::lblStatus->Text = "Bad GGA data";
				}
			}
			else {
			
				MainForm::lblStatus->Text = "Empty GGA data";
			}
					
			if (StringHandler::IsNumeric(aryNMEAString[6]) && StringHandler::IsNumeric(aryNMEAString[7])) {
			
				int InFixQuality = std::stoi(aryNMEAString[6]);
				int InSatsTracked = std::stoi(aryNMEAString[7]);
				Decimal InHDOP = Decimal::Parse(aryNMEAString[8]);
				double InAltitude = std::stof(aryNMEAString[9]);

				if (!(InFixQuality == LastFixQuality) || !(InSatsTracked == LastSatsTracked)) {

					std::string gpstype = "";
					std::string shorttype = "";
					switch (InFixQuality) {

					case 1: //GPS fix (SPS)
						gpstype = "GPS fix (No Differential Correction)";
						shorttype = "GPS";
						break;
					case 2: //DGPS fix
						gpstype = "DGPS";
						shorttype = "DGPS";
						break;
					case 3: //PPS fix
						gpstype = "PPS Fix";
						shorttype = "PPS";
						break;
					case 4: //Real Time Kinematic
						gpstype = "RTK";
						shorttype = "RTK";
						break;
					case 5: //Float RTK
						gpstype = "Float RTK";
						shorttype = "FloatRTK";
						break;
					case 6: //estimated (dead reckoning) (2.3 feature)
						gpstype = "Estimated";
						shorttype = "Estimated";
						break;
					case 7: //Manual input mode
						gpstype = "Manual Input Mode";
						shorttype = "Manual";
						break;
					case 8: //Simulation mode
						gpstype = "Simulation";
						shorttype = "Simulation";
						break;
					case 9: //WAAS
						gpstype = "WAAS";
						shorttype = "WAAS";
						break;
					default: //0 = invalid
						gpstype = "Invalid";
						shorttype = "Invalid";
						break;
					}

					MainForm::lblStatus->Text = shorttype = ":" + std::to_string(InSatsTracked);

					if (!(InFixQuality == LastFixQuality)) {

						if (LastFixQuality == -1) {
							LastFixQuality = 0;
						}

						else {
							MainForm::PlayAudioAlert();
						}

						std::string status = "";

						if (InFixQuality == 5 && LastFixQuality == 4) {

							status = "Degraded";
						}
						else if (InFixQuality == 4 && LastFixQuality == 5) {

							status = "Increased";
						}
						else if (InFixQuality == 8 || LastFixQuality == 8) {

							status = "Changed";
						}
						else if (InFixQuality > LastFixQuality) {

							status = "Increased";
						}
						else {

							status = "Degraded";
						}

						MainForm::LogEvent("GPS Fix Quality " + status + " from " + LastFixQuality + " to " + std::to_string(InFixQuality) + " (" + gpstype + ")");

						if (LastFixQuality == 4) {

							MainForm::LogEvent("Correction Age was " + LastCorrAge);
						}
						if (LastHDOP == InHDOP) {

							MainForm::LogEvent("H-DOP unchanged at " + std::to_string(InHDOP));
						}
						else {

							MainForm::LogEvent("H-DOP was " + LastHDOP + " and now is " + std::to_wstring(InHDOP));
						}

						LastFixQuality = InFixQuality;
					}

					if (!(InStatsTracked == LastSatsTracked)) {

						if (InStatsTracked > LastSatsTracked) {
							MainForm::LogEvent("Number of Satellites tracked Increased from " + LastSatsTracked + " to " + InSatsTracked);
						}
						else {
							MainForm::LogEvent("Number of Satellites tracked Decreased from " + LastSatsTracked + " to " + InSatsTracked);
						}
					}

				}

					if (!(InHDOP == LastHDOP) {
						
						LastHDOP = InHDOP;

						if (Disp2 == "hdop"){
						
								MainForm::lbl2->Text = "HDOP:" + LastHDOP;
						}
						if (Disp3 == "hdop"){
						
							MainForm::lbl3->Text = "HDOP:" + LastHDOP;
						}				
					
					}

					if (!(InAltitude == LastAltitude)) {
					
						LastAltitude = InAltitude;
						if (Disp2 == "elevation-meters"){
						
							//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							MainForm::lbl2->Text = LastAltitude.ToString("#.00") + "m";
						}
						if (Disp3 == "elevation-meters"){
						
							//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							MainForm::lbl3->Text = LastAltitude.ToString("#.00") + "m";
						}
						if (Disp2 == "elevation-feet"){
						
							//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							MainForm::lbl2->Text = (LastAltitude * 3.2808399).ToString("#.0") + "'";
						}
						if (Disp3 == "elevation-feet"){
						
							//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							MainForm::lbl3->Text = (LastAltitude * 3.2808399).ToString("#.0") + "'";
						}
						if (InFixQuality == 4){
						
							MainForm::ElevNewE(static_cast<Decimal>(InAltitude));
						}				
					
					}
			}

			std::string inCorrAge = aryNMEAString(13);

			if (inCorrAge.length() == 0) {				
				
				inCorrAge = "N/A";			
			}

			if (!(LastCorrAge == inCorrAge)) {
				
				LastCorrAge = inCorrAge
				
				if (Disp2 == "age") {
				
					MainForm::lbl2->Text = "Age:" + inCorrAge;
						
				}

				if (Disp3 == "age") {

					MainForm::lbl3->Text = "Age:" + inCorrAge;

				}

			
			}

			std::string inStationID = aryNMEAString(14);

			if (inStationID.length() == 0) {
				
				inStationID = "0";
			
			}

			if (!(LastStationID == inStationID)) {
				
				MainForm::LogEvent("Correction Station ID changed from " + LastStationID + " to " + inStationID);
				LastStationID = inStationID;
				MainForm::PlayAudioAlert();			
			}

		}				

	}

	void GPS::ProcessGPGSA(std::string &code) {
	
		std::vector<std::string> aryNMEAString = StringHandler::split(code, ',');

		// we have at least 15 fields
		if (aryNMEAString->GetUpperBound(0) >= 17){
		
			if (StringHandler::IsNumeric(aryNMEAString[15]) && StringHandler::IsNumeric(aryNMEAString[16]) && StringHandler::IsNumeric(aryNMEAString[17])){
			
				float InPDOP = std::stof(aryNMEAString[15]);
				float InHDOP = std::stof(aryNMEAString[16]);
				float InVDOP = std::stof(aryNMEAString[17]);

				if (!(InHDOP == LastPDOP)) //pdop has changed
				{
					LastPDOP = InPDOP;
					if (Disp2 == "pdop")
					{
						MainForm::lbl2->Text = "PDOP:" + LastPDOP;
					}
					if (Disp3 == "pdop")
					{
						MainForm::lbl3->Text = "PDOP:" + LastPDOP;
					}
				}
				if (!(InHDOP == LastHDOP)) //hdop has changed
				{
					LastHDOP = InHDOP;
					if (Disp2 == "hdop")
					{
						MainForm::lbl2->Text = "HDOP:" + LastHDOP;
					}
					if (Disp3 == "hdop")
					{
						MainForm::lbl3->Text = "HDOP:" + LastHDOP;
					}
				}
				if (!(InVDOP == LastVDOP)) //vdop has changed
				{
					LastVDOP = InVDOP;
					if (Disp2 == "vdop")
					{
						MainForm::lbl2->Text = "VDOP:" + LastVDOP;
					}
					if (Disp3 == "vdop")
					{
						MainForm::lbl3->Text = L"VDOP:" + LastVDOP;
					}
				}
			}
		}	
	
	}

	void GPS::ProcessGPRMC(std::string &code) {
	
		std::vector<std::string> aryNMEAString = StringHandler::split(code, ',');

		if (aryNMEAString[7] != "") {
		
			if (isNumeric(aryNMEAString[7])) {
			
				LastSpeed = std::stof(aryNMEAString[7]) * 1.852;

				float speedsum = LastSpeed;

				for (auto i = 0; i <= 8; i++) {
					
					RecentSpeeds(i) = RecentSpeeds(i + 1);
					speedsum += RecentSpeeds(i);
				
				}

				RecentSpeeds(9) = LastSpeed;
				LastSpeedSmoothed = speedsum / 10;

				if (Disp2 == "speed-mph"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl2->Text = (LastSpeed * 0.621371192).ToString("0.0") + " MPH";
				}
				if (Disp3 == "speed-mph"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl3->Text = (LastSpeed * 0.621371192).ToString("0.0") + " MPH";
				}
				if (Disp2 == "speed-mph-smoothed"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl2->Text = (LastSpeedSmoothed * 0.621371192).ToString("0.0") + " MPH";
				}
				if (Disp3 == "speed-mph-smoothed"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl3->Text = (LastSpeedSmoothed * 0.621371192).ToString("0.0") + " MPH";
				}

				if (Disp2 == "speed-kmh"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl2->Text = LastSpeed.ToString("0.0") + " km/h";
				}
				if (Disp3 == "speed-kmh"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl3->Text = LastSpeed.ToString("0.0") + " km/h";
				}
				if (Disp2 == "speed-kmh-smoothed"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl2->Text = LastSpeedSmoothed.ToString("0.0") + " km/h";
				}
				if (Disp3 == "speed-kmh-smoothed"){
				
					//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					MainForm::lbl3->Text = LastSpeedSmoothed.ToString("0.0") + " km/h";
				}
			
			}		
		
		}
	
		if (aryNMEAString[8] != "") {
		
			if (StringHandler::IsNumeric(aryNMEAString[8])) {
			
				LastHeading = std::stof(aryNMEAString[8]);
				if (Disp2 == "heading") {				
					MainForm::lbl2->Text = LastHeading.ToString("0.0") + Microsoft::VisualBasic::Strings::Chr(176);
				}
				if (Disp3 == "heading") {
					MainForm::lbl3->Text = LastHeading.ToString("0.0") + Microsoft::VisualBasic::Strings::Chr(176);
				}		
			
			}
		
		}
	
	}

	void GPS::RecordLine(std::string &Line) {
	
		RecordQueue += Line + "\r\n";;
		if (RecordQueue->Length > 8000){
		
			WriteRecordingQueueToFile();
		}
	
	}

	void GPS::WriteRecordingQueueToFile() {
	
		if (RecordQueue->Length > 0) {
			
			std::string nmeafolder = Application->StartupPath + "\\NMEA";

			if (!My::Computer::FileSystem::DirectoryExists(nmeafolder)) {
				
				try {
					
					My::Computer::FileSystem::CreateDirectory(nmeafolder);
				}

				catch (std::runtime_error &ex) {

				}
			
			}

			std::string nmeafile = nmeafolder + "\\" + std::to_string(DateTime::Now.Year) + DateTime::Now.Month.ToString("00") + DateTime::Now.Day.ToString("00") + ".txt";

			try{
			
				My::Computer::FileSystem::WriteAllText(nmeafile, RecordQueue, true);
				RecordQueue = ""; //Clear that out
			}
			catch (const std::runtime_error &ex){
			
			}	
		}
	
	
	}

	std::string GPS::GenerateGPGGAcode() {
		
		double posnum = 0;
		double minutes = 0;

		DateTime UTCTime = DateTime::UtcNow;

		std::string mycode = "GPGGA";
		
		if (UTCTime.Hour < std::stoi("10")) {
		
			mycode = mycode + "0";
		
		}

		mycode = mycode + std::to_string(UTCTime.Hour);

		if (UTCTime.Minute < std::stoi("10")) {
		
			mycode = mycode + "0";
		}
		
		mycode = mycode + std::to_string(UTCTime.Minute);
		if (UTCTime.Second < std::stoi("10")){
		
			mycode = mycode + "0";
		}

		mycode = mycode + std::to_string(UTCTime.Second);
		mycode = mycode + ",";

		posnum = std::abs(MainForm::NTRIPManualLat);
		minutes = posnum % 1;
		posnum = posnum - minutes;
		minutes = minutes * 60;
		posnum = (posnum * 100) + minutes;
		//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		mycode = mycode + posnum.ToString("0000.00####");

		if (MainForm::NTRIPManualLat > 0){
		
			mycode = mycode + ",N,";
		}
		else{
		
			mycode = mycode + ",S,";
		}

		posnum = std::abs(MainForm::NTRIPManualLon);
		minutes = posnum % 1;
		posnum = posnum - minutes;
		minutes = minutes * 60;
		posnum = (posnum * 100) + minutes;
		//VB TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		mycode = mycode + posnum.ToString("00000.00####");

		if (MainForm::NTRIPManualLon > 0){
		
			mycode = mycode + ",E,";
		}
		else{
		
			mycode = mycode + ",W,";
		}


		mycode = mycode + "4,10,1,200,M,1,M,";

		mycode = mycode + std::to_string((DateTime::Now.Second % 6) + 3) + ",0";
		mycode = "$" + mycode + "*" + CalculateChecksum(mycode); //Add checksum data
		return mycode;
	} */  	

	std::string GPS::CalculateChecksum(std::string &sentence) {
	
		
		int Checksum = 0;

		for (auto Character : sentence) {
		
			switch (Character) {
			
			case '$':
				break;
			
			case '*':

				goto ExitLabel1;


			default:
				if (Checksum == 0) {
					
					Checksum = static_cast<unsigned char>(Character);
				}

				else {
				
					Checksum = Checksum ^ static_cast<unsigned char>(Character);
					
				}
				break;
			
			}
		
		}
		ExitLabel1;

		return Checksum.ToString("X2");
	
	
	}
}

