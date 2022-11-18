// PNG Data Vehicle for Twitter, PowerShell Edition (PDVPS) v1.0.Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.

#include <fstream>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "Image_with_Scripts_Vec.hpp"

using namespace std::chrono;

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--info") {
		std::cout << "\nPNG Data Vehicle for Twitter, PowerShell Edition (PDVPS) v1.0. Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.\n\n"
			<< "PDVPS enables you to embed any file (max file size 5,233,664 bytes) within a tweetable and 'executable' PNG image.\n\n"
			<< "For further information, please visit https://github.com/CleasbyCode/pdvps\n\n";
	}
	else if (argc < 2 || argc > 2) {
		std::cout << "\nUsage:  pdvps  <your_file>\n\tpdvps  --info\n\n";
	}
	else {
		const std::string DATA_FILE = argv[1];

		std::ifstream readData(DATA_FILE, std::ios::binary);
		if (!readData) {
			std::cerr << "\nData File Read Error : Unable to open/read file : " + DATA_FILE + "\n\n";
			return -1;
		}

		// 5MB (Twitter PNG size limit) - 9KB overhead for default image with embedded scripts = 5,233,664 bytes available for user data file.
		const int MAX_FILE_SIZE_BYTES = 5233664;

		// Create a unique string from Time. This 5 byte string will be embedded within the image file.
		// The string will be used as part of the XOR process to 'encrypt/decrypt' user data file.
		ptrdiff_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		std::string tmpNum = std::to_string(ms);
		tmpNum = std::string(tmpNum.rbegin(), tmpNum.rend());
		std::string xorNum = tmpNum.substr(0, 5);

		// Get and check size of user data file. Display error and quit program if file exceeds size limit.
		readData.seekg(0, readData.end);
		const ptrdiff_t DATA_SIZE = readData.tellg();

		if (DATA_SIZE > MAX_FILE_SIZE_BYTES) {
			std::cerr << "\nData File Size Error: File must not exceed size limit of 5,234,608 bytes.\n\n";
			return -1;
		}

		// Convert user data file size value to a string. 
		// This string will be inserted into the '$fl' (File Length) variable for the embedded PowerShell (PS) script.
		std::string dataSizeLengthString = std::to_string(DATA_SIZE),

		argsLinux,	// Store user command-line arguments for Linux in this string variable.
				// String will be inserted into '$la' (Linux Arguments) variable for the embedded PS script.

		argsWindows,	// Store user command-line arguments for Windows in this string variable.
				// String will be inserted into '$wa' (Windows Arguments) variable for the embedded PS script.

		ext;	// Store extension of user file in this string variable.
			// String will be inserted into '$ext' (extension) variable for the embedded PS script.

		// Int array contains index locations for inserting data items into the embedded PowerShell (PS) script, within the PNG image.
		// [0] value is the insert index location for the PS 'repair code' strings (see below).
		// [1] "" '$la' Linux Arguments string variable.
		// [2] "" '$wa' Windows Arguments string variable.
		// [3] "" '$ext' File Extension of user data file, string variable.
		// [4] "" '$fl', File Length value of user data file, variable.
		// For more information about the PowerShell script, see the "scripts_info.txt" file that's part of this repo on GitHub. 
		int pwshInsertIndex[5] = { DATA_SIZE + 2820, DATA_SIZE + 2427, DATA_SIZE + 2420, DATA_SIZE + 2317, DATA_SIZE + 2310 },

			nameLength = DATA_FILE.length(),
			dot = DATA_FILE.find_last_of('.'),
			histChunkLengthInsertIndex = 2257,	// Index location in image file for hIST chunk length field. 
			histChunkLength = 1360,			// Initial hIST chunk size. This will grow as additional data items are inserted into the PS script.
			bits = 24;

		// If user file does not have an extension, assume it's an executable file, ext will be set to '.exe'.
		if (dot < 0 || nameLength - dot == 1) {
			ext = ".exe";
		}
		// Get extension for user filename and store it in the 'ext' string variable. Used in the PowerShell script as '$ext' variable.
		else {
			ext = nameLength - dot < 5 ? DATA_FILE.substr(dot, (nameLength - dot)) : DATA_FILE.substr(dot, 5);
			for_each(ext.begin(), ext.end(), [](char& c) {
				c = ::tolower(c);
				});
		}
		// If user data file is Python, PowerShell or an Executable (.exe), you can add command-line arguments for these files, if required.
		if (ext == ".py" || ext == ".ps1" || ext == ".exe") {
			const int MAX_ARGS_LENGTH = 100;  // Set a character length limit for command-line arguments.
			std::cout << "\nFor this file type you can provide command-line arguments here, if required.\n\nLinux: ";
			std::getline(std::cin, argsLinux);
			std::cout << "\nWindows: ";
			std::getline(std::cin, argsWindows);
			if (argsLinux.length() > MAX_ARGS_LENGTH || argsWindows.length() > MAX_ARGS_LENGTH) {
				std::cerr << "\nError: Arguments exceed maximum length.\n\n";
				return -1;
			}
		}

		int i = 0,
		pdvpsXorInsertIndex = 2266;	// Insert location index within image file for the 5 byte xorNum string.

		// Insert xorNum string into image file.
		while (pdvpsXorInsertIndex != 2272) ImageVec.at(pdvpsXorInsertIndex++) = xorNum[i++];

		char byte;

		int j = 704,	// Set var j to the 704th index location of image file.
		k = 3,		// Set var k to the 3rd index location of image file
		x = 2266;	// Set var x to the 2266th index location of image file. This is the start location of the xorNum time string.

		std::ifstream readFile(DATA_FILE, std::ios::binary);

		// Index location within image file for inserting user data file.
		const int DATA_FILE_INSERT_INDEX = 2265;

		// Read in user data file into above index location of default PNG image and 'encrypt' file using simple XOR method. 
		for (int i = 0; DATA_SIZE > i; i++) {
			// Read byte from user data file, 
			// XOR the byte against the character at index location j, index location k, index location x;
			// Insert the scrambled byte into PNG file index location. Repeated until end of user file.
			byte = readFile.get();
			ImageVec.insert((ImageVec.begin() + DATA_FILE_INSERT_INDEX) + i, byte ^ ImageVec[j] ^ ImageVec[k] ^ ImageVec[x + i]);
			
			// Decrement j until byte index location 2 then reset back to 704th byte image file index location. Repeat until EOF.
			// Increment k until byte index location 705, then reset back to 3rd byte image file index location. Repeat until EOF.
			// Increment x through each byte of the 5 byte xorNum time string. Repeat until EOF.
			j--; j = j == 2 ? 704 : j;	
			k++; k = k == 705 ? 3 : k;	
			x++; x = x == 2272 ? 2266 : x; 	
		}

		// The user file CANNOT contain any occurrence of the PowerShell End-Comment-Block string '#>'.
		// The embedded PowerShell script will break if it encounters the end-comment-block string anywhere in the user file. 

		// The C++ program will check the user file for this string. If found it will modify a byte for every occurrence of '#>'.
		// It will store index locations of each modified byte in a larger string containing PowerShell code, 
		// which will put back the original '#' character that was changed, before writing the extracted user file to disk.
		const std::string BLOCK_ID = "#>";
		int pwshRepairCodeLength = 0;

		// Search user file for end-comment-block string.
		if (search(ImageVec.begin(), ImageVec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - ImageVec.begin() <= DATA_SIZE + DATA_FILE_INSERT_INDEX) {
			std::string index_locations = "";
			for (int BLOCK_INDEX = 0; DATA_SIZE + DATA_FILE_INSERT_INDEX > BLOCK_INDEX;) {
				if (BLOCK_INDEX > 0) {
					// Build up a string containing index locations for every modified end-comment-block string found within user file.
					index_locations += std::to_string(BLOCK_INDEX) + ',';
					ImageVec[BLOCK_INDEX] = 'X';  // Change the '#' character of each end-comment-block string.
				}
				BLOCK_INDEX = search(ImageVec.begin() + BLOCK_INDEX + 1, ImageVec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - ImageVec.begin();
			}

			// String contains PS 'repair code' to be inserted into the embedded PS script within the PNG image.
			// PS array '$idx=@("<index_locations>")' will contain index locations for each byte that requires setting back to the original '#' character.
			// Index location values are created by the this C++ program, see the 'for loop' above.
			// PowerShell 'while loop', changes the modified byte back to the original '#' character, for each index location.
			std::string repairCode = "$i=-1;$idx=@(" + index_locations + ");while(++$i -lt $idx.count){$bt[$idx[$i]]=0x23};";

			// Remove the last ',' character from 'repair code' string as its not required.
			repairCode.erase(remove(repairCode.end() - 51, repairCode.end(), ','), repairCode.end());
			
			// We need this length value for when we later update the hIST chunk size.
			pwshRepairCodeLength += repairCode.length();  

			// Insert PowerShell 'repair code' string into the embedded PowerShell script at correct index location within image file.
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[0], repairCode.begin(), repairCode.end());
		}

		// Insert Linux & Windows argument strings into the PowerShell script variables '$la' & '$wa', at index location within image file.
		if (argsLinux.length() > 0 || argsWindows.length() > 0) {
			argsLinux.insert(0, "\x20"), argsWindows.insert(0, "\x20");
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[1], argsLinux.begin(), argsLinux.end());
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[2], argsWindows.begin(), argsWindows.end());
		}

		// Insert file extension string into PowerShell script variable '$ext' at index location within image file.
		ImageVec.insert(ImageVec.begin() + pwshInsertIndex[3], ext.begin(), ext.end());

		// Insert file size string of user file into PowerShell script variable '$fl' at index location within image file.
		ImageVec.insert(ImageVec.begin() + pwshInsertIndex[4], dataSizeLengthString.begin(), dataSizeLengthString.end());

		// Add to the current hIST chunk value the length values of all the data items we have inserted into the PowerShell script.
		// This should now be the correct and final hIST chunk length.
		histChunkLength += DATA_SIZE + pwshRepairCodeLength + ext.length() + dataSizeLengthString.length() + argsLinux.length() + argsWindows.length();

		// Update the hIST chunk length value within the hIST chunk length field index location of image file.
		// Because the chunk length will never exceed 5MB, only 3 bytes of the 4 byte chunk length field will be used. (bits = 24).
		while (bits) ImageVec.at(histChunkLengthInsertIndex++) = (histChunkLength >> (bits -= 8)) & 0xff;

		const int SLASH_POS = DATA_FILE.find_last_of("\\/") + 1;

		// Create filename for output file, e.g. "my_music_file.mp3" = "pdv_my_music_file.mp3.png"
		std::string outFile = DATA_FILE.substr(0, SLASH_POS) + "pdv" + "_" + DATA_FILE.substr(SLASH_POS, DATA_FILE.length()) + ".png";

		std::ofstream writeFinal(outFile, std::ios::binary);
		if (!writeFinal) {
			std::cerr << "\nWrite Error: Unable to write to file.\n\n";
			return -1;
		}
		writeFinal.write((char*)&ImageVec[0], ImageVec.size());
		writeFinal.close();

		std::cout << "\nCreated output file " << "'" << outFile << "' " << ImageVec.size() << " bytes." << "\n\nAll Done!\n\n";
	}
	return 0;
}
