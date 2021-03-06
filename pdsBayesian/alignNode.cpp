/*
 *  alignNode.cpp
 *  bayesian
 *
 *  Created by Pat Schloss on 10/11/11.
 *  Copyright 2011 Patrick D. Schloss. All rights reserved.
 *
 */

#include "alignNode.h"
#include "taxonomynode.h"

#include "bayesian.h"

/**************************************************************************************************/

AlignNode::AlignNode(string n, int l): TaxonomyNode(n, l){

	alignLength = 0;
}

/**************************************************************************************************/

void AlignNode::printTheta(){
	cout << "A:\t"; for(int i=0;i<alignLength;i++){	cout << theta[i].A << '\t';		}	cout << endl;
	cout << "T:\t"; for(int i=0;i<alignLength;i++){	cout << theta[i].T << '\t';		}	cout << endl;
	cout << "G:\t"; for(int i=0;i<alignLength;i++){	cout << theta[i].G << '\t';		}	cout << endl;
	cout << "C:\t"; for(int i=0;i<alignLength;i++){	cout << theta[i].C << '\t';		}	cout << endl;
	cout << "I:\t"; for(int i=0;i<alignLength;i++){	cout << theta[i].gap << '\t';	}	cout << endl;
}

/**************************************************************************************************/

void AlignNode::loadSequence(string& sequence){
	
	alignLength = (int)sequence.length();	//	this function runs through the alignment and increments the frequency
											//	of each base for a particular taxon.  we are building the thetas
	
	if(theta.size() == 0){
		theta.resize(alignLength);
		columnCounts.resize(alignLength, 0);
	}
	
	for(int i=0;i<alignLength;i++){
		char base = sequence[i];
		
		if(base == 'A')		{	theta[i].A++;	columnCounts[i]++;		}	//	our thetas will be alignLength x 5  
		else if(base == 'T'){	theta[i].T++;	columnCounts[i]++;		}	//	and we ignore any position that has  
		else if(base == 'G'){	theta[i].G++;	columnCounts[i]++;		}	//	an ambiguous base call
		else if(base == 'C'){	theta[i].C++;	columnCounts[i]++;		}
		else if(base == '-'){	theta[i].gap++; columnCounts[i]++;		}
		else if(base == 'U'){	theta[i].T++;	columnCounts[i]++;		}
	}
	
	numSeqs++;
}	

/**************************************************************************************************/

void AlignNode::checkTheta(){
    
    for(int i=0;i<alignLength;i++){

        if(theta[i].gap == columnCounts[i]){
            columnCounts[i] = 0;
        }
//        else{
//            int maxCount = theta[i].A;
//            
//            if(theta[i].T > maxCount)   {    maxCount = theta[i].T;  }
//            if(theta[i].G > maxCount)   {    maxCount = theta[i].T;  }
//            if(theta[i].C > maxCount)   {    maxCount = theta[i].T;  }
//            if(theta[i].gap > maxCount) {    maxCount = theta[i].T;  }
//        
//            if(maxCount < columnCounts[i] * 0.25){// || maxCount == columnCounts[i]){   //remove any column where the maximum frequency is <50%
//                columnCounts[i] = 0;
//            }
//        }

    }
    
}

/**************************************************************************************************/

void AlignNode::addThetas(vector<thetaAlign> newTheta, int newNumSeqs){
	
	if(alignLength == 0){
		alignLength = (int)newTheta.size();
		theta.resize(alignLength);
		columnCounts.resize(alignLength);
	}
	
	for(int i=0;i<alignLength;i++){	
		theta[i].A += newTheta[i].A;		columnCounts[i] += newTheta[i].A;
		theta[i].T += newTheta[i].T;		columnCounts[i] += newTheta[i].T;
		theta[i].G += newTheta[i].G;		columnCounts[i] += newTheta[i].G;
		theta[i].C += newTheta[i].C;		columnCounts[i] += newTheta[i].C;
		theta[i].gap += newTheta[i].gap;	columnCounts[i] += newTheta[i].gap;
	}
	
	numSeqs += newNumSeqs;
}

/**************************************************************************************************/

double AlignNode::getSimToConsensus(string& query){
	
	double similarity = 0;
	
	int length = 0;
	
	for(int i=0;i<alignLength;i++){
		
		char base = query[i];
		
		if(base != '.' && base != 'N' && columnCounts[i] != 0){
			
			double fraction = 0;
			
			if(base == 'A'){
				fraction = (int) theta[i].A / (double) columnCounts[i];
				similarity += fraction;
				length++;
			}
			else if(base == 'T'){
				fraction = (int) theta[i].T / (double) columnCounts[i];
				similarity += fraction;
				length++;
			}
			else if(base == 'G'){
				fraction = (int) theta[i].G / (double) columnCounts[i];
				similarity += fraction;
				length++;
			}
			else if(base == 'C'){
				fraction = (int) theta[i].C / (double) columnCounts[i];
				similarity += fraction;
				length++;
			}
			else if(base == '-'){
                fraction = (int) theta[i].gap / (double) columnCounts[i];
                similarity += fraction;
                length++;
			}
		}
	}

	if(length != 0){
		similarity /= double(length);
	}
	else {
		similarity = 0;
	}
	
	return similarity;		
}

/**************************************************************************************************/

double AlignNode::getPxGivenkj_D_j(string& query){	//P(x | k_j, D, j)
	
	double PxGivenkj_D_j = 0;
	
	int count = 0;
	double alpha = 1 / (double)totalSeqs;	//flat prior
	
	
	for(int s=0;s<alignLength;s++){
		
		char base = query[s];
		thetaAlign thetaS = theta[s];
 
		if(base != '.' && base != 'N' && columnCounts[s] != 0){
			double Nkj_s = (double)columnCounts[s];	
			double nkj_si = 0;
			
			
			if(base == 'A')		{	nkj_si = (double)thetaS.A;		}
			else if(base == 'T'){	nkj_si = (double)thetaS.T;		}	
			else if(base == 'G'){	nkj_si = (double)thetaS.G;		}	
			else if(base == 'C'){	nkj_si = (double)thetaS.C;		}	
			else if(base == '-'){	nkj_si = (double)thetaS.gap;	}
			else if(base == 'U'){	nkj_si = (double)thetaS.T;		}	
			
//			double alpha = pow(0.2, double(Nkj_s)) + 0.0001;	//need to make 1e-4 a variable in future; this is the non-flat prior

//			if(columnCounts[s] != nkj_si){						//deal only with segregating sites...
				double numerator = nkj_si + alpha;
				double denomenator = Nkj_s + 5.0 * alpha;
				
				PxGivenkj_D_j += log(numerator) - log(denomenator);		
				count++;
//			}
		}
		if(base != '.' && columnCounts[s] == 0 && thetaS.gap == 0){
			count = 0;
			break;
		}

	}

	if(count == 0){	PxGivenkj_D_j = -1e10;	}	
	
	return PxGivenkj_D_j;
}

/**************************************************************************************************/
