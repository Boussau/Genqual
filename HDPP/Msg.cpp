#include <iostream>
#include <mpi.h>
#include "Msg.h"



void Msg::error(std::string s) {

	std::cout << "Error: " << s << std::endl;
	std::cout << "Exiting program" << std::endl;
	MPI::COMM_WORLD.Abort(1);
	exit(1);
}

void Msg::warning(std::string s) {

	std::cout << "Warning: " << s << std::endl;
}
