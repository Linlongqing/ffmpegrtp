# ffmpegrtp

platform: windows x64

dependencies: ffmpeg2.8 or later    jrtp3.2    opencv2.9

discription:
1. get video from laptop's pre-camera and encode it to h264 stream using ffmpeg
2. send it by jrtp
3. receive video by jrtp and decode it by ffmpeg
4. using socket to send jpeg picture encoded by ffmpeg
