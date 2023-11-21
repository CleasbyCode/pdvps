// PNG Data Vehicle, PowerShell Edition (PDVPS v1.1) for Twitter & Reddit. Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.

// Compile program (Linux): $ g++ pdvps.cpp -O2 -s -o pdvps
// Run program: ./pdvps

#include <fstream>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "Image_Vec.hpp"	// Default PNG image file, with embedded PowerShell script for file extraction.
				// https://github.com/CleasbyCode/pdvps/blob/main/src/scripts_info.txt
typedef unsigned char BYTE;

void Begin_Pdvps(char* []);

// Update values, such as chunk lengths, crc, file sizes and other values. Writes them into the relevant vector index locations.
void Value_Updater(std::vector<BYTE>&, size_t, const size_t&);

uint_fast64_t
	// Code to compute CRC32 for the "hIST" chunk (required for Reddit) within this program, is taken from: https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix 
	Crc_Update(const uint_fast64_t&, BYTE*, const uint_fast64_t&),
	Crc(BYTE*, const uint_fast64_t&);

int main(int argc, char** argv) {

	if (argc == 2 && std::string(argv[1]) == "--info") {
		std::cout << "\nPNG Data Vehicle, PowerShell Edition (PDVPS v1.1). Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.\n\n"
			<< "Embed any file type within a PNG image to create a tweetable and 'executable' image.\n\n"
			<< "Post your file-embedded PNG image on Twitter (5MB max.) and Reddit (20MB max).\n"
			<< "For further information, please visit https://github.com/CleasbyCode/pdvps\n\n";
	}
	else if (argc == 2) {
		Begin_Pdvps(argv);
	}
	else {
		std::cout << "\nUsage: pdvps <your_file>\n\t\bpdvps --info\n\n";
	}
	return 0;
}

void Begin_Pdvps (char* argv[]) {

		const std::string DATA_FILE = argv[1];

		std::ifstream read_data_fs(DATA_FILE, std::ios::binary);

		if (!read_data_fs) {
			// Open file failure, display relevant error message and exit program.
			std::cerr << "\nRead Error: Unable to open/read file: \"" + DATA_FILE + "\"\n\n";
			std::exit(EXIT_FAILURE);
		}

		// Read in user's data file, store in vector "Data_Vec".
		std::vector<BYTE> Data_Vec((std::istreambuf_iterator<char>(read_data_fs)), std::istreambuf_iterator<char>());  

		const size_t DATA_SIZE = Data_Vec.size();

		// Convert user data file size value to a string. 
		// This string will be inserted into the '$fl' (File Length) variable for the embedded PowerShell (PS) script.
		std::string data_size_length_string = std::to_string(DATA_SIZE),

			args_linux,	// Store user command-line arguments for Linux in this string variable.
					// String will be inserted into '$la' (Linux Arguments) variable for the embedded PS script.
	
			args_windows,	// Store user command-line arguments for Windows in this string variable.
					// String will be inserted into '$wa' (Windows Arguments) variable for the embedded PS script.

			file_ext;	// Store extension of user file in this string variable.
					// String will be inserted into '$ext' (extension) variable for the embedded PS script.
		
			// Int array contains index locations for inserting data items into the embedded PowerShell (PS) script, within the PNG image.
			// [0] value is the insert index location for the PS 'repair code' strings (see below).
			// [1] "" '$la' Linux Arguments string variable.
			// [2] "" '$wa' Windows Arguments string variable.
			// [3] "" '$ext' File Extension of user data file, string variable.
			// [4] "" '$fl', File Length value of user data file, variable.
			// For more information about the PowerShell script, see the "scripts_info.txt" file that's part of this repo on GitHub. 
		size_t 
			pwsh_insert_index[5] = { DATA_SIZE + 2806, DATA_SIZE + 2413, DATA_SIZE + 2406, DATA_SIZE + 2303, DATA_SIZE + 2296 },
			name_length = DATA_FILE.length(),
			dot_pos = DATA_FILE.find_last_of('.');

		// If user file does not have an extension, default to '.exe'.
		if (dot_pos == 0 || dot_pos > name_length) {
			file_ext = ".exe";
		}
		// Get extension for user filename and store it in the 'ext' string variable. Used in the PowerShell script as '$ext' variable.
		else {
			file_ext = name_length - dot_pos < 5 ? DATA_FILE.substr(dot_pos, (name_length - dot_pos)) : DATA_FILE.substr(dot_pos, 5);
			std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), [](BYTE c) {return std::tolower(c); });
		}
		// If user data file ext is Python, PowerShell or an Executable (.exe), you can add command-line arguments for these files, if required.
		if (file_ext == ".py" || file_ext == ".ps1" || file_ext == ".exe") {
			
			const uint_fast8_t MAX_ARGS_LENGTH = 100;  // Set a character length limit for command-line arguments.

			std::cout << "\nFor this file type you can provide command-line arguments here, if required.\n\nLinux: ";
			std::getline(std::cin, args_linux);
			std::cout << "\nWindows: ";
			std::getline(std::cin, args_windows);
			if (args_linux.length() > MAX_ARGS_LENGTH || args_windows.length() > MAX_ARGS_LENGTH) {
				std::cerr << "\nError: Arguments exceed maximum length.\n\n";
				std::exit(EXIT_FAILURE);
			}
		}
		
		const uint_fast16_t
			XOR_KEY_START_INDEX = 2252,	// This is the start index location of the 4 byte crc value, used for basic xor encryption.
			XOR_KEY_END_INDEX = 2257;   	// End location for our 5 byte xor key (include one extra byte after crc value). 
		
		// Write a 4 byte crc value at index location.
		Value_Updater(Image_Vec, XOR_KEY_START_INDEX, Crc(&Data_Vec[0], Data_Vec.size()));

		uint_fast16_t xor_key_pos = XOR_KEY_START_INDEX;
				
		// Index location within image file for inserting user data file.
		const uint_fast16_t DATA_FILE_INSERT_INDEX = 2251;

		std::cout << "\nEncrypting and embedding data file within PNG image.\n";

		std::vector<BYTE>Encrypt_Vec;
	
		// Read in user data file into vector "Encrypt_Vec" and encrypt each byte using simple XOR method. 
		for (size_t index = 0; index < DATA_SIZE; index++) {

			Encrypt_Vec.emplace_back(Data_Vec[index] ^ Image_Vec[xor_key_pos++]);
			xor_key_pos = xor_key_pos == XOR_KEY_END_INDEX ? XOR_KEY_START_INDEX : xor_key_pos;
		}

		// Insert encrypted data file into vector "Image_Vec".
		Image_Vec.insert((Image_Vec.begin() + DATA_FILE_INSERT_INDEX), Encrypt_Vec.begin(), Encrypt_Vec.end());

		std::cout << "\nOK!\n";

		const std::string BLOCK_ID = "#>"; // Bad string
		size_t repair_code_length = 0;

		// The data file CANNOT contain any occurrence of the PowerShell End-Comment-Block string "#>" (0x23, 0x3E).
		// The embedded PowerShell script will break if it encounters the End-Comment-Block string anywhere in the data file. 

		// The C++ program will check the user file for this string. If found it will modify a byte for every occurrence of "#>".
		// It will store index locations of each modified byte in a larger string containing PowerShell array code, 
		// which will put back the original "#" character that was changed, before writing the extracted user file to disk.
	
		// Search user file for end-comment-block string.
		if (DATA_SIZE + DATA_FILE_INSERT_INDEX >= std::search(Image_Vec.begin(), Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin()) {
			std::string index_locations = "";

			for (size_t block_index = 0; DATA_SIZE + DATA_FILE_INSERT_INDEX > block_index;) {
				if (block_index > 0) {
					// Build up a string containing index locations for every modified end-comment-block string found within data file.
					index_locations += std::to_string(block_index) + ',';
					Image_Vec[block_index] = 'X';  // Change the '#' character of each end-comment-block string.
				}
				block_index = search(Image_Vec.begin() + block_index + 1, Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin();
			}

			// String contains PS 'repair code' to be inserted into the embedded PS script within the PNG image.
			// PS array '$idx=@("<index_locations>")' will contain index locations for each byte that requires setting back to the original '#' character.
			// Index location values are created by this C++ program, see the 'for loop' above.
			// PowerShell 'while loop', changes the modified byte back to the original '#' character, for each index location.
			std::string repair_code = "$i=-1;$idx=@(" + index_locations + ");while(++$i -lt $idx.count){$bt[$idx[$i]]=0x23};"; // PowerShell code in string.

			// Remove the last ',' character from 'repair code' string as its not required.
			repair_code.erase(remove(repair_code.end() - 51, repair_code.end(), ','), repair_code.end());

			// We need this length value for when we later update the hIST chunk size.
			repair_code_length += repair_code.length();

			// Insert PowerShell 'repair code' string into the embedded PowerShell script at correct index location within image file.
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[0], repair_code.begin(), repair_code.end());
		}

		// Insert Linux & Windows argument strings into the PowerShell script variables '$la' & '$wa', at index location within image file.
		if (args_linux.length() || args_windows.length()) {
			args_linux.insert(0, "\x20"), args_windows.insert(0, "\x20");
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[1], args_linux.begin(), args_linux.end());
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[2], args_windows.begin(), args_windows.end());
		}

		// Insert file extension string into PowerShell script variable '$ext' at index location within image file.
		Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[3], file_ext.begin(), file_ext.end());

		// Insert file size string of user file into PowerShell script variable '$fl' at index location within image file.
		Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[4], data_size_length_string.begin(), data_size_length_string.end());

		const std::string IDAT_SIG = "IDAT";

		// Index location in image file for hIST chunk length field. 
		uint_fast16_t hist_chunk_length_insert_index = 2242;	

		// Initial hIST chunk size. This will grow as additional data items are inserted into the PowerShell script.
		size_t hist_chunk_length = 1271; 

		// Get first IDAT chunk index.
		size_t hist_chunk_crc_insert_index = std::search(Image_Vec.begin()+ hist_chunk_length_insert_index, Image_Vec.end(), IDAT_SIG.begin(), IDAT_SIG.end()) - Image_Vec.begin() - 8;  // - 8 is to position the index at the start of hIST chunk's crc field.
	
		// Add to the current hIST chunk value the length values of all the data items we have inserted into the PowerShell script.
		// This should now be the correct and final hIST chunk length.
		hist_chunk_length += DATA_SIZE + repair_code_length + file_ext.length() + data_size_length_string.length() + args_linux.length() + args_windows.length();

		Value_Updater(Image_Vec, hist_chunk_length_insert_index, hist_chunk_length);
		Value_Updater(Image_Vec, hist_chunk_crc_insert_index, Crc(&Image_Vec[hist_chunk_length_insert_index + 4], hist_chunk_length + 4));

		const size_t SLASH_POS = DATA_FILE.find_last_of("\\/") + 1;

		// Create filename for output file, e.g. "my_music_file.mp3" = "pdv_my_music_file.mp3.png"
		const std::string OUTPUT_NAME = DATA_FILE.substr(0, SLASH_POS) + "pdv" + "_" + DATA_FILE.substr(SLASH_POS, DATA_FILE.length()) + ".png";

		std::ofstream write_file_fs(OUTPUT_NAME, std::ios::binary);
		if (!write_file_fs) {
			std::cerr << "\nWrite Error: Unable to write to file.\n\n";
			std::exit(EXIT_FAILURE);
		}
		write_file_fs.write((char*)&Image_Vec[0], Image_Vec.size());

		std::cout << "\nCreated output file " << "\"" << OUTPUT_NAME << "\" " << Image_Vec.size() << " bytes." << "\n\nAll Done.\n\n";

		if (Image_Vec.size() > 5242880 && Image_Vec.size() < 20971521) {
			std::cout << "Warning: Embedded image size exceeds 5MB.\n\nTwitter will convert this PNG image to JPG. Embedded content will be lost.\n\n";
		}
		else if (Image_Vec.size() > 20971520) {
			std::cout << "Warning: Embedded image size exceeds 20MB.\n\nReddit will refuse to upload this image & Twitter will convert it to JPG.\n\n";
		}
		else {
			std::cout << "You can post this embedded image on both Twitter and Reddit.\n\n";
		}	
	}

	void Value_Updater(std::vector<BYTE>& vec, size_t value_insert_index, const size_t& VALUE) {
		uint_fast8_t bits = 32;
		while (bits) {
			vec[value_insert_index++] = (VALUE >> (bits -= 8)) & 0xff;
		}
	}

// The following code (slightly modified) to compute CRC32 the hIST chunk, was taken from: https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix 
uint_fast64_t Crc_Update(const uint_fast64_t& Crc, BYTE* buf, const uint_fast64_t& len)
{
	// Table of CRCs of all 8-bit messages.
	const size_t Crc_Table[256]{
		0x00, 	    0x77073096, 0xEE0E612C, 0x990951BA, 0x76DC419,  0x706AF48F, 0xE963A535, 0x9E6495A3, 0xEDB8832,  0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x9B64C2B,  0x7EB17CBD,
		0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
		0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x1DB7106,  0x98D220BC, 0xEFD5102A, 0x71B18589, 0x6B6B51F,
		0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0xF00F934,  0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x86D3D2D,  0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 0x4DB26158, 0x3AB551CE,
		0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
		0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x3B6E20C,  0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x4DB2615,  0x73DC1683, 0xE3630B12, 0x94643B84, 0xD6D6A3E,  0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0xA00AE27,  0x7D079EB1, 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0,
		0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703,
		0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x26D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x5005713,  0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0xCB61B38,  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0xBDBDF21,  0x86D3D2D4, 0xF1D4E242,
		0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5,
		0x47B2CF7F, 0x30B5FFE9, 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D };

	// Update a running CRC with the bytes buf[0..len - 1] the CRC should be initialized to all 1's, 
	// and the transmitted value is the 1's complement of the final running CRC (see the crc() routine below).
	uint_fast64_t c = Crc;
	uint_fast32_t n;

	for (n = 0; n < len; n++) {
		c = Crc_Table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	}
	return c;
}

// Return the CRC of the bytes buf[0..len-1].
uint_fast64_t Crc(BYTE* buf, const uint_fast64_t& len)
{
	return Crc_Update(0xffffffffL, buf, len) ^ 0xffffffffL;
}
