Contributers:
- Arel Sharon 323012971
- Matan Amichai 315441972

Bonus:
    - We included a bonus.txt file in the root directory
- Dependencies
    - We used boost for parsing the arguments & timeout handleing, It should be automatically installed by the CMake
    - You need to install the linux 'zip' module for it to work.
- How to build?
    - We included a root CMakeList.txt File
        - From this directory
        - cmake -S . -B build
        - cd build
        - make
        - You can cmake with the -Debug flag if you with to run the tests
- How to run?
    After the building process:
    - a /lib folder with the .so files will be created
    - ***Important*** build/simulator/myrobot executable will be created.
- **Windows/MacOS isn't supported!**
