# pdvps

PNG Data Vehicle for Twitter, PowerShell Edition.

Embed any file type (5MB max.) within a tweetable and "*executable*" PNG image.

![Demo Image](https://github.com/CleasbyCode/pdvps/blob/main/Demo_Image/soldier.png)
{***Image demo:  Encrypted MP4 video embedded within this PNG image***}

Works for both Windows and *Linux. 

(*PowerShell required).

pdvps is similar to my other program [***pdvzip***](https://github.com/CleasbyCode/pdvzip). The two main differences in this edition of **pdv** are: 

**1)** It uses a default PNG image for where your data file is embedded and '*encrypted*'. The image is small in size, which maximises the amount of data you can embed (5,233,664 bytes, almost 5MB). Using a default image reduces the required C++ code of the main program. If you want to use other images for embedding files, then stick with [***pdvzip***](https://github.com/CleasbyCode/pdvzip).

**2)** There is a PowerShell script embedded within the default PNG image, that is used to decrypt, extract and open your embedded file, when you execute the PNG file.

For more information on the embedded scripts within the default PNG image, see the [***scripts_info.txt***](https://github.com/CleasbyCode/pdvps/blob/main/src/scripts_info.txt) file supplied with this repo.

For information about PNG image settings and arbitrary data preservation for Twitter, visit the relevant link on [***pdvzip***](https://github.com/CleasbyCode/pdvzip#png-image-requirements-for-arbitrary-data-preservation)

Always use file extensions for your data file: my_doc.pdf, my_video.mp4, my_program.py, etc.

Compile and run the program under Windows or **Linux**.

## Usage (Linux)

```bash
$ g++ pdvps.cpp -s -o pdvps
$
$ ./pdvps

Usage: pdvps <your_file>
       pdvps --info

$ ./pdvps my_document.pdf

Created output file 'pdvps_image.png' 4232948 bytes.

All Done!

```

Once your file has been embedded within the PNG image, it's ready to be shared (tweeted) or "*executed*" whenever you want to extract and open your embedded file.

You can also upload and share the PNG image to *some popular image hosting sites, such as [***Flickr***](https://www.flickr.com/), [***ImgBB***](https://imgbb.com/), [***Imgur***](https://imgur.com/a/zF40QMX), [***ImgPile***](https://imgpile.com/), [***PostImage***](https://postimg.cc/xcCcvpLJ).

## Extracting Your File From the PNG Image
**Linux (PowerShell required)**    
Make sure image file has executable permissions.
```bash

$ chmod +x pdvps_image.png
$
$ ./pdvps_image.png 

```  
**Windows**   
First, rename the '*.png*' file extension to '*.cmd*'.
```bash

G:\demo> ren pdvps_image.png pdvps_image.cmd
G:\demo>
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

