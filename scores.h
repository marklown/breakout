#ifndef HIGH_SCORES_H
#define HIGH_SCORES_H

#define NUM_HIGH_SCORES 10
#define SCORE_NAME_LENGTH 32

typedef struct {
	char name[SCORE_NAME_LENGTH];
	int  value;
} score_t;

void write_high_score(score_t* score);
void read_high_scores(score_t scores[]);

#endif // HIGH_SCORES_H
