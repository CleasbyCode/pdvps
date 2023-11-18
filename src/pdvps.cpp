// PNG Data Vehicle for Twitter, PowerShell Edition (PDVPS v1.1). Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.

#include <fstream>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

typedef unsigned char BYTE;

using namespace std::chrono;

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--info") {
		std::cout << "\nPNG Data Vehicle for Twitter, PowerShell Edition (PDVPS v1.1). Created by Nicholas Cleasby (@CleasbyCode) 1/11/2022.\n\n"
			<< "PDVPS enables you to embed any file (max file size 5,233,664 bytes) within a tweetable and 'executable' PNG image.\n\n"
			<< "For further information, please visit https://github.com/CleasbyCode/pdvps\n\n";
	}
	else if (argc < 2 || argc > 2) {
		std::cout << "\nUsage:  pdvps  <your_file>\n\tpdvps  --info\n\n";
	}
	else {

		const std::string DATA_FILE = argv[1];

		std::ifstream 
			read_data_fs(DATA_FILE, std::ios::binary),
			read_default_image_fs("Default_Image.png", std::ios::binary);

		if (!read_data_fs || !read_default_image_fs) {
			// Open file failure, display relevant error message and exit program.
			const std::string
				READ_ERR_MSG = "\nRead Error: Unable to open/read file: ",
				ERR_MSG = !read_data_fs ? READ_ERR_MSG + "\"" + DATA_FILE + "\"\n\n" : READ_ERR_MSG + "\"" + "Default_Image.png" + "\"\n\n";

			std::cerr << ERR_MSG;
			std::exit(EXIT_FAILURE);
		}

		// Create a unique string from Time. This 5 byte string will be embedded within the image file.
		// The string will be used as part of the XOR process to 'encrypt/decrypt' user data file.
		size_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		std::string temp_num = std::to_string(ms);
		temp_num = std::string(temp_num.rbegin(), temp_num.rend());
		std::string xor_num = temp_num.substr(0, 5);

		std::vector<BYTE>
			Data_Vec((std::istreambuf_iterator<char>(read_data_fs)), std::istreambuf_iterator<char>()),  // Insert user image file into vector "Image_Vec".
			Image_Vec((std::istreambuf_iterator<char>(read_default_image_fs)), std::istreambuf_iterator<char>());

		const size_t
			DATA_SIZE = Data_Vec.size(),
			IMAGE_SIZE = Image_Vec.size();

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
			pwsh_insert_index[5] = { DATA_SIZE + 2802, DATA_SIZE + 2409, DATA_SIZE + 2402, DATA_SIZE + 2299, DATA_SIZE + 2292 },
			name_length = DATA_FILE.length(),
			dot_pos = DATA_FILE.find_last_of('.');

		// If user file does not have an extension, assume it's an executable file, ext will be set to '.exe'.
		if (dot_pos < 0 || name_length - dot_pos == 1) {
			file_ext = ".exe";
		}
		// Get extension for user filename and store it in the 'ext' string variable. Used in the PowerShell script as '$ext' variable.
		else {
			file_ext = name_length - dot_pos < 5 ? DATA_FILE.substr(dot_pos, (name_length - dot_pos)) : DATA_FILE.substr(dot_pos, 5);
			std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), [](char c) {return std::tolower(c); });
		}
		// If user data file is Python, PowerShell or an Executable (.exe), you can add command-line arguments for these files, if required.
		if (file_ext == ".py" || file_ext == ".ps1" || file_ext == ".exe") {
			
			const uint_fast8_t MAX_ARGS_LENGTH = 100;  // Set a character length limit for command-line arguments.

			std::cout << "\nFor this file type you can provide command-line arguments here, if required.\n\nLinux: ";
			std::getline(std::cin, args_linux);
			std::cout << "\nWindows: ";
			std::getline(std::cin, args_windows);
			if (args_linux.length() > MAX_ARGS_LENGTH || args_windows.length() > MAX_ARGS_LENGTH) {
				std::cerr << "\nError: Arguments exceed maximum length.\n\n";
				return -1;
			}
		}

		uint_fast16_t 
			image_xor_insert_index = 2248,	// Insert location index within image file for the 5 byte xorNum string.
			insert_limit = 2254;

		uint_fast8_t i = 0;

		// Insert xor_num string into image file.
		while (image_xor_insert_index != insert_limit) Image_Vec.at(image_xor_insert_index++) = xor_num[i++];

		uint_fast16_t
			j = 704,	// Set var j to the 704th index location of image file.
			k = 3,		// Set var k to the 3rd index location of image file
			x = 2248;	// Set var x to the 2266th index location of image file. This is the start location of the xorNum time string.

		// Index location within image file for inserting user data file.
		const uint_fast16_t DATA_FILE_INSERT_INDEX = 2247;

		size_t index = 0;

		// Read in user data file into above index location of default PNG image and 'encrypt' file using simple XOR method. 
		while (DATA_SIZE > index) {
			// XOR byte from the data file against the character at index location j, index location k, index location x;
			// Insert the scrambled byte into PNG file index location. Repeated until end of data file.
			Image_Vec.insert((Image_Vec.begin() + DATA_FILE_INSERT_INDEX) + index, Data_Vec[index] ^ Image_Vec[j] ^ Image_Vec[k] ^ Image_Vec[x + index]);
			index++;
			// Decrement j until byte index location 2 then reset back to 704th byte image file index location. Repeat until EOF.
			// Increment k until byte index location 705, then reset back to 3rd byte image file index location. Repeat until EOF.
			// Increment x through each byte of the 5 byte xorNum time string. Repeat until EOF.
			j--; j = j == 2 ? 704 : j;
			k++; k = k == 705 ? 3 : k;
			x++; x = x == 2254 ? 2248 : x;
		}

		// The user file CANNOT contain any occurrence of the PowerShell End-Comment-Block string '#>'.
		// The embedded PowerShell script will break if it encounters the end-comment-block string anywhere in the user file. 

		// The C++ program will check the user file for this string. If found it will modify a byte for every occurrence of '#>'.
		// It will store index locations of each modified byte in a larger string containing PowerShell code, 
		// which will put back the original '#' character that was changed, before writing the extracted user file to disk.
		const std::string BLOCK_ID = "#>";
		size_t repair_code_length = 0;

		// Search user file for end-comment-block string.
		if (DATA_SIZE + DATA_FILE_INSERT_INDEX >= std::search(Image_Vec.begin(), Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin()) {
			std::string index_locations = "";
			for (size_t BLOCK_INDEX = 0; DATA_SIZE + DATA_FILE_INSERT_INDEX > BLOCK_INDEX;) {
				if (BLOCK_INDEX > 0) {
					// Build up a string containing index locations for every modified end-comment-block string found within user file.
					index_locations += std::to_string(BLOCK_INDEX) + ',';
					Image_Vec[BLOCK_INDEX] = 'X';  // Change the '#' character of each end-comment-block string.
				}
				BLOCK_INDEX = search(Image_Vec.begin() + BLOCK_INDEX + 1, Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin();
			}

			// String contains PS 'repair code' to be inserted into the embedded PS script within the PNG image.
			// PS array '$idx=@("<index_locations>")' will contain index locations for each byte that requires setting back to the original '#' character.
			// Index location values are created by the this C++ program, see the 'for loop' above.
			// PowerShell 'while loop', changes the modified byte back to the original '#' character, for each index location.
			std::string repair_code = "$i=-1;$idx=@(" + index_locations + ");while(++$i -lt $idx.count){$bt[$idx[$i]]=0x23};";

			// Remove the last ',' character from 'repair code' string as its not required.
			repair_code.erase(remove(repair_code.end() - 51, repair_code.end(), ','), repair_code.end());

			// We need this length value for when we later update the hIST chunk size.
			repair_code_length += repair_code.length();

			// Insert PowerShell 'repair code' string into the embedded PowerShell script at correct index location within image file.
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[0], repair_code.begin(), repair_code.end());
		}

		// Insert Linux & Windows argument strings into the PowerShell script variables '$la' & '$wa', at index location within image file.
		if (args_linux.length() > 0 || args_windows.length() > 0) {
			args_linux.insert(0, "\x20"), args_windows.insert(0, "\x20");
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[1], args_linux.begin(), args_linux.end());
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[2], args_windows.begin(), args_windows.end());
		}

		// Insert file extension string into PowerShell script variable '$ext' at index location within image file.
		Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[3], file_ext.begin(), file_ext.end());

		// Insert file size string of user file into PowerShell script variable '$fl' at index location within image file.
		Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[4], data_size_length_string.begin(), data_size_length_string.end());

		uint_fast16_t hist_chunk_length_insert_index = 2239;	// Index location in image file for hIST chunk length field. 
			
		size_t hist_chunk_length = 1360;			// Initial hIST chunk size. This will grow as additional data items are inserted into the PS script.
	
		// Add to the current hIST chunk value the length values of all the data items we have inserted into the PowerShell script.
		// This should now be the correct and final hIST chunk length.
		hist_chunk_length += DATA_SIZE + repair_code_length + file_ext.length() + data_size_length_string.length() + args_linux.length() + args_windows.length();

		uint_fast8_t bits = 24;

		// Update the hIST chunk length value within the hIST chunk length field index location of image file.
		// Because the chunk length will never exceed 5MB, only 3 bytes of the 4 byte chunk length field will be used. (bits = 24).
		while (bits) Image_Vec.at(hist_chunk_length_insert_index++) = (hist_chunk_length >> (bits -= 8)) & 0xff;

		const size_t SLASH_POS = DATA_FILE.find_last_of("\\/") + 1;

		// Create filename for output file, e.g. "my_music_file.mp3" = "pdv_my_music_file.mp3.png"
		const std::string OUTPUT_NAME = DATA_FILE.substr(0, SLASH_POS) + "pdv" + "_" + DATA_FILE.substr(SLASH_POS, DATA_FILE.length()) + ".png";

		std::ofstream write_file_fs(OUTPUT_NAME, std::ios::binary);
		if (!write_file_fs) {
			std::cerr << "\nWrite Error: Unable to write to file.\n\n";
			return -1;
		}
		write_file_fs.write((char*)&Image_Vec[0], Image_Vec.size());

		std::cout << "\nCreated output file " << "'" << OUTPUT_NAME << "' " << Image_Vec.size() << " bytes." << "\n\nAll Done!\n\n";
	}
	return 0;
}

