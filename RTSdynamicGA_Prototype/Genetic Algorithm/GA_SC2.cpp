 #pragma once

#include <random>
#include <algorithm>
#include <iostream> //FOR DEBUG
#include <bits/stdc++.h>

typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;

#define LOOPS_LIMIT -1 // USED TO LIMIT NUMBERS OF LOOP RUNNING GA, -1 = INFINITY, INT = # of LOOPS limit
#define GENE_LENGTH 81 // LENGTH OF STRING OF SUDOKU
#define MINVAL 1 //MIN VAL FOR SUDOKU
#define MAXVAL 9 //MAX VAL FOR SUDOKU
#define POPULATION_SIZE 100
#define MUTATION_RATE 0.1
#define CROSSOVER_RATE 0.1
#define REMAINING_INDIVIDUALS_SIZE 30
#define CROSSOVER_INDIVIDUALS_SIZE 20
#define MUTATED_INDIVIDUALS_SIZE 30
#define FILLED_INDIVIDUALS_SIZE 20
#define LOCAL_STRUCK_TIMES 10000
#define OLD_GEN_AFTER_RE_NUMS 0

#define INITIAL_STATE 1 // INITIAL STATE FOR RTS AI

#define SUDOKU_ROW 9
#define SUDOKU_COL 9
#define DUPLICATED_ROW_COST 10
#define DUPLICATED_COL_COST 10
#define DUPLICATED_TABLE_COST 100
#define UNSET_COST 1
#define ILLEGAL_COST 10000


std::random_device gen;
// B -> Build Rules
// R -> Research Rules
// E -> Economy Rules
// C -> Combat Rules
struct chromosome {
	//WEIGHT[i] / SUM(WEIGHT[i])i=0...N
	vector<Rule> gene; //GENE as Behavior
    int  fitness, preferWeight;

	//MAY CHANGE THIS FUNC TO RETURN INT LATERS.
	void fitnessCalc() {

	}

	void clear() {
		memset(gene, 0, sizeof(gene));
		fitness = 0;
		preferWeight = 0;
	}

	void randomize() {

		std::uniform_int_distribution<int> distribution(MINVAL, MAXVAL);
		for (int i = 0; i < GENE_LENGTH; i++) {
			int rndval = distribution(gen);

//			gene[i] = rndval;
		}
	}

    void generateGene_RND(){
        int state = INITIAL_STATE;
        while (getStateTier(state) <= MAX_STATE_TIER){
            Script s = generateScript_RND(state);
            this.concatScript(s);
            state = getNextState(state, s);
        }
    }

    void concatScript(Script s){
       for (int i=0;i<s.ruleList.size();i++){
            this.gene.push_back(s.ruleList[i]);
       }
    }

	bool operator <(const chromosome &ot) {
		return fitness < ot.fitness;
	}



}population[POPULATION_SIZE], prototype;

void setPrototype(int pro[]) {
	for (int i = 0; i < GENE_LENGTH; i++)
		prototype.gene[i] = pro[i];
}

void onHold();
void initialize();
void reinitialize(int remainingNum);//reinitialize with first remainingNum-th from old generation
void execute();
void sortToFitness();
void clearNotFitIndividuals();
int chooseIndividual();
void createCrossOverIndividuals();
void mutateIndividuals();
void generateNewIndividuals();
void calculateFitness();
chromosome crossOver(chromosome &A, chromosome &B);
chromosome mutate(chromosome &A);
void printChromosome(int idx);

std::random_device rnd;
int curPopSize = 0;

std::uniform_int_distribution<int> dist(0, 5);
void onHold() {
	int cmd;
	scanf_s("%d", &cmd);
}

void initialize() {
	std::uniform_int_distribution<int> distribution(MINVAL, MAXVAL);

	for (int i = 0; i < POPULATION_SIZE; i++) {
        population[i].generateGene_RND();
    }
}

void reinitialize(int remainingNum) {
	std::uniform_int_distribution<int> distribution(MINVAL, MAXVAL);

	for (int i = remainingNum; i < POPULATION_SIZE; i++) {
        population[i].generateGene_RND();
	}

}


void execute() {
	int loopTimes = 0, curans = -1, loopStill = 0, bestAns = 1e9;
	while (loopTimes < LOOPS_LIMIT || LOOPS_LIMIT < 0) { // TERMINATION CONDITION NOT MET , LOOPS_LIMIT < 0 is INFINITY

		int rem = REMAINING_INDIVIDUALS_SIZE;
		loopStill++;
		if (loopStill > LOCAL_STRUCK_TIMES) {//IF GOT STRUCK ON LOCAL > LOCAL STRUCK TIMES -> RENEW ALL
			printf("RE !\n");
			reinitialize(OLD_GEN_AFTER_RE_NUMS);
			loopStill = 0;
			continue;
		}
		calculateFitness();

		if (curans != population[0].fitness) {
			curans = population[0].fitness;
			bestAns = std::min(curans, bestAns);
			printChromosome(0);
			printf("Best Ans : %d\n", bestAns);
			//printChromosome(1);
			//onHold();
		}
		if (population[0].fitness == 0) break;
		loopTimes++;

		sortToFitness();
		clearNotFitIndividuals();
		createCrossOverIndividuals();
		mutateIndividuals();
		generateNewIndividuals(); // RANDOMLY CREATE NEW SET OF INDIVIDUALS TO FILL THE REMAINING SLOTS
	}
	onHold();
}

void sortToFitness() {
	std::sort(population, population + POPULATION_SIZE);
}

void clearNotFitIndividuals() {
	curPopSize = REMAINING_INDIVIDUALS_SIZE;
	for (int i = REMAINING_INDIVIDUALS_SIZE; i < POPULATION_SIZE; i++) {
		population[i].clear();
	}
}

int chooseIndividual() {
	std::uniform_int_distribution<int> genePosDist(0, curPopSize-1);
	return genePosDist(gen);
}

// B -> Build Rules
// R -> Research Rules
// E -> Economy Rules
// C -> Combat Rules
// S -> State Rule
chromosome stateCrossOver(chromosome c1,chromosome c2){ // 30%
    set<StateRule> s1;
    chromosome ret;

    for (int i=0;i < c1.gene.size();i++){
            if (typeid(c1.gene[i]) == typeid(StateRule)){
                for (int j=0;j < c2.gene.size();j++){
                    if (typeid(c2.gene[j]) != typeid(StateRule)) continue;
                        if (c1.gene[i].getStateID() == c2.gene[i].getStateID()){
                            std::uniform_int_distribution<int> dist(0,1);
                            if (dist(gen)){ // CHANGE
                                do{
                                    ret.push_back(c2.gene[j]);
                                    j++;
                                }while (j < c2.gene.size() && typeid(c2.gene[i]) != typeid(StateRule));
                                if (j < c2.gene.size()) ret.push_back(c2.gene[j]);
                                break;
                            }else{ // STAY
                               do{
                                    ret.push_back(c1.gene[i]);
                                    i++;
                               }while (i < c1.gene.size() && typeid(c1.gene[i]) != typeid(StateRule));
                               break;
                            }
                        }
                }
            }else{
                ret.push_back(c1.gene[i]);
                i++;
            }
    }

    return ret;
}

void replaceMutate(int idx){ // 30%
    std::uniform_int_distribution<int> distribution(0, 3);

    for (int i=0;i<population[idx].gene.size();i++){
        //if (gene[i].tag == 'B') continue; // Building rules are excluded
        //if (BuildRule* g = dynamic_cast<*BuildRule>(&BuildRule))
        if (typeid(gene[i]) == typeid(BuildRule)) continue;
        int rndval = distribution(gen);
        if (rndval == 0){// RANDOM with 25% to replace
            replaceRule(&gene[i]);
        }
    }
}

void biasedMutate(int idx){ // 30%
    std::uniform_int_distribution<int> distribution(0, 1);
    for (int i=0;i<population[idx].gene.size();i++){
        //if (gene[i].tag == 'E' || gene[i].tag == 'C'){
        if (typeid(gene[i]) == typeid(EconomyRule) || typeid(gene[i]) == typeid(CombatRule)){
            if (distribution(gen)){
                biasRule(gene[i]);
            }
        }
    }
}

void randomizeIndividual(int idx){ // 10%

}

void replaceRule(Rule &g){
    std::uniform_int_distribution<int> dist(0,2);
    int rndval = dist(gen);
    if (rndval==0){ // Research Rule


    }else if (rndval == 1){ // Economy Rule

    }else if (rndval == 2){ // Combat Rule

    }
}

void biasRule(Rule &g){
    if (typeid(g) == typeid(EconomyRule)){

    }else if (typeid(g) == typeid(CombatRule)){

    }
}

void createCrossOverIndividuals() {
	for (int i = 0; i < CROSSOVER_INDIVIDUALS_SIZE; i++) {
		int a, b;
		/*int c,a, b;
		a = i, b = i + 1;
		if (chooseIndividual() % 2) {//random swap
		c = a;
		a = b;
		b = c;
		}*/
		a = chooseIndividual();
		//b = chooseIndividual(); //DUPLICATED ELEMENT?
		do {
			b = chooseIndividual();
		} while (a == b);
		population[curPopSize] = crossOver(population[a], population[b]);
		//population[curPopSize] = crossOver(population[a], population[b]);//MUTATE FIRST k-th PAIR
		curPopSize++;
	}
	//curPopSize += CROSSOVER_INDIVIDUALS_SIZE;
}

void mutateIndividuals() {
	for (int i = 0; i < MUTATED_INDIVIDUALS_SIZE; i++) {
		int a = chooseIndividual();
		population[curPopSize] = mutate(population[a]);
		curPopSize++;
	}
	//curPopSize += MUTATED_INDIVIDUALS_SIZE;
}

void generateNewIndividuals() {
	for (int i = 0; i < FILLED_INDIVIDUALS_SIZE; i++) {
		population[curPopSize].SudokuRandomize();
		curPopSize++;
	}
	//curPopSize += FILLED_INDIVIDUALS_SIZE;
}

void calculateFitness() {
	for (int i = 0; i < POPULATION_SIZE; i++) {
		population[i].fitnessCalc();
	}
}

chromosome crossOver(chromosome &A, chromosome &B) {
	std::uniform_int_distribution<int> distribution(0, GENE_LENGTH - 1);
	int rndval = distribution(gen);

	chromosome ret;

	for (int i = 0; i < GENE_LENGTH; i++) {
		//std::swap(population[idxA].gene[i], population[idxB].gene[i]);
		if (i <= rndval)
			ret.gene[i] = A.gene[i];
		else
			ret.gene[i] = B.gene[i];
	}
	return ret;
}

chromosome mutate_old(chromosome &A) {
	//int posMutate = rnd(0, GENE_LENGTH - 1);
	//int MutatedVal = rnd(MINVAL, MAXVAL);
	//population[idx].gene[posMutate] = MutatedVal;
	std::uniform_int_distribution<int> genePosDist(0, GENE_LENGTH - 1);
	std::uniform_int_distribution<int> geneValDist(MINVAL, MAXVAL);
	int posMutate = genePosDist(gen);
	int mutatedVal = geneValDist(gen);
	chromosome ret = A;
	ret.gene[posMutate] = mutatedVal;
	return ret;
}

chromosome mutate(chromosome &A) {
	//int posMutate = rnd(0, GENE_LENGTH - 1);
	//int MutatedVal = rnd(MINVAL, MAXVAL);
	//population[idx].gene[posMutate] = MutatedVal;
	std::uniform_int_distribution<int> genePosDist(0, GENE_LENGTH - 1);
	std::uniform_int_distribution<int> geneValDist(MINVAL, MAXVAL);
	int posMutate = genePosDist(gen);
	while (prototype.gene[posMutate] != 0) posMutate = genePosDist(gen); //KEEP RANDOM UNTIL FOUND UNDEFINED SLOT
	int mutatedVal = geneValDist(gen);
	chromosome ret = A;
	ret.gene[posMutate] = mutatedVal;
	return ret;
}



void printChromosome(int idx) {
	printf("CHROMOSOME %d : \n", idx);

	for (int i = 0; i < SUDOKU_ROW; i++) {
		for (int j = 0; j < SUDOKU_COL; j++) {
			printf("%d ", population[idx].gene[i*SUDOKU_COL + j]);
		}
		printf("\n");
	}
	printf("with fitness : %d\n\n", population[idx].fitness);
}
