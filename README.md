# pdvps

PNG Data Vehicle for Twitter, PowerShell Edition (PDVPS).

Embed any file of up to ≈5MB within a tweetable and '*executable*' PNG polyglot image.

Works for both Windows and *Linux. 

(*PowerShell required).

PDVPS is similar to my other program [***pdvzip***](https://github.com/CleasbyCode/pdvzip). The two major differences in this edition of **pdv** are: 

**1).** It is supplied with a default PNG image that you use to embed your file, which will be '*encrypted*', using simple XOR. The image is small in size, which maximises the amount of data you can embed (5,234,608 bytes, almost 5MB), it also substantially reduces the required C++ code of the main program. (If you want to use your own image, then stick with [***pdvzip***](https://github.com/CleasbyCode/pdvzip)).

**2).** There is an embedded PowerShell script within this default PNG image that is used to decrypt, extract and open/play/run (depending on media type) your embedded file. This eliminates the need to use an external unzip tool. 

This program is really just a simple experiment of mine in testing if I could achieve some practical use of PowerShell code embedded within a PNG image.

For more information on the embedded scripts within the default PNG image, see the [***scripts_info.txt***](https://github.com/CleasbyCode/pdvps/blob/main/scripts_info.txt) file supplied with this repo.

For information about PNG image settings and arbitrary data preservation for Twitter, visit the relevant link on [***pdvzip***](https://github.com/CleasbyCode/pdvzip#png-image-requirements-for-arbitrary-data-preservation)

Always use file extensions for your data file: my_doc.pdf, my_video.mp4, my_program.py, etc. A filename without an extension will be considered an '.exe' file.

Compile and run the program under Windows or **Linux**.

## Usage (Linux)

```c
$ g++ pdvps.cpp -w -o pdvps
$
$ ./pdvps

Usage:  pdvps  <your_file>
        pdvps  --info

$ ./pdvps my_document.pdf

Created output file 'pdv_my_document.pdf.png' 4232948 bytes.

All Done!

$
```

Once your file has been embedded within the PNG image, it's ready to be shared (tweeted) or '*executed*' whenever you want to extract and open/play/run (depending on media type) your embedded file.

You can also upload and share the PNG image to *some popular image hosting sites, such as [***Flickr***](https://www.flickr.com/), [***ImgBB***](https://imgbb.com/), [***Imgur***](https://imgur.com/a/zF40QMX), [***ImgPile***](https://imgpile.com/), [***ImageShack***](https://imageshack.com/), [***PostImage***](https://postimg.cc/xcCcvpLJ), etc. **Not all image hosting sites are compatible, e.g. [***ImgBox***](https://imgbox.com/), [***Reddit***](https://www.reddit.com/).*

## Extracting Your File From the PNG Image
**Linux (PowerShell required)**    
Make sure image file has executable permissions.
```c

$ chmod +x pdv_image_file.png
$
$ ./pdv_image_file.png 

```  
**Windows**   
First, rename the '*.png*' file extension to '*.cmd*'.
```c

G:\demo> ren pdv_image_file.png pdv_image_file.cmd
G:\demo>
G:\demo> .\pdv_image_file.cmd

```

**Image Example**  

[Twitter](https://twitter.com/CleasbyCode/status/1591029378586058752) 

Image is embedded with an MP4 video file. Click the image to fully expand it, then right-click image, select "Save image as..." from menu. Run image as per instructions above.

**Video Example**

[YouTube](https://www.youtube.com/watch_popup?v=d7QjxLWQ0kM)

Linux demo of *pdvps* embedding a PDF document within the PNG Image, then running the image file to extract and display the PDF document. Followed by tweeting the PNG image and saving it to a Windows machine, where we run it again to extract and display the PDF document on Windows.

##

