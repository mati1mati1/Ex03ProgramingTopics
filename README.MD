Contributers:
- Arel Sharon 323012971
- Matan Amichai 315441972


Bonus:
- We included a bonus.txt file in the root directory
How to build?
- We included a CMakeList.txt File, you may use it to build 
- We tested mainly on Ubuntu22 WSL, MacOS
- **Windows isn't supported!**

Solution approach:
All design considirations are present in HLD
We've documented the Algorithm we chose in MappingAlgorith.cpp nextStep(), calculateNextStep() functions which describe in detail the working of the algorithm

Examples:
- We have included a ton of examples that we used to test under the /test/examples directory
- The minimum_sample_inputs includes the 3 required ones (Which are taken from /test/examples)
    - Input a shows a house with unreachable dirt, the vacuum correctly understands that there is no way to progress with its limited battery and stops after the first outing
    - Input b shows a regular house but with limited max steps (vastly lower than max battery)
    - Input c shows a sparse house with several walls in the middle# Ex03ProgramingTopics
