LIBS     =   -lm

CFLAGS   =  -O3

CC       =   mpic++

OBJECTS  =   main.o Alignment.o CondLikes.o Franchise.o FranchiseHq.o IoManager.o MbBitfield.o MbEigensystem.o MbMath.o MbRandom.o MbTransitionMatrix.o Mcmc.o MenuItem.o MenuItemInfo.o Model.o Msg.o Parm.o Parm_key.o Parm_length.o Parm_statefreqs.o Parm_subrates.o Parm_tree.o QualityScores.o RateMatrix.o RateMatrixMngr.o Restaurant.o SeedFactory.o Settings.o SiteLikes.o SiteModels.o StateSets.o Table.o TableFactory.o TableInfo.o TableImage.o TableImageFactory.o Util.o

PROGS    = hdpp2

all:		$(PROGS)

hdpp2:		$(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o hdpp2
		
main.o:	main.cpp
		$(CC) $(CFLAGS) -c main.cpp
		
Alignment.o:	Alignment.cpp
		$(CC) $(CFLAGS) -c Alignment.cpp

CondLikes.o:	CondLikes.cpp
		$(CC) $(CFLAGS) -c CondLikes.cpp
		
Franchise.o:	Franchise.cpp
		$(CC) $(CFLAGS) -c Franchise.cpp

FranchiseHq.o:	FranchiseHq.cpp
		$(CC) $(CFLAGS) -c FranchiseHq.cpp

IoManager.o:	IoManager.cpp
		$(CC) $(CFLAGS) -c IoManager.cpp

MbBitfield.o:	MbBitfield.cpp
		$(CC) $(CFLAGS) -c MbBitfield.cpp

MbEigensystem.o:	MbEigensystem.cpp
		$(CC) $(CFLAGS) -c MbEigensystem.cpp
		
MbMath.o:	MbMath.cpp
		$(CC) $(CFLAGS) -c MbMath.cpp
		
MbRandom.o:	MbRandom.cpp
		$(CC) $(CFLAGS) -c MbRandom.cpp
		
MbTransitionMatrix.o:	MbTransitionMatrix.cpp
		$(CC) $(CFLAGS) -c MbTransitionMatrix.cpp

Mcmc.o:	Mcmc.cpp
		$(CC) $(CFLAGS) -c Mcmc.cpp

MenuItem.o:	MenuItem.cpp
		$(CC) $(CFLAGS) -c MenuItem.cpp

MenuItemInfo.o:	MenuItemInfo.cpp
		$(CC) $(CFLAGS) -c MenuItemInfo.cpp

Model.o:	Model.cpp
		$(CC) $(CFLAGS) -c Model.cpp

Msg.o:	Msg.cpp
		$(CC) $(CFLAGS) -c Msg.cpp

Parm.o:	Parm.cpp
		$(CC) $(CFLAGS) -c Parm.cpp
		
Parm_key.o:	Parm_key.cpp
		$(CC) $(CFLAGS) -c Parm_key.cpp

Parm_length.o:	Parm_length.cpp
		$(CC) $(CFLAGS) -c Parm_length.cpp

Parm_statefreqs.o:	Parm_statefreqs.cpp
		$(CC) $(CFLAGS) -c Parm_statefreqs.cpp

Parm_subrates.o:	Parm_subrates.cpp
		$(CC) $(CFLAGS) -c Parm_subrates.cpp

Parm_tree.o:	Parm_tree.cpp
		$(CC) $(CFLAGS) -c Parm_tree.cpp
		
QualityScores.o:	QualityScores.cpp
		$(CC) $(CFLAGS) -c QualityScores.cpp

RateMatrix.o:	RateMatrix.cpp
		$(CC) $(CFLAGS) -c RateMatrix.cpp

RateMatrixMngr.o:	RateMatrixMngr.cpp
		$(CC) $(CFLAGS) -c RateMatrixMngr.cpp

Restaurant.o:	Restaurant.cpp
		$(CC) $(CFLAGS) -c Restaurant.cpp

SeedFactory.o:	SeedFactory.cpp
		$(CC) $(CFLAGS) -c SeedFactory.cpp

Settings.o:	Settings.cpp
		$(CC) $(CFLAGS) -c Settings.cpp
		
SiteLikes.o:	SiteLikes.cpp
		$(CC) $(CFLAGS) -c SiteLikes.cpp
		
SiteModels.o:	SiteModels.cpp
		$(CC) $(CFLAGS) -c SiteModels.cpp
		
StateSets.o:	StateSets.cpp
		$(CC) $(CFLAGS) -c StateSets.cpp

Table.o:	Table.cpp
		$(CC) $(CFLAGS) -c Table.cpp

TableFactory.o:	TableFactory.cpp
		$(CC) $(CFLAGS) -c TableFactory.cpp

TableInfo.o:	TableInfo.cpp
		$(CC) $(CFLAGS) -c TableInfo.cpp

TableImage.o:	TableImage.cpp
		$(CC) $(CFLAGS) -c TableImage.cpp

TableImageFactory.o:	TableImageFactory.cpp
		$(CC) $(CFLAGS) -c TableImageFactory.cpp
		
Util.o:	Util.cpp
		$(CC) $(CFLAGS) -c Util.cpp

clean:		
		rm -f *.o
