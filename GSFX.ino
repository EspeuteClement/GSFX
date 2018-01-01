#include <Gamebuino-Meta.h>
#include "GSFX.h"

GSFX gsfx;
bool is_init = 0;

const int8_t NUM_PATTERNS = 8;



struct Save
{
    uint8_t is_init;
    GSFX::Pattern patterns[NUM_PATTERNS];
} save = {0};

const SaveDefault savefileDefaults[] {
    {0, SAVETYPE_BLOB, 0, sizeof(Save)}
};

const int num_params = 5;
struct params_settings
{
    int32_t defaults[num_params];
    int32_t min[num_params];
    int32_t max[num_params];
    int32_t incr[num_params];
};

const params_settings square =
{
    {0x8000, 0, 8192,0,5000},
    {0x0, -64, 1000, -64, 0},
    {0xFFFF, 64, 26000, 64, 50000},
    {256, 1,256,1,1000},
};

const params_settings noise =
{
    {0x8000, 0, 0,0,5000},
    {0x0, -128, 0, -128, 0},
    {0xFFFF, 128, 32<<12, 128, 50000},
    {256, 1,256<<4,4,1000},
};

const params_settings * const waves_settings[] = {&noise,&square};

const char * names[] = {"VOLUME", "VOL -/+", "PERIOD", "PER -/+", "LENGTH"};
const int mult[] = {256,1,16,1,5000};
uint32_t s_vol = 0;
uint32_t cursor = 0;
uint32_t timer = 0;
uint32_t default_timer = 100;

const uint32_t bar_length = 32;

GSFX::Pattern * pattern;
uint8_t current_pattern_fx;
uint8_t current_save_slot;

uint32_t message_time;
char message_buffer[32];

bool is_copy;
GSFX::FX copy;

char * waves_names[] = {"NOISE","SQUARE"};

void setup()
{
    gb.begin();
    gb.save.config(savefileDefaults);
    is_init = gsfx.init() != -1;

    gb.save.get(0, &save, sizeof(Save));

    if (!save.is_init)
    {
        for (int p = 0; p < NUM_PATTERNS; p++)
        {
            pattern = &save.patterns[p];
            memset(pattern, 0, sizeof(GSFX::Pattern));
            current_pattern_fx = 0;
            pattern->length = 1;
            for (int j = 0; j < 8; j ++)
            {
                for (int i = 0; i < num_params; i++)
                {
                    pattern->fxs[j].params[i+1] = square.defaults[i];
                    pattern->fxs[j].type = GSFX::WaveType::SQUARE;
                }
            }
        }

    }

    is_copy = true;
    
    pattern = &save.patterns[0];
    current_save_slot = 0;


}

void loop()
{
    if(gb.update())
    {
        gb.display.clear();
        gb.display.setColor(WHITE);
        //gb.display.printf("%d",gb.getCpuLoad());

        pattern = &save.patterns[current_save_slot];
        GSFX::FX * fx = &pattern->fxs[current_pattern_fx];

        if (gb.buttons.pressed(BUTTON_A))
        {
            /*gsfx.play(
                {
                    {{GSFX::WaveType::SQUARE, params[0],params[1],params[2],params[3],params[4]},
                    {GSFX::WaveType::SQUARE, params[0],params[1],params[2],params[3],params[4]}},
                    2
                });*/

            gsfx.play(*pattern);
        }

        if (gb.buttons.pressed(BUTTON_UP))
        {
            cursor = (cursor + 10 - 1) % 10;
        }
        else if (gb.buttons.pressed(BUTTON_DOWN))
        {
            cursor = (cursor + 10 + 1) % 10;
        }

        int y_pos = 0;
        
        gb.display.setCursorX(8);
        y_pos = -1 * 6 + 10;
        gb.display.setCursorY(y_pos);
        
        gb.display.printf("WAVE %s", waves_names[(uint32_t)fx->type]);

        for (int i = 0; i < 5; i++)
        {
            gb.display.setCursorX(8);
            y_pos = i * 6 + 10;
            gb.display.setCursorY(y_pos);
            gb.display.printf(names[i]);
            

            int32_t value = fx->params[i+1];
            int32_t min = waves_settings[(uint32_t)fx->type]->min[i];
            int32_t max = waves_settings[(uint32_t)fx->type]->max[i];

            // Draw bar

            gb.display.drawFastHLine(40,y_pos + 3,bar_length);

            //distance
            int32_t d = 40 + ((value-min) * bar_length) / (max - min);
            int32_t zero = 40 + ((0-min) * bar_length) / (max - min);

            gb.display.drawLine(d,y_pos,d,y_pos+4);
            gb.display.setColor(GRAY);
            gb.display.drawLine(zero,y_pos-1,zero,y_pos+4);
            gb.display.setColor(WHITE);
            //gb.display.printf("%d",fx->params[i+1]);
        }

        y_pos += 6;
        gb.display.setCursorX(8);
        gb.display.setCursorY(y_pos);

        gb.display.printf("PATTERN POS %d\n",current_pattern_fx);
        gb.display.setCursorX(8);
        gb.display.printf("PATTERN NUM %d\n",pattern->length);
        gb.display.setCursorX(8);
        gb.display.printf("SAVE SLOT %d\n",current_save_slot);

        gb.display.setCursorX(2);
        gb.display.setCursorY(cursor * 6 + 10 - 6);
        gb.display.printf(">");
        

        int dir = 0;
        if (gb.buttons.repeat(BUTTON_LEFT, 0))
        {
            if (timer == 1 || timer > 12)
                dir = -1;
        }
        else if (gb.buttons.repeat(BUTTON_RIGHT,0))
        {
            if (timer == 1 || timer > 12)
                dir = 1;
        }
        else
        {
            timer = 0;
        }

        timer ++;

        if (cursor == 0)
        {
            fx->type = (GSFX::WaveType)(((int)fx->type + dir + (int)GSFX::WaveType::WAVE_COUNT) % (int)GSFX::WaveType::WAVE_COUNT);
            if (dir!=0)
            {
                for (int i = 0; i < num_params; i++)
                {
                    fx->params[i+1] = waves_settings[(uint32_t)fx->type]->defaults[i];
                }
            }
        }
        else if (cursor > 0 && cursor < 6)
        {

            int tmp_cur = cursor - 1;

            gb.display.setCursorX(30);
            gb.display.setCursorY(64-6);
            gb.display.printf("V:%d",fx->params[tmp_cur+1]);

            int offset = waves_settings[(uint32_t)fx->type]->incr[tmp_cur];
            if (gb.buttons.repeat(BUTTON_B,0))
            {
                offset *= 10;

                if (tmp_cur == 2)
                {
                    offset = 0;
                    if (dir == 1)
                    {
                        fx->params[tmp_cur+1] <<= 1;
                    }
                    else if (dir == -1)
                    {
                        fx->params[tmp_cur+1] >>= 1;
                    }
                }
            }

            fx->params[tmp_cur+1] += offset * dir;
            
            if (default_timer < 7)
            {
                default_timer ++;
                if (gb.buttons.pressed(BUTTON_B))
                {
                    fx->params[tmp_cur+1] = waves_settings[(uint32_t)fx->type]->defaults[tmp_cur];
                }
            }
            else if (gb.buttons.pressed(BUTTON_B))
            {
                default_timer = 0;   
            }
        }
        else if (cursor == 6)
        {
            // Pattern position
            current_pattern_fx = (current_pattern_fx + pattern->length + dir) % pattern->length;
        }
        else if (cursor == 7)
        {
            // Pattern number
            pattern->length = (((pattern->length-1) + GSFX::MAX_PATTERNS + dir ) % GSFX::MAX_PATTERNS) + 1;
        }
        else if (cursor == 8)
        {
            current_save_slot = (current_save_slot + dir + NUM_PATTERNS) % NUM_PATTERNS;
        }
        else if (cursor == 9)
        {
            if (dir) is_copy = !is_copy;
            if (gb.buttons.pressed(BUTTON_B))
            {
                 message_time = 50;
                sprintf(message_buffer,is_copy ? "COPIED" : "PASTED");
                if (is_copy)
                {
                    copy = *fx;
                }
                else
                {
                    *fx = copy;
                }
            }
        }

        gb.display.setCursorX(8);
        gb.display.setCursorY(64-6);
        gb.display.printf(is_copy ? "COPY" : "PASTE");

        if (gb.buttons.pressed(BUTTON_MENU))
        {
            save.is_init = 1;
            gb.save.set(0, &save, sizeof(Save));
            message_time = 50;
            sprintf(message_buffer,"SAVED");
        }

        if (message_time > 0)
        {
            message_time--;
            gb.display.setCursorX(40);
            gb.display.setCursorY(64-6);
            gb.display.printf(message_buffer);
        }

        

    }
}