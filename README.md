# square_solver
Consumer-Produser based solver of quadratic equations

Working Cycle:
-------------------
QuadReader -> Buffer(Coeffs) -> QuadSolver -> Buffer -> QuadPrinter(Equation)

Build:
-------------------
Execute next commands in project directory:
* cd build
* cmake ..
* make

Run:
-------------------
Program can read any sequence of integer numbers that fits to INT32 boundaries
and sepparated by space.
Any other symbols or numbers out of boundaries will be ignored.

* Manual input
./square_solver 1 -2, -3

* File input (There is a sample file into build directory)
./square_solver $(cat inputsample)

* Output sample:
(1 -2 -3) => (-1, 3)
