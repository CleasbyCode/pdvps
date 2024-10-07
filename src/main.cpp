// PNG Data Vehicle, PowerShell Edition (PDVPS v1.2) for X/Twitter. Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.

// Compile program (Linux): 
// $ g++ main.cpp -O2 -s -o pdvps
// $ sudo cp pdvps /usr/bin
// $ pdvps

#include "pdvps.h"

int main(int argc, char** argv) {
	if (argc !=2) {
		std::cout << "\nUsage: pdvps <your_file>\n\t\bpdvps --info\n\n";
	} else if (argc == 2 && std::string(argv[1]) == "--info") {
		displayInfo();
	} else {
		const std::string DATA_FILENAME = std::string(argv[1]);

		const std::regex REG_EXP("(\\.[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+)?[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+?(\\.[a-zA-Z0-9]+)?");
		
		if (regex_match(DATA_FILENAME, REG_EXP) && std::filesystem::exists(DATA_FILENAME) && std::filesystem::is_regular_file(DATA_FILENAME)) {
			startPdv(DATA_FILENAME);
		} else { 
			std::cerr << (!regex_match(DATA_FILENAME, REG_EXP)
				? "\nInvalid Input Error: Characters not supported by this program found within filename arguments"
				: "\nData File Error: File not found. Check the filename and try again")
			 << ".\n\n";

		std::exit(EXIT_FAILURE);
		}
	}
}
