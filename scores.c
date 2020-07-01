#include "scores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static void sort(score_t* d, int length){
	int i, j;
	for (i = 1; i < length; i++) {
		score_t tmp;
		memcpy(&tmp,&d[i],sizeof(score_t));
		for (j = i; j >= 1 && tmp.value < d[j-1].value; j--) {
			memcpy(&d[j],&d[j-1],sizeof(score_t));
		}
		memcpy(&d[j],&tmp,sizeof(score_t));
	}
}

void write_high_score(score_t* score)
{
	FILE* fp = fopen("scores.txt","a+");
	if (fp) {
		fprintf(fp,"%s,%d\n",score->name,score->value);
		fclose(fp);
	}
}

void read_high_scores(score_t scores[])
{
	int count=0;
	FILE* fp = fopen("scores.txt","r");
	score_t all_scores[1024]; /* read 1024 max scores, after that, tough luck */
	memset(all_scores,0,sizeof(all_scores));
	memset(scores,0,sizeof(score_t)*NUM_HIGH_SCORES);
	if (fp) {
		char name[SCORE_NAME_LENGTH];
		int value;
		char buffer[1024];
		while (fgets(buffer,sizeof(buffer),fp)) {
			sscanf(buffer,"%[^,],%d",name,&value);
			all_scores[count].value = value;
			strcpy(all_scores[count].name,name);
			count++;
		}
		fclose(fp);
	}
	score_t* scores_trunc = (score_t*)malloc(sizeof(score_t)*count);
	memcpy(scores_trunc,all_scores,sizeof(score_t)*count);
	sort(scores_trunc, count); /* sort the score list */
	for (int i=0;i<count;i++) {
		scores[i].value = scores_trunc[count-1-i].value;
		strcpy(scores[i].name,scores_trunc[count-1-i].name);
	}
	free(scores_trunc);
}
