# Spatial Sorting ExpLoad

This is the code associated with simulations conducted in [](). If you use this code, please cite this publication.

This program simulates dispersal evolution across a full species range during expansion (not only the front-most deme). 
It uses a stepping-stone migration model, and allows the user to specify the initial migration 
rate as well as the mutational effect size of new mutations impacting dispersal rate. The dispersal trait is modelled as a 
quantitative trait such that each individual inherits its migration rate from as the average of both parents’ 
trait value plus a random mutational deviation drawn from a Normal distribution with mean 0 and standard deviation as specified.
Landscapes may be 1- or 2-dimensional and of desired proportions, where each deme has the specified carrying capacity. 
Each individual possesses 2,000 bi-allelic, unconditionally deleterious or beneficial loci, 
where the proportion deleterious is specified by phi. These loci are separate from those impacting 
dispersal evolution and only impact an individual's fitness. Fitness is multiplicative across loci, generations are 
non-overlapping, and growth is instantaneous in newly-colonized demes. 


The program is executed with an input file specifying the desired parameters. 
All parameters are as follows, and must be provided in this exact order with no other preceding text in the input file. 
All text after the last parameter is ignored by the program. These inputs are the same as those from [Gilbert *et al.* 2018](https://journals.plos.org/plosgenetics/article?id=10.1371/journal.pgen.1007450) 
except that now the dominance parameter (*h*) can also be specified, and is not hard-coded into the program. Unfortunately, the effect size (standard deviation of draws from the Normal distribution) of mutations impacting dispersal evolution *is* hard-coded, and 
can be modified in deme.cpp lines 130 and 428 for the soft selection model, and 318 and 523 for the hard selection model.

* `m1` = width of the landscape (set to 1 for a linear stepping stone model)
* `m2` = length of the landscape
* `starting_demes` = the number of demes to be colonized from the ancestral population
* `niche_width` = the width of the metapopulation during a range shift, ignored during a standard expansion
* `capacity` = carrying capacity per deme
* `anc_pop_size` = the size of the ancestral population
* `burnin_time` = the number of generations to burn in the ancestral population
* `expansion_start` = the number of generations to burn in the initial colonized populations (starting_demes) on the landscape before the expansion/shift begins
* `theta` = the number of generations between each successive shift of the metapopulation forward during a range shift
* `generations` = the total number of generations to run the simulation from when the expansion/shift begins; if the landscape is fully crossed, the simulation will continue and shifting populations will exist in a niche_width by m1 sized grid of demes for the duration of the siimulation
* `snapshot` = output simulation results at every generation that is a multiple of this number
* `replicates` = the number of simulation replicates to perform
* `expansionMode` = set equal to 0 for a linear expansion across the length of the landscape, all other modes are deprecated in this version of the code
* `expansionModeKim` = set to 0 for a standard full expansion, 1 for an open-front shift, and 2 for a shift controlled at both the front and trailing edges
* `selectionMode` = 0 for soft selection, 1 for hard selection
* `mu` = genome-wide mutation rate
* `m` = migration rate
* `s` = mean effect size of deleterious mutations (effect sizes are fixed, but code can be modified to an exponential distribution with this set as mean)
* `phi` = the proportion of loci which are unconditionally deleterious mutations, all others are then given the opposite s as set above, i.e. a mean of -0.1 
makes all deleterious mutations have s = -0.1 and all beneficial mutations s = +0.1  When the 2000-loci version of the program is used, phi is a proportion of these 2000 loci, but the last 1000 loci are always neutral regardless of the value of phi, e.g. for 900 deleterious loci, 100 beneficial loci, and 1000 neutral loci, phi should be set to 0.45 and *s* to -0.1.
* `h` = dominance parameter
