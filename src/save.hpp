#pragma once
#include "common.h"

struct SavedScore
{
    char name[10];
    u16 score;
};

struct SavedSurvivalRecord
{
    char name[10];
    u16 record;
};

struct OptionsData
{
    u8 flags;
};

#define OPTION_SOUND    1
#define OPTION_MUSIC    2

struct SaveData 
{
    char header[4];
    OptionsData options;
    SavedScore hiScores[8]; 
    SavedSurvivalRecord survivalRecords[8];
};

static_assert(sizeof(SaveData) <= 1024);

void save_read(SaveData* sd);
void save_init(SaveData* sd);
void save_write(const SaveData* sd);
void save_store_score(SaveData* sd, const char* name, u16 score);
void save_store_survival_record(SaveData* sd, const char* name, u16 score);
void save_erase_records(SaveData* sd);
void save_set_option_flags(SaveData* sd, u8 flags);
u8   save_get_option_flags(const SaveData* sd);