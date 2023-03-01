// PNG Data Vehicle for Twitter, PowerShell Edition (PDVPS v1.0). Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.

#include <fstream>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "ImageVec.hpp"

using namespace std::chrono;

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--info") {
		std::cout << "\nPNG Data Vehicle for Twitter, PowerShell Edition (PDVPS v1.0). Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.\n\n"
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
		
		const int MAX_FILE_SIZE_BYTES = 5233664;

		ptrdiff_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		std::string tmpNum = std::to_string(ms);
		tmpNum = std::string(tmpNum.rbegin(), tmpNum.rend());
		std::string xorNum = tmpNum.substr(0, 5);

		readData.seekg(0, readData.end);
		const ptrdiff_t DATA_SIZE = readData.tellg();

		if (DATA_SIZE > MAX_FILE_SIZE_BYTES) {
			std::cerr << "\nData File Size Error: File must not exceed size limit of 5,234,608 bytes.\n\n";
			return -1;
		}
		
		std::string 
			dataSizeLengthString = std::to_string(DATA_SIZE),
			argsLinux,	
			argsWindows,		
			ext;		
		ptrdiff_t 
			pwshInsertIndex[5] = { DATA_SIZE + 2820, DATA_SIZE + 2427, DATA_SIZE + 2420, DATA_SIZE + 2317, DATA_SIZE + 2310 },
			nameLength = DATA_FILE.length(),
			dot = DATA_FILE.find_last_of('.'),
			histChunkLengthInsertIndex = 2257,	
			histChunkLength = 1360,			
			bits = 24;
		
		if (dot < 0 || nameLength - dot == 1) {
			ext = ".exe";
		}
		else {
			ext = nameLength - dot < 5 ? DATA_FILE.substr(dot, (nameLength - dot)) : DATA_FILE.substr(dot, 5);
			std::transform(ext.begin(), ext.end(), ext.begin(), [](char c) {return std::tolower(c); });
			std::cout << '\n' << ext << '\n';
		}
	
		if (ext == ".py" || ext == ".ps1" || ext == ".exe") {
			const int MAX_ARGS_LENGTH = 100; 
			std::cout << "\nFor this file type you can provide command-line arguments here, if required.\n\nLinux: ";
			std::getline(std::cin, argsLinux);
			std::cout << "\nWindows: ";
			std::getline(std::cin, argsWindows);
			if (argsLinux.length() > MAX_ARGS_LENGTH || argsWindows.length() > MAX_ARGS_LENGTH) {
				std::cerr << "\nError: Arguments exceed maximum length.\n\n";
				return -1;
			}
		}

		int 
			i = 0,
			pdvpsXorInsertIndex = 2266;	
		
		while (pdvpsXorInsertIndex != 2272) ImageVec.at(pdvpsXorInsertIndex++) = xorNum[i++];

		char byte;

		int 
			j = 704,	
			k = 3,		
			x = 2266;	

		std::ifstream readFile(DATA_FILE, std::ios::binary);

		const int DATA_FILE_INSERT_INDEX = 2265;

		for (int i = 0; DATA_SIZE > i; i++) {
			byte = readFile.get();
			ImageVec.insert((ImageVec.begin() + DATA_FILE_INSERT_INDEX) + i, byte ^ ImageVec[j] ^ ImageVec[k] ^ ImageVec[x + i]);
			j--; j = j == 2 ? 704 : j;
			k++; k = k == 705 ? 3 : k;
			x++; x = x == 2272 ? 2266 : x;
		}
		
		const std::string BLOCK_ID = "#>";
		ptrdiff_t pwshRepairCodeLength = 0;
		
		if (search(ImageVec.begin(), ImageVec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - ImageVec.begin() <= DATA_SIZE + DATA_FILE_INSERT_INDEX) {
			std::string index_locations = "";
			for (ptrdiff_t BLOCK_INDEX = 0; DATA_SIZE + DATA_FILE_INSERT_INDEX > BLOCK_INDEX;) {
				if (BLOCK_INDEX > 0) {
					index_locations += std::to_string(BLOCK_INDEX) + ',';
					ImageVec[BLOCK_INDEX] = 'X';  
				}
				BLOCK_INDEX = search(ImageVec.begin() + BLOCK_INDEX + 1, ImageVec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - ImageVec.begin();
			}
			
			std::string repairCode = "$i=-1;$idx=@(" + index_locations + ");while(++$i -lt $idx.count){$bt[$idx[$i]]=0x23};";
			
			repairCode.erase(remove(repairCode.end() - 51, repairCode.end(), ','), repairCode.end());
			
			pwshRepairCodeLength += repairCode.length();
			
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[0], repairCode.begin(), repairCode.end());
		}
		
		if (argsLinux.length() > 0 || argsWindows.length() > 0) {
			argsLinux.insert(0, "\x20"), argsWindows.insert(0, "\x20");
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[1], argsLinux.begin(), argsLinux.end());
			ImageVec.insert(ImageVec.begin() + pwshInsertIndex[2], argsWindows.begin(), argsWindows.end());
		}

		
		ImageVec.insert(ImageVec.begin() + pwshInsertIndex[3], ext.begin(), ext.end());

		ImageVec.insert(ImageVec.begin() + pwshInsertIndex[4], dataSizeLengthString.begin(), dataSizeLengthString.end());
		
		histChunkLength += DATA_SIZE + pwshRepairCodeLength + ext.length() + dataSizeLengthString.length() + argsLinux.length() + argsWindows.length();

		while (bits) ImageVec.at(histChunkLengthInsertIndex++) = (histChunkLength >> (bits -= 8)) & 0xff;

		const ptrdiff_t SLASH_POS = DATA_FILE.find_last_of("\\/") + 1;
		
		std::string outFile = "pdvps_image.png";

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
