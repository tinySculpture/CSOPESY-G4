# CSOPESY Major Output: OS Emulator

## Developers – S19 Group 4
- **Luistro, Josiah Mari**  
- **Rimando, Alonzo Andrei**  
- **Santos, Andrea Li**  
- **Vasquez, Ryan Clemence**

---

## Notes
- The **entry point** of the program, containing the main function, is `main.cpp`.
- Ensure that `config.txt` includes a complete list of parameters and is located in the same directory as `main.cpp`.
- If running via Visual Studio, make sure the C++ development package is installed.

---

## Running the Program

### From Visual Studio
1. Clone the repository.
2. Access the project by opening the solution file `CSOPESY-MP.sln`.
3. Build and run the project from within Visual Studio.

### From the Terminal
1. After cloning the repository, navigate to the project directory.
2. Compile the source files using the following command:
   ```bash
   g++ -std=c++20 *.cpp -o main -pthread 
   ```
3. Run the executable:
   ```bash
   .\main
   ```
