#include <mpi.h>
#include <iomanip>
#include <iostream>
#include "Msg.h"
#include "Settings.h"



Settings::Settings(int argc, char *argv[]) {

#	if 0
	char *cmdString[30];
	cmdString[ 0] = (char *)"hdp";
	cmdString[ 1] = (char *)"-est_parms";
	cmdString[ 2] = (char *)"true";
	cmdString[ 3] = (char *)"-aln_path";
	cmdString[ 4] = (char *)"/Users/johnh/Desktop/mammal";
	cmdString[ 5] = (char *)"-output_name";
	cmdString[ 6] = (char *)"/Users/johnh/Desktop/hdpp4x4/results_ucla/myresults";
	cmdString[ 7] = (char *)"-treatment_tree";
	cmdString[ 8] = (char *)"same";
	cmdString[ 9] = (char *)"-treatment_length";
	cmdString[10] = (char *)"rv";
	cmdString[11] = (char *)"-treatment_subrates";
	cmdString[12] = (char *)"diff";
	cmdString[13] = (char *)"-treatment_freq";
	cmdString[14] = (char *)"diff";
	cmdString[15] = (char *)"-alpha_franchise";
	cmdString[16] = (char *)"4.0";
	cmdString[17] = (char *)"-ek_restaurant";
	cmdString[18] = (char *)"3.0";
	cmdString[19] = (char *)"-aln_size";
	cmdString[20] = (char *)"36";
	cmdString[21] = (char *)"-length";
	cmdString[22] = (char *)"10000000";
	argc = 23;
	argv = cmdString;
#	endif

	/* set default values for parameters */
	pathName                    = "";
	qualityScorePathName        = "";
	outputFileName              = "";
	equalNumTaxa                = false;
	specifiedNumTaxa            = 4;
	brlenLambda                 = 10.0;
	menuAlpha                   = 0.80;
	ekRestaurant                = 3.0;
	treatmentTopology           = PARM_SAME;
	treatmentBasefreqs          = PARM_DIFF;
	treatmentSubRates           = PARM_DIFF;
	treatmentLength             = PARM_DIFF;	
	chainLength                 = 100000;
	printFrequency              = 2;
	sampleFrequency             = 50;
	burnIn                      = 100;	
	numAuxiliaryMenuItems       = 10;
	estimateParms               = true;
	numSimulatedTaxa            = 20;
	numSimulatedSites           = 100;
	numSimulatedRestaurants     = 20;

	if (argc > 1)
		{
		if (argc % 2 == 0)
			{
			std::cout << "Usage:" << std::endl;
			std::cout << "   -aln_path <PATH>                   : Path to directory holding alignments" << std::endl;
			std::cout << "	 -qscore_path <PATH>				: Path to directory holding quality scores" << std::endl;
			std::cout << "   -output_name <NAME>                : Output file name" << std::endl;
			std::cout << "   -est_parms <true/false>            : Estimate parameters (true) or simulate data (false)" << std::endl;
			std::cout << "   -aln_size <NUMBER>                 : Only read files with an equal number of taxa" << std::endl;
			std::cout << "   -length <NUMBER>                   : Number of MCMC cycles" << std::endl;
			std::cout << "   -print_freq <NUMBER>               : Number of MCMC cycles" << std::endl;
			std::cout << "   -sample_freq <NUMBER>              : Number of MCMC cycles" << std::endl;
			std::cout << "   -burn <NUMBER>                     : Burn-in period" << std::endl;
			std::cout << "   -brlen <NUMBER>                    : Parameter of exponential prior for branch lengths" << std::endl;
			std::cout << "   -alpha_franchise <NUMBER>          : Concentration parameter for the franchise" << std::endl;
			std::cout << "   -ek_restaurant <NUMBER>            : Prior mean of the number of restaurant tables" << std::endl;
			std::cout << "   -treatment_tree <diff/same>        : How to treat the tree topology" << std::endl;
			std::cout << "   -treatment_length <diff/same/rv>   : How to treat the tree-length parameter" << std::endl;
			std::cout << "   -treatment_subrates <diff/same/rv> : How to treat the substitution rates" << std::endl;
			std::cout << "   -treatment_freq <diff/same/rv>     : How to treat the nucleotide frequencies" << std::endl;
			std::cout << "   -num_aux <NUMBER>                  : How many auxiliary tables to use" << std::endl;
			std::cout << "   -num_sim_taxa <NUMBER>             : How many taxa to simulate" << std::endl;
			std::cout << "   -num_sim_restaurants <NUMBER>      : How many restaurants to simulate" << std::endl;
			std::cout << "   -num_sim_sites <NUMBER>            : How many sites to simulate" << std::endl;

			std::cout << std::endl;
			std::cout << "Example:" << std::endl;
			std::cout << "   hdp -i <input file> -o <output file>" << std::endl;
			exit(0);
			}
			
		/* read the command-line arguments */
		std::string status = "none";
		for (int i=1; i<argc; i++)
			{
			std::string cmd = argv[i];
			//cout << cmd << endl;
			if (status == "none")
				{
				/* read the parameter specifier */
				if ( cmd == "-aln_path" )
					status = "aln_path";
				else if (cmd == "-qscore_path" )
					status = "qscore_path";
				else if ( cmd == "-output_name" )
					status = "output_name";
				else if ( cmd == "-aln_size" )
					status = "aln_size";
				else if ( cmd == "-length" )
					status = "length";
				else if ( cmd == "-print_freq" )
					status = "print_freq";
				else if ( cmd == "-sample_freq" )
					status = "sample_freq";
				else if ( cmd == "-burn" )
					status = "burn";
				else if ( cmd == "-brlen" )
					status = "brlen";
				else if ( cmd == "-alpha_franchise" )
					status = "alpha_franchise";
				else if ( cmd == "-ek_restaurant" )
					status = "ek_restaurant";
				else if ( cmd == "-treatment_tree" )
					status = "treatment_tree";
				else if ( cmd == "-treatment_length" )
					status = "treatment_length";
				else if ( cmd == "-treatment_subrates" )
					status = "treatment_subrates";
				else if ( cmd == "-treatment_freq" )
					status = "treatment_freq";
				else if ( cmd == "-num_aux" )
					status = "num_aux";
				else if ( cmd == "-est_parms" )
					status = "est_parms";
				else if ( cmd == "-num_sim_taxa" )
					status = "num_sim_taxa";
				else if ( cmd == "-num_sim_restaurants" )
					status = "num_sim_restaurants";
				else if ( cmd == "-num_sim_sites" )
					status = "num_sim_sites";
				else
					{
					std::cerr << "Could not interpret option \"" << cmd << "\"." << std::endl;
					exit(1);
					}
				}
			else
				{
				/* read the parameter */
				if ( status == "aln_path" )
					{
					pathName = argv[i];
					}
				else if (status == "qscore_path")
					{
					qualityScorePathName = argv[i];
					}
				else if ( status == "output_name" )
					{
					outputFileName = argv[i];
					}
				else if ( status == "aln_size" )
					{
					specifiedNumTaxa = atoi(argv[i]);
					equalNumTaxa = true;
					}
				else if ( status == "length" )
					{
					chainLength = atoi(argv[i]);
					}
				else if ( status == "print_freq" )
					{
					printFrequency = atoi(argv[i]);
					}
				else if ( status == "sample_freq" )
					{
					sampleFrequency = atoi(argv[i]);
					}
				else if ( status == "burn" )
					{
					burnIn = atoi(argv[i]);
					}
				else if ( status == "brlen" )
					{
					brlenLambda = atof(argv[i]);
					}
				else if ( status == "alpha_franchise" )
					{
					menuAlpha = atof(argv[i]);
					}
				else if ( status == "ek_restaurant" )
					{
					ekRestaurant = atof(argv[i]);
					}


				else if ( status == "est_parms" )
					{
					std::string tempStr = argv[i];
					if (tempStr[0] == 'T' || tempStr[0] == 't')
						estimateParms = true;
					else if (tempStr[0] == 'F' || tempStr[0] == 'f')
						estimateParms = false;
					else 
						Msg::error("Problem identifying \"est_parms\" setting");
					}
				else if ( status == "num_sim_taxa" )
					{
					numSimulatedTaxa = atoi(argv[i]);
					}
				else if ( status == "num_sim_restaurants" )
					{
					numSimulatedRestaurants = atoi(argv[i]);
					}
				else if ( status == "num_sim_sites" )
					{
					numSimulatedSites = atoi(argv[i]);
					}
				else if ( status == "treatment_tree" )
					{
					std::string tempStr = argv[i];
					if (tempStr == "diff")
						treatmentTopology = PARM_DIFF;
					else if (tempStr == "same")
						treatmentTopology = PARM_SAME;
					else
						Msg::error("Unknown topology treatment");
					}
				else if ( status == "treatment_length" )
					{
					std::string tempStr = argv[i];
					if (tempStr == "diff")
						treatmentLength = PARM_DIFF;
					else if (tempStr == "same")
						treatmentLength = PARM_SAME;
					else if (tempStr == "rv")
						treatmentLength = PARM_RV;
					else
						Msg::error("Unknown dN/dS treatment");
					}
				else if ( status == "treatment_subrates" )
					{
					std::string tempStr = argv[i];
					if (tempStr == "diff")
						treatmentSubRates = PARM_DIFF;
					else if (tempStr == "same")
						treatmentSubRates = PARM_SAME;
					else if (tempStr == "rv")
						treatmentSubRates = PARM_RV;
					else
						Msg::error("Unknown ti/tv treatment");
					}
				else if ( status == "treatment_freq" )
					{
					std::string tempStr = argv[i];
					if (tempStr == "diff")
						treatmentBasefreqs = PARM_DIFF;
					else if (tempStr == "same")
						treatmentBasefreqs = PARM_SAME;
					else if (tempStr == "rv")
						treatmentBasefreqs = PARM_RV;
					else
						Msg::error("Unknown nucleotide frequency treatment");
					}
				else if ( status == "num_aux" )
					{
					numAuxiliaryMenuItems = atoi(argv[i]);
					}
				else
					{
					Msg::error("Unknown status reading command line information");
					}
				status = "none";
				}
			}
		}
	else
		{
		Msg::error("You must provide command line arguments");
		}
}

Settings::~Settings(void) {

}

void Settings::print(void) {

	if ( MPI::COMM_WORLD.Get_rank() == 0 )
		{
		if ( getAreParametersEstimated() == true )
			{
			std::string noYes[2] = { "No", "Yes" };
			std::string treatment[3] = { "Different", "Same", "Random variable" };
			std::cout << std::fixed << std::setprecision(2);
			std::cout << "   User settings:"                                                                   << std::endl;
			std::cout << "   * Path name                             = " << pathName                           << std::endl;
			std::cout << "	 * Quality score path name				 = " << qualityScorePathName			   << std::endl;
			std::cout << "   * Output file name                      = " << outputFileName                     << std::endl;
			std::cout << "   * Only use alignments of equal size     = " << noYes[equalNumTaxa]                << std::endl;
			std::cout << "   * Specified number of taxa              = " << specifiedNumTaxa                   << std::endl;
			std::cout << "   * Branch length lambda                  = " << brlenLambda                        << std::endl;
			std::cout << "   * Franchise alpha                       = " << menuAlpha                          << std::endl;
			std::cout << "   * Prior number of tables in restaurants = " << ekRestaurant                       << std::endl;
			std::cout << "   * Topology treatment                    = " << treatment[treatmentTopology]       << std::endl;
			std::cout << "   * Nucleotide frequency treatment        = " << treatment[treatmentBasefreqs]      << std::endl;
			std::cout << "   * Substitution rate treatment           = " << treatment[treatmentSubRates]       << std::endl;
			std::cout << "   * Tree length treatment                 = " << treatment[treatmentLength]         << std::endl;
			std::cout << "   * Number of MCMC cycles                 = " << chainLength                        << std::endl;
			std::cout << "   * Number of MCMC samples to burn        = " << burnIn                             << std::endl;
			std::cout << std::endl;
			}
		else 
			{
			std::string noYes[2] = { "No", "Yes" };
			std::string treatment[3] = { "Different", "Same", "Random variable" };
			std::cout << std::fixed << std::setprecision(2);
			std::cout << "   User settings:"                                                                   << std::endl;
			std::cout << "   * Path name                             = " << pathName                           << std::endl;
			std::cout << "	 * Quality score path name				 = " << qualityScorePathName			   << std::endl;
			std::cout << "   * Output file name                      = " << outputFileName                     << std::endl;
			std::cout << "   * Number of taxa                        = " << numSimulatedTaxa                   << std::endl;
			std::cout << "   * Number of restaurants                 = " << numSimulatedRestaurants            << std::endl;
			std::cout << "   * Number of sites for each restaurant   = " << numSimulatedSites                  << std::endl;
			std::cout << "   * Branch length lambda                  = " << brlenLambda                        << std::endl;
			std::cout << "   * Franchise alpha                       = " << menuAlpha                          << std::endl;
			std::cout << "   * Prior number of tables in restaurants = " << ekRestaurant                       << std::endl;
			std::cout << "   * Topology treatment                    = " << treatment[treatmentTopology]       << std::endl;
			std::cout << "   * Nucleotide frequency treatment        = " << treatment[treatmentBasefreqs]      << std::endl;
			std::cout << "   * Substitution rate treatment           = " << treatment[treatmentSubRates]       << std::endl;
			std::cout << "   * Tree length treatment                 = " << treatment[treatmentLength]         << std::endl;
			std::cout << std::endl;
			}
		}
}
