void startPdv(const std::string& DATA_FILENAME) {

	const size_t TMP_DATA_FILE_SIZE = std::filesystem::file_size(DATA_FILENAME);

	constexpr uint_fast32_t MAX_FILE_SIZE = 5242880;
	
	if (TMP_DATA_FILE_SIZE > MAX_FILE_SIZE) {
		std::cerr << "\nFile Size Error: Data file exceeds the maximum limit of 5MB.\n\n";
    		std::exit(EXIT_FAILURE);
	}

	std::ifstream data_file_ifs(DATA_FILENAME, std::ios::binary);

	if (!data_file_ifs) {
		std::cerr << "\nRead File Error: Unable to read data file.\n\n";
		std::exit(EXIT_FAILURE);
	}

	std::vector<uint_fast8_t>Data_Vec((std::istreambuf_iterator<char>(data_file_ifs)), std::istreambuf_iterator<char>());  
	
	const uint_fast32_t DATA_FILE_SIZE = static_cast<uint_fast32_t>(Data_Vec.size());

	// Convert user data file size value to a string. 
	// This string will be inserted into the '$fl' (File Length) variable for the embedded PowerShell (PS) script.
	std::string data_size_length_string = std::to_string(DATA_FILE_SIZE),
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
		uint_fast32_t 
			pwsh_insert_index[5] = { DATA_FILE_SIZE + 2806, DATA_FILE_SIZE + 2413, DATA_FILE_SIZE + 2406, DATA_FILE_SIZE + 2303, DATA_FILE_SIZE + 2296 },
			name_length = static_cast<uint_fast32_t>(DATA_FILENAME.length()),
			dot_pos = static_cast<uint_fast32_t>(DATA_FILENAME.find_last_of('.'));

		if (dot_pos == 0 || dot_pos > name_length) {
			file_ext = ".exe";
		} else {
			file_ext = name_length - dot_pos < 5 ? DATA_FILENAME.substr(dot_pos, (name_length - dot_pos)) : DATA_FILENAME.substr(dot_pos, 5);
			std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), [](uint_fast8_t c) {return std::tolower(c); });
		}
		// If user file_ext is Python, PowerShell or an Executable, you can add command-line arguments for these files, if required.
		if (file_ext == ".py" || file_ext == ".ps1" || file_ext == ".exe") {
			
			const uint_fast8_t MAX_ARGS_LENGTH = 100;  

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
			XOR_KEY_START_INDEX = 2252,	// This is the start index location of the 4 byte CRC value, used for basic xor encryption.
			XOR_KEY_END_INDEX = 2257;   	// End location for our 5 byte xor key (include one extra byte after CRC value). 

		uint_fast8_t bits = 32;		

		// Write a 4 byte CRC value at index location.
		valueUpdater(Image_Vec, XOR_KEY_START_INDEX, crcUpdate(&Data_Vec[0], static_cast<uint_fast32_t>(Data_Vec.size())), bits);

		uint_fast16_t xor_key_pos = XOR_KEY_START_INDEX;
				
		// Index location within image file for inserting user data file.
		const uint_fast16_t DATA_FILE_INSERT_INDEX = 2251;

		std::cout << "\nEncrypting and embedding data file within PNG image.\n";

		std::vector<uint_fast8_t>Encrypt_Vec;

		for (uint_fast32_t index = 0; index < DATA_FILE_SIZE; index++) {

			Encrypt_Vec.emplace_back(Data_Vec[index] ^ Image_Vec[xor_key_pos++]);
			xor_key_pos = xor_key_pos == XOR_KEY_END_INDEX ? XOR_KEY_START_INDEX : xor_key_pos;
		}

		Image_Vec.insert((Image_Vec.begin() + DATA_FILE_INSERT_INDEX), Encrypt_Vec.begin(), Encrypt_Vec.end());

		const std::string BLOCK_ID = "#>"; // Problem characters.

		uint_fast32_t repair_code_length = 0;

		// The data file CANNOT contain any occurrence of the PowerShell end-comment-block string "#>" (0x23, 0x3E).
		// The embedded PowerShell script will break if it encounters the end-comment-block string anywhere in the data file. 

		// The C++ program will check the user file for these characters. If found it will modify a byte for every occurrence of "#>".
		// It will store index locations of each modified byte in a larger string containing PowerShell array code, 
		// which will put back the original "#" character that was changed, before writing the extracted user file to disk.
	
		// Search user file for end-comment-block string.
		if (DATA_FILE_SIZE + DATA_FILE_INSERT_INDEX >= std::search(Image_Vec.begin(), Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin()) {
			std::string index_locations = "";

			for (uint_fast32_t block_index = 0; DATA_FILE_SIZE + DATA_FILE_INSERT_INDEX > block_index;) {
				if (block_index > 0) {
					// Build up a string containing index locations for every modified end-comment-block string found within data file.
					index_locations += std::to_string(block_index) + ',';
					Image_Vec[block_index] = 'X';  // Change the '#' character of each end-comment-block string.
				}
				block_index = static_cast<uint_fast32_t>(std::search(Image_Vec.begin() + block_index + 1, Image_Vec.end(), BLOCK_ID.begin(), BLOCK_ID.end()) - Image_Vec.begin());
			}

			// String contains PowerShell 'repair code' to be inserted into the embedded script within the PNG image.
			// PS array '$idx=@("<index_locations>")' will contain index locations for each byte that requires setting back to the original '#' character.
			// Index location values are created by this C++ program.
			// PowerShell 'while loop', changes the modified byte back to the original '#' character, for each index location.
			std::string repair_code = "$i=-1;$idx=@(" + index_locations + ");while(++$i -lt $idx.count){$bt[$idx[$i]]=0x23};"; // PowerShell code in string.

			// Remove the last ',' character from 'repair code' string as its not required.
			repair_code.erase(remove(repair_code.end() - 51, repair_code.end(), ','), repair_code.end());

			// We need this length value for when we later update the hIST chunk size.
			repair_code_length += static_cast<uint_fast32_t>(repair_code.length());

			// Insert PowerShell 'repair code' string into the embedded PowerShell script at correct index location within default image file.
			Image_Vec.insert(Image_Vec.begin() + pwsh_insert_index[0], repair_code.begin(), repair_code.end());
		}

		// Insert Linux & Windows argument strings into the PowerShell script variables '$la' & '$wa', at index location within default image file.
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
		// Get first IDAT chunk index.
		uint_fast32_t 
			hist_chunk_length = 1271,
			hist_chunk_crc_insert_index = static_cast<uint_fast32_t>(std::search(Image_Vec.begin()+ hist_chunk_length_insert_index, Image_Vec.end(), IDAT_SIG.begin(), IDAT_SIG.end()) - Image_Vec.begin()) - 8;  
	
		// Add to the current hIST chunk value the length values of all the data items we have inserted into the PowerShell script.
		// This should now be the correct and final hIST chunk length.
		hist_chunk_length += DATA_FILE_SIZE + repair_code_length + static_cast<uint_fast32_t>(file_ext.length() + data_size_length_string.length() + args_linux.length() + args_windows.length());

		valueUpdater(Image_Vec, hist_chunk_length_insert_index, hist_chunk_length, bits);
		valueUpdater(Image_Vec, hist_chunk_crc_insert_index, crcUpdate(&Image_Vec[hist_chunk_length_insert_index + 4], hist_chunk_length + 4), bits);

		const uint_fast8_t SLASH_POS = static_cast<uint_fast8_t>(DATA_FILENAME.find_last_of("\\/")) + 1;

		// Create filename for output file, e.g. "my_music_file.mp3" = "pdv_my_music_file.mp3.png"
		const std::string OUTPUT_NAME = DATA_FILENAME.substr(0, SLASH_POS) + "pdv" + "_" + DATA_FILENAME.substr(SLASH_POS, DATA_FILENAME.length()) + ".png";

		std::ofstream write_file_ofs(OUTPUT_NAME, std::ios::binary);
		if (!write_file_ofs) {
			std::cerr << "\nWrite Error: Unable to write to file.\n\n";
			std::exit(EXIT_FAILURE);
		}
		write_file_ofs.write((char*)&Image_Vec[0], Image_Vec.size());

		std::cout << "\nCreated output file " << "\"" << OUTPUT_NAME << "\" " << Image_Vec.size() << " bytes." << "\n\nAll Done.\n\n";

		if (Image_Vec.size() > MAX_FILE_SIZE) {
			std::cout << "Warning: Embedded image size exceeds 5MB.\n\nX/Twitter will convert this PNG image to JPG. Embedded content will be lost.\n\n";
		} 	
	}
