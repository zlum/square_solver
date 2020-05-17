# square_solver
Consumer-Produser based solver of quadratic equations

Working Cycle:
--------------------------------------------------------------------------------
QuadReader -> Buffer(Coeffs) -> QuadSolver -> Buffer -> QuadPrinter(Equation)

Build:
--------------------------------------------------------------------------------
Execute next commands in project directory:

* cd build
* cmake ..
* make

Run:
--------------------------------------------------------------------------------
Program can read any sequence numbers sepparated by space.
Any other symbols or numbers out of boundaries will be ignored.

* Manual input
./square_solver 1.32 -44 8

* File input (There is a sample file into build directory)
./square_solver $(cat inputsample)

* Output sample:
(1.32 -44 8) => (0.182820886110124552153889643102, 33.150512447223208781179443690232)
