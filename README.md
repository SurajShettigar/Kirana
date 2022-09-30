# Kirana

An offline render engine with a realtime viewport made from Vulkan API.

## Installation

### Prerequisites

- <b>CMake</b> Version 3.0 or greater
- <b>Boost</b>
    - Download [Boost 1.76.0](https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/)
    - Extract it to `C:\Program Files\boost\boost_1_76_0`
    - Open Command Prompt from the above directory and run the following commands:
        ```
        bootstrap
        .\b2
        ```

### Project Installation

The repository uses submodules, so clone the repository using:

```
git clone --recurse-submodules https://github.com/SurajShettigar/Kirana.git
```

Extract `data.zip` file into the current directory of the project. 
It contains the default resources needed for the app.

