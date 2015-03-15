## Introduction ##
If you own a wireless router or set-top box, you probably know about the W.A.P. (web admin page) used to configure it.

Besides embedded systems, web interfaces also come in handy for server applications (such as auto-builders) where you want more security than VNC, dynamically generated forms that look good, and debug tools for real-time simulation like games. Adding an HTTP server to your application means you can control it with a web browser through a network connection.

## Usage ##
This project was originally written in C# but the C++ port is included.

Start the sample application and a web browser pointed at http://localhost:8080/

## Web page layout ##
The web interface for web-config is with a navigation sidebar on the left and a form on the right. The form is a simple table with label and input columns.

![http://www.codeproject.com/KB/game/WebConfig/WebConfigA.jpg](http://www.codeproject.com/KB/game/WebConfig/WebConfigA.jpg)

## Using the code ##
To allow instant feedback, I use onclick and onchange to auto-submit certain forms. Each form is a collection of inputs that are related, such as "Debug" or "Camera".

Adding an input in the code is brief, so it can be removed later before shipping. The input is named by the form and label. Lambda expressions are used to get and set the value.

Sliders can be int or float with decimal places, forms can be set to auto-save and auto-submit, and buttons and links can be used to perform actions and show resource files.

To demonstrate WebConfig, I used a bouncing ball simulation. WebConfig inputs configure various simulation parameters in real-time. Change the number of balls and see the animation change accordingly.

## Future plans ##
web-config needs a secure password form.

## Testing ##
I tested on Firefox 3.0 and IE 8.0; notice they look a little different.

## Credits ##
It would have taken a lot longer to write this article without:

[Carpe DHTML slider](http://carpe.ambiprospect.com/slider/)<br>
<a href='http://www.codeproject.com/KB/IP/CSHTTPServer.aspx'>HTTP Skeleton Server</a><br>
<a href='http://beta.codeproject.com/KB/graphics/BallAnimation.aspx'>Ball Animation</a><br>