//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player {
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
	int getGrade_n;//�ش� �÷��̾��� �������� ���� ���� ���� 
} player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif


//function prototypes
#if 0

#endif

//calculate average grade of the player
float calcAverageGrade(int player)
{
	float average=0; 
	average=cur_player[player].accumCredit/cur_player[player].getGrade_n;//�������� ������ ���� �������� ���� ������ 
	return average;
}

smmObjGrade_e takeLecture(int player, char *lectureName, int credit)//take the lecture (insert a grade of the player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	//���� ������ ���� �������� �ű�� 
	smmObjGrade_e grade=rand()%9;
	
	char join;
    char choice;
	printf("Lecture (credit:%d, energy:%d) starts! are you going to join? or drop?", smmObj_getNodeCredit, smmObj_getNodeEnergy);//���� ���� ���� ���� 
    scanf("%s", &choice);
	
	//���Ǹ� �����ϱ⿡ ����� �������� �ִ°�? 
           if(choice==join)
           {
           	  if(cur_player[player].energy>=smmObj_getNodeEnergy)
           	  {
                 cur_player[player].accumCredit+=smmObj_getNodeCredit(boardPtr);//������ ������ ���� �ջ��ϱ� 
                 cur_player[player].energy-=smmObj_getNodeEnergy(boardPtr);//���� �������� ���� ������ �Ҹ� 
                 printf("->%s successfully takes the lecture %s with grade %c(average : %f), remained energy : %i)",
				         player, lectureName, grade, calcAverageGrade(player), cur_player[player].energy);
				 cur_player[player].getGrade_n++;//�ش� �÷��̾��� �������� ���� ���� ���� 
		      }
		      else
              printf("%s is too hungry to take the lecture (remained:%d, required:%d)", cur_player[player].name, cur_player[player].energy, smmObj_getNodeCredit);
		   }
		   else
		     printf("-> Player %s drops the lecture!", cur_player[player].name);//���Ǹ� ����� ��� 
	
}

void printGrades(int player)//print grade history of the player
{
	int i;
	void *gradePtr;
	for(i=0;i<smmdb_len(LISTNO_OFFSET_GRADE+player);i++)
	{
		gradePtr=smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
		printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
}

int isGraduated(int credit)//check if any player is graduated
{
	int i=0;
	int graduated_player=0;
	int graduate_credit=30;//30������ �Ѿ�� ���� ���� 
	for(i=0;i<player_nr;i++)
	{
		if(credit>=graduate_credit)
		graduated_player++;//���� �л� �� ���ϱ� 
	}
	
	return graduated_player;
}


void printPlayerStatus(void)//print all player status at the beginning of each turn
{
	int i;
	
	for(i=0;i<player_nr;i++)
	{
		printf("%s : credit %i, energy %i, position %i\n",
		             cur_player[i].name, 
					 cur_player[i].accumCredit, 
					 cur_player[i].energy, 
					 cur_player[i].position);
	}
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	
	for(i=0;i<n;i++)
	{
		//input name
	    printf("�÷��̾� %i �� �̸��� �Է��ϼ��� : ", i);//�ȳ� ���� 
	    scanf("%s", cur_player[i].name);
	    fflush(stdin);
	    
 	    //set position
 	    //player_position=0;
 	    cur_player[i].position=0;
    	
	    //set energy
	    //player_energy[i]=initEnergy;
	    cur_player[i].energy=initEnergy;
	    cur_player[i].accumCredit=0;
	    cur_player[i].flag_graduate=0;
	}
	
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
    if (c == 'g')
        printGrades(player);
#endif

    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	//int type=smmObj_getNodeType(cur_player[player].position);
	int type=smmObj_getNodeType(boardPtr);
	char *name=smmObj_getNodeName(boardPtr);
	void *gradePtr;
		
	
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
        	
           
           smmObjGrade_e takeLecture(int player, smmObj_getNodeName(boardPtr), int credit);
           
           //grade generation
           gradePtr=smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, smmObjGrade_e grade);
           smmdb_addTail(LISTNO_OFFSET_GRADE+player, gradePtr);
           
           break;
           
        //case restaurant:
        case SMMNODE_TYPE_RESTAURANT:
        	
           cur_player[player].energy+=smmObj_getNodeEnergy(boardPtr);
		   printf("Let's eat in %s and charge %d energies (remained energy : %d)", smmObj_getNodeName(boardPtr), smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
           
           
           break;
           
        //case laboratory
        case SMMNODE_TYPE_LAVORATORY:
        	
           if(cur_player[player].energy>20)//������ �ϰ� �� ���
           {
           	 cur_player[player].energy-=smmObj_getNodeEnergy(boardPtr);
			 printf("This is experiment time! You lose %d energy. (remained energy:%d)", smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
		   }
           else//������ ���� �ʰ� �� ���
		     printf("This is not experiment time. You can go through this lab."); 
           
           break;
           
        //case home
        case SMMNODE_TYPE_HOME:
        	
           cur_player[player].energy+=smmObj_getNodeEnergy(boardPtr);
           printf("returned to HOME! energy charged by 18 (total : %d)", cur_player[player].energy);
           
           break;
           
        //case gotolab
        case SMMNODE_TYPE_GOTOLAB:
        
           printf("OMG! This is experiment time!! Player %s goes to the lab.\n", player);//����ǿ� ����.
           printf("Experiment time! Let's see if you can satisfy professor (threshold: 2)\n");
           int die_result;
           die_result=rolldie(player);
           
		   if(die_result>2)
		   {
		   	  printf("Experiment result : %d, success! %s can exit this lab!", die_result, player);
		   }
           
           break;
           
        //case foodchance
        case SMMNODE_TYPE_FOODCHANCE:
        
           printf("%s gets a good chance! press any key to pick a food card:");
           
		   cur_player[player].energy+=smmObj_getNodeEnergy(boardPtr);
		   printf("%s picks %s and charges %d (remained energy : %d)", player, smmObjType_card, smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
           
           break;
           
        //case festival
        case SMMNODE_TYPE_FESTIVAL:
        
           printf("%s participates to Snow Festival! press any number key to pick a festival card:", player);
           
           printf("MISSION : %s !!\n (Press any key when missoin is ended.)", );
           
           break;
           
           
        default:
           break;
    }
    return 0;
}

void goForward(int player, int step)//make player go "step" steps on the board (check if player is graduated)
{
	void *boardPtr;
	cur_player[player].position+=step;//���� �ֻ��� ����ŭ ������ ����
	boardPtr=smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	printf("%s go to node %i (name : %s)\n", cur_player[player].name, cur_player[player].position, smmObj_getNodeName(boardPtr));
}

void* findGrade(int player, char *lectureName)//find the grade from the player's grade history
{
	printf("Congratulations on your graduation!\n"); 
	void *boardPtr;
	int i=0;
	int credit;
	credit=cur_player[player].accumCredit;
	
	for(i=0;i<player_nr;i++)
	{
		if(credit>=30)
		{
			for(i=0;i<cur_player[player].getGrade_n;i++)
			{
				printf("player %s : %s(credit : %d), (grade : %d)", &player, &lectureName, smmObj_getNodeCredit(boardPtr), smmObj_getNodeGrade(boardPtr));
			}
		}
	}
	
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        //char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade
        void *boardObj=smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if(type==SMMNODE_TYPE_HOME)
           initEnergy=energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    for(i=0;i<board_nr;i++)
    {
       void *boardObj=smmdb_getData(LISTNO_NODE, i);
       
       printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
	           i, smmObj_getNodeName(boardObj), smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)), smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
	}
    
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    #if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    //���� ���� 
    printf("===========================================================");
	printf("-----------------------------------------------------------");
	printf("Sookmyung Marble!! Let's Graduate(total credit : 30)!!");
	printf("-----------------------------------------------------------");
    printf("===========================================================");
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
	do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr<0||player_nr>MAX_PLAYER);
    
    cur_player=(player_t*)malloc(player_nr*sizeof(player_t));
    
    generatePlayers(player_nr, initEnergy);
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated(cur_player[turn].accumCredit)<1) //is anybody graduated?
    {
        int die_result;
        
				
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result=rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn=(turn+1)%player_nr;
                
    }
    
    void* findGrade(int player, char *lectureName);//find the grade from the player's grade history
    
    free(cur_player);
    system("PAUSE");
    return 0;
}
