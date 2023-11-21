# pdvps

**PNG Data Vehicle, PowerShell Edition for Twitter (5MB max.) & Reddit (20MB max.)**

Embed any file type within a tweetable and "[*executable*](https://github.com/CleasbyCode/pdvps#extracting-your-file-from-the-png-image)" **PNG** image.  

![Demo Image](https://github.com/CleasbyCode/pdvps/blob/main/Demo_Image/pdv_soldier.png)  
{*Encrypted **JPG** image embedded within this **PNG** image*}

Works for both **Windows** and ***Linux**. 

(***PowerShell** required. *Always use the [***latest version***](https://github.com/PowerShell/PowerShell/releases) of PowerShell*).

pdvps is similar to my other program [***pdvzip***](https://github.com/CleasbyCode/pdvzip). The two main differences in this edition are: 

**1)** *It uses a default **PNG** image for where your data file is encrypted & embedded.  
The image is small in size, which maximises the amount of data you can embed.*  

*Using a default image simplifies the program, with less code required.  
If you want to use different images for embedding files, then stick with [***pdvzip***](https://github.com/CleasbyCode/pdvzip).*

**2)** *There is a **PowerShell** script embedded within the default **PNG** image, that is used to decrypt,  
extract and open your embedded file, when you "[*execute*](https://github.com/CleasbyCode/pdvps/blob/main/README.md#extracting-your-file-from-the-png-image)" the **PNG** file.*

For more information on the embedded **scripts** within the default **PNG** image,  
see the [***scripts_info.txt***](https://github.com/CleasbyCode/pdvps/blob/main/src/scripts_info.txt) file supplied with this repo.

For information about **PNG** image settings and arbitrary data preservation for **Twitter**,  
visit the relevant link on [***pdvzip***](https://github.com/CleasbyCode/pdvzip#png-image-requirements-for-arbitrary-data-preservation)

Always use file extensions for your data file: my_doc.pdf, my_video.mp4, my_program.py, etc.

Compile and run the program under **Windows** or **Linux**.

## Usage (Linux)

```console
$ g++ pdvps.cpp -O2 -s -o pdvps
$ ./pdvps

Usage: pdvps <your_file>
       pdvps --info

$ ./pdvps my_document.pdf

Please wait...

Encrypting and embedding data file within PNG image.

OK!

Created output file ".\pdvps_image.png" 4232948 bytes.

All Done.

You can post this embedded image on both Twitter and Reddit.

```

Once your file has been embedded within the **PNG** image, it's ready to be shared (tweeted) or "*executed*" whenever you want to extract and open your embedded file.

## Extracting Your File From the PNG Image
**Linux (PowerShell required)**    
Make sure image file has executable permissions.
```console

$ chmod +x pdvps_image.png
$ ./pdvps_image.png 

```  
**Windows**   
First, rename the '*.png*' file extension to '*.cmd*'.
```console

G:\demo> ren pdvps_image.png pdvps_image.cmd
G:\demo> .\pdvps_image.cmd

```

[**Video Demo**](https://www.youtube.com/watch_popup?v=FCleYo9vJas)

My other programs you may find useful:-

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [imgprmt: CLI tool to embed an image prompt (e.g. "Midjourney") within a tweetable JPG-HTML polyglot image.](https://github.com/CleasbyCode/imgprmt)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [jzp: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/jzp)  

##

