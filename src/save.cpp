#include "save.hpp"
#include "util.hpp"

static void save_store_name(char* dest, const char* src);

void save_read(SaveData* sd)
{
    diskr(sd, sizeof(SaveData));
}

void save_write(const SaveData* sd)
{
    diskw(sd, sizeof(SaveData));
}

void save_init(SaveData* sd)
{
    strcpy(sd->header, "SAV");
    for(int i = 0; i < 8; i++)
    {
        strcpy(sd->hiScores[i].name,        "         ");
        strcpy(sd->survivalRecords[i].name, "         ");
    }

    sd->options.flags |= OPTION_SOUND;
}

void save_erase_records(SaveData* sd)
{
    for(int i = 0; i < 8; i++)
    {
        strcpy(sd->hiScores[i].name,        "         ");
        sd->hiScores[i].score = 0;
    }
    save_write(sd);
}

void save_store_score(SaveData* sd, const char* name, u16 score)
{
    // Find index to place score
    int scoreIdx = -1;
    for(int i = 0; i < 8; i++)
    {
        const SavedScore& s = sd->hiScores[i];
        if(s.score < score)
        {
            scoreIdx = i;
            break;
        }
    }

    if(scoreIdx != -1)
    {
        // Move scores below down.
        for(int i = 7; i > scoreIdx; i--)
        {
            memcpy((u8*)&sd->hiScores[i], (u8*)&sd->hiScores[i-1], sizeof(SavedScore));
        }

        sd->hiScores[scoreIdx].score = score;
        save_store_name(sd->hiScores[scoreIdx].name, name);
        save_write(sd);
    }
}

void save_store_survival_record(SaveData* sd, const char* name, u16 record)
{
    // Find index to place score
    int scoreIdx = -1;
    for(int i = 0; i < 8; i++)
    {
        const SavedSurvivalRecord& s = sd->survivalRecords[i];
        if(s.record < record)
        {
            scoreIdx = i;
            break;
        }
    }

    if(scoreIdx != -1)
    {
        // Move scores below down.
        for(int i = 7; i > scoreIdx; i--)
        {
            memcpy((u8*)&sd->survivalRecords[i], (u8*)&sd->survivalRecords[i-1], sizeof(SavedSurvivalRecord));
        }

        sd->survivalRecords[scoreIdx].record = record;
        save_store_name(sd->survivalRecords[scoreIdx].name, name);
        save_write(sd);
    }
}

static void save_store_name(char* dest, const char* src)
{
    strcpy(dest, src);
    for(int i = (int)strlen(src); i < 9; i++)
    {
        dest[i] = ' ';
    }
    dest[9] = '\0';
}

void save_set_option_flags(SaveData* sd, u8 flags)
{
    sd->options.flags = flags;
    save_write(sd);
}

u8   save_get_option_flags(const SaveData* sd)
{
    return sd->options.flags;
}