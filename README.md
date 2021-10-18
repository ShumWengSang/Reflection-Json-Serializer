# Reflection-Json-Serializer
C++ example serializing using runtime reflection to json files

These files are meant be read with my blog post at https://0x00000000.dev/reflection-serializer/

## Build
You will need to modify the CMakeLists.txt to get this to build on your system. There are two dependancies, [nlohmann's Json For Modern C++](https://github.com/nlohmann/json), and Acki-M's [Real Time Type Reflection (RTTR) library](https://github.com/rttrorg/rttr)

### RTTR
Follow the instructions for [installing RTTR here](https://www.rttr.org/doc/master/building_install_page.html) to install RTTR. If you're new to CMake, heres what you'll need to do.

1) Download the repository/code.
2) Ensure you have CMake installed and can be accessed on command line
3) Ensure that you're on Windows and Visual Studios is installed (and the default for CMake)
4) Create a folder called "Build"
5) Enter the command line in Build directory
6) enter "cmake .."
7) This should generate a Visual Studio solution of RTTR project inside "Build" folder
8) Use Visual Studio to open solution
9) Build project CMakePredefinedTargets/INSTALL
10) Note the output directory and look for rttr-config.cmake file ![picture example]()
11) Modify my CMakeLists.txt 
```cmake
// Change this to where the rttr-config.cmake file is!
set(RTTR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rttr/build/install/cmake)
```

### Json For Modern C++
1) [Download single header release ](https://github.com/nlohmann/json/releases)
2) Modify main.cpp and source.cpp as required to include the downloaded file

## Output
Currently we are serializing
```c++
enum class color
{
    red,
    green,
    blue
};

struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};

struct shape
{
    shape(std::string n) : name(n) {}
    shape() = default;
    void set_visible(bool v) { visible = v; }
    bool get_visible() const { return visible; }

    color color_ = color::blue;
    std::string name = "";
    point2d position;
    std::map<color, point2d> dictionary;

    RTTR_ENABLE()
private:
    bool visible = false;
};

struct circle : shape
{
    circle(std::string n) : shape(n) {}
    circle() = default;
    double radius = 5.2;
    std::vector<point2d> points;

    int no_serialize = 100;

    RTTR_ENABLE(shape)
};

struct Container
{
    std::unordered_map<int, circle> map;
    std::vector<int> array = {10,1};
    RTTR_ENABLE()
};
```

and the Json output is. Look at main() for more information.
```json
{
    "Circle 1": {
        "color": "red",
        "dictionary": [
            {
                "key": "red",
                "value": {
                    "x": 5,
                    "y": 6
                }
            },
            {
                "key": "green",
                "value": {
                    "x": 1,
                    "y": 2
                }
            },
            {
                "key": "blue",
                "value": {
                    "x": 3,
                    "y": 4
                }
            }
        ],
        "name": "Circle #1",
        "points": [
            {
                "x": 1,
                "y": 1
            },
            {
                "x": 23,
                "y": 42
            }
        ],
        "position": {
            "x": 12,
            "y": 66
        },
        "radius": 5.123,
        "visible": true
    }
}

############################################

{
    "Circle 2": {
        "color": "blue",
        "dictionary": [],
        "name": "Circle #2",
        "points": [],
        "position": {
            "x": 0,
            "y": 0
        },
        "radius": 5.2,
        "visible": false
    }
}
Map:
 
{
    "Map": {
        "Array": [
            10,
            1
        ],
        "Hash Map": [
            {
                "key": 0,
                "value": {
                    "color": "red",
                    "dictionary": [
                        {
                            "key": "red",
                            "value": {
                                "x": 5,
                                "y": 6
                            }
                        },
                        {
                            "key": "green",
                            "value": {
                                "x": 1,
                                "y": 2
                            }
                        },
                        {
                            "key": "blue",
                            "value": {
                                "x": 3,
                                "y": 4
                            }
                        }
                    ],
                    "name": "Circle #1",
                    "points": [
                        {
                            "x": 1,
                            "y": 1
                        },
                        {
                            "x": 23,
                            "y": 42
                        }
                    ],
                    "position": {
                        "x": 12,
                        "y": 66
                    },
                    "radius": 5.123,
                    "visible": true
                }
            },
            {
                "key": 1,
                "value": {
                    "color": "blue",
                    "dictionary": [],
                    "name": "Circle #2",
                    "points": [],
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "radius": 5.2,
                    "visible": false
                }
            }
        ]
    }
}

```
