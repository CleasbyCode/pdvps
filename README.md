# pdvps

PNG Data Vehicle for Twitter, PowerShell Edition (PDVPS).

Embed any file of up to ~5MB within a tweetable and '*executable*' PNG polyglot image.

![Demo Image Embedded with PowerShell Script & Encrypted MP4 Video File](https://github.com/CleasbyCode/pdvps/blob/main/Demo_Image/rick.png)
{***Image demo:  Encrypted MP4 video embedded within this PNG image***}

Works for both Windows and *Linux. 

(*PowerShell required).

pdvps is similar to my other program [***pdvzip***](https://github.com/CleasbyCode/pdvzip). The two main differences in this edition of **pdv** are: 

**1).** It is supplied with a default PNG image that you use to embed your file, which will be '*encrypted*', using simple XOR method. The image is small in size, which maximises the amount of data you can embed (5,233,664 bytes, almost 5MB), it also reduces the required C++ code of the main program. (If you want to use your own image, then stick with [***pdvzip***](https://github.com/CleasbyCode/pdvzip)).

**2).** There is an PowerShell script embedded within the default PNG image that is used to decrypt, extract and open/play/run (depending on media type) your embedded file.

For more information on the embedded scripts within the default PNG image, see the [***scripts_info.txt***](https://github.com/CleasbyCode/pdvps/blob/main/scripts_info.txt) file supplied with this repo.

For information about PNG image settings and arbitrary data preservation for Twitter, visit the relevant link on [***pdvzip***](https://github.com/CleasbyCode/pdvzip#png-image-requirements-for-arbitrary-data-preservation)

Always use file extensions for your data file: my_doc.pdf, my_video.mp4, my_program.py, etc. A filename without an extension will be considered an '.exe' file.

Compile and run the program under Windows or **Linux**.

## Usage (Linux)

```c
$ g++ pdvps.cpp -o pdvps
$
$ ./pdvps

Usage:  pdvps  <your_file>
        pdvps  --info

$ ./pdvps my_document.pdf

Created output file 'pdvps_image.png' 4232948 bytes.

All Done!

$
```

Once your file has been embedded within the PNG image, it's ready to be shared (tweeted) or '*executed*' whenever you want to extract and open/play/run (depending on media type) your embedded file.

You can also upload and share the PNG image to *some popular image hosting sites, such as [***Flickr***](https://www.flickr.com/), [***ImgBB***](https://imgbb.com/), [***Imgur***](https://imgur.com/a/zF40QMX), [***ImgPile***](https://imgpile.com/), [***ImageShack***](https://imageshack.com/), [***PostImage***](https://postimg.cc/xcCcvpLJ), etc. **Not all image hosting sites are compatible, e.g. [***ImgBox***], [***Reddit***](https://github.com/CleasbyCode/pdvrdt).*

## Extracting Your File From the PNG Image
**Linux (PowerShell required)**    
Make sure image file has executable permissions.
```c

$ chmod +x pdv_image.png
$
$ ./pdv_image.png 

```  
**Windows**   
First, rename the '*.png*' file extension to '*.cmd*'.
```c

G:\demo> ren pdv_image.png pdv_image.cmd
G:\demo>
G:\demo> .\pdv_image.cmd

```

[**Video Demo (YouTube)**](https://www.youtube.com/watch_popup?v=FCleYo9vJas) - Running PNG image on Linux. PowerShell script will decrypt, extract and play the embedded mp4 video file.  

Other editions of **pdv** you may find useful:-  

* [pdvzip - PNG Data Vehicle for Twitter, ZIP Edition](https://github.com/CleasbyCode/pdvzip)  
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  

##

