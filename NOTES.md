# Some Notes
Here are some notes and hints for this project.

# Creating a Video
Using imageMagick you can create an video from a list of images:

`ffmpeg.exe -r 60 -i image%d.png -c:v libx264 -framerate 60 -pix_fmt yuv420p out.mp4`

This video will map the images 1:1 to a 60FPS mp4 video of the same resolution.
