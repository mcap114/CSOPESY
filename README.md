CAPAROS, Miguel Antonio 
MARTINEZ, Azeliah 
PAREDES, Bill Jethro 
VILLANUEVA, Keisha Leigh

CSOPESY S22 

---

# Project Structure:
---------------------

- **main.cpp** – Entry point of the application.  
- **CLI/** – Contains the CLI interface and command handlers.  
- **Scheduler/** – Contains both FCFS and RR schedulers.  
- **Core/** – Includes CPU timer logic and configuration loader.  
- **Screen/** – Implements the virtual screen system for processes.  
- **Utilities/** – Contains the ReportGenerator for CPU utilization logs.  
- **config.txt** – Configuration file used during initialization.  
- **csopesy-log.txt** – Output log file from the `report-util` command.

---

# Instructions:
----------------

1. Build the project (Visual Studio or CMake supported).
2. Ensure `config.txt` is located in the same directory as the executable.
3. Run the application. It will open a command-line interface.

**Main commands available:**
- `initialize` – Reads `config.txt` and initializes the scheduler and CPU.
- `screen -s <name>` – Creates a new process screen.
- `screen -r <name>` – Returns to an existing screen.
- `screen -ls` – Lists current running screens and process summary.
- `process-smi` – Displays real-time process info when inside a screen.
- `scheduler-start` – Begins generating dummy background processes.
- `scheduler-stop` – Stops background process generation.
- `report-util` – Generates CPU usage report into `csopesy-log.txt`.
- `exit` – Exits the current screen or terminates the emulator.

**Note:** You must run `initialize` before any other command.

---