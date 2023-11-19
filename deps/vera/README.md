![](.github/vera.jpg)

# VERA  [![](https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/patriciogonzalezvivo)

VERA is a C++11 & OpenGL/WebGL framework designed for creating and displaying real-time visual application in a big variety of surfaces.

VERA applications can be compiled as:
- native OpenGL ES app on Windows, MacOS and Linux
- browser WASM/WebGL app 

VERA brings support for:
- [Looking Glass Holographic Displays](https://lookingglassfactory.com/)
- [WebXR](https://www.w3.org/TR/webxr/)

VERA is the merge between, [ADA](https://github.com/patriciogonzalezvivo/ada) (a minimal library to create OpenGL ES & WebGL apps) and [HILMA](https://github.com/patriciogonzalezvivo/hilma) (a 2D/3D geometry library). 

VERA have an `App` class and a drawing API similar to [P5js's API](https://p5js.org/reference/) that could be use for more novel artist that are searching to improve the performance of their art works or displaying them in a bigger set of formats, devices or displays. This doesn't mean P5js code, addons and plugins will work out of the box in VERA, but mostly that `app.h` and `ops/draw.h` will provide functionality familiar and (when possible) width identical behavior. At the end of the day C++ is a different programing language with specific and unique sintax. Hopefully VERA can also serve for generative or novel artist already familiar to [Processing](https://p5js.org/reference/) and [P5js](https://p5js.org/reference/) that desire to adventure into C++. It can be provide a nice transition to [OpenFrameworks](https://github.com/openframeworks/openFrameworks) and [Cinder](https://github.com/cinder/Cinder).

Ultimatelly VERA is designed to let artist code their works once and display them in a biffer range of surfaces. From native apps to custom displays powered by embebed systems such us [Raspberry Pi](https://www.raspberrypi.org/); or from holographic screen like those from [Looking Glass Factory](https://lookingglassfactory.com/), sandboxed web app (like [generative NFT projects](https://objkt.com/asset/hicetnunc/447619) ) to [AR/VR experiences through WebXR protocol](https://www.w3.org/TR/webxr/).

## Examples and projects using VERA

- [very simple Hello World example](https://patriciogonzalezvivo.github.io/vera_hello_world/). Which you can view and compile the [full project](https://github.com/patriciogonzalezvivo/vera_hello_world) and display it in a big range of surfaces like: native OSX, Window and Linux apps; console app with out window manager for embebed systems like [RaspberryPi](https://www.raspberrypi.com/), [Nvidia Jetson Nano](https://developer.nvidia.com/embedded/jetson-nano-developer-kit) and [Khadas's VIM4](https://www.khadas.com/vim4); on the web embebed WASM apps or selfcontain WASM/WebGL interactives for NFT project; WebXR experiences for VR headsets like the Oculus Quest2 or realtime applications for holographic displays like [LGF](https://lookingglassfactory.com/).
![VERA_bigger](https://user-images.githubusercontent.com/346914/180854090-6d9d1f53-c48d-4108-93b9-4101a6b7c9e1.gif)

## Support VERA

With your support will be possible to:
- improve the documentation
- bring more examples
- bring support for more devices and type of displays
