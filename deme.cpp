#include <iostream> // For input/output
#include <fstream> // For file input/output
#include <string.h>  // For strcpy
#include <time.h>  // For time
#include <stdlib.h>  // For toupper and tolower
#include <cstdlib>      // added by KJG trying to fix abs error line 303
#include <cmath>        // added by KJG trying to fix abs error line 303
#include <math.h>
#include <vector>
#include <list>
#include "range_expansion.h"
//#include "rng.h"
#include "rng2.h"



using namespace std;

inline double max(double a, double b) { return (a < b) ? b : a; }

double Deme::m = 0;
double Deme::s = 0;
double Deme::h = 0;
double Deme::mutation_rate = 0;

Deme::Deme()
{
    age = 0;
}
          

Deme::~Deme()
{
}


void Deme::initialize()
{
    m = 0.01;
    capacity = 100;
    s = 0.01;
    h = 0.5;
    mutation_rate = 0.01;
}


void Deme::colonize()
{
    Individual ind;
    int i;
    
    for(i = 0; i < capacity; i++) 
    {
      this_generation.push_back(ind);
    }  
}



void Deme::reproduce(int wf)
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2; // logistic growth rate parameter here
    bool front;
                
    front = (ID >= (wf - 1));    
    
    no_ind = this_generation.size();
    //cout << "test: " <<this_generation.size() << " " << no_ind;
    if (no_ind > 0)
    {
        //calculate expected number of offspring 
        //expected_offspring = capacity;   // Demes are filled immediately
    
        expected_offspring = no_ind * (r/(1 + (double)(no_ind*(r-1))/capacity));  // beverton-holt
        
        //realized offspring is obtained from a Poisson distribution
        realized_offspring = randpois(expected_offspring);    
        
        //realized_offspring = capacity;
        
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;i++)
        {
                // generate new individual
                mom = randint(0,this_generation.size()-1);    // draw parents randomly
                dad = randint(0,this_generation.size()-1); 
    
                // create new gametes from parents
                it=this_generation.begin();
                advance(it,mom);
                gamete_mom = it->getNewGameteMM2(mutation_rate,mutation_rate,s); 
                
        
                it=this_generation.begin();
                advance(it,dad);
                gamete_dad = it->getNewGameteMM2(mutation_rate,mutation_rate,s); 
                
    
                //add to next generation
                ind.setGenotype(gamete_mom,gamete_dad);
                //ind.updateDistance(ID,wf);
                next_generation.push_back(ind);
        }
       
        // replace old generation by new
        this_generation = next_generation;
    }
}

void Deme::reproduceSS(int wf)
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2;
    double mom_fit,dad_fit;
    double mig_mom,mig_dad,off_m;
    double mig_mut_sd = 0.005;

    
    bool front;
    
    front = (ID >= (wf-1));
    
    no_ind = this_generation.size();
    
   
    max_fit = 0;
    
    for (it = this_generation.begin();it != this_generation.end();it++)
    {
       max_fit = fmax(max_fit,it->getRelativeFitness(s, h));
    }
    
    
    if (no_ind > 0)
    {
        age++;
        
        //calculate expected number of offspring 
        //expected_offspring = capacity;   //demes are filled immediately
    
        expected_offspring = no_ind * (r/(1 + (double)(no_ind*(r-1))/capacity));  // beverton-holt
        
        //realized offspring is obtained from a poisson distribution
        if (expected_offspring > 0)
        {
                realized_offspring = randpois(expected_offspring);   
        }
        else 
        {
            realized_offspring = 0;
        }
        
    
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;)
        {
                // generate new individual

                // create new gametes from parents
                do
                {
                    mom = randint(0,this_generation.size()-1);    // draw parents with prob proportional to their fitnesses
                    it=this_generation.begin();
                    advance(it,mom);
                    mom_fit = it->getRelativeFitness(s, h);
                    gamete_mom = it->getNewGamete(mutation_rate,s,front);
                    mig_mom = it->getIndMigRate();
                   
                }while( mom_fit < randreal(0,max_fit));

                do
                {                   
                    dad = randint(0,this_generation.size()-1); 
                    it=this_generation.begin();
                    advance(it,dad);
                    dad_fit = it->getRelativeFitness(s, h);
                    gamete_dad = it->getNewGamete(mutation_rate,s,front);
                    mig_dad = it->getIndMigRate();
                    
                }while( dad_fit < randreal(0,max_fit));
        
              
                // calculate the offspring's new migration rate
                off_m = (mig_mom + mig_dad) / 2 + rand_normal(0, mig_mut_sd);    // ADD the actual mutation rate for migration trait here plus the normally distributed effect size
                if(off_m > 0.5) off_m = 0.5;
                if(off_m < 0) off_m = 0;
    
    

                //create new individual
                ind.setGenotype(gamete_mom,gamete_dad);
                ind.setIndMigRate(off_m);

                
                
                
               
///                ind.setWFID(wf_cum);

                next_generation.push_back(ind);
                i++;    
        }
        // replace old generation by new
        this_generation = next_generation;
    }
}

void Deme::reproduceSSAM(int wf)                     // soft selection plus assortative mating with respect to fitness
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2;
    double mom_fit,dad_fit;
    bool front;
    
    double a = 0.01;                                                               // parameter measuring strength of assortment
    
    front = (ID >= (wf-1));
    
    no_ind = this_generation.size();
    
    
    max_fit = 0;
    
    for (it = this_generation.begin();it != this_generation.end();it++)
    {
       max_fit = fmax(max_fit,it->getRelativeFitness(s, h));
    }
    
    
    if (no_ind > 0)
    {
        //calculate expected number of offspring 
        //expected_offspring = capacity;   //demes are filled immediately
    
        expected_offspring = no_ind * (r/(1 + (double)(no_ind*(r-1))/capacity));  // beverton-holt
        
        
        //realized offspring is obtained from a poisson distribution
        realized_offspring = randpois(expected_offspring);    
        
        //no stochastic fluctuations in demography
        //realized_offspring = expected_offspring;
    
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;)
        {
                // generate new individual
                mom = randint(0,this_generation.size()-1);    // draw parents with prob proportional to their fitnesses
                dad = randint(0,this_generation.size()-1); 
    
                // create new gametes from parents
                it=this_generation.begin();
                advance(it,mom);
                
                mom_fit = it->getRelativeFitness(s, h);
                
                gamete_mom = it->getNewGameteMM2(mutation_rate,0,s);  
    
        
                it=this_generation.begin();
                advance(it,dad);
                
                dad_fit = it->getRelativeFitness(s, h);
                
                gamete_dad = it->getNewGameteMM2(mutation_rate,0,s); 
                
                //create new individual
                ind.setGenotype(gamete_mom,gamete_dad);
                
        
                if (dad_fit > randreal(0,max_fit) && mom_fit > randreal(0,max_fit) && (pow(abs(dad_fit - mom_fit),a) < randreal(0,1)) ) 
                {
                        next_generation.push_back(ind);
                        i++;
                }
        }
        // replace old generation by new
        this_generation = next_generation;
    }  
}


void Deme::reproduceHS1(double mean_fit,int wf)		// hard selection
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2;
    double K = capacity;
    double mom_fit,dad_fit;
    double mig_mom,mig_dad,off_m;
    double mig_mut_sd = 0.005;
    bool front;
    
    front = (ID >= (wf-1));
    
    r = r * mean_fit;
    
    K = min((double)2*capacity,capacity * mean_fit);
    
    
    max_fit = 0;
    
    for (it = this_generation.begin();it != this_generation.end();it++)
    {
       max_fit = fmax(max_fit,it->getRelativeFitness(s, h));
    }
    

    
    no_ind = this_generation.size();
    //cout << "test: " <<this_generation.size() << " " << no_ind;
    if (no_ind > 0)
    {
        //calculate expected number of offspring 
        //expected_offspring = capacity;   //demes are filled immediately
    
        expected_offspring = max(0,no_ind * (r/(1 + (double)(no_ind*(r-1))/K)));  // beverton-holt
        realized_offspring = 0;
        
        if (r <= 1)
        {
            expected_offspring = no_ind * r; 
        }   
        
        //realized offspring is obtained from a poisson distribution
        if (expected_offspring  > 0)
        {
            realized_offspring = randpois(expected_offspring);    
        }
        
  
        
        //no stochastic fluctuations in demography
        //realized_offspring = expected_offspring;
    
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;)
        {
                // generate new individual
                mom = randint(0,this_generation.size()-1);    // draw parents with prob proportional to their fitnesses
                dad = randint(0,this_generation.size()-1); 
    
                // create new gametes from parents
                it=this_generation.begin();
                advance(it,mom);
                
                mom_fit = it->getRelativeFitness(s, h);
                
                gamete_mom = it->getNewGamete(mutation_rate,s,front); //getNewGameteMM2(mutation_rate,mutation_rate,s); 
                mig_mom = it->getIndMigRate();
    
        
                it=this_generation.begin();
                advance(it,dad);
                
                dad_fit = it->getRelativeFitness(s, h);
                
                gamete_dad = it->getNewGamete(mutation_rate,s,front);//getNewGameteMM2(mutation_rate,mutation_rate,s); 
                mig_dad = it->getIndMigRate();
                
                // calculate the offspring's new migration rate
                off_m = (mig_mom + mig_dad) / 2 + rand_normal(0, mig_mut_sd);    // ADD the actual mutation rate for migration trait here plus the normally distributed effect size
                if(off_m > 0.5) off_m = 0.5;
                if(off_m < 0) off_m = 0;
    
    

                //create new individual
                ind.setGenotype(gamete_mom,gamete_dad);
                ind.setIndMigRate(off_m);

                
                
                if (dad_fit > randreal(0,max_fit) && mom_fit > randreal(0,max_fit)) 
                {
                        next_generation.push_back(ind);
                        i++;
                }
        }
        // replace old generation by new
        this_generation = next_generation;
    }
}


void Deme::reproduceSSburnin(int wf, double phi, double h)
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2;
    double mom_fit,dad_fit;
    extern double fitnessConstant;
    fitnessConstant = 1;
    double mig_mom,mig_dad,off_m;
    double mig_mut_sd = 0.005;

    
    bool front;
    
    front = (ID >= (wf-1));
    
    no_ind = this_generation.size();
    
   
    max_fit = 0;
    
    for (it = this_generation.begin();it != this_generation.end();it++)
    {
       max_fit = fmax(max_fit,it->getRelativeFitness(s, h));
    }
    
    
    //cout << "test: " <<this_generation.size() << " " << no_ind;
    if (no_ind > 0)
    {
        age++;
        
        //calculate expected number of offspring 
        //expected_offspring = capacity;   //demes are filled immediately
    
        expected_offspring = no_ind * (r/(1 + (double)(no_ind*(r-1))/capacity));  // beverton-holt
        
        
        //realized offspring is obtained from a poisson distribution
        if (expected_offspring > 0)
        {
                realized_offspring = randpois(expected_offspring);   
        }
        else 
        {
            realized_offspring = 0;
        }
        
    
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;)
        {
                // generate new individual
                // create new gametes from parents
                do
                {
                    mom = randint(0,this_generation.size()-1);    // draw parents with prob proportional to their fitnesses
                    it=this_generation.begin();
                    advance(it,mom);
                    mom_fit = it->getRelativeFitness(s, h);
                    gamete_mom = it->getNewGameteBurnin(mutation_rate,s, phi);
                    mig_mom = it->getIndMigRate();
                    
                }while( mom_fit < randreal(0,max_fit));

                do
                {                   
                    dad = randint(0,this_generation.size()-1); 
                    it=this_generation.begin();
                    advance(it,dad);
                    dad_fit = it->getRelativeFitness(s, h);
                    gamete_dad = it->getNewGameteBurnin(mutation_rate,s, phi);
                    mig_dad = it->getIndMigRate();

                    
                }while( dad_fit < randreal(0,max_fit));
        
                // calculate the offspring's new migration rate
                off_m = (mig_mom + mig_dad) / 2 + rand_normal(0, mig_mut_sd);    // ADD the actual mutation rate for migration trait here plus the normally distributed effect size
                if(off_m > 0.5) off_m = 0.5;
                if(off_m < 0) off_m = 0;
    
    

                //create new individual
                ind.setGenotype(gamete_mom,gamete_dad);
                ind.setIndMigRate(off_m);
               
///                ind.setWFID(wf_cum);

                next_generation.push_back(ind);
                i++;    
        }
        // replace old generation by new
        this_generation = next_generation;
    }
}

void Deme::reproduceHSburnin(double mean_fit,int wf, double phi, double dom)		// hard selection
{
    int no_ind,i;
    double expected_offspring;
    int realized_offspring;
    Individual ind;
    int mom,dad;
    heritableUnit gamete_mom,gamete_dad;
    list<Individual>::iterator it;
    double r = 2;
    double K = capacity;
    double mom_fit,dad_fit;
    bool front;
    double mig_mom,mig_dad,off_m;
    double mig_mut_sd = 0.005;
    
    front = (ID >= (wf-1));
    
    r = r * mean_fit;
    
    K = min((double)2*capacity,capacity * mean_fit);
    
    
    max_fit = 0;
    
    for (it = this_generation.begin();it != this_generation.end();it++)
    {
       max_fit = fmax(max_fit,it->getRelativeFitness(s, h));
    }
    

    
    no_ind = this_generation.size();
    //cout << "test: " <<this_generation.size() << " " << no_ind;
    if (no_ind > 0)
    {
        //calculate expected number of offspring 
        //expected_offspring = capacity;   //demes are filled immediately
    
        expected_offspring = max(0,no_ind * (r/(1 + (double)(no_ind*(r-1))/K)));  // beverton-holt
        realized_offspring = 0;
        
        if (r <= 1)
        {
            expected_offspring = no_ind * r; 
        }   
        
        //realized offspring is obtained from a poisson distribution
        if (expected_offspring  > 0)
        {
            realized_offspring = randpois(expected_offspring);    
        }
        
        
        //no stochastic fluctuations in demography
        //realized_offspring = expected_offspring;
    
        next_generation.clear();
   
        for (i = 0;i<realized_offspring;)
        {
                // generate new individual
                mom = randint(0,this_generation.size()-1);    // draw parents with prob proportional to their fitnesses
                dad = randint(0,this_generation.size()-1); 
    
                // create new gametes from parents
                it=this_generation.begin();
                advance(it,mom);
                
                mom_fit = it->getRelativeFitness(s, h);
                
                gamete_mom = it->getNewGameteBurnin(mutation_rate,s,phi); //getNewGameteMM2(mutation_rate,mutation_rate,s); 
                mig_mom = it->getIndMigRate();
    
        
                it=this_generation.begin();
                advance(it,dad);
                
                dad_fit = it->getRelativeFitness(s, h);
                
                gamete_dad = it->getNewGameteBurnin(mutation_rate,s,phi);//getNewGameteMM2(mutation_rate,mutation_rate,s); 
                mig_dad = it->getIndMigRate();
                
                // calculate the offspring's new migration rate
                off_m = (mig_mom + mig_dad) / 2 + rand_normal(0, mig_mut_sd);    // ADD the actual mutation rate for migration trait here plus the normally distributed effect size
                if(off_m > 0.5) off_m = 0.5;
                if(off_m < 0) off_m = 0;
    
    

                //create new individual
                ind.setGenotype(gamete_mom,gamete_dad);
                ind.setIndMigRate(off_m);
                
                
                
                if (dad_fit > randreal(0,max_fit) && mom_fit > randreal(0,max_fit)) 
                {
                        next_generation.push_back(ind);
                        i++;
                }
        }
        // replace old generation by new
        this_generation = next_generation;
    }
}

void Deme::select()			// old function for viability seln
{
    list<Individual>::iterator it;
    double fitness=1;
    double mean_fit = 1;
     
 
    for (it = this_generation.begin();it!=this_generation.end();)
    {
        fitness = it->getFitness(s);
        
        if (fitness < randreal(0,1)) 
        {
            it = this_generation.erase(it);
        }
        else
        {
            it++;
        }
    }
}

Migrants Deme::getMigrantsNoEvolve()
{
    list<Individual>::iterator it;
    Migrants migrants;
    double indMigRate;
    
    //pick migrants, remove migrants from original deme
    for (it = this_generation.begin(); it != this_generation.end(); )
    {
        if (randreal(0,1)<indMigRate) {migrants.push_back(*it); it = this_generation.erase(it); } // original migration function randomly chooses m proportion of migrants out of the population
        else    {it++; }
    }
    
    return(migrants);
}

Migrants Deme::getMigrants()
{
    list<Individual>::iterator it;
    Migrants migrants;
    double indMigRate;
    
    //pick migrants, remove migrants from original deme
    for (it = this_generation.begin(); it != this_generation.end(); )
    {
        // put a function here that recalculates m per individual
        indMigRate = it->getIndMigRate();
        
        //cout << "test what is mig rate " << indMigRate << endl;
        
        if (randreal(0,1)<indMigRate) {migrants.push_back(*it); it = this_generation.erase(it); } // original migration function randomly chooses m proportion of migrants out of the population
        else    {it++; }
    }
    
    return(migrants);
}


Migrants Deme::sampleIndividuals(int samplesize)
{
    list<Individual>::iterator it;
    Individual ind;
    Migrants sampled_individuals;
    int tot_individuals;
    int i;
    
    tot_individuals = this_generation.size();
    
    
    for (i =0 ; i < samplesize ; i++)
    {
        it = this_generation.begin();
       
        advance(it,randint(0,tot_individuals-1));
       
        ind = *it;
        
        sampled_individuals.push_back(ind);
    }
    
    return(sampled_individuals);
}

void Deme::print()
{
    cout << "\n" << "Individuals: " << this_generation.size() <<  "   ";
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it != this_generation.end(); it++)
    {
        //it->print();
        //cout << "\n Fitness: " << it->getFitness(s) << "\n";
    }
}

void Deme::addMigrant(Individual ind)
{
    this_generation.push_back(ind);   
}

void Deme::printStat()
{
    double mean_fit=0;
    unsigned long number_muts=0;
    
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        mean_fit += it->getRelativeFitness(s, h);
    }

    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        number_muts += it->getNumberMutations();
    }
    
    
    if(this_generation.size()>0)
    {
        mean_fit /= this_generation.size();
    }
    
    cout <<  "\nMean fitness in ancestral population  " << mean_fit ;
    cout << "\n Number of individuals in ancestral population:  " << this_generation.size() << "\n";
    cout << " Number of mutations in ancestral population:  " << number_muts << "\n";
}

double Deme::getMeanFit()
{
    double mean_fit=0;
    
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        mean_fit += it->getRelativeFitness(s, h);
    }
    
    mean_fit /= this_generation.size();
    
    if (mean_fit!= mean_fit) 
    {
        mean_fit = -1;
    }
    
    return(mean_fit);
}

double Deme::getMeanMigTrait()
{
    double mean_mig=0;
    
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        mean_mig += it->getIndMigRate();
    }
    
    mean_mig /= this_generation.size();
    
    if (mean_mig!= mean_mig) 
    {
        mean_mig = -1;
    }
    
    return(mean_mig);
}

double Deme::getVarFit(double mean_fit)
{
    double var_fit=0;
    
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        var_fit += pow(log(mean_fit) - log(it->getRelativeFitness(s, h)),2);
    }
    
    var_fit /= (this_generation.size()-1);
    
    if (var_fit!= var_fit) 
    {
        var_fit = -1;
    }
    
    return(var_fit);
}

///* KJG adding
double Deme::getDemeDensity()
{
    double deme_density=0;
    
    list<Individual>::iterator it;
    
    deme_density = this_generation.size();
    
    if (deme_density != deme_density) 
    {
        deme_density = -1;
    }
    
    return(deme_density);
}
//*/

double Deme::getHeterozygosity(vector<int> a_loci,int loci_begin,int loci_end)
{
    double het=0;
    vector<double> p;
    vector<double> q;

    int used_loci = 0;
    
    p.resize(loci_end);
    fill_n(p.begin(),loci_end,0);
    
   
    //double p2 = 0;
    
    
    list<Individual>::iterator it;
        
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        q = it->getSumAlleles(loci_begin,loci_end);
        
        for (int i = 0;i< a_loci.size(); i++)
        {
             p[a_loci[i]] = p[a_loci[i]] + q[a_loci[i]]; 
        }
    }
    
    
    for (int i = 0;i< a_loci.size(); i++)
    {
       p[a_loci[i]] = p[a_loci[i]]/(2*this_generation.size()); 
       het += (2*p[a_loci[i]]*(1-p[a_loci[i]]));
       used_loci++;
    }
    
    het /= used_loci;

    return(het);
}

void Deme::setParams(int K,double mu,double sel,double mig, double dom)
{
        m=mig;
        capacity=K;
        s=sel;
        mutation_rate=mu; 
        h=dom;
        
       // this->setDemeMigRate(mig);
        
}

void Deme::setDemeMigRate(double m){
    
    list<Individual>::iterator it;
    
    it = this_generation.begin();
    it->setIndMigRate(m);   // there is only one deme for the ancestral pop so can just have this line and remove the below
    
    // these are repetitive (above and below commands)
    //for (it = this_generation.begin();it!=this_generation.end();it++)
    //{
    //    it->setIndMigRate(m);
    //}
}


void Deme::setParams(int K)
{    
    capacity=K;    
}

void Deme::setID(int i)
{
        ID = i;        
}

bool Deme::colonized()
{
    if (this_generation.size() > 0)
        return true;
    else return false; 
}


int Deme::getSize()
{
    return(this_generation.size());
}


/*Count Deme::getStatMut()
{
    Count c,cnew;
    
    
    c.resize(4);
    fill_n(c.begin(),4,0);
    
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        cnew = it->getMutationCount();
        c[0] = cnew[0] + c[0];
        c[1] = cnew[1] + c[1];
        c[2] = cnew[2] + c[2];
        c[3] = cnew[3] + c[3];
    }
    
    c[0] /= max(1,this_generation.size());
    c[1] /= max(1,this_generation.size());
    c[2] /= max(1,this_generation.size());
    c[3] /= max(1,this_generation.size());
    
    return(c);
}

*/

/*void Deme::ResetMutationOrigin()
{
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        it->ResetMutationOrigin();
    }
}
*/

double Deme::sample_wfID(int max_age)
{
    list<Individual>::iterator it;
    int no_ind = this_generation.size();
    
    if(no_ind <= 0)
    {
        return(0);
    }
    
    if(age > max_age)
    {    
        return(0);
    }
    
    it = this_generation.begin();
       
    advance(it,randint(0,no_ind-1));
    
    return((it->getWFID()));
}

void Deme::normalizeFitness()
{
    double mean_fit;
    
    mean_fit = getMeanFit();
    list<Individual>::iterator it;
    
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        it->normalizeFitness(mean_fit);
    }
}

vector<int> Deme::getAscLoci(int loci_begin,int loci_end)
{
    vector<int> a_loci;
    list<Individual>::iterator it;
    vector<double> p;
    vector<double> q;
    int loci  =loci_end-loci_begin;
  
    
    p.resize(loci_end);
    
    fill_n(p.begin(),loci_end,0);
        
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        q = it->getSumAlleles( loci_begin, loci_end);
        
        for (int i = 0;i< loci; i++)
        {
             p[i] = p[i] + q[i]; 
        }
    }
    
    
    
    for (int i = 0;i< loci; i++)
    {
       p[i] = p[i]/(2*this_generation.size()); 
       if (min(p[i],1-p[i])>=0.05)
       {
           a_loci.push_back(i);
       }
    }
    
    return(a_loci);
}


vector<int> Deme::getAscLociSample(int loci_begin,int loci_end,int n)                              // n individuals are sampled and the sites at which this sample is polymorphic are returned 
{                                                                               // this is done for the subset (1, ... ,loci) 
    vector<int> a_loci;
    list<Individual>::iterator it;
    vector<double> p;
    vector<double> q;
    int no_ind,id;
    vector<int> inds;
    int i,j,start;
    float min_freq;
    
    min_freq = 1/(2*(float)n);
    min_freq = 0.05;
    
    start = 0;
    
    p.resize(loci_end);
    a_loci.resize(0);
    
    fill_n(p.begin(),loci_end,0);
        
    no_ind = this_generation.size();
    
    if (no_ind < n)
        return(a_loci);
    
    inds.resize(no_ind);
    for (i = 0; i<no_ind; i++) inds[i]=i;
        
    for (j = 0; j < n; j++)
    {
        id = randint(0,no_ind-1);
        it = this_generation.begin();
        
        advance(it,inds[id]);
        
        inds.erase(inds.begin()+id);
        no_ind--;
        
        
        q = it->getSumAlleles(loci_begin,loci_end);
        
        for (i = 0;i< loci_end; i++)
        {
             p[i] = p[i] + q[i]; 
        }
    }
    
    
    
    for (int i = 0;i< loci_end; i++)
    {
       p[i] = p[i]/(2*n); 
       if (min(p[i],1-p[i])>=min_freq)
       {
           a_loci.push_back(i);
       }
    }
    
    return(a_loci);
}

vector<double> Deme::getFrequencies(int loci_begin,int loci_end)
{
    vector<int> a_loci;
    list<Individual>::iterator it;
    vector<double> p;
    vector<double> q;
    int loci = loci_end-loci_begin;
  
    
    p.resize(loci);
    
    fill_n(p.begin(),loci_end,0);
        
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        q = it->getSumAlleles(loci_begin, loci_end);
        
        for (int i = 0;i< loci; i++)
        {
             p[i] = p[i] + q[i]; 
        }
    }
    
    
    
    for (int i = 0;i< loci; i++)
    {
       p[i] = p[i]/(max(1,2*this_generation.size())); 
    }
    return(p);
}

vector<double> Deme::getGenotypeFrequencies(int loci_begin,int loci_end,int genotype)
{
    vector<int> a_loci;
    list<Individual>::iterator it;
    vector<double> p;
    vector<double> q;
    int loci = loci_end-loci_begin;
  
    
    p.resize(loci);
    
    fill_n(p.begin(),loci_end,0);
        
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        q = it->getSumGenotypes(loci_begin, loci_end,genotype);
        
        for (int i = 0;i< loci; i++)
        {
             p[i] = p[i] + q[i]; 
        }
    }
    
    
    for (int i = 0;i< loci; i++)
    {
       p[i] = p[i]/(max(1,this_generation.size())); 
    }
    return(p);
}

double Deme::getInversionFrequency()
{
    list<Individual>::iterator it;
    double f = 0;
          
    for (it = this_generation.begin();it!=this_generation.end();it++)
    {
        f = it->getInversionCount();
    }
     
    f = double(f)/double(max(1,this_generation.size()));
    return(f);
}

int Deme::getAge()
{
    return(age);
}

void Deme::set_selection_dist(double mut_prop)
{
    Individual ind;
    ind.set_selection_dist(s,mut_prop);
}